#pragma once
#include <openssl/evp.h>
#include "Misc/Macro.h"

class EVPCtx {
public:
    EVPCtx() noexcept;
    ~EVPCtx() noexcept;

    DEFAULT_COPY_CTOR(EVPCtx);
    DEFAULT_MOVE_CTOR(EVPCtx);

    EVP_CIPHER_CTX* Ctx() noexcept;

private:
	EVP_CIPHER_CTX* ctx_ = nullptr;
};
