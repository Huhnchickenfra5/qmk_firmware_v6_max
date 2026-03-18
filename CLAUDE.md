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
