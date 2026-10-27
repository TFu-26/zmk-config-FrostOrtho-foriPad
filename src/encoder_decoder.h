/* SPDX-License-Identifier: MIT */
#pragma once
#include <stdbool.h>
#include <stdint.h>

/* A is bit 1, B is bit 0. Match the upstream EC11 direction convention.
 * Emit only a complete Gray-code cycle ending at the open/open detent.
 * Contact bounce retraces and cancels; a skipped state discards the partial
 * cycle. No time-based lockout, so an intentional reversal remains immediate.
 */
struct primeorb_encoder_decoder {
    uint8_t previous;
    int8_t quarter_steps;
    bool synchronized;
};

static inline void primeorb_encoder_reset(struct primeorb_encoder_decoder *d, uint8_t ab) {
    d->previous = ab;
    d->quarter_steps = 0;
    d->synchronized = (ab == 3);
}

static inline int primeorb_encoder_update(struct primeorb_encoder_decoder *d, uint8_t ab) {
    static const int8_t delta[16] = {
        0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0
    };
    const uint8_t previous = d->previous;
    d->previous = ab;
    if ((previous ^ ab) == 3) {
        d->quarter_steps = 0;
        d->synchronized = false;
    } else if (d->synchronized) {
        d->quarter_steps += delta[(previous << 2) | ab];
    }
    if (ab != 3) {
        return 0;
    }
    int step = d->synchronized ? d->quarter_steps / 4 : 0;
    d->quarter_steps = 0;
    d->synchronized = true;
    return step;
}
