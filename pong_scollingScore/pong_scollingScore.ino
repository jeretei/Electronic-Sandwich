#include <FastLED.h>
#include <Adafruit_FreeTouch.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define PIN 6
#define NUM_LEDS 64
#define MAX_POWER_MILLIAMPS 200
#define DATA_PIN 6

CRGB leds[NUM_LEDS];

Adafruit_FreeTouch player1_up = Adafruit_FreeTouch(A0, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
Adafruit_FreeTouch player1_down = Adafruit_FreeTouch(A1, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
Adafruit_FreeTouch player2_up = Adafruit_FreeTouch(A2, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
Adafruit_FreeTouch player2_down = Adafruit_FreeTouch(A3, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

int player1_pos = 3;
int player2_pos = 3;
int ball_x = 4;
int ball_y = 4;
int ball_dx = 1;
int ball_dy = 1;
int player1_score = 0;
int player2_score = 0;
bool game_over = false;
bool show_score = false;
unsigned long show_score_start_time = 0;
const unsigned long show_score_duration = 2000;

unsigned long lastBallUpdateTime = 0;
const unsigned long ballUpdateInterval = 200; // Delay for ball updates

unsigned long lastPaddleUpdateTime = 0;
const unsigned long paddleUpdateInterval = 100; // Delay for paddle updates

void setup() {
  // Ensure the microcontroller initializes properly
  delay(1000); // Add a delay to allow power to stabilize

  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_MILLIAMPS);
  
  Serial.begin(115200);
  
  // Wait for Serial to initialize (if connected)
  unsigned long start = millis();
  while (!Serial && millis() - start < 2000); // Timeout after 2 seconds
  if (Serial) {
    Serial.println("Pong Game");
  }
  
  pinMode(LED_BUILTIN, OUTPUT);

  if (!player1_up.begin() || !player1_down.begin() || !player2_up.begin() || !player2_down.begin()) {
    if (Serial) {
      Serial.println("Failed to initialize FreeTouch sensors");
    }
  }

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(100);
}

void loop() {
  if (game_over) {
    // Wait for both players to activate their controls to restart
    if (player1_up.measure() > 600 && player1_down.measure() > 600 &&
        player2_up.measure() > 600 && player2_down.measure() > 600) {
      player1_score = 0;
      player2_score = 0;
      game_over = false;
      ball_x = 4;
      ball_y = 4;
      ball_dx = 1;
      ball_dy = 1;
    }
    return;
  }

  if (show_score) {
    scrollScores();
    return;
  }

  unsigned long currentTime = millis();

  // Update paddles
  if (currentTime - lastPaddleUpdateTime >= paddleUpdateInterval) {
    lastPaddleUpdateTime = currentTime;

    // Read touch sensors
    int player1_up_val = player1_up.measure();
    int player1_down_val = player1_down.measure();
    int player2_up_val = player2_up.measure();
    int player2_down_val = player2_down.measure();

    // Print sensor values to Serial Monitor
    Serial.print("Player 1 Up: ");
    Serial.print(player1_up_val);
    Serial.print(" | Player 1 Down: ");
    Serial.print(player1_down_val);
    Serial.print(" | Player 2 Up: ");
    Serial.print(player2_up_val);
    Serial.print(" | Player 2 Down: ");
    Serial.println(player2_down_val);

    // Update player positions
    if (player1_up_val > 600 && player1_pos > 0) {
      player1_pos--;
    }
    if (player1_down_val > 600 && player1_pos < 5) {
      player1_pos++;
    }
    if (player2_up_val > 600 && player2_pos > 0) {
      player2_pos--;
    }
    if (player2_down_val > 600 && player2_pos < 5) {
      player2_pos++;
    }
  }

  // Update ball
  if (currentTime - lastBallUpdateTime >= ballUpdateInterval) {
    lastBallUpdateTime = currentTime;

    // Update ball position
    ball_x += ball_dx;
    ball_y += ball_dy;

    // Ball collision with top and bottom
    if (ball_y <= 0 || ball_y >= 7) {
      ball_dy = -ball_dy;
    }

    // Ball collision with paddles
    if (ball_x == 1 && ball_y >= player1_pos && ball_y <= player1_pos + 2) {
      ball_dx = -ball_dx;
    }
    if (ball_x == 6 && ball_y >= player2_pos && ball_y <= player2_pos + 2) {
      ball_dx = -ball_dx;
    }

    // Ball out of bounds
    if (ball_x < 0) {
      player2_score++;
      show_score = true;
      show_score_start_time = millis();
    }
    if (ball_x > 7) {
      player1_score++;
      show_score = true;
      show_score_start_time = millis();
    }

    if (player1_score >= 10 || player2_score >= 10) {
      game_over = true;
      displayWinner();
      return;
    }

    // Clear the matrix
    matrix.clear();

    // Draw the paddles
    matrix.drawPixel(0, player1_pos, matrix.Color(0, 255, 255)); // Cyan
    matrix.drawPixel(0, player1_pos + 1, matrix.Color(0, 255, 255)); // Cyan
    matrix.drawPixel(0, player1_pos + 2, matrix.Color(0, 255, 255)); // Cyan

    matrix.drawPixel(7, player2_pos, matrix.Color(255, 0, 255)); // Magenta
    matrix.drawPixel(7, player2_pos + 1, matrix.Color(255, 0, 255)); // Magenta
    matrix.drawPixel(7, player2_pos + 2, matrix.Color(255, 0, 255)); // Magenta

    // Draw the ball
    matrix.drawPixel(ball_x, ball_y, matrix.Color(255, 0, 0)); // Red in GRB

    // Show the matrix
    matrix.show();
  }
}

void scrollScores() {
  String score_text = String(player1_score) + " to " + String(player2_score);
  int textLength = score_text.length() * 6; // Each character is 6 pixels wide
  for (int i = 8; i > -textLength; i--) {
    matrix.clear();
    matrix.setCursor(i, 0);
    matrix.print(score_text);
    matrix.show();
    delay(50);
  }
  show_score = false; // Ensure scrolling only happens once
  ball_x = 4; // Reset ball position
  ball_y = 4;
  ball_dx = (random(2) == 0) ? 1 : -1; // Randomize initial direction
  ball_dy = (random(2) == 0) ? 1 : -1;
}

void displayWinner() {
  matrix.clear();
  matrix.setCursor(0, 0);
  if (player1_score >= 10) {
    scrollText("Cyan Wins!");
  } else {
    scrollText("Magenta Wins!");
  }
}

void scrollText(String text) {
  int textLength = text.length() * 6; // Each character is 6 pixels wide
  for (int i = 8; i > -textLength; i--) {
    matrix.clear();
    matrix.setCursor(i, 0);
    matrix.print(text);
    matrix.show();
    delay(100);
  }
}
