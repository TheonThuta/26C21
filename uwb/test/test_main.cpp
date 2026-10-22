#include "uwb_common.h"
#include "uwb_timestamp.h"
#include "uwb_ranging.h"
#include "uwb_frame.h"
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
    cout<< "Poll" << static_cast<int>(MessageType::Poll)<< endl;
    cout<< "Response" << static_cast<int>(MessageType::Response)<< endl;
    cout<< "Final" << static_cast<int>(MessageType::Final)<< endl;
    cout<< "Report" << static_cast<int>(MessageType::Report)<< endl;

    checkRoundTrip(0x0102030405, 0x0102030405);
    checkRoundTrip(0, 0);
    checkRoundTrip(0xFFFFFFFFFF, 0xFFFFFFFFFF);
    checkRoundTrip(0x1FFFFFFFFFF, 0xFFFFFFFFFF);   // 41-bit input, top bit dropped

    checkDiff(0x1000, 0x0800, 0x800);
    checkDiff(0x5, 0xFFFFFFFFFB, 10);
    checkDiff(0x300000000, 0x100000000, 0x200000000);

    RangingTimestamps t = {1000, 4200, 10200, 2100, 5100, 11300};
    cout << round1(t) << endl;
    cout << reply1(t) << endl;
    cout << round2(t) << endl;
    cout << reply2(t) << endl;

    cout << timeOfFlightTicks(t) << endl;
    cout << ticksToMeters(timeOfFlightTicks(t)) << endl;

    std::uint8_t frame[kHeaderSize] = {};
    writeHeader(frame, MessageType::Poll, 9, 3, 7);

    FrameHeader h = readHeader(frame);
    cout << static_cast<int>(h.type) << " "
        << static_cast<int>(h.source) << " "
        << static_cast<int>(h.destination) << " "
        << static_cast<int>(h.sequence) << endl;

    cout << boolalpha;
    cout << isAddressedTo(h, 3) << endl;    // expect: true
    cout << isAddressedTo(h, 4) << endl;    // expect: false

    cout << isKnownType(h) << endl;          // h is the good poll from before, expect: true

    std::uint8_t bad[kHeaderSize] = {99, 9, 3, 7};
    FrameHeader badHeader = readHeader(bad);
    cout << isKnownType(badHeader) << endl;  // expect: false

    cout<<matchesExchange(h, 9, 7) << endl;
    cout<<matchesExchange(h, 9, 8) << endl;
    cout<<matchesExchange(h, 5, 7) << endl;
}

