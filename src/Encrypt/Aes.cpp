#include "Aes.h"
#include "SSLErr.h"
#include <limits>

using namespace boost::system;

AES::AES(std::string key, std::string iv)
	: key_(std::move(key))
	, iv_(std::move(iv)) {
}

Result<gsl::span<uint8_t>> AES::Encrypt(gsl::span<const uint8_t> input, gsl::span<uint8_t> output) {
	if (ctx_.Ctx() == nullptr) {
		return MAKE_EC(sslerr::MakeEc(sslerr::errc::invalid_context));
	}

	const EVP_CIPHER* cipher	= EVP_aes_256_cbc();
	const size_t	  required	= GetRequireBufferSize(cipher, input.size());

	if (output.size() < required) {
		return MAKE_EC(sslerr::MakeEc(sslerr::errc::buffer_too_small));
	}
	if (input.size() > static_cast<size_t>(std::numeric_limits<int>::max())) {
		return MAKE_EC(errc::make_error_code(errc::value_too_large));
	}

	const int inputSize = static_cast<int>(input.size());
	int		  len		= 0;
	int		  total		= 0;
	if (EVP_EncryptInit_ex(ctx_.Ctx(), cipher, nullptr, (uint8_t*)key_.c_str(), (uint8_t*)iv_.c_str()) != 1) {
		return MAKE_EC(sslerr::ConsumeError());
	}
	if (EVP_EncryptUpdate(ctx_.Ctx(), output.data(), &len, input.data(), inputSize) != 1) {
		return MAKE_EC(sslerr::ConsumeError());
	}
	total += len;

	if (EVP_EncryptFinal_ex(ctx_.Ctx(), output.data() + total, &len) != 1) {
		return MAKE_EC(sslerr::ConsumeError());
	}
	total += len;
	return output.subspan(0, total);
}

Result<gsl::span<uint8_t>> AES::Decrypt(gsl::span<const uint8_t> input, gsl::span<uint8_t> output) {
	if (ctx_.Ctx() == nullptr) {
		return MAKE_EC(sslerr::MakeEc(sslerr::errc::invalid_context));
	}

	const EVP_CIPHER* cipher = EVP_aes_256_cbc();
	if (output.size() < input.size()) {
		return MAKE_EC(sslerr::MakeEc(sslerr::errc::buffer_too_small));
	}
	if (input.size() > static_cast<size_t>(std::numeric_limits<int>::max())) {
		return MAKE_EC(errc::make_error_code(errc::value_too_large));
	}

	const int inputSize = static_cast<int>(input.size());
	int		  len		= 0;
	int		  total		= 0;
	if (EVP_DecryptInit_ex(ctx_.Ctx(), cipher, nullptr, (uint8_t*)key_.c_str(), (uint8_t*)iv_.c_str()) != 1) {
		return MAKE_EC(sslerr::ConsumeError());
	}
	if (EVP_DecryptUpdate(ctx_.Ctx(), output.data(), &len, input.data(), inputSize) != 1) {
		return MAKE_EC(sslerr::ConsumeError());
	}
	total += len;

	if (EVP_DecryptFinal_ex(ctx_.Ctx(), output.data() + total, &len) != 1) {
		return MAKE_EC(sslerr::ConsumeError());
	}
	total += len;
	return output.subspan(0, total);
}

size_t AES::GetRequireBufferSize(const EVP_CIPHER* cipher, size_t original) noexcept {
	const int	 blockSize = EVP_CIPHER_block_size(cipher);
	const size_t required  = ((original / blockSize) + 1) * blockSize;
	return required;
}

size_t AES::GetRequireBufferSize(size_t original) noexcept {
	auto cipher = EVP_aes_256_cbc();
	return GetRequireBufferSize(cipher, original);
}
