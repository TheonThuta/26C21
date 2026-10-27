#include "uwb_common.h"
#include "uwb_timestamp.h"
#include "uwb_ranging.h"
#include "uwb_frame.h"
#include "uwb_anchor.h"
#include "uwb_tag.h"
#include <iostream>
using namespace std;

void checkRoundTrip(std::uint64_t input, std::uint64_t expected) {
    std::uint8_t buf[kTimestampSize] = {};
    packTimestamp(input, buf);
    std::uint64_t result = unpackTimestamp(buf);

    if (result == expected) {
        cout << "PASS: " << hex << input << dec << endl;
    } else {
        cout << "FAIL: input " << hex << input
            << "expected " << expected
            << "got " << result << dec << endl;
    }
    }

void checkDiff(std::uint64_t later, std::uint64_t earlier, std::uint64_t expected) {
    std::uint64_t result = timestampDiff(later, earlier);

    if (result == expected) {
        cout << "PASS diff:" << dec << result << endl;
    } else {
        cout << "FAIL diff: expected "<< expected
            << " got " << result << endl;
    }
}

int main() {
    cout << boolalpha;
    std::uint8_t frame[kHeaderSize] = {};

    // anchor side: id 2 answers a poll from tag 9, sequence 1
    AnchorSituation anchor;
    anchor.anchorId = 2;
    writeHeader(frame, MessageType::Poll, 9, 2, 1);
    handlePoll(anchor, readHeader(frame), 2100);
    recordResponseSent(anchor, 5100);
    writeHeader(frame, MessageType::Final, 9, 2, 1);
    handleFinal(anchor, readHeader(frame), 11300);
    std::uint8_t report[kReportFrameSize] = {};
    buildReport(anchor, report);

    // tag side: same exchange
    TagSituation t;
    t.tagId = 9;
    beginAttempt(t, 2, 1000);                           // sequence becomes 1
    writeHeader(frame, MessageType::Response, 2, 9, 1);
    handleResponse(t, readHeader(frame), 4200);
    recordFinalSent(t, 10200);

    AnchorResult res;
    cout << handleReport(t, report, kReportFrameSize, res) << endl;   // expect: true
    cout << res.valid << " " << res.distance << endl;                 // expect: true 0.469...
    cout << static_cast<int>(t.state) << endl;                        // expect: 1 (idle again)
    cout << handleReport(t, report, kReportFrameSize, res) << endl;   // expect: false (already finished)
}

