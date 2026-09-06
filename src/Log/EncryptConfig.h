#pragma once
#include <spdlog/common.h>

namespace spdlog {
class EncryptionConfig {
	static constexpr size_t k_aes_cbc_iv_size = 16;

public:
	static EncryptionConfig Make(std::string key, filename_t filename, bool enabled);

	const std::string& GetIV() const;
	const std::string& GetKey() const;
	const filename_t&  GetFileName() const;

	explicit operator bool() const noexcept {
		return enable_;
	}

private:
	EncryptionConfig(std::string key, filename_t filename, bool enabled);
	static filename_t  ToFilenameString(const std::string& value);
	static std::string GenIV();

	std::string GenPath();

private:
	filename_t	filename_;
	std::string iv_;
	std::string key_;
	bool		enable_ = false;
};
} //namespace spdlog
