// Hasth Vani - five flex sensor ADC test (CURRENT wiring, split breadboard)
// ESP32 DevKit (30-pin), Arduino IDE, Serial Monitor at 115200 baud
//
// This supersedes flex_five_sensors_test.ino. That version drove GPIO33 HIGH
// as a fake 3.3V rail because the ESP32 covered the real 3V3 pin on the
// original single-breadboard layout. The wiring was later rebuilt on a
// SPLIT breadboard (strips 1|2|3 | ESP32 | 4|5|6) which frees up the 3V3
// pin directly, so no artificial supply pin is needed any more - the five
// voltage dividers are powered straight from the ESP32's 3V3 pin and GND.
//
// Circuit per finger: 3V3 -> flex sensor -> [node] -> 10k resistor -> GND
// The node (junction between sensor and resistor) goes to the ADC pin below.

const int THUMB  = 36;  // GPIO36 = VP
const int INDEX  = 39;  // GPIO39 = VN
const int MIDDLE = 34;
const int RING   = 35;
const int LITTLE = 32;

void setup() {
  Serial.begin(115200);
  delay(500);
  analogReadResolution(12);          // 0 - 4095
  analogSetAttenuation(ADC_11db);    // full 0 - 3.3 V range
  Serial.println("Hasth Vani flex sensor test v2 (real 3V3 rail, split breadboard)");
}

void loop() {
  int thumb  = analogRead(THUMB);
  int index  = analogRead(INDEX);
  int middle = analogRead(MIDDLE);
  int ring   = analogRead(RING);
  int little = analogRead(LITTLE);

  Serial.printf("Thumb: %d | Index: %d | Middle: %d | Ring: %d | Little: %d\n",
                thumb, index, middle, ring, little);
  delay(100);
}

// KNOWN ISSUE (open, as of 28 Sep 2026):
// Thumb reads far lower than the other fingers even straight - about 247
// after bypassing the sensor with a direct wire to 3.3V, versus the
// ~1500-2500 expected for a ~25k straight sensor against a 10k resistor.
// Narrowed to either: (a) the resistor in the Thumb divider is not
// actually 10k (misread colour bands), or (b) that particular flex
// sensor has abnormally high resistance / internal damage.
// Next step: read the resistor's colour bands with a multimeter, and try
// swapping in a spare flex sensor / resistor to isolate which part is bad.
