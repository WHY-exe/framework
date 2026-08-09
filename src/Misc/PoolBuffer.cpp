#include "PoolBuffer.h"
#include <stdexcept>

namespace misc {
PoolBuffer::PoolBuffer()
	: buffer_(MemPool::malloc()) {
}

PoolBuffer::~PoolBuffer() noexcept {
	if (buffer_ != nullptr) {
		MemPool::free(buffer_);
	}
}

PoolBuffer::PoolBuffer(PoolBuffer&& rhs) noexcept {
	*this = std::move(rhs);
}

PoolBuffer& PoolBuffer::operator=(PoolBuffer&& rhs) noexcept {
	buffer_	 = rhs.buffer_;
	rhs.buffer_ = nullptr;
	return *this;
}

void* PoolBuffer::Ptr() noexcept {
	return buffer_;
}

const void* PoolBuffer::Ptr() const noexcept {
	return buffer_;
}

uint8_t& PoolBuffer::operator[](size_t index) {
	if (index >= MEM_POOL_CHUNK) {
		throw std::length_error("[misc::PoolBuffer] index excced boundary");
	}
	return ((uint8_t*)buffer_)[index];
}

const uint8_t& PoolBuffer::operator[](size_t index) const {
	if (index >= MEM_POOL_CHUNK) {
		throw std::length_error("[misc::PoolBuffer] index excced boundary");
	}
	return ((uint8_t*)buffer_)[index];
}

} //namespace misc