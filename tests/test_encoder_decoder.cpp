/* SPDX-License-Identifier: MIT */
#include <cassert>
#include <initializer_list>
#include <iostream>
#include "../src/encoder_decoder.h"

static int feed(primeorb_encoder_decoder &d, std::initializer_list<uint8_t> states) {
    int result = 0;
    for (auto state : states) { result += primeorb_encoder_update(&d, state); }
    return result;
}

int main() {
    primeorb_encoder_decoder d;
    primeorb_encoder_reset(&d, 3);
    // EC12E2440301: 24 pulses / detents, four Gray transitions each.
    int steps = 0;
    for (int i = 0; i < 24; ++i) { steps += feed(d, {2, 0, 1, 3}); }
    assert(steps == 24);
    for (int i = 0; i < 24; ++i) { assert(feed(d, {1, 0, 2, 3}) == -1); }
    // Slow or fast input is identical; there is no timing dependency.
    assert(feed(d, {2, 3, 2, 2, 0, 2, 0, 1, 0, 1, 3, 1, 3}) == 1);
    assert(feed(d, {1, 3, 1, 0, 1, 0, 2, 0, 2, 3, 2, 3}) == -1);
    assert(feed(d, {2, 0, 2, 3}) == 0); // partial turn and reversal
    assert(feed(d, {2, 3, 1, 3}) == 0); // detent chatter
    assert(feed(d, {2, 0, 1, 3, 1, 0, 2, 3}) == 0); // immediate true reversal
    assert(feed(d, {0, 1, 3}) == 0); // illegal diagonal, then re-sync
    assert(feed(d, {2, 0, 3}) == 0); // diagonal ending at detent
    assert(feed(d, {2, 0, 1, 3}) == 1);
    for (uint8_t start = 0; start < 3; ++start) {
        primeorb_encoder_reset(&d, start);
        assert(feed(d, {3}) == 0); // power-up halfway through a cycle
        assert(feed(d, {2, 0, 1, 3}) == 1);
    }
    // Exhaust all 4^8 input sequences. No output before a valid full path;
    // no emitted step away from the detent; accumulator stays bounded.
    for (unsigned sequence = 0; sequence < 65536; ++sequence) {
        primeorb_encoder_reset(&d, 3);
        unsigned bits = sequence;
        for (int i = 0; i < 8; ++i) {
            uint8_t state = bits & 3;
            bits >>= 2;
            int step = primeorb_encoder_update(&d, state);
            assert(step >= -1 && step <= 1);
            assert(step == 0 || state == 3);
            assert(d.quarter_steps >= -3 && d.quarter_steps <= 3);
        }
    }
    std::cout << "Encoder decoder: all checks passed\n";
}
