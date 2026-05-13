/**
 * MyMqtt.h
 * Benjamin Hartmann | 10/2025
 */

#ifndef _MY_MQTT_H_
#define _MY_MQTT_H_

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "MqttCredentials.h"

#define QOS 1       // Quality of Service Level
#define RETAIN true // retained message

class MyMqtt {
private:
  unsigned long lastMsg = 0;
  char msg[50];
  String topic;
  String messageTemp;
  String messageMQTT;
  String messageError;
  bool newMessage = false; // Message income

  String _clientId =
      "ESP-" + String(ESP.getChipId()) + "-" + String(random(0xffff), HEX);
  WiFiClient _wifi;
  PubSubClient _client = PubSubClient(_wifi);

  String _allTopics;
  String _lwtTopic;
  String _deviceNameTopic;
  String _devicePlaceTopic;
  String _wifiSsidTopic;
  String _wifiIpTopic;
  String _wifiMacTopic;
  String _debugTopic;

  String _raceSettingTopic;
  String _raceStatusTopic;
  String _raceDataTopic;
  String _raceWinnerTopic;
  String _raceWinnerTimeTopic;
  String _errorMessageTopic;

  String _deviceName;
  String _devicePlace;

  /**
   * Connect to the MQTT broker.
   * This function is blocking.
   */
  void connect() {
    // Loop until we're reconnected
    while (!_client.connected()) {
      Serial.print("Attempting MQTT connection...");

      // Attempt to connect
      if (_client.connect(_clientId.c_str(), MY_MQTT_USERNAME, MY_MQTT_PASSWORD,
                          _lwtTopic.c_str(), QOS, RETAIN, "offline")) {
        Serial.printf(" - Connected to MQTT Broker: %s:%d\n", MY_MQTT_BROKER,
                      MY_MQTT_PORT);
        _client.subscribe(_allTopics.c_str());

        _client.publish(_lwtTopic.c_str(), "online", RETAIN);
        _client.publish(_deviceNameTopic.c_str(), _deviceName.c_str(), RETAIN);
        _client.publish(_devicePlaceTopic.c_str(), _devicePlace.c_str(),
                        RETAIN);
      } else {
        Serial.printf("failed, rc=%d, trying again in 5 seconds\n",
                      _client.state());
        delay(5000);
      }
    }
  }

public:
  MyMqtt(String deviceName, String devicePlace, String topicBase)
      : _allTopics(topicBase + "#"), _lwtTopic(topicBase + "wifiStatus"),
        _deviceNameTopic(topicBase + "deviceName"),
        _devicePlaceTopic(topicBase + "devicePlace"),
        _wifiSsidTopic(topicBase + "wifiSsid"),
        _wifiIpTopic(topicBase + "wifiIp"),
        _wifiMacTopic(topicBase + "wifiMac"), _debugTopic(topicBase + "debug"),
        _raceStatusTopic(topicBase + "raceStatus"),
        _raceDataTopic(topicBase + "raceData"),
        _raceWinnerTopic(topicBase + "raceWinner"),
        _raceWinnerTimeTopic(topicBase + "raceWinnerTime"),
        _errorMessageTopic(topicBase + "errorMessage"), _deviceName(deviceName),
        _devicePlace(devicePlace) {}

  /**
   * Initialize MQTT client and connect to broker.
   */
  void begin() {
    _client.setServer(MY_MQTT_BROKER, MY_MQTT_PORT);
    _client.setCallback(
        [this](const char *topic, const byte *payload, unsigned int length) {
          // If message is for a subscribed topic, print it.
          if (strcmp(topic, _allTopics.c_str()) == 0) {
            Serial.print("Message arrived [");
            Serial.print(topic);
            Serial.print("] ");
            for (unsigned int i = 0; i < length; i++) {
              Serial.print((char)payload[i]);
            }
            Serial.println();
          }
        });

    connect();
  }

  /**
   * Maintain MQTT connection and process incoming messages.
   */
  void loop() {
    _client.loop();
  }

  /**
   * @brief Ensure that the MQTT client is connected to the broker. If not, attempt to reconnect.
   * 
   */
  void ensureConnected() {
    if (!_client.connected()) {
      connect();
    }
  }

  /**
   * @brief Publish WiFi data to MQTT broker.
   *
   * @param ssid SSID to be published
   * @param ip IP address to be published
   * @param mac MAC address to be published
   */
  void publishWifiData(String ssid, String ip, String mac) {
    _client.publish(_wifiSsidTopic.c_str(), ssid.c_str(), RETAIN);
    _client.publish(_wifiIpTopic.c_str(), ip.c_str(), RETAIN);
    _client.publish(_wifiMacTopic.c_str(), mac.c_str(), RETAIN);
  }

  /**
   * @brief Publish race status to MQTT broker.
   *
   * @param status Race status to be published
   * @param time Current time
   * @param pos1 Position of player 1
   * @param pos2 Position of player 2
   * @param pos3 Position of player 3
   */
  void publishRaceStatus(String status, unsigned long time, int pos1, int pos2,
                         int pos3) {
    _client.publish(_raceStatusTopic.c_str(), status.c_str(), RETAIN);

    JsonDocument doc;

    doc["time"] = time;
    doc["pos1"] = pos1;
    doc["pos2"] = pos2;
    doc["pos3"] = pos3;

    String res;
    serializeJson(doc, res);

    _client.publish(_raceDataTopic.c_str(), res.c_str());
  }

  /**
   * @brief Publish race winner to MQTT broker.
   *
   * @param player Name of the winning player
   * @param time Time when the race was won
   */
  void publishRaceWinner(String player, unsigned long time) {
    _client.publish(_raceWinnerTopic.c_str(), player.c_str(), RETAIN);
    _client.publish(_raceWinnerTimeTopic.c_str(), String(time).c_str(), RETAIN);
  }

  bool isConnected() {
    return _client.connected();
  }
};

#endif // _MY_MQTT_H_
