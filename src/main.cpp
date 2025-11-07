/**
 * @file main.cpp
 * @author Benjamin Hartmann (b.hartmann@students.htl-leonding.ac.at)
 * @brief Main file for OpenLED-Race project
 * @version 5.0
 * @date 2025-11-06
 *
 * @copyright Copyright (c) 2025 HTL Leonding
 *
 */

#include <Arduino.h>

#include "myStatus.h"
#include "myMqtt.h"
#include "myStatus.h"
#include "myWifi.h"
#include "ledRace.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();

  // Pin mode setup
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(P1_PIN, INPUT);
  pinMode(P2_PIN, INPUT);
  pinMode(P3_PIN, INPUT);
  pinMode(START_PIN, INPUT);

  // Hardware setup
  initLcd();
  delay(100);
  initNeoPixel();

  // WiFi setup
  showStatus("Try connect to WLAN", "    Please Wait     ");
  connectToWiFi();
  showStatus("WiFi: " + getWifiSSID(), "IP: " + getWifiIP());
  Serial.printf("SSID: %s\n", getWifiSSID().c_str());
  Serial.printf("IP Address: %s\n", getWifiIP().c_str());
  Serial.printf("MAC Address: %s\n", getWifiMAC().c_str());
  delay(3000);

  // MQTT setup
  showStatus("Connecting to MQTT", "    Please Wait     ");
  initMqtt();
  showStatus("MQTT connected:", MY_MQTT_BROKER);
  delay(3000);

  // Final ready status
  showStatus("Ready to Start", "Press Race Button");
}

void loop() {
  // put your main code here, to run repeatedly:
  ensureWiFiConnected();
  loopMqtt();
  loopButton();
  loopRace();
}
