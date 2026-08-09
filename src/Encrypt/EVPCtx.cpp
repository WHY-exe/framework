#include "EVPCtx.h"

EVPCtx::EVPCtx() noexcept {
	if (ctx_ == nullptr) {
		ctx_ = EVP_CIPHER_CTX_new();
	}
}

EVPCtx::~EVPCtx() noexcept {
	if (ctx_ != nullptr) {
		EVP_CIPHER_CTX_free(ctx_);
	}
}

EVP_CIPHER_CTX* EVPCtx::Ctx() noexcept {
	return ctx_;
}
