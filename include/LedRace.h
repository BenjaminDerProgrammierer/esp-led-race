#include <Adafruit_NeoPixel.h>

#ifndef _LED_RACE_H_
#define _LED_RACE_H_

#include "MyMelody.h"

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
#define COLOR_PLAYER_1 _ledStrip.Color(255, 165, 0)  // Orange
#define COLOR_PLAYER_2 _ledStrip.Color(0, 255, 255)  // Cyan
#define COLOR_PLAYER_3 _ledStrip.Color(255, 0, 255)  // Magenta

class LedRace {
   private:
    Adafruit_NeoPixel _ledStrip =
        Adafruit_NeoPixel(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

    unsigned long lastMicActionPlayer1 = 0;
    unsigned long lastMicActionPlayer2 = 0;
    unsigned long lastMicActionPlayer3 = 0;
    unsigned long lastButtonAction = 0;

    const unsigned long micDebounceDelay = 60;       // 60ms debounce
    const unsigned long buttonDebounceDelay = 1000;  // 1s button held

    int positionPlayer1 = START_POS;
    int positionPlayer2 = START_POS;
    int positionPlayer3 = START_POS;

    bool startButtonState = BUTTON_RELEASE;
    bool buttonState = BUTTON_RELEASE;

    bool gameRunning = false;
    unsigned long gameTime = 0;
    unsigned long startTime = 0;

    static LedRace* instance;

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
        if (instance) instance->micActionPlayer1();
    }

    IRAM_ATTR static void micActionPlayer2Wrapper() {
        if (instance) instance->micActionPlayer2();
    }

    IRAM_ATTR static void micActionPlayer3Wrapper() {
        if (instance) instance->micActionPlayer3();
    }

    // TODO
    void buttonPressed() {
        Serial.println("Button pressed");
        if (gameRunning) {
            gameRunning = false;
            publishRaceStatus("Stopped");

            detachInterrupt(PIN_MIC_1);
            detachInterrupt(PIN_MIC_2);
            detachInterrupt(PIN_MIC_3);

            // TODO: Display race stop

            stopGate();
        } else {
            publishRaceStatus("Prepare4Race");

            // TODO: Display race start

            clearStrip();
            _ledStrip.show();

            positionPlayer1 = START_POS;
            positionPlayer2 = START_POS;
            positionPlayer3 = START_POS;

            countDownGate();

            gameRunning = true;
            startTime = millis();

            publishRaceStatus("Running");

            attachInterrupt(digitalPinToInterrupt(PIN_MIC_1), micActionPlayer1Wrapper, RISING);
            attachInterrupt(digitalPinToInterrupt(PIN_MIC_2), micActionPlayer2Wrapper, RISING);
            attachInterrupt(digitalPinToInterrupt(PIN_MIC_3), micActionPlayer3Wrapper, RISING);
        }
    }

    void publishRaceStatus(String status) {
        // TODO: implement MQTT publish
    }

    void sendRaceData() {
        // TODO: implement MQTT publish
    }

    void publishWinner(String player, unsigned long time) {
        // TODO: implement MQTT publish
    }

   public:
    LedRace() {}

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

        // TODO: initialize LCD
        // TODO: display WiFi info on LCD
        // TODO: publish MQTT WiFi info
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

            // TODO: display player steps on LCD
            // TODO: publish MQTT

            // detachInterrupt(PIN_MIC_1);
            // detachInterrupt(PIN_MIC_2);
            // detachInterrupt(PIN_MIC_3);

            clearStrip();
            startGate();

            for (int i = 0; i < CAR_LENGTH; i++) {
                _ledStrip.setPixelColor(positionPlayer1 + i, COLOR_PLAYER_1);
                _ledStrip.setPixelColor(positionPlayer2 + i, COLOR_PLAYER_2);
                _ledStrip.setPixelColor(positionPlayer3 + i, COLOR_PLAYER_3);
            }
            _ledStrip.show();

            // attachInterrupt(digitalPinToInterrupt(PIN_MIC_1),
            // micActionPlayer1Wrapper, RISING);
            // attachInterrupt(digitalPinToInterrupt(PIN_MIC_2),
            // micActionPlayer2Wrapper, RISING);
            // attachInterrupt(digitalPinToInterrupt(PIN_MIC_3),
            // micActionPlayer3Wrapper, RISING);
        }

        if (positionPlayer1 >= NUMPIXELS - START_POS) {
            detachInterrupt(PIN_MIC_1);
            gameTime = millis() - startTime;
            gameRunning = false;

            sendRaceData();
            publishRaceStatus("Finish");
            publishWinner("Player_1", gameTime);

            // TODO: Display winner & winner's time on LCD

            stopGate();
        }

        if (positionPlayer2 >= NUMPIXELS - START_POS) {
            detachInterrupt(PIN_MIC_2);
            gameTime = millis() - startTime;
            gameRunning = false;

            sendRaceData();
            publishRaceStatus("Finish");
            publishWinner("Player_2", gameTime);

            // TODO: Display winner & winner's time on LCD

            stopGate();
        }

        if (positionPlayer3 >= NUMPIXELS - START_POS) {
            detachInterrupt(PIN_MIC_3);
            gameTime = millis() - startTime;
            gameRunning = false;

            sendRaceData();
            publishRaceStatus("Finish");
            publishWinner("Player_3", gameTime);

            // TODO: Display winner & winner's time on LCD

            stopGate();
        }
    }
};

LedRace* LedRace::instance = nullptr;

#endif  // _LED_RACE_H_