// Hasth Vani - MAX98357A speaker test TEMPLATE (UNTESTED)
//
// STATUS: the speaker / MAX98357A module was not yet in hand as of the
// last hardware inventory check, so this sketch has never been run on
// real hardware. It is a starting point, not a verified test - expect to
// debug it once the part arrives.
//
// Proposed wiring (not yet physically built - pick free GPIOs that don't
// clash with the flex sensors [32,34,35,36,39] or the MPU6050 I2C bus
// [21,22] on this board):
//   MAX98357A BCLK -> GPIO26
//   MAX98357A LRC  -> GPIO25
//   MAX98357A DIN  -> GPIO27
//   MAX98357A GAIN -> leave floating for 9dB gain, or see datasheet for
//                     other presets (SD to GND / VIN / floating also
//                     selects left/right/stereo channel mixing - check
//                     the datasheet against how the module is wired).
//   MAX98357A VIN  -> 5V (from ESP32 VIN, same rail as everything else)
//   MAX98357A GND  -> GND
//
// This sketch just plays a short test tone (a sine wave) so you can
// confirm the wiring and I2S config work before wiring in real audio
// (the recognised sign's word/phrase) from the sign-recognition logic.

#include <driver/i2s.h>
#include <math.h>

#define I2S_BCLK 26
#define I2S_LRC  25
#define I2S_DIN  27
#define SAMPLE_RATE 16000

void setup() {
  Serial.begin(115200);

  i2s_config_t cfg = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 256,
    .use_apll = false
  };
  i2s_pin_config_t pins = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_LRC,
    .data_out_num = I2S_DIN,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pins);
  Serial.println("Hasth Vani speaker test - playing 440 Hz tone");
}

void loop() {
  static float phase = 0;
  const int N = 256;
  int16_t buf[N];
  for (int i = 0; i < N; i++) {
    buf[i] = (int16_t)(sin(phase) * 8000);   // modest volume, not full scale
    phase += 2.0 * PI * 440.0 / SAMPLE_RATE; // 440 Hz test tone
    if (phase > 2.0 * PI) phase -= 2.0 * PI;
  }
  size_t written;
  i2s_write(I2S_NUM_0, buf, sizeof(buf), &written, portMAX_DELAY);
}
