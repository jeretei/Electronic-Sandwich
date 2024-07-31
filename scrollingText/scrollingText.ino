#include <FastLED.h>
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

const char* customText = "Replace this text with your custom text";
int textColor = matrix.Color(255, 0, 0); // Red text, change to any RGB value you desire

void setup() {
  // Ensure the microcontroller initializes properly
  delay(1000); // Add a delay to allow power to stabilize

  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_MILLIAMPS);
  
  Serial.begin(115200);
  
  // Wait for Serial to initialize (if connected)
  unsigned long start = millis();
  while (!Serial && millis() - start < 2000); // Timeout after 2 seconds
  if (Serial) {
    Serial.println("Scrolling Text Display");
  }

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(128); // Set brightness to 50%
  matrix.setTextColor(textColor);
}

void loop() {
  scrollText(customText);
}

void scrollText(const char* text) {
  int textLength = strlen(text) * 6; // Each character is 6 pixels wide
  for (int i = 8; i > -textLength; i--) {
    matrix.clear();
    matrix.setCursor(i, 0);
    matrix.print(text);
    matrix.show();
    delay(100);
  }
}
