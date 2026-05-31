// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <spdlog/details/null_mutex.h>
#include <spdlog/details/synchronous_factory.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <string>

namespace spdlog {
namespace sinks {
/*
 * Trivial file sink with single file as target
 */
template <typename Mutex>
class FileSinkEx final : public basic_file_sink<Mutex> {
public:
	explicit FileSinkEx(
		const filename_t&		   filename,
		bool					   truncate		  = false,
		const file_event_handlers& event_handlers = {},
		bool					   encrypt_enable = false,
		std::string				   encryption_key = "");

protected:
	void sink_it_(const details::log_msg& msg) override;
	void flush_() override;

private:
	bool		encrypt_enable_;
	std::string encryption_key_;
};

using FileSinkExMt = FileSinkEx<std::mutex>;
using FileSinkExSt = FileSinkEx<details::null_mutex>;

} // namespace sinks

//
// factory functions
//
template <typename Factory = spdlog::synchronous_factory>
inline std::shared_ptr<logger> basic_file_logger_ex_mt(
	const std::string&		   logger_name,
	const filename_t&		   filename,
	bool					   truncate		  = false,
	const file_event_handlers& event_handlers = {},
	bool					   encrypt_enable = false,
	std::string				   encryption_key = "") {
	return Factory::template create<sinks::FileSinkExMt>(logger_name, filename, truncate,
		event_handlers, encrypt_enable, std::move(encryption_key));
}

template <typename Factory = spdlog::synchronous_factory>
inline std::shared_ptr<logger> basic_file_logger_ex_st(
	const std::string&		   logger_name,
	const filename_t&		   filename,
	bool					   truncate		  = false,
	const file_event_handlers& event_handlers = {},
	bool					   encrypt_enable = false,
	std::string				   encryption_key = "") {
	return Factory::template create<sinks::FileSinkExSt>(logger_name, filename, truncate,
		event_handlers, encrypt_enable, std::move(encryption_key));
}

} // namespace spdlog
