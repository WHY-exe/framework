#pragma once
#include <gsl/span>
#include <string>
#include <tuple>
#include "EVPCtx.h"
#include "Misc/NameHelper.h"


class AES {
public:
    AES(std::string key, std::string iv);
    ~AES() noexcept = default;

    std::tuple<gsl::span<uint8_t>, misc::ErrorCode> Encrypt(gsl::span<const uint8_t> input, gsl::span<uint8_t> output);
    std::tuple<gsl::span<uint8_t>, misc::ErrorCode> Decrypt(gsl::span<const uint8_t> input, gsl::span<uint8_t> output);

    static size_t GetRequireBufferSize(const EVP_CIPHER* cipher, size_t original) noexcept;
    static size_t GetRequireBufferSize(size_t original) noexcept;

private:
    EVPCtx      ctx_;
    std::string key_;
    std::string iv_;
};
