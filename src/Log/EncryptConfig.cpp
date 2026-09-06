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
	: filename_(std::move(filename))
	, key_(std::move(key))
	, iv_(misc::ToHex(GenIV()))
	, enable_(enabled) {
	filename_ = GenPath();
}

const std::string& EncryptionConfig::GetKey() const {
	return key_;
}

const std::string& EncryptionConfig::GetIV() const {
	return iv_;
}

const filename_t& EncryptionConfig::GetFileName() const {
	return filename_;
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
	std::tie(basename, ext) = details::file_helper::split_by_extension(filename_);
	return basename + SPDLOG_FILENAME_T(".") + ToFilenameString(iv_) + ext;
}

std::string EncryptionConfig::GenIV() {
	std::string iv(k_aes_cbc_iv_size, '\0');
	if (RAND_bytes(reinterpret_cast<unsigned char*>(&iv[0]), static_cast<int>(iv.size())) != 1) {
		throw spdlog_ex("failed to generate encryption iv");
	}
	return iv;
}

} //namespace spdlog