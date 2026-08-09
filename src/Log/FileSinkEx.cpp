#include "FileSinkEx.h"
#include <spdlog/common.h>
#include <spdlog/details/os.h>

template class SPDLOG_API spdlog::sinks::FileSinkEx<std::mutex>;
template class SPDLOG_API spdlog::sinks::FileSinkEx<spdlog::details::null_mutex>;

namespace spdlog {
namespace sinks {

template <typename Mutex>
SPDLOG_INLINE FileSinkEx<Mutex>::FileSinkEx(
	const filename_t&		   filename,
	bool					   truncate,
	const file_event_handlers& event_handlers,
	bool					   encrypt_enable,
	std::string				   encryption_key)
	: basic_file_sink<Mutex>(filename, truncate, event_handlers) {
	if (encrypt_enable && encryption_key.empty()) {
		throw std::logic_error("encrypt key not set");
	}
	if (encrypt_enable) {
		aes_ = std::make_unique<AES>(encryption_key, encryption_key);
	}
}

template <typename Mutex>
SPDLOG_INLINE void FileSinkEx<Mutex>::sink_it_(const details::log_msg& msg) {
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

	basic_file_sink<Mutex>::to_file(std::move(formatted));
}

template <typename Mutex>
SPDLOG_INLINE void FileSinkEx<Mutex>::flush_() {
	basic_file_sink<Mutex>::flush();
}

} // namespace sinks
} // namespace spdlog