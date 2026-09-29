"""Retain the real Hasth Vani debugging observations, then recall them.

Writes every raw Hindsight response to memory_service/evidence/ as JSON.
Run from memory_service/:  py run_demo.py
"""

import json
import pathlib
import sys

from hasth_vani_memory import HasthVaniMemory, HindsightError

EVIDENCE = pathlib.Path(__file__).resolve().parent / "evidence"

RETAIN = [
    ("retain_1_thumb.json",
     "Sensor: thumb flex (left glove). Reading: about 247 while the other four "
     "fingers read 1500-2500. Condition: measured straight, and again with the "
     "flex sensor bypassed entirely by a jumper direct to 3.3V. Conclusion: the "
     "reading did not change under bypass, so the flex sensor alone is NOT "
     "proven to be the cause. Next check: measure the 10k divider resistor with "
     "a multimeter to confirm its value, then swap in a spare sensor.",
     "Hasth Vani flex-sensor debugging"),

    ("retain_2_tft_compile.json",
     "Component: TFT_eSPI library on ESP32 Arduino core 3.x. Problem: the "
     "library would not compile because gpio_input_get() was removed from the "
     "core. Fix applied: replaced gpio_input_get() with REG_READ(GPIO_IN_REG) "
     "in 3 places in TFT_eSPI_ESP32.c. Conclusion: this is a core-3.x API "
     "removal, not a wiring fault. Result: the sketch now compiles and uploads.",
     "Hasth Vani display debugging"),

    ("retain_3_tft_white.json",
     "Component: 2.4in TFT shield on the left glove ESP32. Reading: after the "
     "core-3.x compile fix the sketch uploads and verifies successfully, but "
     "the screen shows solid white with nothing drawn. Condition: User_Setup.h "
     "is configured assuming an ILI9341 controller. Conclusion: upload is fine, "
     "so this is almost certainly the wrong driver chip rather than bad wiring "
     "or a bad upload. Next check: identify the real controller on the shield "
     "(candidates ILI9325, ILI9328, HX8347, ST7781) and change the _DRIVER line "
     "in User_Setup.h to match.",
     "Hasth Vani display debugging"),

    ("retain_4_power_rail.json",
     "Topic: breadboard power rail for the five flex dividers (left glove). "
     "Original layout: the board covered the ESP32's real 3V3 pin, so GPIO33 "
     "was driven HIGH and used as a fake 3.3V rail for all five dividers. "
     "Problem: that made crosstalk between fingers very hard to reason about. "
     "Fix applied: rebuilt the circuit on a split breadboard so the real 3V3 "
     "pin is reachable, and all dividers now run from real 3V3. Result: 4 of 5 "
     "fingers read correctly after the rebuild; only the thumb is still wrong.",
     "Hasth Vani power and wiring"),
]

RECALL = [
    ("recall_thumb.json", "What do we already know about the thumb flex sensor?"),
    ("recall_tft.json", "Why is the TFT screen white?"),
]


def save(name, data):
    path = EVIDENCE / name
    path.write_text(json.dumps(data, indent=2), encoding="utf-8")
    print(f"  -> {path.name}")


def main() -> int:
    EVIDENCE.mkdir(parents=True, exist_ok=True)
    memory = HasthVaniMemory()
    print(f"bank={memory.config.bank_id} base={memory.base_url}\n")

    for name, content, context in RETAIN:
        print(f"RETAIN {name}")
        try:
            save(name, memory.retain(content, context=context))
        except HindsightError as exc:
            print(f"  FAILED: {exc}", file=sys.stderr)
            return 1

    for name, query in RECALL:
        print(f"RECALL {query!r}")
        try:
            save(name, memory.recall(query))
        except HindsightError as exc:
            print(f"  FAILED: {exc}", file=sys.stderr)
            return 1

    print("\nall done")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
