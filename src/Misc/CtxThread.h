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
		return ioCtx_;
	}

	inline Executor GetExecutor() {
		return ioCtx_.get_executor();
	}

private:
	std::atomic_bool bStop_;
	boost::thread	 thread_;
	Context			 ioCtx_;
	WorkGuardPtr	 ctxGuard_;
};
} //namespace misc
