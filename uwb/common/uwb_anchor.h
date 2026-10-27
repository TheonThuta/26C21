#pragma once
#include "uwb_frame.h"
#include "uwb_timestamp.h"

enum class AnchorState : std::uint8_t {
    Listening = 1,
    WaitingForFinal = 2
};

struct AnchorSituation {
    AnchorState state = AnchorState::Listening;
    std::uint8_t anchorId = kUnsetId;
    std::uint8_t tagId = kUnsetId;
    std::uint8_t sequence = 0;
    std::uint64_t pollReceived = 0;
    std::uint64_t responseSent = 0;
    std::uint64_t finalReceived = 0;
};

inline bool handlePoll(AnchorSituation& a, const FrameHeader& h, std::uint64_t rxTime) {
    if (!isKnownType(h)) {
        return false;
    } if (!isAddressedTo(h, a.anchorId)) {
        return false;
    } if (h.type != MessageType::Poll) {
        return false;
    }
    a.state = AnchorState::WaitingForFinal;
    a.tagId = h.source;
    a.sequence = h.sequence;
    a.pollReceived = rxTime;
    a.responseSent = 0;
    a.finalReceived = 0;

    return true;
}

inline bool handleFinal(AnchorSituation& a, const FrameHeader& h, std::uint64_t rxTime) {
    if (!isKnownType(h)) {
        return false;
    } if (!isAddressedTo(h, a.anchorId)) {
        return false;
    } if (h.type != MessageType::Final) {
        return false;
    } if (a.state != AnchorState::WaitingForFinal) {
        return false;
    } if (!matchesExchange(h, a.tagId, a.sequence)) {
        return false;
    }
    a.finalReceived = rxTime;
    a.state = AnchorState::Listening;

    return true;
}

inline void endExchange(AnchorSituation& a) {
    a.state = AnchorState::Listening;
    a.tagId = kUnsetId;
    a.sequence = 0;
    a.pollReceived = 0;
    a.responseSent = 0;
    a.finalReceived = 0;
}

inline bool recordResponseSent(AnchorSituation& a, std::uint64_t txTime) {
    if (a.state != AnchorState::WaitingForFinal) {
        return false;
    }
    a.responseSent = txTime;
    return true;
}

inline bool buildReport(const AnchorSituation& a, std::uint8_t* out) {
    if (a.state != AnchorState::Listening) {
        return false;
    }
    writeHeader(out, MessageType::Report, a.anchorId, a.tagId, a.sequence);
    packTimestamp(a.pollReceived, out + kHeaderSize + 0 * kTimestampSize);
    packTimestamp(a.responseSent, out + kHeaderSize + 1 * kTimestampSize);
    packTimestamp(a.finalReceived, out + kHeaderSize + 2 * kTimestampSize);

    return true;
}