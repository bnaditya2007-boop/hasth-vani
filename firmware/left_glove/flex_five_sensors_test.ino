// Hasth Vani - five flex sensor ADC test
// ESP32 DevKit (30-pin), Arduino IDE, Serial Monitor at 115200 baud

const int THUMB  = 36;  // GPIO36 = VP
const int INDEX  = 39;  // GPIO39 = VN
const int MIDDLE = 34;  // GPIO34 = 34 (printed D34)
const int RING   = 35;  // GPIO35 = 35 (printed D35)
const int LITTLE = 32;  // GPIO32 = 32 (printed D32)

// D33 is driven HIGH and used as the 3.3 V source for the five dividers.
// (The 3V3 pin cannot be reached with only one free breadboard column.)
const int SUPPLY_3V3 = 33;

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(SUPPLY_3V3, OUTPUT);
  digitalWrite(SUPPLY_3V3, HIGH);
  analogReadResolution(12);          // 0 - 4095
  analogSetAttenuation(ADC_11db);    // full 0 - 3.3 V range
  Serial.println("Hasth Vani flex sensor test");
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
