#pragma once
#include <cstdint>
#include <cstddef> // needed for std::size_t

enum class MessageType : std::uint8_t {
    Poll = 1,
    Response = 2,
    Final = 3,
    Report = 4
};

//frame layout : type, source, destination, sequence
constexpr std::size_t kTypeIndex = 0;
constexpr std::size_t kSourceIndex = 1;
constexpr std::size_t kDestinationIndex = 2;
constexpr std::size_t kSequenceIndex = 3;

//header, timestamp, and report sizes in bytes
constexpr std::size_t kHeaderSize = 4;
constexpr std::size_t kTimestampSize = 5;
constexpr std::size_t kReportPayloadSize = kTimestampSize * 3;

//report frame size
constexpr std::size_t kReportFrameSize = kHeaderSize + kReportPayloadSize;

//anchor count and retry limit
constexpr std::uint8_t kAnchorCount = 5;
constexpr std::uint8_t kMaxAttempts = 2;

constexpr std::uint8_t kUnsetId = 0;