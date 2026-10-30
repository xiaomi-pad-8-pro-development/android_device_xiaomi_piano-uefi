# UEFI for the Xiaomi Pad 8 Pro (piano)

UEFI and the Visor boot menu, loaded from the boot image. Nothing Xiaomi ships is replaced.

> Work in progress. Unlocked bootloader and a way back (fastboot, recovery) needed.

```
ABL ──► boot.img (kernel + selector + UEFI)
          ├─ nothing held   UEFI ──► Visor ──► Android, Linux, …
          ├─ volume down    fastboot
          └─ volume up      recovery
```

> Status: worked on the HyperOS 4.0.0.44 firmware. Being retested on 4.0.5 and the current ROM.

## Install

Run **UEFI Builder (piano)** in Actions, then flash in OrangeFox or TWRP:

| Zip | Does |
| --- | --- |
| `installer` | Installs UEFI and Visor (decrypt data first) |
| `uninstall` | Restores boot, keeps the ESP |
| `uninstall-clean` | Restores boot, gives the ESP back to data |

Flash the installer again after every ROM update. Back up data first.

Details: [docs/DETAILS.md](docs/DETAILS.md)
