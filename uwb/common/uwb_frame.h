#pragma once
#include <cstdint>
#include "uwb_common.h"

// constructs frame header
inline void writeHeader(std::uint8_t* out, MessageType type, std::uint8_t source, std::uint8_t destination, std::uint8_t sequence) {
    out[kTypeIndex] = static_cast<std::uint8_t>(type);
    out[kSourceIndex] = source;
    out[kDestinationIndex] = destination;
    out[kSequenceIndex] = sequence;
}

struct FrameHeader {
    MessageType type;
    std::uint8_t source;
    std::uint8_t destination;
    std::uint8_t sequence;
};

// reads frame header
inline FrameHeader readHeader(const std::uint8_t* in) {
    FrameHeader h;
    h.type = static_cast<MessageType>(in[kTypeIndex]);
    h.source = in[kSourceIndex];
    h.destination = in[kDestinationIndex];
    h.sequence = in[kSequenceIndex];
    return h;
}

// checks message address
inline bool isAddressedTo(const FrameHeader& h, std::uint8_t myId) {
    return h.destination == myId;
}

// checks message type
inline bool isKnownType(const FrameHeader& h) {
    return h.type == MessageType::Poll 
        || h.type == MessageType::Response
        || h.type == MessageType::Final
        || h.type == MessageType::Report;
}

// checks if message is from same session
inline bool matchesExchange(const FrameHeader& h, std::uint8_t expectedSender, std::uint8_t expectedTicket) {
    return h.source == expectedSender && h.sequence == expectedTicket;
}