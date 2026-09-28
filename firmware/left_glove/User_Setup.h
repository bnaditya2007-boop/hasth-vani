// Hasth Vani - custom User_Setup.h for 2.4" TFT shield (ILI9341 assumed, 8-bit parallel)
//
// This is a BACKUP / reference copy of the file that must be dropped into
// <Arduino sketchbook>/libraries/TFT_eSPI/User_Setup.h (replacing the
// library's own default) for tft_display_test.ino to compile and address
// the correct pins. See firmware/README.md for the full install steps.
//
// NOTE (28 Sep 2026): this config makes the sketch compile and upload
// cleanly, but the physical display currently shows solid white - see the
// KNOWN ISSUE note at the bottom of tft_display_test.ino. The driver line
// below (ILI9341_DRIVER) is the most likely thing that needs to change
// once the shield's real controller chip is identified.

#define USER_SETUP_INFO "User_Setup"

#define ILI9341_DRIVER

#define TFT_PARALLEL_8_BIT

#define TFT_CS   -1   // tied to GND
#define TFT_DC    2   // RS
#define TFT_RST  -1   // tied to 3.3V
#define TFT_WR   13
#define TFT_RD   -1   // tied to 3.3V

#define TFT_D0   16
#define TFT_D1   17
#define TFT_D2   18
#define TFT_D3   19
#define TFT_D4   23
#define TFT_D5    4
#define TFT_D6    5
#define TFT_D7   15

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

#define SMOOTH_FONT
