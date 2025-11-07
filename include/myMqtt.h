/**
 * MyMqtt.h
 * Benjamin Hartmann | 11/2025
 */

#ifndef _MY_MQTT_H_
#define _MY_MQTT_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "MqttCredentials.h"
#include "myWifi.h"

// MQTT Topics and Settings
#define MQTT_ALL_TOPICS "Open_LED_Race/#"  // Subscribe to all subtopics
#define MQTT_LWT_TOPIC "Open_LED_Race/WiFiStatus"  // Last Will & Testament
#define MQTT_DEVICE_NAME_TOPIC "Open_LED_Race/DeviceName"
#define MQTT_DEVICE_PLACE_TOPIC "Open_LED_Race/DevicePlace"
#define MQTT_WIFI_SSID_TOPIC "Open_LED_Race/WifiSsid"
#define MQTT_WIFI_IP_TOPIC "Open_LED_Race/WifiIp"
#define MQTT_MAC_TOPIC "Open_LED_Race/MacAddress"
#define MQTT_DEBUG_TOPIC "Open_LED_Race/Debug"

#define MQTT_RACE_SETTING_TOPIC "Open_LED_Race/RaceSetting"
#define MQTT_RACE_STATUS_TOPIC "Open_LED_Race/RaceStatus"
#define MQTT_RACE_RUNNING_TOPIC "Open_LED_Race/RaceRunning"
#define MQTT_RACE_WINNER_TOPIC "Open_LED_Race/RaceWinner"
#define MQTT_RACE_WINNER_TIME_TOPIC "Open_LED_Race/RaceWinnerTime"


#define DEVICE_NAME "Benjamin's D1 Mini ESP8266"
#define DEVICE_PLACE "Benjamin's Zimmer"

#define QOS 1       // Quality of Service Level
#define RETAIN true // retained message

// Create a unique client ID
String clientId = "ESP-" + String(ESP.getChipId()) + "-" + String(random(0xffff), HEX);

WiFiClient espClient;
PubSubClient client(espClient);

// unsigned long lastMsgTime = 0;
// #define MSG_BUFFER_SIZE  (50)
// char msg[MSG_BUFFER_SIZE];
// String topic;
// String messageTemp;
// String messageMQTT;
// String messageError;
// bool   newMessage = false;    // Message income

/**
 * Send initial MQTT messages (LWT, device info, WiFi info).
 */
void sendInitMessages() {
  client.publish(MQTT_LWT_TOPIC, "online", RETAIN);
  client.publish(MQTT_DEVICE_NAME_TOPIC, DEVICE_NAME, RETAIN);
  client.publish(MQTT_DEVICE_PLACE_TOPIC, DEVICE_PLACE, RETAIN);
  client.publish(MQTT_WIFI_SSID_TOPIC, WiFi.SSID().c_str(), RETAIN);
  client.publish(MQTT_WIFI_IP_TOPIC, WiFi.localIP().toString().c_str(), RETAIN);
  client.publish(MQTT_DEBUG_TOPIC, "MQTT Client Initialized", RETAIN);

  client.publish(MQTT_RACE_STATUS_TOPIC, "Stopped", RETAIN);
}

/**
 * Connect to the MQTT broker.
 * This function is blocking.
 */
void connect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId.c_str(), MY_MQTT_USERNAME, MY_MQTT_PASSWORD, MQTT_LWT_TOPIC, QOS, RETAIN, "offline")) {
      Serial.printf(" - Connected to MQTT Broker: %s:%d\n", MY_MQTT_BROKER, MY_MQTT_PORT);

      client.subscribe(MQTT_ALL_TOPICS); // Subscribe to all interesting topics
      sendInitMessages();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/**
 * MQTT message callback function.
 */
void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, MQTT_ALL_TOPICS) == 0) { // if topic matches
    Serial.printf("Message arrived [%s]: ", topic);
    for (unsigned int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
  }
}

/**
 * Publish WiFi information (SSID, IP, MAC) via MQTT.
 */
void publishWiFi() {
  client.publish(MQTT_WIFI_SSID_TOPIC, getWifiSSID().c_str(), RETAIN);
  client.publish(MQTT_WIFI_IP_TOPIC, getWifiIP().c_str(), RETAIN);
  client.publish(MQTT_MAC_TOPIC, getWifiMAC().c_str(), RETAIN);
}

/**
 * @brief Sends race data via MQTT
 */
void publishRaceData(unsigned long player1Time, unsigned long player2Time, unsigned long player3Time,
                     int player1_steps, int player2_steps, int player3_steps) {
  JsonDocument out;
  out["P1_Time_ms"] = player1Time;
  out["P2_Time_ms"] = player2Time;
  out["P3_Time_ms"] = player3Time;

  out["P1_Steps"] = player1_steps;
  out["P2_Steps"] = player2_steps;
  out["P3_Steps"] = player3_steps;

  const size_t capacity = measureJson(out) + 1;
  char char_array_out[capacity];
  serializeJson(out, char_array_out, capacity);
  client.publish(MQTT_RACE_RUNNING_TOPIC, char_array_out, RETAIN);
}

void publishWinner(int winnerNumber, unsigned long playerTime) {
  client.publish(MQTT_RACE_STATUS_TOPIC, "Finish", RETAIN);
  client.publish(MQTT_RACE_WINNER_TOPIC, String(winnerNumber).c_str(), RETAIN);
  client.publish(MQTT_RACE_WINNER_TIME_TOPIC, String(playerTime).c_str(), RETAIN);
}

void publishStatus(const char* status) {
  client.publish(MQTT_RACE_STATUS_TOPIC, status, RETAIN);
}

/**
 * Initialize MQTT client and connect to the broker.
 */
void initMqtt() {
  publishWiFi();
  publishRaceData(0, 0, 0, 0, 0, 0);
  client.setServer(MY_MQTT_BROKER, MY_MQTT_PORT);
  client.setCallback(callback);

  connect();
}

/**
 * Maintain MQTT connection and process incoming messages.
 */
void loopMqtt() {
  // Ensure the client is connected
  if (!client.connected()) {
    connect();
  }

  // Maintain MQTT connection
  client.loop();
}

#endif  // _MY_MQTT_H_
