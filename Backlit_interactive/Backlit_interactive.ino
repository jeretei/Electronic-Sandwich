#include <FastLED.h>
#include <Adafruit_FreeTouch.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define PIN 6
#define NUM_LEDS 64
#define MAX_POWER_MILLIAMPS 200

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

Adafruit_FreeTouch color_up = Adafruit_FreeTouch(A0, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
Adafruit_FreeTouch color_down = Adafruit_FreeTouch(A1, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
Adafruit_FreeTouch brightness_up = Adafruit_FreeTouch(A2, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
Adafruit_FreeTouch brightness_down = Adafruit_FreeTouch(A3, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);

uint8_t currentColor = 0;
uint8_t brightness = 128;

const uint8_t minBrightness = 30;
const uint8_t maxBrightness = 225;

void setup() {
  // Ensure the microcontroller initializes properly
  delay(1000); // Add a delay to allow power to stabilize

  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_MILLIAMPS);
  
  Serial.begin(115200);
  
  // Wait for Serial to initialize (if connected)
  unsigned long start = millis();
  while (!Serial && millis() - start < 2000); // Timeout after 2 seconds
  if (Serial) {
    Serial.println("RGBW LED Control");
  }

  if (!color_up.begin() || !color_down.begin() || !brightness_up.begin() || !brightness_down.begin()) {
    if (Serial) {
      Serial.println("Failed to initialize FreeTouch sensors");
    }
  }

  matrix.begin();
  matrix.setBrightness(brightness);
}

void loop() {
  // Read touch sensors
  int color_up_val = color_up.measure();
  int color_down_val = color_down.measure();
  int brightness_up_val = brightness_up.measure();
  int brightness_down_val = brightness_down.measure();

  // Output sensor values to Serial monitor
  if (Serial) {
    Serial.print("Color Up: "); Serial.print(color_up_val);
    Serial.print(", Color Down: "); Serial.print(color_down_val);
    Serial.print(", Brightness Up: "); Serial.print(brightness_up_val);
    Serial.print(", Brightness Down: "); Serial.println(brightness_down_val);
  }

  // Update color
  if (color_up_val > 600) {
    currentColor = (currentColor + 1) % 4;
    delay(300); // debounce delay
  }
  if (color_down_val > 600) {
    currentColor = (currentColor == 0) ? 3 : (currentColor - 1);
    delay(300); // debounce delay
  }

  // Update brightness
  if (brightness_up_val > 600 && brightness < maxBrightness) {
    brightness = min(brightness + 15, maxBrightness);
    matrix.setBrightness(brightness);
    delay(300); // debounce delay
  }
  if (brightness_down_val > 600 && brightness > minBrightness) {
    brightness = max(brightness - 15, minBrightness);
    matrix.setBrightness(brightness);
    delay(300); // debounce delay
  }

  // Set the matrix color
  uint32_t color;
  switch (currentColor) {
    case 0:
      color = matrix.Color(255, 0, 0); // Red
      break;
    case 1:
      color = matrix.Color(0, 255, 0); // Green
      break;
    case 2:
      color = matrix.Color(0, 0, 255); // Blue
      break;
    case 3:
      color = matrix.Color(255, 255, 255); // White
      break;
  }

  matrix.fillScreen(color);
  matrix.show();
  delay(100); // Update rate for the LEDs
}
