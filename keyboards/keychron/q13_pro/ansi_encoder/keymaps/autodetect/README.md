# Q13 Pro ANSI encoder: automatic host layout

This keymap keeps Keychron's stock VIA layers and automatically selects the
base layer after QMK fingerprints a USB host:

- macOS and iOS use `MAC_BASE` / `MAC_FN`.
- Windows and Linux use `WIN_BASE` / `WIN_FN`.
- After a KVM reconnects USB to the replacement host, the keyboard waits 250
  ms and performs one soft reset so OS detection runs from a clean state.
- The reset is deferred until the replacement host has configured USB; the
  keyboard does not reset while the cable or KVM data path is disconnected.
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
the keyboard in cable mode and hold Fn while pressing Esc to enter STM32 DFU
without disconnecting USB. The stock hold-Esc-while-connecting method remains
available, but Fn+Esc is the preferred recovery-safe update path.
