// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
#include "RotationFileSinkEx.h"

#include <openssl/rand.h>
#include <spdlog/common.h>
#include <spdlog/details/file_helper.h>
#include <spdlog/fmt/fmt.h>

#include <ctime>
#include <string>
#include <utility>

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
	: RotationFileSinkEx(max_size, max_files, rotate_on_open, event_handlers,
		  EncryptionConfig::Make(std::move(encryption_key), std::move(base_filename), encrypt_enable)) {
}

template <typename Mutex>
SPDLOG_INLINE RotationFileSinkEx<Mutex>::RotationFileSinkEx(
	std::size_t				   max_size,
	std::size_t				   max_files,
	bool					   rotate_on_open,
	const file_event_handlers& event_handlers,
	EncryptionConfig		   enc_config)
	: rotating_file_sink<Mutex>(enc_config.GetFileName(), max_size, max_files, rotate_on_open, event_handlers) {
	if (enc_config) {
		aes_ = std::make_unique<AES>(enc_config.GetKey(), enc_config.GetIV());
	}
}

template <typename Mutex>
SPDLOG_INLINE void RotationFileSinkEx<Mutex>::sink_it_(const details::log_msg& msg) {
	memory_buf_t formatted;
	base_sink<Mutex>::formatter_->format(msg, formatted);

	memory_buf_t ouput_buf;
	if (aes_ != nullptr) {
		auto input_size = formatted.size();
		ouput_buf.resize(AES::GetRequireBufferSize(input_size));
		auto ret = aes_->Encrypt(
			gsl::span<const uint8_t>((uint8_t*)formatted.data(), input_size), gsl::span<uint8_t>((uint8_t*)ouput_buf.data(), ouput_buf.size()));
		if (!ret) {
			throw spdlog_ex("encryption failed");
		}
		ouput_buf.resize(ret->size());
	} else {
		ouput_buf = std::move(formatted);
	}
	auto new_size = rotating_file_sink<Mutex>::get_current_size() + formatted.size();
	rotating_file_sink<Mutex>::to_file(std::move(ouput_buf), new_size);
}

template <typename Mutex>
SPDLOG_INLINE void RotationFileSinkEx<Mutex>::flush_() {
	rotating_file_sink<Mutex>::flush_();
}

} // namespace sinks
} // namespace spdlog

template class SPDLOG_API spdlog::sinks::RotationFileSinkEx<std::mutex>;
template class SPDLOG_API spdlog::sinks::RotationFileSinkEx<spdlog::details::null_mutex>;
