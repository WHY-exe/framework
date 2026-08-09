#pragma once
#include <openssl/evp.h>

class EVPCtx {
public:
    EVPCtx() noexcept;
    ~EVPCtx() noexcept;

    EVP_CIPHER_CTX* Ctx() noexcept;


private:
	EVP_CIPHER_CTX* ctx_ = nullptr;
};
