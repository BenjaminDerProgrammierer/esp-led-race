#include "LedRace.h"
#include "MyMqtt.h"
#include "MyWifi.h"

MyWifi wifi = MyWifi();
MyMqtt mqtt = MyMqtt("ESP8266 NodeMCU V1", "", "Open_LED_Race/");
LedRace game = LedRace(&mqtt);

void setup() {
  Serial.begin(115200);

  wifi.connect();
  mqtt.begin();
  mqtt.publishWifiData(wifi.getWifiSSID(), wifi.getWifiIP(), wifi.getWifiMAC());
  game.begin();
}

void loop() {
  wifi.ensureConnected();
  mqtt.loop();
  game.loop();
}
