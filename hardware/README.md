# Hasth Vani - hardware

Circuit reference, component list, and wiring diagrams. Status as of 28 Sep 2026.

> **A note on the diagrams folder:** most of the SVGs in `diagrams/` were
> drawn early in the project as general reference (system architecture,
> schematics, flowcharts) and are still accurate. The exception is
> `05_tft_wiring.svg`, which shows a generic SPI-style TFT connection drawn
> before the shield was actually wired up. The pin table in this README and
> the interactive model in `interactive_models/` reflect the real, tested
> 8-bit-parallel wiring - trust those over that one diagram.

## Bill of materials

| Part | Qty | Used on | Notes |
|---|---|---|---|
| ESP32 DevKit (30-pin) | 2 | Left + right glove | One board per glove |
| Flex sensor (~25k straight) | 5 | Each glove (10 total) | One per finger |
| 10 kΩ resistor | 5 | Each glove (10 total) | Bottom half of each flex sensor's voltage divider |
| MPU6050 (accel + gyro) | 2 | Each glove | I2C, address 0x68 |
| 2.4" TFT LCD shield, 8-bit parallel (driver chip TBD - see Known Issues) | 1 | Left glove only | Wired with loose dupont wires, not plugged into header pins |
| MAX98357A I2S amp module | 1 | Right glove only | **Not yet in hand** as of last inventory check |
| Small speaker (~3 W) | 1 | Right glove only | **Not yet in hand** |
| AA battery holder (4x) | 1+ | Either glove, swappable | ~6 V nominal - see Power notes |
| Breadboard (split layout) | 1 per glove | Both | See breadboard geometry below |
| USB-C cable | 1+ | Dev/programming + alternate power | Never connect at the same time as the battery |

## Left glove pinout (ESP32 #1)

**Flex sensors** (voltage divider: 3V3 → sensor → node → 10kΩ → GND, node → ADC pin):

| Finger | GPIO | Board silkscreen |
|---|---|---|
| Thumb | 36 | VP |
| Index | 39 | VN |
| Middle | 34 | D34 |
| Ring | 35 | D35 |
| Little | 32 | D32 |

**MPU6050 (I2C):** SDA → GPIO21, SCL → GPIO22, plus 3V3 and GND. Address 0x68.

**TFT shield (8-bit parallel):**

| Signal | GPIO | Notes |
|---|---|---|
| D0-D7 | 16, 17, 18, 19, 23, 4, 5, 15 | Data bus, in order |
| DC (RS) | 2 | |
| WR | 13 | |
| RD | tied to 3.3V | not driven by a GPIO - write-only |
| RST | tied to 3.3V | not driven by a GPIO |
| CS | tied to GND | permanently selected |
| Power | 5V from ESP32 VIN, GND shared | |

## Right glove pinout (ESP32 #2) - flex/MPU wired identically to the left glove

Same flex sensor and MPU6050 pin assignments as above (a fresh ESP32 board,
so the same GPIO numbers are free). The speaker side is not yet wired -
`firmware/right_glove/speaker_i2s_test_template.ino` proposes:

| Signal | GPIO |
|---|---|
| MAX98357A BCLK | 26 |
| MAX98357A LRC | 25 |
| MAX98357A DIN | 27 |
| MAX98357A VIN | 5V (ESP32 VIN) |
| MAX98357A GND | GND |

These are free GPIOs chosen to avoid clashing with the flex/MPU pins above,
but they're a proposal, not yet verified on real hardware.

## Breadboard geometry (split layout)

Both gloves use a **split breadboard**: strips 1 (power rail) | 2 (a-e) |
3 (f-j) | *ESP32 sits here* | 4 (a-e) | 5 (f-j) | 6 (power rail). The ESP32's
left pin row plugs into strip 3 column j; its right pin row plugs into
strip 4 column a. This frees up the real 3V3 pin (unlike an earlier,
narrower layout where the ESP32 covered it and a spare GPIO had to fake a
3.3V rail - see `firmware/README.md` for that history). All 5 flex sensor
ADC pins live in the ESP32's right-hand pin row. See diagrams 13-16 for the
general breadboard-connectivity reference.

## Power notes

- **USB-C and the battery pack are alternatives, never both at once.**
  Both feed the same VIN pin. Connecting both simultaneously risks one
  source backfeeding current into the other (e.g. the laptop's 5V pushing
  into a battery pack, or a >5V battery pack pushing current back into the
  laptop's USB port).
- Battery **+** goes to **VIN** (the raw, unregulated input), never
  directly to the 3V3 pin/rail - that bypasses the onboard regulator.
- 2x AA (3.12 V) is **not enough** headroom for the onboard regulator
  (needs roughly ≥4.5-5V realistically). This project uses **4x AA
  (~6 V nominal)**.
- USB carries both power and the serial/upload data link. On battery
  power alone, Serial Monitor and sketch uploads are unavailable - the
  phone-app-to-glove link needs to be wireless (BLE or WiFi, both
  built into the ESP32) rather than depending on the USB cable.
- Voltage only adds in series (multiple cells in one pack). A splitter or
  parallel connection between two different sources does not add voltage
  and is not a safe way to boost a weak supply.

## Diagrams

24 SVG diagrams in `diagrams/`, roughly in build order:

1. System architecture · 2-3. Flex sensor schematics (single + 5-sensor) ·
4. MPU6050 wiring · 5. TFT wiring *(superseded - see note above)* ·
6. MAX98357A/speaker · 7-8. Left/right glove full circuits ·
9. Power & charging · 10. Two-glove system overview · 11-12. Mobile app
flow & data flow · 13-16. Breadboard geometry & connectivity ·
17-21. Troubleshooting/test/calibration flowcharts · 22. Build sequence ·
23. Debugging decision tree · 24. Final assembly.

## Interactive wiring model

`interactive_models/full_wiring_left_glove.html` - open in any browser.
Shows the complete left-glove circuit (flex + MPU6050 + TFT + both power
inputs) fully wired by default; click any wire, sensor, module, or table
row to isolate just that connection (everything else dims). Includes a
built-in netlist checker.

## Full build manual

`Hasth_Vani_Hardware_Build_and_Wiring_Manual.pdf` - the original detailed
build-and-wiring manual (component inventory, per-finger build steps,
checklists). Written before the split-breadboard rewire and the TFT
pin-out above were finalised, so prefer this README and the interactive
model for anything that conflicts.
