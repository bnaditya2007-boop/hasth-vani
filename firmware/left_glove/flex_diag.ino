// Hasth Vani - flex sensor crosstalk diagnostic
// Serial Monitor: 115200 baud, line ending "No Line Ending" or "Newline" both fine.
// Keep ALL fingers straight for the first 4 seconds: the sketch records a baseline.
// Then bend ONE finger at a time. Commands (type a letter + Enter):
//   b = re-take baseline   s = supply D33 on/off   d = D33 max drive strength on/off

const int PINS[5] = {36, 39, 34, 35, 32};
const char* NAMES[5] = {"Thumb", "Index", "Middle", "Ring", "Little"};
const int SUPPLY = 33;

int base[5];
bool haveBase = false;
bool supplyOn = true;
bool strong = false;
unsigned long t0;

// Settled read: throw away the first sample (sample-and-hold capacitor still holds the
// previous channel), wait, then average n samples. spread = max - min = noise/float check.
int settledRead(int pin, int n, int &spread) {
  analogRead(pin);
  delayMicroseconds(400);
  long sum = 0; int mn = 4095, mx = 0;
  for (int i = 0; i < n; i++) {
    int v = analogRead(pin);
    sum += v;
    if (v < mn) mn = v;
    if (v > mx) mx = v;
    delayMicroseconds(150);
  }
  spread = mx - mn;
  return sum / n;
}

void takeBaseline() {
  int sp;
  for (int i = 0; i < 5; i++) base[i] = settledRead(PINS[i], 32, sp);
  haveBase = true;
  Serial.print(">>> BASELINE (settled): ");
  for (int i = 0; i < 5; i++) { Serial.print(NAMES[i]); Serial.print("="); Serial.print(base[i]); Serial.print(" "); }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(SUPPLY, OUTPUT);
  digitalWrite(SUPPLY, HIGH);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  Serial.println("\nHasth Vani flex diagnostic. Keep all fingers STRAIGHT for 4 s...");
  t0 = millis();
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == 'b') takeBaseline();
    if (c == 's') { supplyOn = !supplyOn; digitalWrite(SUPPLY, supplyOn); Serial.print(">>> D33 supply "); Serial.println(supplyOn ? "ON" : "OFF (all pins should read ~0)"); }
    if (c == 'd') { strong = !strong; gpio_set_drive_capability((gpio_num_t)SUPPLY, strong ? GPIO_DRIVE_CAP_3 : GPIO_DRIVE_CAP_2); Serial.print(">>> D33 drive "); Serial.println(strong ? "MAX" : "default"); }
  }
  if (!haveBase && millis() - t0 > 4000) takeBaseline();

  int raw[5], set[5], spr[5];
  for (int i = 0; i < 5; i++) raw[i] = analogRead(PINS[i]);            // back-to-back, like the old sketch
  for (int i = 0; i < 5; i++) set[i] = settledRead(PINS[i], 16, spr[i]); // settled + averaged

  Serial.print("RAW ");
  for (int i = 0; i < 5; i++) { Serial.print(raw[i]); Serial.print(i < 4 ? " " : ""); }
  Serial.print(" | SET ");
  for (int i = 0; i < 5; i++) { Serial.print(set[i]); Serial.print(i < 4 ? " " : ""); }
  Serial.print(" | NOISE ");
  for (int i = 0; i < 5; i++) { Serial.print(spr[i]); Serial.print(i < 4 ? " " : ""); }

  if (haveBase) {
    int moved = 0, movedRaw = 0;
    Serial.print(" | MOVED(set): ");
    for (int i = 0; i < 5; i++) {
      if (base[i] > 300 && set[i] < base[i] * 0.75) { Serial.print(NAMES[i]); Serial.print(" "); moved++; }
    }
    if (moved == 0) Serial.print("none");
    Serial.print(" | verdict: ");
    if (moved == 0) Serial.print("idle");
    else if (moved == 1) Serial.print("ISOLATED (good)");
    else Serial.print("SEVERAL MOVE TOGETHER (shared problem)");
  }
  Serial.println();
  delay(250);
}
