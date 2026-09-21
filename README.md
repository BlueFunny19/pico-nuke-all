# Pico Universal Flash Nuke

Tired of hunting for the right flash_nuke.uf2 for your RP2040 or RP2350 based board? Your Pico, your Pico W, your Pico 2 W, no, I'm not keyword stuffing... you are!

That nightmare is over. My gift to you: Pico Universal Flash Nuke!

* The only .uf2 you'll ever need for all your nuking purposes!
* Detects the size of a Pico or RP2XXX board's attached flash and nukes it accordingly.
* *should* work with both RP2040 and RP2350 using a single, handy .uf2.

(This incredibly delicately combined uf2 was created using the awesome power of `cat`)

Grab Pico Universal Flash Nuke from the releases page: https://github.com/Gadgetoid/pico-universal-flash-nuke/releases/latest

## Pico All / Waveshare RP2350-One status lights

Use **pico_nuke_all_waveshare_rp2350_one.uf2** for the Waveshare RP2350-One.
The generic Pico / Pico 2 image does not select this board's GPIO 16 RGB LED.

- **Breathing red:** a four-second warning before erasure, then continuing while flash is erased and checked.
- **Steady red:** every erased byte and the final picotool marker have been checked. The RGB LED is latched red before the device returns to BOOTSEL, ready for firmware installation.
- **Fast red blinking:** flash identification, erasure or marker verification failed. The program stays in RAM; reconnect in BOOTSEL to recover.

The RGB LED uses the same RGB byte order as Pico All on this board. Both cores
run from SRAM, so the breathing animation continues during blocking erase calls.
Single-colour Pico LEDs use their native colour; no firmware can turn those red.
The onboard RGB's latched colour lasts while powered, until another program
updates it. Unplugging clears it.

This still erases the whole external flash and leaves only the existing
first-page NUKE marker. It does **not** clear RP2350 OTP, Secure Boot or Secure
Lock. A locked board needs an image signed locally with its original trusted
key. Do not load Nuke just to preview its light pattern.

Build with Pico SDK 2.2.0 or later:

    cmake -S . -B build/waveshare -DPICO_BOARD=waveshare_rp2350_one
    cmake --build build/waveshare

For a locked board, add -DSECURE_BOOT_PKEY=/path/to/original-private.pem to the
configure command. Keep the key local. The output is build/waveshare/flash_nuke.uf2.
This is a RAM-only program; the existing rollback version is retained.

Host tests exercise the real erase sequence against simulated flash, including
failed erase/program operations, without touching a board:

    cc -std=c11 -Wall -Wextra -Werror -DPICO_NO_FLASH=1 -I tests/stubs tests/nuke_test.c -o /tmp/nuke-test
    /tmp/nuke-test

## Support Me

I work on Pico shinies by day, occasionally cranking out balmy tools to make my job easier and sharing them with the world.

If they help you too, great! If you want to throw me a bone for my troubles, see below:

* Ko-Fi - https://ko-fi.com/gadgetoid
* GitHub - https://github.com/sponsors/Gadgetoid
* Patreon - https://www.patreon.com/c/gadgetoid
* PayPal - https://www.paypal.com/paypalme/gadgetoid

Find some of my other projects below:

* dir2uf2 - Pack a directory and append it to a MicroPython uf2 - https://github.com/gadgetoid/dir2uf2
* py_decl - Python code to read Pico's binary declaration format - https://github.com/gadgetoid/py_decl
* Pico pinouts - https://pico.pinout.xyz 
