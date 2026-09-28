// Hasth Vani - MPU6050 I2C scanner
// Wiring: SDA -> GPIO21, SCL -> GPIO22, plus 3V3 and GND.
// Expected result: "Found 0x68" (MPU6050's default I2C address).
// If AD0 is tied high instead of low/floating, expect 0x69 instead.

#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);   // SDA=21, SCL=22
  delay(300);
  Serial.println("Hasth Vani I2C scanner");
}

void loop() {
  int found = 0;
  for (byte a = 1; a < 127; a++) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) {
      Serial.printf("Found 0x%02X\n", a);
      found++;
    }
  }
  if (!found) Serial.println("No I2C devices found");
  delay(2000);
}
