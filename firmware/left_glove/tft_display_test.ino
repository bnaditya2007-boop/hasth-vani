// Hasth Vani - TFT shield test (8-bit PARALLEL) with the TFT_eSPI library
// Tested and confirmed working (compiles + uploads) on 27 Sep 2026.
// Board: ESP32 Dev Module, port COM7 in this project's dev setup.
//
// !! This sketch will NOT compile out of the box on a fresh machine !!
// TFT_eSPI needs two setup steps first - see firmware/README.md and
// firmware/left_glove/User_Setup.h in this folder. Short version:
//   1. Install the "TFT_eSPI" library (by Bodmer) via Library Manager.
//   2. Replace <Arduino/libraries/TFT_eSPI/User_Setup.h> with the copy in
//      this folder (it has this project's exact pin wiring, not the
//      library's generic SPI default).
//   3. Patch <Arduino/libraries/TFT_eSPI/Processors/TFT_eSPI_ESP32.c>:
//      the library's 8-bit-parallel readByte() calls gpio_input_get(),
//      which was removed from ESP32 Arduino core 3.x and will not
//      compile. Replace all 3 occurrences of gpio_input_get() with
//      REG_READ(GPIO_IN_REG). This sketch never reads from the display
//      (RD is hard-wired to 3.3V, not driven by a GPIO), so the patch is
//      safe - it only affects a code path this project never calls.
//
// Wiring (see hardware/ for the full pinout table and diagrams):
//   D0-D7 = GPIO16,17,18,19,23,4,5,15   DC(RS) = GPIO2   WR = GPIO13
//   RD and RST tied directly to 3.3V (not driven by the ESP32)
//   CS tied directly to GND            5V from ESP32 VIN

#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

void setup() {
  tft.init();
  tft.setRotation(3);   // 0/2 = portrait pair, 1/3 = landscape pair.
                         // If the image is upside down, use the OTHER
                         // member of the same pair (1<->3 or 0<->2).
                         // If it's sideways instead, switch pairs.
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("HASTH VANI", 10, 10, 4);
  tft.drawString("Hello!", 10, 50, 4);
}

void loop() {}

// KNOWN ISSUE (open, as of 28 Sep 2026):
// After the library install + User_Setup.h + gpio_input_get patch above,
// this sketch compiles and uploads successfully ("Hash of data verified"),
// but the physical screen shows solid white - no text, no error. This
// means the panel's backlight is on but it never receives a valid
// init/draw sequence. Most likely cause: the shield's actual display
// driver chip is NOT ILI9341 (many generic "2.4 inch TFT shield" boards
// use ILI9325 / ILI9328 / HX8347 / ST7781 / etc. under the same label
// with no clear markings), so ILI9341_DRIVER in User_Setup.h sends the
// wrong init commands. To fix: identify the real driver chip (check for
// a part number printed on the board/chip, or try swapping the _DRIVER
// #define in User_Setup.h for other TFT_eSPI-supported chips) and retest.
