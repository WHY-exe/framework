#include "CtxThread.h"

namespace misc {
CtxThread::CtxThread()
	: m_bStop(true)
	, m_thread()
	, m_ioCtx()
	, m_ctxGuard(std::make_unique<WorkGuard>(m_ioCtx.get_executor())) {
}

CtxThread::~CtxThread() noexcept {
	Stop();
}

bool CtxThread::Start(Token token) {
	if (!m_bStop.exchange(false)) {
		return false;
	}
	if (m_thread.joinable()) {
		m_thread.join();
	}
	if (m_ioCtx.stopped()) {
		m_ioCtx.restart();
	}
	if (!m_ctxGuard) {
		m_ctxGuard = std::make_unique<WorkGuard>(m_ioCtx.get_executor());
	}

	m_thread = boost::thread(
		[this](Token token) {
			try {
				m_ioCtx.run();
				if (token) {
					token(nullptr);
				}
			} catch (...) {
				if (token) {
					token(std::current_exception());
				}
			}
			m_bStop = true;
		},
		std::move(token));
	return true;
}

bool CtxThread::IsStop() const noexcept {
	return m_bStop;
}

bool CtxThread::Stop() noexcept {
	if (m_bStop.exchange(true)) {
		return false;
	}
	try {
		m_ctxGuard->reset();
		if (m_thread.joinable()) {
			m_thread.join();
		}
	} catch (...) {
		return false;
	}
	return true;
}
} //namespace misc
