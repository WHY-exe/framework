#pragma once
#include <spdlog/common.h>

namespace spdlog {
class EncryptionConfig {
	static constexpr size_t kAesCbcIvSize = 16;

public:
	static EncryptionConfig Make(std::string key, filename_t filename, bool enabled);

	const std::string& GetIV() const;
	const std::string& GetKey() const;
	const filename_t&  GetFileName() const;

	operator bool() const noexcept {
		return m_enable;
	}

private:
	EncryptionConfig(std::string key, filename_t filename, bool enabled);
	static filename_t  ToFilenameString(const std::string& value);
	static std::string GenIV();

	std::string GenPath();

private:
	filename_t	m_filename;
	std::string m_iv;
	std::string m_key;
	bool		m_enable = false;
};
} //namespace spdlog
