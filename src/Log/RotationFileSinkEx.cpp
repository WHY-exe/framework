// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
#include "RotationFileSinkEx.h"

#include <spdlog/common.h>
#include <spdlog/fmt/fmt.h>

#include <ctime>
#include <string>

template class SPDLOG_API spdlog::sinks::RotationFileSinkEx<std::mutex>;
template class SPDLOG_API spdlog::sinks::RotationFileSinkEx<spdlog::details::null_mutex>;

namespace spdlog {
namespace sinks {

template <typename Mutex>
SPDLOG_INLINE RotationFileSinkEx<Mutex>::RotationFileSinkEx(
	filename_t				   base_filename,
	std::size_t				   max_size,
	std::size_t				   max_files,
	bool					   rotate_on_open,
	const file_event_handlers& event_handlers,
	bool					   encrypt_enable,
	std::string				   encryption_key)
	: rotating_file_sink<Mutex>(base_filename, max_size, max_files, rotate_on_open, event_handlers) {
	if (encrypt_enable && encryption_key.empty()) {
		throw std::logic_error("encrypt key not set");
	}
	if (encrypt_enable) {
		aes_ = std::make_unique<AES>(encryption_key, encryption_key);
	}
}

template <typename Mutex>
SPDLOG_INLINE void RotationFileSinkEx<Mutex>::sink_it_(const details::log_msg& msg) {
	memory_buf_t formatted;
	base_sink<Mutex>::formatter_->format(msg, formatted);
	
	if (aes_ != nullptr) {
		auto inputSize = formatted.size();
		formatted.resize(AES::GetRequireBufferSize(inputSize));
		auto [ret, ec] = aes_->Encrypt(
			gsl::span<const uint8_t>((uint8_t*)formatted.data(), inputSize), gsl::span<uint8_t>((uint8_t*)formatted.data(), formatted.size()));
		if (ec) {
			return;
		}
		formatted.resize(ret.size());
	}
	auto new_size = rotating_file_sink<Mutex>::get_current_size() + formatted.size();
	rotating_file_sink<Mutex>::to_file(std::move(formatted), new_size);
}

template <typename Mutex>
SPDLOG_INLINE void RotationFileSinkEx<Mutex>::flush_() {
	rotating_file_sink<Mutex>::flush_();
}

} // namespace sinks
} // namespace spdlog
