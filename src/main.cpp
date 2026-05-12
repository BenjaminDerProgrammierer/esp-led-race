#include <Arduino.h>

#include "LedRace.h"
#include "MyDisplay.h"
#include "MyMqtt.h"
#include "MyWifi.h"

MyDisplay display = MyDisplay("ESP8266 LED Race", "V5");
MyWifi wifi = MyWifi();
MyMqtt mqtt = MyMqtt("ESP8266 NodeMCU V1", "", "Open_LED_Race/");
LedRace game = LedRace(&mqtt, &display);

void setup() {
  Serial.begin(115200);

  display.begin();
  display.showStatus("Connecting to WiFi...");
  wifi.connect();
  display.showStatus("Connected to WiFi", ("SSID: " + wifi.getWifiSSID()).c_str(), ("IP: " + wifi.getWifiIP()).c_str());
  delay(2000);
  display.showStatus("Connecting to MQTT...");
  mqtt.begin();
  mqtt.publishWifiData(wifi.getWifiSSID(), wifi.getWifiIP(), wifi.getWifiMAC());
  display.showStatus("Connected to MQTT", ("SSID: " + wifi.getWifiSSID()).c_str(), ("IP: " + wifi.getWifiIP()).c_str());
  delay(2000);
  game.begin();
}

void loop() {
  wifi.ensureConnected();
  mqtt.loop();
  game.loop();
}
