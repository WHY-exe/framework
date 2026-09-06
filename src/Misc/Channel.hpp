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
		: m_tmChanFull(ex)
		, m_tmChanRecv(ex)
		, m_maxSize(maxSize)
		, m_close(false) {
	}

	template <class CompletionToken>
	auto Send(T&& value, CompletionToken&& token) {
		using Signature = void(ErrorCode);
		return asio::async_initiate<CompletionToken, Signature>(
			[this, value = std::forward<T>(value)](auto&& handler) mutable {
				if (m_queue.size() < m_maxSize) {
					m_queue.push(std::forward<T>(value));
					std::move(handler)({});
					return;
				}
				m_tmChanFull.expires_at(asio::steady_timer::time_point::max());
				m_tmChanFull.async_wait(
					[handler = std::forward<decltype(handler)>(handler), value = std::forward<T>(value), this](ErrorCode ec) mutable {
						if (m_close) {
							std::move(handler)(ec);
							return;
						}
						bool signalRecv = m_queue.empty();
						m_queue.push(std::forward<T>(value));
						std::move(handler)({});
						if (signalRecv) {
							m_tmChanRecv.cancel();
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
				if (!m_queue.empty()) {
					auto value = PopValue(m_queue);
					std::move(handler)({}, std::move(value));
					return;
				}
				m_tmChanRecv.expires_at(asio::steady_timer::time_point::max());
				m_tmChanRecv.async_wait(
					[handler = std::forward<decltype(handler)>(handler), this](ErrorCode ec) mutable {
						if (m_close) {
							std::move(handler)(ec, {});
							return;
						}
						bool signalSend = m_queue.size() == m_maxSize;
						auto value		= PopValue(m_queue);
						std::move(handler)({}, std::move(value));
						if (signalSend) {
							m_tmChanFull.cancel();
						}
					});
			},
			token);
	}

	void Close() {
		asio::dispatch(m_exec, [this] {
			m_close.exchange(true);
			m_tmChanFull.cancel();
			m_tmChanRecv.cancel();
			m_queue.swap({});
		});
	}

private:
	asio::steady_timer m_tmChanFull;
	asio::steady_timer m_tmChanRecv;
	executor_type	   m_exec;
	std::queue<T>	   m_queue;
	size_t			   m_maxSize;
	std::atomic_bool   m_close;
};

template <typename T, typename Executor>
class MPSCChannel {
public:
	explicit MPSCChannel(size_t maxSize)
		: m_maxSize(maxSize)
		, m_close(false) {
	}

	Result<void> Send(T&& value) {
		if (m_close) {
			return MAKE_EC(boost::system::errc::make_error_code(boost::system::errc::operation_canceled));
		}
		std::unique_lock lk(m_mutex);
		if (m_queue.size() >= m_maxSize) {
			m_cvFul.wait(lk, [this] { return m_queue < m_maxSize; });
		}
		bool bEmpty = m_queue.empty();
		m_queue.push(std::forward<T>(value));
		if (bEmpty) {
			m_cvEmpty.notify_one();
		}
		return {};
	}

	Result<T> Recv() {
		if (m_close) {
			return MAKE_EC(boost::system::errc::make_error_code(boost::system::errc::operation_canceled));
		}
		std::unique_lock lk(m_mutex);
		if (m_queue.empty()) {
			m_cvEmpty.wait(lk, [this] { return !m_queue.empty(); });
		}
		bool bFull = m_queue.size() >= m_maxSize;
		auto ret = PopValue(m_queue);
		if (bFull) {
			m_cvFul.notify_all();
		}
		return ret;
	}

	void Close() {
		m_close.exchange(true);
	}

private:
	std::mutex				m_mutex;
	std::condition_variable m_cvFul;
	std::condition_variable m_cvEmpty;
	std::queue<T>			m_queue;
	size_t					m_maxSize;
	std::atomic_bool		m_close;
};
} //namespace misc