# Hasth Vani

Two-glove ESP32 sign-language prototype. Status as of 28 Sep 2026.

- **Left glove:** 5 flex sensors + MPU6050 → ESP32 → TFT display (shows text from the phone app).
- **Right glove:** 5 flex sensors + MPU6050 → ESP32 → MAX98357A → speaker (speaks the recognised sign).
- **Mobile app:** speech-to-text, sends the recognised text to the left glove.

```
├── firmware/        ESP32/Arduino sketches (left_glove/, right_glove/)
├── hardware/        Circuit diagrams (SVG), build & wiring manual
├── images/          diagrams/ (PNG exports), photos/ (real build photos)
├── memory_service/  Hindsight engineering-memory adapter (Python)
├── docs/            Prototype explanation, build plans, 3D wiring viewer
└── archive/         Old snapshots — safe to delete
```

Start with `firmware/README.md` for what's tested vs. untested and the
required TFT_eSPI library setup, and `hardware/README.md` for the
component list and pin tables.

## Where things stand

| Piece | Status |
|---|---|
| Flex sensors (left glove) | Wiring rebuilt on a split breadboard with a real 3.3V rail; 4 of 5 fingers read correctly. Thumb reads too low - open issue. |
| MPU6050 (left glove) | Scanner + raw readout sketches ready; not yet confirmed on the rebuilt wiring. |
| TFT display (left glove) | Library installed and configured, sketch compiles/uploads, but screen shows blank white - open issue, likely wrong driver chip assumed. |
| Right glove (flex + MPU) | Same circuit as left glove, but nothing physically wired/tested yet. |
| Speaker (right glove) | MAX98357A not yet in hand; only an untested code template exists. |
| Mobile app | Not started. |
| Power | 4x AA battery pack (~6V) and USB-C both wired to VIN as alternatives (never simultaneously). |

## Open issues

1. **Thumb flex sensor reads ~247 instead of the expected ~1500-2500**, even
   after bypassing the sensor with a direct 3.3V wire. Likely a wrong
   resistor value or a damaged sensor - needs a multimeter check.
2. **TFT screen is blank white** after a successful upload. Likely the
   shield's real display controller isn't ILI9341 as assumed - needs the
   actual chip identified (photo of the board would help) and
   `User_Setup.h`'s driver line updated to match.

See `firmware/README.md` and `hardware/README.md` for full detail on both.

## How Hindsight memory is used

Hardware debugging leaves no stack trace. What you ruled out lives in your head
until it doesn't — so this project keeps its debugging history in
[Hindsight](https://github.com/vectorize-io/hindsight) instead. `memory_service/`
is a small adapter over two operations:

```text
retain -> store a tested observation, failure, decision, or next check
recall -> retrieve relevant history before choosing the next experiment
```

**Gateway pattern.** The memory layer never runs on the glove:

```text
flex sensors / MPU6050 / ESP32 firmware   (real-time, no credentials)
            |
            v
     gateway: laptop, Pi, or phone backend
            |
            v
     memory_service/  ->  Hindsight
```

The ESP32s only sample ADCs and talk I2C/I2S. They hold no API key and make no
cloud calls. The gateway already receives glove telemetry, so that is where the
adapter and the `HINDSIGHT_API_KEY` live.

**Bank.** Everything goes into one bank, `hasth-vani` (`HINDSIGHT_BANK_ID`), so
recall stays focused on this project.

Every sensor observation is forced into the same shape — *sensor, reading,
condition, conclusion, next check* — because the conclusion ("the sensor alone is
not proven to be the cause") is what stops a test being re-run three weeks later.

```bash
py cli.py retain "Thumb reads ~247 even with a direct 3.3V bypass..." --context "Hasth Vani flex-sensor debugging"
py cli.py recall "What do we already know about the thumb flex sensor?"
```

Setup and real retain/recall output: `memory_service/README.md`.
Full write-up: **https://dev.to/bnaditya2007boop/my-gloves-thumb-read-247-hindsight-remembered-why-5ml**
