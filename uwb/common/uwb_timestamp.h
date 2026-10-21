#pragma once
#include <cstdint>
#include <cstddef>
#include "uwb_common.h"

//byte order (little-endian), lowest byte in out[0]
inline void packTimestamp(std::uint64_t value, std::uint8_t * out){
    out[0] = static_cast<std::uint8_t>(value & 0xFF);
    out[1] = static_cast<std::uint8_t>((value >> 8) & 0xFF);
    out[2] = static_cast<std::uint8_t>((value >> 16) & 0xFF);
    out[3] = static_cast<std::uint8_t>((value >> 24) & 0xFF);
    out[4] = static_cast<std::uint8_t>((value >> 32) & 0xFF);
}

static_assert(kTimestampSize == 5, "packTimestamp writes exactly 5 bytes");

inline std::uint64_t unpackTimestamp(const std::uint8_t * in) {
    return static_cast<std::uint64_t>(in[0])
    | (static_cast<std::uint64_t>(in[1]) << 8)
    | (static_cast<std::uint64_t>(in[2]) << 16)
    | (static_cast<std::uint64_t>(in[3]) << 24)
    | (static_cast<std::uint64_t>(in[4]) << 32);
}

constexpr std::uint64_t kMask = 0xFFFFFFFFFF;

inline std::uint64_t timestampDiff(std::uint64_t later, std::uint64_t earlier) {
    return (later - earlier) & kMask;
}