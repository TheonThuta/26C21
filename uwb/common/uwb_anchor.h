#pragma once
#include "uwb_frame.h"

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
