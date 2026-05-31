#pragma once
#include "ConstExpr.h"
#include <boost/pool/singleton_pool.hpp>
#include <cstdint>

namespace misc {
struct PoolTag_ {};

using MemPool = boost::singleton_pool<PoolTag_, MEM_POOL_CHUNK>;

class PoolBuffer {
public:
    PoolBuffer();
    ~PoolBuffer() noexcept;

    PoolBuffer(const PoolBuffer&)            = delete;
    PoolBuffer& operator=(const PoolBuffer&) = delete;

    PoolBuffer(PoolBuffer&& rhs) noexcept;
    PoolBuffer& operator=(PoolBuffer&& rhs) noexcept;

    void*       Ptr() noexcept;
    const void* Ptr() const noexcept;

    uint8_t&       operator[](size_t index);
    const uint8_t& operator[](size_t index) const;

private:
    void* m_buffer = nullptr;
};
}  // namespace misc
