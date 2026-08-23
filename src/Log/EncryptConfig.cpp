#include "EncryptConfig.h"
#include <openssl/rand.h>
#include <spdlog/details/file_helper.h>
#include <spdlog/details/os.h>

#include "Misc/Util.h"

namespace spdlog {

EncryptionConfig EncryptionConfig::Make(std::string key, filename_t filename, bool enabled) {
	return EncryptionConfig(std::move(key), std::move(filename), enabled);
}

EncryptionConfig::EncryptionConfig(std::string key, filename_t filename, bool enabled)
	: m_filename(std::move(filename))
	, m_key(std::move(key))
	, m_iv(misc::ToHex(GenIV()))
	, m_enable(enabled) {
	m_filename = GenPath();
}

const std::string& EncryptionConfig::GetKey() const {
	return m_key;
}

const std::string& EncryptionConfig::GetIV() const {
	return m_iv;
}

const filename_t& EncryptionConfig::GetFileName() const {
	return m_filename;
}

filename_t EncryptionConfig::ToFilenameString(const std::string& value) {
#ifdef  SPDLOG_WCHAR_FILENAMES
    return filename_t(value.begin(), value.end());
#else
    return value;
#endif
}


filename_t EncryptionConfig::GenPath() {
	filename_t basename;
	filename_t ext;
	std::tie(basename, ext) = details::file_helper::split_by_extension(m_filename);
	return basename + SPDLOG_FILENAME_T(".") + ToFilenameString(m_iv) + ext;
}

std::string EncryptionConfig::GenIV() {
	std::string iv(kAesCbcIvSize, '\0');
	if (RAND_bytes(reinterpret_cast<unsigned char*>(&iv[0]), static_cast<int>(iv.size())) != 1) {
		throw spdlog_ex("failed to generate encryption iv");
	}
	return iv;
}

} //namespace spdlog