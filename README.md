# Pico Nuke All

English | [中文](README.zh.md)

Erase the external Flash on a **Waveshare RP2350-One**, then return to BOOTSEL for firmware installation.

## Features

- Detects the Flash capacity and verifies erasure
- Runs from RAM, with a red status light throughout the operation
- Preserves OTP, Secure Boot and Secure Lock settings
- Works with PicoForge All's firmware signing and update flow

## Download and use

Download `pico_nuke_all-1.7-unsigned.uf2` from [Releases](https://github.com/XiaoNetwork-Astral/pico-nuke-all/releases/latest).

> [!WARNING]
> Published firmware is unsigned; sign it yourself before installation using **[PicoForge All](https://github.com/XiaoNetwork-Astral/pico-forge-all) → Firmware** and your local key. A device with Secure Boot enabled requires its original trusted key.
>
> Running Nuke permanently deletes all firmware, credentials, PINs and settings in external Flash. Prepare a signed Pico All image for restoration first. OTP and hardware locks remain in place.

1. Select the Nuke UF2 and your signing key in PicoForge All → Firmware, then sign it
2. Choose Flash and confirm the erase prompt; press and release the device button (BOOTSEL) when the light flashes
3. After completion, install your signed [Pico All](https://github.com/XiaoNetwork-Astral/pico-all/releases/latest) image

When loaded directly from BOOTSEL, Nuke starts immediately without another button prompt. After verification, Flash contains only the small `NUKE` marker used by picotool.

| Light | Meaning |
| --- | --- |
| Breathing red | Installed Pico All is waiting for erase confirmation |
| Steady red | Erasing / verifying, then ready in BOOTSEL after completion |
| Fast red blinking | Erase or verification failed |

## Build

Requires an Arm toolchain, CMake, Ninja and Pico SDK 2.3.1.

```sh
git clone https://github.com/XiaoNetwork-Astral/pico-nuke-all.git
cd pico-nuke-all
cmake -S . -B build -G Ninja -DPICO_SDK_PATH=/path/to/pico-sdk -DPICO_BOARD=waveshare_rp2350_one
cmake --build build
```

Output: unsigned `build/flash_nuke.uf2`. Source builds also support `pico` and `pico2`; those boards use their own LED hardware. Host tests in `tests/` simulate erasure without touching a device.

## License and credits

[BSD-3-Clause](LICENSE). Based on Phil Howard's [Pico Universal Flash Nuke](https://github.com/Gadgetoid/pico-universal-flash-nuke) and Raspberry Pi's Flash Nuke example.
