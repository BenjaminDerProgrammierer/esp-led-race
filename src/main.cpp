#include "LedRace.h"

LedRace game = LedRace();

void setup() {
  Serial.begin(115200);

  game.begin();
}

void loop() {
  game.loop();
}
