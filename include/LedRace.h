#include <Adafruit_NeoPixel.h>

#ifndef _LED_RACE_H_
#define _LED_RACE_H_

#include "MyMelody.h"
#include "MyMqtt.h"
#include "MyDisplay.h"

#define PIN_NEOPIXEL 0
#define PIN_BUZZER 15
#define PIN_BUTTON 12
#define PIN_MIC_1 13
#define PIN_MIC_2 14
#define PIN_MIC_3 2

#define NUMPIXELS 300
#define CAR_LENGTH 4
#define BUTTON_PRESS 0
#define BUTTON_RELEASE 1
#define START_POS 10
#define BRIGHTNESS 50
#define COLOR_OFF _ledStrip.Color(0, 0, 0)
#define COLOR_RED _ledStrip.Color(255, 0, 0)
#define COLOR_GREEN _ledStrip.Color(0, 255, 0)
#define COLOR_PLAYER_1 _ledStrip.Color(255, 165, 0) // Orange
#define COLOR_PLAYER_2 _ledStrip.Color(0, 255, 255) // Cyan
#define COLOR_PLAYER_3 _ledStrip.Color(255, 0, 255) // Magenta

class LedRace {
private:
  Adafruit_NeoPixel _ledStrip =
      Adafruit_NeoPixel(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
  MyMqtt *mqtt;
  MyDisplay *display;

  unsigned long lastMicActionPlayer1 = 0;
  unsigned long lastMicActionPlayer2 = 0;
  unsigned long lastMicActionPlayer3 = 0;
  unsigned long lastButtonAction = 0;

  const unsigned long micDebounceDelay = 60;      // 60ms debounce
  const unsigned long buttonDebounceDelay = 1000; // 1s button held

  int positionPlayer1 = START_POS;
  int positionPlayer2 = START_POS;
  int positionPlayer3 = START_POS;

  bool startButtonState = BUTTON_RELEASE;
  bool buttonState = BUTTON_RELEASE;

  bool gameRunning = false;
  unsigned long gameTime = 0;
  unsigned long startTime = 0;

  static LedRace *instance;

  int lastPositions = 0;

  void publishRaceStatus(String status, unsigned long time, int pos1, int pos2,
                         int pos3) {
    if (status == "Stopped") {
      mqtt->publishRaceStatus("Stopped", time, pos1, pos2, pos3);
    } else if (status == "Countdown") {
      mqtt->publishRaceStatus("Countdown", time, pos1, pos2, pos3);
    } else if (status == "Running") {
      mqtt->publishRaceStatus("Running", time, pos1, pos2, pos3);
    } else if (status == "Finished") {
      mqtt->publishRaceStatus("Finished", time, pos1, pos2, pos3);
    }
    // Serial.printf("Race Status: %s, Time: %lu, Positions: %d, %d, %d\n",
    // status.c_str(), time, pos1, pos2, pos3);
  }

  void publishWinner(String player, unsigned long time) {
    mqtt->publishRaceWinner(player, time);
    // Serial.printf("Race Winner: %s, Time: %lu\n", player.c_str(), time);
  }

  void clearStrip() {
    for (int i = 0; i < NUMPIXELS; i++) {
      _ledStrip.setPixelColor(i, COLOR_OFF);
    }
  }

  String stringTime(unsigned long playerTime) {
    int sec = playerTime / 1000 % 60;
    int msec = playerTime % 1000;
    int min = playerTime / 1000 / 60;

    return String(min) + ":" + String(sec) + "," + String(msec / 10);
  }

  void countDownGate() {
    MyMelody melody = MyMelody(PIN_BUZZER);

    for (int i = 0; i < 5; i++) {
      _ledStrip.setPixelColor(i, COLOR_RED);
      _ledStrip.show();
      melody.playNote(i);
    }

    startGate();
    melody.playNote(6);
  }

  void stopGate() {
    MyMelody melody = MyMelody(PIN_BUZZER);
    for (int i = 0; i < 5; i++) {
      _ledStrip.setPixelColor(i, COLOR_RED);
      _ledStrip.setPixelColor(i + 5, COLOR_OFF);
    }
    _ledStrip.show();
    melody.playNote(7);
  }

  void startGate() {
    for (int i = 0; i < 5; i++) {
      _ledStrip.setPixelColor(i, COLOR_GREEN);
      _ledStrip.setPixelColor(i + 5, COLOR_OFF);
    }
    _ledStrip.show();
  }

  void micActionPlayer1() {
    if (millis() - lastMicActionPlayer1 > micDebounceDelay) {
      positionPlayer1++;
      lastMicActionPlayer1 = millis();
    }
  }

  void micActionPlayer2() {
    if (millis() - lastMicActionPlayer2 > micDebounceDelay) {
      positionPlayer2++;
      lastMicActionPlayer2 = millis();
    }
  }

  void micActionPlayer3() {
    if (millis() - lastMicActionPlayer3 > micDebounceDelay) {
      positionPlayer3++;
      lastMicActionPlayer3 = millis();
    }
  }

  IRAM_ATTR static void micActionPlayer1Wrapper() {
    if (instance)
      instance->micActionPlayer1();
  }

  IRAM_ATTR static void micActionPlayer2Wrapper() {
    if (instance)
      instance->micActionPlayer2();
  }

  IRAM_ATTR static void micActionPlayer3Wrapper() {
    if (instance)
      instance->micActionPlayer3();
  }

  void buttonPressed() {
    Serial.println("Button pressed");
    if (gameRunning) {
      gameRunning = false;
      publishRaceStatus("Stopped", gameTime, positionPlayer1 - 10,
                        positionPlayer2 - 10, positionPlayer3 - 10);

      detachInterrupt(PIN_MIC_1);
      detachInterrupt(PIN_MIC_2);
      detachInterrupt(PIN_MIC_3);

      display->showStatus("Race Stopped");

      stopGate();
      delay(2000);
      display->showStatus("Press the button", "to start the race!", "");
    } else {
      publishRaceStatus("Countdown", 0, 0, 0, 0);

      display->showStatus("Get ready!");

      clearStrip();
      _ledStrip.show();

      positionPlayer1 = START_POS;
      positionPlayer2 = START_POS;
      positionPlayer3 = START_POS;

      countDownGate();

      gameRunning = true;
      startTime = millis();

      publishRaceStatus("Running", 0, 0, 0, 0);

      attachInterrupt(digitalPinToInterrupt(PIN_MIC_1), micActionPlayer1Wrapper,
                      RISING);
      attachInterrupt(digitalPinToInterrupt(PIN_MIC_2), micActionPlayer2Wrapper,
                      RISING);
      attachInterrupt(digitalPinToInterrupt(PIN_MIC_3), micActionPlayer3Wrapper,
                      RISING);
    }
  }

public:
  LedRace(MyMqtt *_mqtt, MyDisplay *_display)
      : mqtt(_mqtt), display(_display) {}

  void begin() {
    instance = this;

    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_MIC_1, INPUT);
    pinMode(PIN_MIC_2, INPUT);
    pinMode(PIN_MIC_3, INPUT);
    pinMode(PIN_BUTTON, INPUT);

    _ledStrip.begin();
    _ledStrip.setBrightness(BRIGHTNESS);
    _ledStrip.show();

    publishRaceStatus("Stopped", 0, 0, 0, 0);
    display->showStatus("Press the button", "to start the race!", "");
  }

  void loop() {
    int reading = digitalRead(PIN_BUTTON);

    if (reading != buttonState) {
      lastButtonAction = millis();
    }

    buttonState = reading;

    if ((millis() - lastButtonAction) > buttonDebounceDelay &&
        reading != startButtonState) {
      startButtonState = reading;

      if (startButtonState == BUTTON_PRESS) {
        buttonPressed();
      }
    }

    if (gameRunning) {
      gameTime = millis() - startTime;

      // TODO: Optimize by only updating display every 100ms or when positions change
      display->showStatus((String("Player 1: ") + String(positionPlayer1 - 10)).c_str(), (String("Player 2: ") + String(positionPlayer2 - 10)).c_str(),
              (String("Player 3: ") + String(positionPlayer3 - 10)).c_str());

      publishRaceStatus("Running", gameTime, positionPlayer1 - 10,
                        positionPlayer2 - 10, positionPlayer3 - 10);

      // detachInterrupt(PIN_MIC_1);
      // detachInterrupt(PIN_MIC_2);
      // detachInterrupt(PIN_MIC_3);

      // Update LED strip only if positions have changed to minimize updates and
      // potential flickering
      int currentPositions =
          positionPlayer1 + positionPlayer2 + positionPlayer3;
      if (currentPositions != lastPositions) {
        clearStrip();
        startGate();

        for (int i = 0; i < CAR_LENGTH; i++) {
          _ledStrip.setPixelColor(positionPlayer1 + i, COLOR_PLAYER_1);
          _ledStrip.setPixelColor(positionPlayer2 + i, COLOR_PLAYER_2);
          _ledStrip.setPixelColor(positionPlayer3 + i, COLOR_PLAYER_3);
        }
        _ledStrip.show();

        lastPositions = currentPositions;
      }

      // attachInterrupt(digitalPinToInterrupt(PIN_MIC_1),
      // micActionPlayer1Wrapper, RISING);
      // attachInterrupt(digitalPinToInterrupt(PIN_MIC_2),
      // micActionPlayer2Wrapper, RISING);
      // attachInterrupt(digitalPinToInterrupt(PIN_MIC_3),
      // micActionPlayer3Wrapper, RISING);

      if (positionPlayer1 >= NUMPIXELS - START_POS) {
        detachInterrupt(PIN_MIC_1);
        gameTime = millis() - startTime;
        gameRunning = false;

        publishRaceStatus("Finished", gameTime, positionPlayer1 - 10,
                          positionPlayer2 - 10, positionPlayer3 - 10);
        publishWinner("Player_1", gameTime);

      display->showStatus("Player 1 wins!", stringTime(gameTime).c_str(), "Congratulations!");

        stopGate();
      }

      if (positionPlayer2 >= NUMPIXELS - START_POS) {
        detachInterrupt(PIN_MIC_2);
        gameTime = millis() - startTime;
        gameRunning = false;

        publishRaceStatus("Finished", gameTime, positionPlayer1 - 10,
                          positionPlayer2 - 10, positionPlayer3 - 10);
        publishWinner("Player_2", gameTime);

        display->showStatus("Player 2 wins!", stringTime(gameTime).c_str(), "Congratulations!");

        stopGate();
      }

      if (positionPlayer3 >= NUMPIXELS - START_POS) {
        detachInterrupt(PIN_MIC_3);
        gameTime = millis() - startTime;
        gameRunning = false;

        publishRaceStatus("Finished", gameTime, positionPlayer1 - 10,
                          positionPlayer2 - 10, positionPlayer3 - 10);
        publishWinner("Player_3", gameTime);

        display->showStatus("Player 3 wins!", stringTime(gameTime).c_str(), "Congratulations!");

        stopGate();
      }
    }
  }
};

LedRace *LedRace::instance = nullptr;

#endif // _LED_RACE_H_