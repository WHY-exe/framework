#pragma once
#include "Macro.h"

#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/thread.hpp>


namespace misc {
class CtxThread {
	using Executor	   = boost::asio::io_context::executor_type;
	using Context	   = boost::asio::io_context;
	using WorkGuard	   = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
	using WorkGuardPtr = std::unique_ptr<WorkGuard>;
	using Token		   = std::function<void(std::exception_ptr)>;

public:
	CtxThread();
	~CtxThread() noexcept;
	// delete copy and move constructor and assign operator
	DEL_COPY_CTOR(CtxThread);
	DEL_MOVE_CTOR(CtxThread);
	// run the io_context on internal thread, store the result in future
	bool Start(Token token);
	// stop the io_context and wait for thread to join
	bool Stop() noexcept;
	// tell whether the io_context is stopped
	bool IsStop() const noexcept;

	inline operator Context&() {
		return m_ioCtx;
	}

	inline Executor GetExecutor() {
		return m_ioCtx.get_executor();
	}

private:
	std::atomic_bool m_bStop;
	boost::thread	 m_thread;
	Context			 m_ioCtx;
	WorkGuardPtr	 m_ctxGuard;
};
} //namespace misc
