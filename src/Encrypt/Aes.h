#pragma once
#include <gsl/span>
#include <string>
#include <tuple>
#include "EVPCtx.h"
#include "Misc/NameHelper.h"
#include "Misc/Error.h"

class AES {
public:
    AES(std::string key, std::string iv);
    ~AES() noexcept = default;

    Result<gsl::span<uint8_t>> Encrypt(gsl::span<const uint8_t> input, gsl::span<uint8_t> output);
    Result<gsl::span<uint8_t>> Decrypt(gsl::span<const uint8_t> input, gsl::span<uint8_t> output);

    static size_t GetRequireBufferSize(const EVP_CIPHER* cipher, size_t original) noexcept;
    static size_t GetRequireBufferSize(size_t original) noexcept;

private:
    EVPCtx      ctx_;
    std::string key_;
    std::string iv_;
};
