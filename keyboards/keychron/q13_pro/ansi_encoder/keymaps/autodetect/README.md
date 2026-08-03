# Q13 Pro ANSI encoder: automatic host layout

This keymap keeps Keychron's stock VIA layers and automatically selects the
base layer after QMK fingerprints a USB host:

- macOS and iOS use `MAC_BASE` / `MAC_FN`.
- Windows and Linux use `WIN_BASE` / `WIN_FN`.
- A KVM USB reinitialization triggers a short keyboard soft reset so OS
  detection runs again without unplugging the keyboard cable.
- An inconclusive result keeps the physical Mac/Windows switch setting.
- Bluetooth keeps the physical switch setting because the CKBT51 module does
  not pass USB enumeration packets from the wireless host to QMK.

The stock modifier positions are unchanged:

- Mac: Control, Option, Command.
- Windows/Linux: Control, Windows, Alt.

Build from the `wls_2025q1` branch:

```sh
qmk compile -kb keychron/q13_pro/ansi_encoder -km autodetect
```

Flash only to a Keychron Q13 Pro ANSI encoder (`VID 3434`, `PID 06D0`). Put
the keyboard in cable mode and enter the STM32 DFU bootloader by holding Esc
while connecting the USB cable, or by using the reset button under Space.
