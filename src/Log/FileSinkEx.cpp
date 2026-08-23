#include "FileSinkEx.h"
#include <openssl/rand.h>
#include <spdlog/common.h>
#include <spdlog/details/file_helper.h>
#include <spdlog/details/os.h>

#include <tuple>
#include <utility>

namespace spdlog {
namespace sinks {

template <typename Mutex>
SPDLOG_INLINE FileSinkEx<Mutex>::FileSinkEx(
	const filename_t&		   filename,
	bool					   truncate,
	const file_event_handlers& event_handlers,
	bool					   encrypt_enable,
	std::string				   encryption_key)
	: FileSinkEx(truncate, event_handlers, EncryptionConfig::Make(std::move(encryption_key), std::move(filename), encrypt_enable)) {
}

template <typename Mutex>
SPDLOG_INLINE FileSinkEx<Mutex>::FileSinkEx(
	bool					   truncate,
	const file_event_handlers& event_handlers,
	EncryptionConfig		   enc_config)
	: basic_file_sink<Mutex>(enc_config.GetFileName(), truncate, event_handlers) {
	if (enc_config) {
		aes_ = std::make_unique<AES>(enc_config.GetKey(), enc_config.GetIV());
	}
}

template <typename Mutex>
SPDLOG_INLINE void FileSinkEx<Mutex>::sink_it_(const details::log_msg& msg) {
	memory_buf_t formatted;
	base_sink<Mutex>::formatter_->format(msg, formatted);

	if (aes_ != nullptr) {
		auto inputSize = formatted.size();
		formatted.resize(AES::GetRequireBufferSize(inputSize));
		auto ret = aes_->Encrypt(
			gsl::span<const uint8_t>((uint8_t*)formatted.data(), inputSize), gsl::span<uint8_t>((uint8_t*)formatted.data(), formatted.size()));
		if (!ret) {
			throw spdlog_ex("encryption failed");
		}
		formatted.resize(ret->size());
	}

	basic_file_sink<Mutex>::to_file(std::move(formatted));
}

template <typename Mutex>
SPDLOG_INLINE void FileSinkEx<Mutex>::flush_() {
	basic_file_sink<Mutex>::flush();
}

} // namespace sinks
} // namespace spdlog

template class SPDLOG_API spdlog::sinks::FileSinkEx<std::mutex>;
template class SPDLOG_API spdlog::sinks::FileSinkEx<spdlog::details::null_mutex>;
