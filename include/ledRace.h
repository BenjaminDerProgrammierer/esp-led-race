#ifndef _LED_RACE_H_
#define _LED_RACE_H_

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include "myStatus.h"
#include "myMqtt.h"

// Pin definitions
#define NEOPIXEL_PIN 0
#define BUZZER_PIN 15
#define START_PIN 12
#define P1_PIN 13
#define P2_PIN 14
#define P3_PIN 2

// Game settings
#define PLAYER_0_COLOR pixels.Color(0, 0, 0)    // Color for empty track
#define PLAYER_1_COLOR pixels.Color(255, 0, 0)  // Color for Player 1 (red)
#define PLAYER_2_COLOR pixels.Color(0, 255, 0)  // Color for Player 2 (green)
#define PLAYER_3_COLOR pixels.Color(0, 0, 255)  // Color for Player 3 (blue)
#define NUMPIXELS 300                           // Count of NeoPixel LEDs
#define CAR_LENGTH 4                            // Length of the car in LEDs
#define START_POS 10   // First LED on the track is No. 10
#define BRIGHTNESS 50  // Brightness of the LEDs

Adafruit_NeoPixel pixels =
    Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Total steps to finish the race
int raceSteps = NUMPIXELS - START_POS;

#define ON 0
#define OFF 1

// Button
bool currentButtonState = OFF;
bool lastButtonState = OFF;
unsigned long lastButtonTime = 0;
#define BUTTON_DELAY 1000  // Button press debounce time in ms

// Player Microphones
unsigned long lastPlayer1Mic = 0;
unsigned long lastPlayer2Mic = 0;
unsigned long lastPlayer3Mic = 0;
#define MIC_DEBOUNCE_DELAY 60

// MQTT send delay
unsigned long lastMqttTime = 0;
#define SEND_MQTT_DELAY 2000

// Race State variables
int player1Steps = 0;
int player2Steps = 0;
int player3Steps = 0;
bool raceRunning = false;
unsigned long raceStartTime = 0;

/**
 * To be called once to initialize the NeoPixel strip
 */
void initNeoPixel() {
  pixels.begin();
  pixels.setBrightness(BRIGHTNESS);
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, PLAYER_0_COLOR);
  }
  pixels.show();
}

/**
 * Interrupt Service Routine for Player 1's microphone
 */
IRAM_ATTR void player1MicAction() {
  if ((millis() - lastPlayer1Mic) > MIC_DEBOUNCE_DELAY) {
    player1Steps++;
    lastPlayer1Mic = millis();
  }
}

/**
 * Interrupt Service Routine for Player 2's microphone
 */
IRAM_ATTR void player2MicAction() {
  if ((millis() - lastPlayer2Mic) > MIC_DEBOUNCE_DELAY) {
    player2Steps++;
    lastPlayer2Mic = millis();
  }
}

/**
 * Interrupt Service Routine for Player 3's microphone
 */
IRAM_ATTR void player3MicAction() {
  if ((millis() - lastPlayer3Mic) > MIC_DEBOUNCE_DELAY) {
    player3Steps++;
    lastPlayer3Mic = millis();
  }
}

/**
 * Plays countdown sounds and lights on the neopixel strip
 */
void soundCountdown() {
  int tones[] = {261, 294, 330, 349, 392, 440, 494};
  int duration = 500;

  // LEDs:
  // R R R R R G G G G G
  // 0 1 2 3 4 5 6 7 8 9

  for (int i = 0; i < 5; i++) {
    tone(BUZZER_PIN, tones[i], duration);

    // Gradual light up red LEDs (0-4)
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));  // Rot
    pixels.show();

    delay(duration);
  }

  // Disable red LEDs and enable green LEDs
  for (int i = 0; i < 5; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    pixels.setPixelColor(i + 5, pixels.Color(0, 255, 0));  // Grün
  }
  pixels.show();

  // Sound final tone
  tone(BUZZER_PIN, tones[6], duration);
}

/**
 * Plays stop sound and lights on the neopixel strip
 */
void soundStop() {
  int tones[] = {261, 294, 330, 349, 392, 440, 494};
  int duration = 500;

  // LEDs:
  // R R R R R G G G G G
  // 0 1 2 3 4 5 6 7 8 9

  // Disable green LEDs and enable red LEDs
  for (int i = 0; i < 5; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    pixels.setPixelColor(i + 5, pixels.Color(255, 0, 0));  // ROT
  }
  pixels.show();
  tone(BUZZER_PIN, tones[6], duration);  // Ton ausgeben
}

/**
 * Terminates the race with no winner
 */
void terminateRace() {
  raceRunning = false;
  publishStatus("Stopped");

  detachInterrupt(P1_PIN);
  detachInterrupt(P2_PIN);
  detachInterrupt(P3_PIN);

  showRaceStoppedStatus();

  delay(100);

  soundStop();
}

/**
 * To be called when a player has won the race
 *
 * @param winnerNumber Number of the winning player (1, 2 or 3)
 */
void finishRace(int winnerNumber) {
  detachInterrupt(P1_PIN);
  unsigned long playerTime = millis() - raceStartTime;
  raceRunning = false;

  publishRaceData(playerTime, playerTime, playerTime, player1Steps,
                  player2Steps, player3Steps);
  publishWinner(winnerNumber, playerTime);

  showWinner(winnerNumber);

  soundStop();
}

/**
 * Starts the race
 */
void startRace() {
  publishStatus("Prepare4Race");
  showPrepareRaceStatus();
  soundCountdown();
  publishStatus("Running");

  // Delete all LEDs
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, PLAYER_0_COLOR);
  }
  pixels.show();

  raceRunning = true;
  raceStartTime = millis();

  lastPlayer1Mic = millis();
  lastPlayer2Mic = millis();
  lastPlayer3Mic = millis();

  player1Steps = 0;
  player2Steps = 0;
  player3Steps = 0;

  // start Interrupt
  attachInterrupt(digitalPinToInterrupt(P1_PIN), player1MicAction, FALLING);
  attachInterrupt(digitalPinToInterrupt(P2_PIN), player2MicAction, FALLING);
  attachInterrupt(digitalPinToInterrupt(P3_PIN), player3MicAction, FALLING);
}

/**
 * To be called in the main loop to handle the start/stop button
 */
void loopButton() {
  int reading = digitalRead(START_PIN);

  //********* Start-Button gedrückt = LOW (pullup)
  if (reading != lastButtonState) {
    lastButtonTime = millis();
  }

  // Entprellzeit abgelaufen
  if ((millis() - lastButtonTime) > BUTTON_DELAY) {
    if (reading != currentButtonState) {
      currentButtonState = reading;

      if (currentButtonState == ON) {
        // Stop or Start Race
        if (raceRunning) {
          terminateRace();
        } else {
          startRace();
        }
      }
    }
  }
  lastButtonState = reading;
}

/**
 * To be called in the main loop to handle the race logic
 */
void loopRace() {
  if (raceRunning) {
    unsigned long playerTime = millis() - raceStartTime;

    showGameStatus(playerTime, player1Steps, player2Steps, player3Steps);

    if ((millis() - lastMqttTime) > SEND_MQTT_DELAY) {
      lastMqttTime = millis();
      publishRaceData(playerTime, playerTime, playerTime, player1Steps,
                      player2Steps, player3Steps);
    }

    // Clear interrupts to safely update LED strip
    detachInterrupt(P1_PIN);
    detachInterrupt(P2_PIN);
    detachInterrupt(P3_PIN);

    // Clear strip
    for (int i = START_POS; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, PLAYER_0_COLOR);
    }

    // Draw players
    for (int i = 0; i < CAR_LENGTH; i++) {
      pixels.setPixelColor(player1Steps + START_POS + i, PLAYER_1_COLOR);
      pixels.setPixelColor(player2Steps + START_POS + i, PLAYER_2_COLOR);
      pixels.setPixelColor(player3Steps + START_POS + i, PLAYER_3_COLOR);
    }
    pixels.show();

    // Re-enable interrupts
    attachInterrupt(digitalPinToInterrupt(P1_PIN), player1MicAction, RISING);
    attachInterrupt(digitalPinToInterrupt(P2_PIN), player2MicAction, RISING);
    attachInterrupt(digitalPinToInterrupt(P3_PIN), player3MicAction, RISING);
  }

  if (player1Steps >= raceSteps) {
    finishRace(1);
  }

  if (player2Steps >= raceSteps) {
    finishRace(2);
  }

  if (player3Steps >= raceSteps) {
    finishRace(3);
  }
}

#endif // _LED_RACE_H_