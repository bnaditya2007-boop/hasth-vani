// Hasth Vani - MPU6050 raw accelerometer/gyro test
// No external MPU6050 library required - talks to the chip directly over
// I2C so it will compile on any machine with just the built-in Wire library.
// Wiring: SDA -> GPIO21, SCL -> GPIO22, 3V3, GND. Address 0x68 (AD0 low).
//
// Run mpu6050_i2c_scanner.ino first to confirm the chip answers at 0x68
// before using this sketch.

#include <Wire.h>

const int MPU_ADDR = 0x68;

void writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

int16_t readWord(uint8_t reg) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 2, true);
  int16_t hi = Wire.read();
  int16_t lo = Wire.read();
  return (hi << 8) | lo;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  delay(300);
  writeReg(0x6B, 0x00);   // PWR_MGMT_1: wake the chip up (it starts asleep)
  Serial.println("Hasth Vani MPU6050 movement test");
  Serial.println("ax  ay  az  |  gx  gy  gz   (raw 16-bit, not yet scaled)");
}

void loop() {
  int16_t ax = readWord(0x3B);
  int16_t ay = readWord(0x3D);
  int16_t az = readWord(0x3F);
  int16_t gx = readWord(0x43);
  int16_t gy = readWord(0x45);
  int16_t gz = readWord(0x47);

  Serial.printf("%6d %6d %6d  |  %6d %6d %6d\n", ax, ay, az, gx, gy, gz);
  delay(200);
}

// Reading a steady ~16000 to ~17000 on az with the board flat and the
// other two axes near 0 confirms the chip and wiring are good (that's
// +1g on the Z axis at the default +-2g range). Tilting the board should
// move that ~16000 count between axes accordingly.
