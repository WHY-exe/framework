#include "CtxThread.h"

namespace misc {
CtxThread::CtxThread()
	: bStop_(true)
	, thread_()
	, ioCtx_()
	, ctxGuard_(std::make_unique<WorkGuard>(ioCtx_.get_executor())) {
}

CtxThread::~CtxThread() noexcept {
	Stop();
}

bool CtxThread::Start(Token token) {
	if (!bStop_.exchange(false)) {
		return false;
	}
	if (thread_.joinable()) {
		thread_.join();
	}
	if (ioCtx_.stopped()) {
		ioCtx_.restart();
	}
	if (!ctxGuard_) {
		ctxGuard_ = std::make_unique<WorkGuard>(ioCtx_.get_executor());
	}

	thread_ = boost::thread(
		[this](Token token) {
			try {
				ioCtx_.run();
				if (token) {
					token(nullptr);
				}
			} catch (...) {
				if (token) {
					token(std::current_exception());
				}
			}
			bStop_ = true;
		},
		std::move(token));
	return true;
}

bool CtxThread::IsStop() const noexcept {
	return bStop_;
}

bool CtxThread::Stop() noexcept {
	if (bStop_.exchange(true)) {
		return false;
	}
	try {
		ctxGuard_->reset();
		if (thread_.joinable()) {
			thread_.join();
		}
	} catch (...) {
		return false;
	}
	return true;
}
} //namespace misc
