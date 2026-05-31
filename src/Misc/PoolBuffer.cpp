#include "PoolBuffer.h"
#include <stdexcept>

namespace misc {
PoolBuffer::PoolBuffer()
	: m_buffer(MemPool::malloc()) {
}

PoolBuffer::~PoolBuffer() noexcept {
	if (m_buffer != nullptr) {
		MemPool::free(m_buffer);
	}
}

PoolBuffer::PoolBuffer(PoolBuffer&& rhs) noexcept {
	*this = std::move(rhs);
}

PoolBuffer& PoolBuffer::operator=(PoolBuffer&& rhs) noexcept {
	m_buffer	 = rhs.m_buffer;
	rhs.m_buffer = nullptr;
	return *this;
}

void* PoolBuffer::Ptr() noexcept {
	return m_buffer;
}

const void* PoolBuffer::Ptr() const noexcept {
	return m_buffer;
}

uint8_t& PoolBuffer::operator[](size_t index) {
	if (index >= MEM_POOL_CHUNK) {
		throw std::length_error("[misc::Data] index excced boundary");
	}
	return ((uint8_t*)m_buffer)[index];
}

const uint8_t& PoolBuffer::operator[](size_t index) const {
	if (index >= MEM_POOL_CHUNK) {
		throw std::length_error("[misc::Data] index excced boundary");
	}
	return ((uint8_t*)m_buffer)[index];
}

} //namespace misc