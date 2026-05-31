// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <spdlog/sinks/rotating_file_sink.h>
#include <mutex>
#include <string>

namespace spdlog {
namespace sinks {
//
// Rotating file sink based on size
//
template <typename Mutex>
class RotationFileSinkEx final : public rotating_file_sink<Mutex> {
public:
	RotationFileSinkEx(
		filename_t				   base_filename,
		std::size_t				   max_size,
		std::size_t				   max_files,
		bool					   rotate_on_open = false,
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

using RotationFileSinkExMt = RotationFileSinkEx<std::mutex>;
using RotationFileSinkExSt = RotationFileSinkEx<details::null_mutex>;

} // namespace sinks

//
// factory functions
//

template <typename Factory = spdlog::synchronous_factory>
inline std::shared_ptr<logger> rotating_logger_ex_mt(
	const std::string&		   logger_name,
	const filename_t&		   filename,
	size_t					   max_file_size,
	size_t					   max_files,
	bool					   rotate_on_open = false,
	const file_event_handlers& event_handlers = {},
	bool					   encrypt_enable = false,
	std::string				   encryption_key = "") {
	return Factory::template create<sinks::RotationFileSinkExMt>(
		logger_name, filename, max_file_size, max_files, rotate_on_open, event_handlers, encrypt_enable, std::move(encryption_key));
}

template <typename Factory = spdlog::synchronous_factory>
inline std::shared_ptr<logger> rotating_logger_ex_st(
	const std::string&		   logger_name,
	const filename_t&		   filename,
	size_t					   max_file_size,
	size_t					   max_files,
	bool					   rotate_on_open = false,
	const file_event_handlers& event_handlers = {},
	bool					   encrypt_enable = false,
	std::string				   encryption_key = "") {
	return Factory::template create<sinks::RotationFileSinkExSt>(
		logger_name, filename, max_file_size, max_files, rotate_on_open, event_handlers, encrypt_enable, std::move(encryption_key));
}
} // namespace spdlog
