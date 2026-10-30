# UEFI for the Xiaomi Pad 8 Pro (piano)

UEFI and the Visor boot menu, loaded from the boot image. Nothing Xiaomi ships is replaced.

> Work in progress. Unlocked bootloader and a way back (fastboot, recovery) needed.

```
ABL ──► boot.img (kernel + selector + UEFI)
          ├─ nothing held   UEFI ──► Visor ──► Android, Linux, …
          ├─ volume down    fastboot
          └─ volume up      recovery
```

| Feature | State |
| --- | --- |
| Visor, Android from Visor | Works |
| Keys, reboot, shut down | Works |
| Debug fastboot (`piano-uefi`) | Works |
| ESP installer zips | Built, not tested on a tablet |
| Linux from Visor | Not yet |

## Install

Run **UEFI Builder (piano)** in Actions, then flash in OrangeFox or TWRP:

| Zip | Does |
| --- | --- |
| `installer` | Installs UEFI and Visor (decrypt data first) |
| `uninstall` | Restores boot, keeps the ESP |
| `uninstall-clean` | Restores boot, gives the ESP back to data |

Flash the installer again after every ROM update. Back up data first.

Details: [docs/DETAILS.md](docs/DETAILS.md)
