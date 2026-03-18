/* CUSTOM: Custom Code Header — Generic Key-Spam Toggle System
 *
 * All custom keycodes and function declarations for the V6 Max via keymap.
 * Guarded by CUSTOM_CODE_ENABLE so disabling produces stock firmware.
 */

#pragma once

#ifdef CUSTOM_CODE_ENABLE

#include "keychron_common.h"

/* CUSTOM: Circle key (F13) LED index — from g_led_config in ansi_encoder.c */
#define CIRCLE_KEY_LED_INDEX 16

/* CUSTOM: Custom keycodes — must start from NEW_SAFE_RANGE
 * (defined in keychron_common.h) to avoid collisions with Keychron's range. */
enum custom_keycodes {
    CC_QSPAM = NEW_SAFE_RANGE,  // CUSTOM: Toggle Q auto-clicker on/off
    // Add more spammer triggers here, e.g.:
    // CC_ESPAM,
};

/* CUSTOM: Called from process_record_user() in keymap.c.
 * Returns false to consume the keycode, true to pass through. */
bool process_record_custom(uint16_t keycode, keyrecord_t *record);

/* CUSTOM: Called from housekeeping_task_user() in keymap.c.
 * Runs every scan cycle to advance all active spammer state machines. */
void custom_code_task(void);

#endif // CUSTOM_CODE_ENABLE
