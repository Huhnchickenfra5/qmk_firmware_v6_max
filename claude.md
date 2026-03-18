# Keychron V6 Max QMK Build Guide

## Environment

- **Repo:** https://github.com/Huhnchickenfra5/qmk_firmware_v6_max.git
- **Branch:** `wireless_playground`
- **Local path:** `C:\Data\Coding\V6Max`
- **MCU:** STM32F401 (ARM — needs `arm-none-eabi-gcc`, not AVR)
- **Toolchain:** QMK MSYS installed at `C:\QMK_MSYS`

## Build Command

The standard `make` and `qmk` CLI don't work from Git Bash / VS Code terminal directly
because QMK MSYS's Python can't resolve the home directory outside its own shell.
**Always build via the QMK MSYS shell connector:**

```bash
cmd.exe /c "C:\QMK_MSYS\shell_connector.cmd -c \"make keychron/v6_max/ansi_encoder:default\""
```

For the `via` keymap (editable via VIA app):
```bash
cmd.exe /c "C:\QMK_MSYS\shell_connector.cmd -c \"make keychron/v6_max/ansi_encoder:via\""
```

## Output

Built firmware lands in `C:\Data\Coding\V6Max\`:
- `keychron_v6_max_ansi_encoder_default.bin`
- `keychron_v6_max_ansi_encoder_default.hex`

## Flashing

Put the V6 Max into DFU mode: hold **Escape** while plugging in USB.

Then flash from a QMK MSYS terminal:
```bash
qmk flash -kb keychron/v6_max/ansi_encoder -km default
```
Or use **QMK Toolbox** with the `.bin` file.

## Setup (first time only)

```bash
git clone https://github.com/Huhnchickenfra5/qmk_firmware_v6_max.git .
git checkout wireless_playground
git submodule update --init --recursive
```

## Notes

- The `wireless_playground` branch contains Keychron's wireless/BT support — not in upstream QMK.
- V6 Max keyboard directory: `keyboards/keychron/v6_max/`
- ANSI + encoder variant: `keyboards/keychron/v6_max/ansi_encoder/`
- Current debounce: `sym_eager_pk` at 20ms (in `info.json`)
- A custom `asym_eager_defer_pk` debounce is available in `keyboards/keychron/common/debounce/`
  but is NOT yet enabled for V6 Max — could fix double-click/chatter issues if needed.

## Custom Code Rules

### 1. Isolate all custom code in a dedicated file pair
- All custom logic lives in `keyboards/keychron/v6_max/ansi_encoder/keymaps/via/custom_code.h` and `custom_code.c`
- Never scatter custom logic across multiple Keychron source files

### 2. Guard everything with a single feature flag
- Wrap all custom code with `#ifdef CUSTOM_CODE_ENABLE` / `#endif`
- Toggle on/off with one line in `keyboards/keychron/v6_max/ansi_encoder/keymaps/via/rules.mk`:
  ```
  CUSTOM_CODE_ENABLE = yes
  ```
- Add build flag: `OPT_DEFS += -DCUSTOM_CODE_ENABLE`
- When disabled, the build produces identical firmware to stock

### 3. Define custom keycodes starting from NEW_SAFE_RANGE
- Keychron reserves `QK_KB_0` through `NEW_SAFE_RANGE` — custom keycodes must start at `NEW_SAFE_RANGE`
- Define the enum in `custom_code.h`

### 4. Hook into keymap.c with minimal changes (two hooks)
- **Key events** — add to `process_record_user()`:
  ```c
  #ifdef CUSTOM_CODE_ENABLE
      if (!process_record_custom(keycode, record)) {
          return false;
      }
  #endif
  ```
- **Timed sequences / loops** — add `housekeeping_task_user()` to keymap.c:
  ```c
  void housekeeping_task_user(void) {
  #ifdef CUSTOM_CODE_ENABLE
      custom_code_task();
  #endif
  }
  ```
- Add `#include "custom_code.h"` at the top (guarded)
- No other Keychron files are modified

### 5. Use non-blocking timer pattern for delayed sequences
- Never use blocking delays (`_delay_ms`, `wait_ms`) in custom code
- Use `timer_read()` + `timer_elapsed()` with a state machine:
  - `process_record_custom()` sets the initial state and starts the timer
  - `custom_code_task()` checks elapsed time each scan cycle and advances the sequence
- Keep all state in `static` variables inside `custom_code.c`

### 6. Keymap assignments: replace `_______` slots
- Assign custom keycodes to currently-unused (`_______`) slots in existing layers
- If a new layer is needed, add it to `enum layers` in keymap.c
- Comment every custom keycode placement with `// CUSTOM:` so it's easy to find and revert

### 7. Comment everything
- Every custom function, keycode, state, and keymap slot must have a comment explaining what it does
- Use a consistent `// CUSTOM:` prefix for inline comments in keymap.c

### 8. Never modify files outside the keymap directory
- All changes stay within `keyboards/keychron/v6_max/ansi_encoder/keymaps/via/`
- Do not modify `keychron_common.h/c`, `v6_max.c`, or any shared Keychron code
- Build flags go only in the keymap's `rules.mk`

### 9. SRC addition
- In `rules.mk`:
  ```
  SRC += custom_code.c
  ```

### 10. Generic spammer architecture
- All auto-key features use the `key_spammer_t` struct in `custom_code.c`
- Config + runtime state in one struct: trigger keycode, target key, timing range, state machine
- All spammers live in the `spammers[]` array — generic loops handle everything
- To add a new spammer: add enum value in `custom_code.h` + new array entry in `custom_code.c`
- Never write per-key logic — the array-driven design handles it
