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

void setup() {
  // Ensure the microcontroller initializes properly
  delay(1000); // Add a delay to allow power to stabilize

  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_MILLIAMPS); //limit amps to LED matrix to prevent failure
  
  Serial.begin(115200);
  
  // Wait for Serial to initialize (if connected)
  unsigned long start = millis();
  while (!Serial && millis() - start < 2000); // Timeout after 2 seconds
  if (Serial) {
    Serial.println("White LED Backlit");
  }

  matrix.begin();
  matrix.setBrightness(100); // Set lower brightness (100 out of 255)
}

void loop() {
  matrix.fillScreen(matrix.Color(255, 255, 255)); // Set all LEDs to white
  matrix.show(); // Update the matrix to display the changes
}
