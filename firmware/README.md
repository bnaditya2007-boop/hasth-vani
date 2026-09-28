# Hasth Vani - firmware

ESP32/Arduino code for both gloves. Status as of 28 Sep 2026.

## Layout

```
firmware/
├── left_glove/
│   ├── flex_five_sensors_test.ino       (OLD - D33-as-fake-3V3-rail wiring, kept for reference)
│   ├── flex_five_sensors_test_v2.ino    (CURRENT - real 3V3 rail, split breadboard)
│   ├── flex_diag.ino                    Crosstalk diagnostic sketch (baseline + per-finger isolation check)
│   ├── mpu6050_i2c_scanner.ino          Confirms the MPU6050 answers at 0x68
│   ├── mpu6050_movement_test.ino        Raw accel/gyro readout, no external library needed
│   ├── tft_display_test.ino             TFT shield test - compiles & uploads, display currently blank (see below)
│   └── User_Setup.h                     Backup of the TFT_eSPI library config this project needs
└── right_glove/
    ├── flex_five_sensors_test_v2.ino    Same circuit as the left glove (see hardware/README.md)
    ├── mpu6050_i2c_scanner.ino          Same circuit as the left glove
    ├── mpu6050_movement_test.ino        Same circuit as the left glove
    └── speaker_i2s_test_template.ino    UNTESTED - MAX98357A speaker not yet in hand
```

Board setting for every sketch: **ESP32 Dev Module**. In this project's dev
setup the boards showed up on **COM7**; yours may differ. Serial Monitor:
**115200 baud**.

## Setting up TFT_eSPI (required before tft_display_test.ino will compile)

This is the part that isn't written down anywhere else, so it's worth
spelling out in full. The 2.4" TFT shield is wired in 8-bit parallel mode,
which needs the `TFT_eSPI` library configured specifically for this
project's pins - the library's own default `User_Setup.h` assumes a
different (SPI) display and will not work.

1. In Arduino IDE: **Tools > Manage Libraries...**, search `TFT_eSPI`,
   install the one **by Bodmer** (this project used v2.5.43).
2. Find your sketchbook's library folder - typically
   `Documents/Arduino/libraries/TFT_eSPI/` (on some Windows setups
   `Documents` is redirected into OneDrive, e.g.
   `C:\Users\<you>\OneDrive\Documents\Arduino\libraries\TFT_eSPI\`).
3. Replace that folder's `User_Setup.h` with the copy in
   `firmware/left_glove/User_Setup.h` from this repo.
4. **Patch a real bug in the library.** Open
   `TFT_eSPI/Processors/TFT_eSPI_ESP32.c` and find the `readByte()`
   function (search for `gpio_input_get`). It calls a function that was
   removed from ESP32 Arduino core 3.x, so the library will not compile
   against a current core - you'll get
   `error: 'gpio_input_get' was not declared`. Replace all **3**
   occurrences of `gpio_input_get()` with `REG_READ(GPIO_IN_REG)`. This is
   a known, safe fix (confirmed against the TFT_eSPI GitHub issue tracker):
   it only affects reading pixel data back from the display, which this
   project's sketches never do (`RD` is hard-wired to 3.3V, not driven by
   a GPIO).
5. Re-open/re-verify the sketch. It should now compile and upload cleanly.

With those two files in place, `tft_display_test.ino` compiles, uploads,
and verifies ("Hash of data verified") - but see the known issue below.

## Known issues (open)

- **TFT shows solid white, no image.** After the setup above, the sketch
  uploads successfully but the physical screen just lights up white with
  no text. This points to the shield's actual display controller chip
  *not* being ILI9341 (`User_Setup.h` currently assumes ILI9341) - many
  generic "2.4 inch TFT shield" boards use ILI9325 / ILI9328 / HX8347 /
  ST7781 / etc. under the same label with no clear markings. Next step:
  identify the real chip from any printed part number on the board, then
  change the `_DRIVER` line in `User_Setup.h` to match and re-test.
- **Thumb flex sensor reads far too low.** Straight, it reads ~247 even
  after bypassing the sensor with a direct wire to 3.3V (expected
  ~1500-2500 for a ~25k sensor against the 10k resistor). Likely a wrong
  resistor value or a damaged sensor in that one divider - needs a
  multimeter check on the resistor and a sensor swap test.
- **Right glove is entirely untested.** The flex/MPU circuit should be
  identical to the left glove's (see `hardware/README.md`), but nothing
  has been wired or powered on that board yet, and the speaker sketch is
  a template only - the MAX98357A module wasn't in hand as of the last
  inventory check.

## Suggested build order

1. `flex_five_sensors_test_v2.ino` - confirm all 5 fingers read cleanly
   and independently (use `flex_diag.ino` if any finger looks like it's
   affecting the others).
2. `mpu6050_i2c_scanner.ino` then `mpu6050_movement_test.ino` - confirm
   the IMU is alive and responds to tilting.
3. `tft_display_test.ino` - once the blank-screen issue above is fixed.
4. Repeat 1-2 on the right glove board.
5. `speaker_i2s_test_template.ino` once the MAX98357A is available.
6. Only after each piece works alone: combine flex + MPU + TFT into one
   sketch for the left glove, and flex + MPU + speaker for the right.
   (No combined "main" sketch exists yet - each part is still being
   brought up individually.)
