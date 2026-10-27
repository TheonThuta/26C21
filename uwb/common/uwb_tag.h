#pragma once
#include "uwb_ranging.h"

struct AnchorResult {
    double distance = 0.0;
    bool valid = false;
};

struct RoundTripResult {
    AnchorResult anchors[kAnchorCount];
};

inline std::uint8_t countValidAnchors(const RoundTripResult& r) {
    std::uint8_t count = 0;
    for (std::uint8_t i = 0; i < kAnchorCount; ++i) {
        if (r.anchors[i].valid) {
            ++count;
        }
    }
    return count;
}

enum class TagState : std::uint8_t {
    Idle = 1,
    WaitingForResponse = 2,
    WaitingForReport = 3
};

struct TagSituation {
    TagState state = TagState::Idle;
    std::uint8_t tagId = kUnsetId;
    std::uint8_t anchorId = kUnsetId;
    std::uint8_t sequence = 0;
    std::uint8_t attempts = 0;
    RangingTimestamps times = {};
};

inline bool beginAttempt(TagSituation& t, std::uint8_t anchorId, std::uint64_t pollTime) {
    if (t.state != TagState::Idle) {
        return false;
    }
    if (anchorId == kUnsetId) {
        return false;
    }
    if (t.attempts >= kMaxAttempts) {
        return false;
    }
    t.times = {};
    t.times.tagPoll = pollTime;
    t.anchorId = anchorId;
    t.sequence++;
    t.attempts++;
    t.state = TagState::WaitingForResponse;
    return true;
}

inline bool handleResponse(TagSituation& t, const FrameHeader& h, std::uint64_t rxTime) {
    if (!isKnownType(h)) {
        return false;
    } 
    if (!isAddressedTo(h, t.tagId)) {
        return false;
    } 
    if (h.type != MessageType::Response) {
        return false;
    } 
    if (t.state != TagState::WaitingForResponse) {
        return false;
    } 
    if (!matchesExchange(h, t.anchorId, t.sequence)) {
        return false;
    }
    t.times.tagResponse = rxTime;
    t.state = TagState::WaitingForReport;

    return true;
}

inline bool recordFinalSent(TagSituation& t, std::uint64_t txTime) {
    if (t.state != TagState::WaitingForReport) {
        return false;
    }
    t.times.tagFinal = txTime;
    return true;
}

inline bool handleReport(TagSituation& t, const std::uint8_t* frame,
                         std::size_t length, AnchorResult& result) {
    if (length != kReportFrameSize) {
        return false;                       // wrong size, don't read past the buffer
    }
    if (t.state != TagState::WaitingForReport) {
        return false;
    }

    FrameHeader h = readHeader(frame);      // safe now, we know at least 4 bytes exist
    if (!isKnownType(h)) {
        return false;
    }
    if (h.type != MessageType::Report) {
        return false;
    }
    if (!isAddressedTo(h, t.tagId)) {
        return false;
    }
    if (!matchesExchange(h, t.anchorId, t.sequence)) {
        return false;
    }

    t.times.anchorPoll     = unpackTimestamp(frame + kHeaderSize + 0 * kTimestampSize);
    t.times.anchorResponse = unpackTimestamp(frame + kHeaderSize + 1 * kTimestampSize);
    t.times.anchorFinal    = unpackTimestamp(frame + kHeaderSize + 2 * kTimestampSize);

    result.distance = ticksToMeters(timeOfFlightTicks(t.times));
    result.valid = true;
    t.state = TagState::Idle;               // exchange finished
    return true;
}