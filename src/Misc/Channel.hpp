#pragma once
#include "NameHelper.h"
#include <boost/asio/dispatch.hpp>
#include <boost/asio/steady_timer.hpp>
#include <condition_variable>
#include <mutex>
#include <queue>
#include "Error.h"

namespace misc {
template <typename T>
T PopValue(std::queue<T>& queue) {
	auto value = std::move(queue.front());
	queue.pop();
	return value;
}

template <typename T, typename Executor>
class AsyncChannel {
	using executor_type = Executor;

public:
	explicit AsyncChannel(const executor_type& ex, size_t maxSize)
		: tmChanFull_(ex)
		, tmChanRecv_(ex)
		, maxSize_(maxSize)
		, close_(false) {
	}

	template <class CompletionToken>
	auto Send(T&& value, CompletionToken&& token) {
		using Signature = void(ErrorCode);
		return asio::async_initiate<CompletionToken, Signature>(
			[this, value = std::forward<T>(value)](auto&& handler) mutable {
				if (queue_.size() < maxSize_) {
					queue_.push(std::forward<T>(value));
					std::move(handler)({});
					return;
				}
				tmChanFull_.expires_at(asio::steady_timer::time_point::max());
				tmChanFull_.async_wait(
					[handler = std::forward<decltype(handler)>(handler), value = std::forward<T>(value), this](ErrorCode ec) mutable {
						if (close_) {
							std::move(handler)(ec);
							return;
						}
						bool signalRecv = queue_.empty();
						queue_.push(std::forward<T>(value));
						std::move(handler)({});
						if (signalRecv) {
							tmChanRecv_.cancel();
						}
					});
			},
			token);
	}

	template <class CompletionToken>
	auto Recv(CompletionToken&& token) {
		using Signature = void(ErrorCode, T);
		return asio::async_initiate<CompletionToken, Signature>(
			[this](auto&& handler) mutable {
				if (!queue_.empty()) {
					auto value = PopValue(queue_);
					std::move(handler)({}, std::move(value));
					return;
				}
				tmChanRecv_.expires_at(asio::steady_timer::time_point::max());
				tmChanRecv_.async_wait(
					[handler = std::forward<decltype(handler)>(handler), this](ErrorCode ec) mutable {
						if (close_) {
							std::move(handler)(ec, {});
							return;
						}
						bool signalSend = queue_.size() == maxSize_;
						auto value		= PopValue(queue_);
						std::move(handler)({}, std::move(value));
						if (signalSend) {
							tmChanFull_.cancel();
						}
					});
			},
			token);
	}

	void Close() {
		asio::dispatch(exec_, [this] {
			close_.exchange(true);
			tmChanFull_.cancel();
			tmChanRecv_.cancel();
			queue_.swap({});
		});
	}

private:
	asio::steady_timer tmChanFull_;
	asio::steady_timer tmChanRecv_;
	executor_type	   exec_;
	std::queue<T>	   queue_;
	size_t			   maxSize_;
	std::atomic_bool   close_;
};

template <typename T, typename Executor>
class MPSCChannel {
public:
	explicit MPSCChannel(size_t maxSize)
		: maxSize_(maxSize)
		, close_(false) {
	}

	Result<void> Send(T&& value) {
		if (close_) {
			return MAKE_EC(boost::system::errc::make_error_code(boost::system::errc::operation_canceled));
		}
		std::unique_lock lk(mutex_);
		if (queue_.size() >= maxSize_) {
			cvFul_.wait(lk, [this] { return queue_ < maxSize_; });
		}
		bool bEmpty = queue_.empty();
		queue_.push(std::forward<T>(value));
		if (bEmpty) {
			cvEmpty_.notify_one();
		}
		return {};
	}

	Result<T> Recv() {
		if (close_) {
			return MAKE_EC(boost::system::errc::make_error_code(boost::system::errc::operation_canceled));
		}
		std::unique_lock lk(mutex_);
		if (queue_.empty()) {
			cvEmpty_.wait(lk, [this] { return !queue_.empty(); });
		}
		bool bFull = queue_.size() >= maxSize_;
		auto ret = PopValue(queue_);
		if (bFull) {
			cvFul_.notify_all();
		}
		return ret;
	}

	void Close() {
		close_.exchange(true);
	}

private:
	std::mutex				mutex_;
	std::condition_variable cvFul_;
	std::condition_variable cvEmpty_;
	std::queue<T>			queue_;
	size_t					maxSize_;
	std::atomic_bool		close_;
};
} //namespace misc