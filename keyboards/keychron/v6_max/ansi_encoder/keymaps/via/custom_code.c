/* CUSTOM: Custom Code Implementation — Generic Key-Spam Toggle System
 *
 * Scalable auto-key system using a data-driven spammer array.
 * Each spammer is a non-blocking state machine that toggles on/off
 * and repeatedly taps a target key with randomized timing.
 *
 * To add a new spammer:
 *   1. Add a trigger keycode to the enum in custom_code.h (e.g. CC_ESPAM)
 *   2. Add a new entry to the spammers[] array below
 *   That's it — the generic loop handles everything else.
 *
 * State machine per spammer:
 *   SPAM_IDLE     — inactive, no keys being sent
 *   SPAM_PRESSING — target key held down, waiting for hold duration to expire
 *   SPAM_WAITING  — target key released, waiting for next cycle to begin
 */

#ifdef CUSTOM_CODE_ENABLE

#include QMK_KEYBOARD_H
#include "custom_code.h"
#include <stdlib.h>

/* CUSTOM: State machine states (shared by all spammers) */
enum spam_state {
    SPAM_IDLE,      // Not active
    SPAM_PRESSING,  // Target key held, waiting for hold_ms to expire
    SPAM_WAITING,   // Target key released, waiting for interval_ms to expire
};

/* CUSTOM: Generic spammer struct — config + runtime state in one */
typedef struct {
    /* Config (set once, don't change at runtime) */
    uint16_t trigger_keycode;    // Custom keycode that toggles this spammer
    uint16_t target_keycode;     // Key to spam (e.g. KC_Q)
    uint16_t hold_min;           // Min hold duration in ms
    uint16_t hold_max;           // Max hold duration in ms
    uint16_t interval_min;       // Min cycle interval in ms (press-to-press)
    uint16_t interval_max;       // Max cycle interval in ms
    /* Runtime (managed by generic code below) */
    uint8_t  state;              // Current spam_state
    uint16_t timer;              // Timestamp of current cycle start
    uint16_t hold_ms;            // Current randomized hold duration
    uint16_t interval_ms;        // Current randomized cycle interval
} key_spammer_t;

/* ========================================================================
 * SPAMMER CONFIGURATION — add new spammers here
 * ======================================================================== */
static key_spammer_t spammers[] = {
    /* CUSTOM: Q-Spam — press circle key to toggle auto-clicking 'q'
     * Hold: 50-100ms, Interval: 200-400ms */
    {
        .trigger_keycode = CC_QSPAM,
        .target_keycode  = KC_Q,
        .hold_min = 50,  .hold_max = 100,
        .interval_min = 200, .interval_max = 400,
        .state = SPAM_IDLE,
    },
    // Add more entries here, e.g.:
    // { .trigger_keycode = CC_ESPAM, .target_keycode = KC_E,
    //   .hold_min = 50, .hold_max = 100,
    //   .interval_min = 300, .interval_max = 500,
    //   .state = SPAM_IDLE },
};

#define SPAMMER_COUNT (sizeof(spammers) / sizeof(spammers[0]))

/* CUSTOM: Track whether rand() has been seeded */
static bool rand_seeded = false;

/* CUSTOM: Random value in [min, max] inclusive */
static uint16_t random_range(uint16_t min, uint16_t max) {
    return min + (rand() % (max - min + 1));
}

/* CUSTOM: Start a new press cycle for a spammer */
static void spammer_begin_press(key_spammer_t *s) {
    s->hold_ms     = random_range(s->hold_min, s->hold_max);
    s->interval_ms = random_range(s->interval_min, s->interval_max);
    s->timer       = timer_read();
    register_code(s->target_keycode);
    s->state       = SPAM_PRESSING;
}

/* CUSTOM: Stop a spammer, release key if held */
static void spammer_stop(key_spammer_t *s) {
    if (s->state == SPAM_PRESSING) {
        unregister_code(s->target_keycode);
    }
    s->state = SPAM_IDLE;
}

/* CUSTOM: Key event handler — iterates all spammers to find matching trigger */
bool process_record_custom(uint16_t keycode, keyrecord_t *record) {
    for (uint8_t i = 0; i < SPAMMER_COUNT; i++) {
        if (keycode == spammers[i].trigger_keycode) {
            if (record->event.pressed) {
                if (!rand_seeded) {
                    srand(timer_read());
                    rand_seeded = true;
                }
                if (spammers[i].state == SPAM_IDLE) {
                    spammer_begin_press(&spammers[i]);
                } else {
                    spammer_stop(&spammers[i]);
                }
            }
            return false;  // Consume this keycode
        }
    }
    return true;  // Not a spammer trigger, pass through
}

/* CUSTOM: Periodic task — advances all active spammer state machines */
void custom_code_task(void) {
    for (uint8_t i = 0; i < SPAMMER_COUNT; i++) {
        key_spammer_t *s = &spammers[i];
        switch (s->state) {
            case SPAM_PRESSING:
                /* Target key is held — release when hold duration expires */
                if (timer_elapsed(s->timer) >= s->hold_ms) {
                    unregister_code(s->target_keycode);
                    s->state = SPAM_WAITING;
                }
                break;
            case SPAM_WAITING:
                /* Target key released — start next cycle when interval expires */
                if (timer_elapsed(s->timer) >= s->interval_ms) {
                    spammer_begin_press(s);
                }
                break;
            default:
                break;
        }
    }
}

#endif // CUSTOM_CODE_ENABLE
