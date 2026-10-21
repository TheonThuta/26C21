#pragma once
#include "uwb_timestamp.h"

struct RangingTimestamps {
    std::uint64_t tagPoll; // tag sent poll
    std::uint64_t tagResponse; // tag received response
    std::uint64_t tagFinal; // tag sent final
    std::uint64_t anchorPoll; // anchor received poll
    std::uint64_t anchorResponse; // anchor sent response
    std::uint64_t anchorFinal;  // anchor received final
};

// interval is time between two events measured on one devices clock
// reply interval is how long a device waited before answering
// round interval is how long the whole trip as seen by device that waited for the answer

// round 1 - poll sent, response received (tag)
inline std::uint64_t round1(const RangingTimestamps& ranging) {
    return timestampDiff(ranging.tagResponse, ranging.tagPoll);
}

// reply 1 - poll received, response sent (anchor)
inline std::uint64_t reply1(const RangingTimestamps& ranging){
    return timestampDiff(ranging.anchorResponse, ranging.anchorPoll);
}

// round 2 - response sent, final recevied (anchor)
inline std::uint64_t round2(const RangingTimestamps& ranging) {
    return timestampDiff(ranging.anchorFinal, ranging.anchorResponse);
}

// reply 2 - response received, final sent (tag)
inline std::uint64_t reply2(const RangingTimestamps& ranging){
    return timestampDiff(ranging.tagFinal, ranging.tagResponse);
}