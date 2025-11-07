/**
 * MyWifi.h
 * Benjamin Hartmann | 11/2025
 */

#ifndef _MY_WIFI_H_
#define _MY_WIFI_H_

#include <ESP8266WiFi.h>

#include "WifiCredentials.h"

#define WIFI_TIMEOUT 30000  // Time in ms to check WiFi connection
unsigned long lastWifiCheckTime = 0;

// Option for static IP
// #define STATIC_IP
#ifdef STATIC_IP
IPAddress  local_IP(192, 168, 0, 152);
IPAddress  gateway(192, 168, 0, 1);
IPAddress  subnet(255, 255, 255, 0);
IPAddress  dns(192, 168, 0, 1);
#endif

/**
 * Connect to a WiFi network using the predefined credentials from
 * ``WifiCredentials.h``.
 */
void connectToWiFi() {
  WiFi.begin(MY_WIFI_SSID, MY_WIFI_PASSWORD);
#ifdef STATIC_IP
  WiFi.config(ip, gateway, subnet);
#endif
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" - Connected to WiFi!");

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

/**
 * Get the current WiFi connection status.
 * @return true if connected, false otherwise
 */
bool getWifiConnectedState() {
    return WiFi.status() == WL_CONNECTED;
}

/**
 * Get the current WiFi SSID.
 * @return The SSID of the connected WiFi network.
 */
String getWifiSSID() {
    return WiFi.SSID();
}

/**
 * Get the current WiFi IP address.
 * @return The IP address of the connected WiFi network.
 */
String getWifiIP() {
    return WiFi.localIP().toString();
}

/**
 * Get the current WiFi MAC address.
 * @return The MAC address of the WiFi interface.
 */
String getWifiMAC() {
    return WiFi.macAddress();
}

/**
 * Ensure the WiFi connection is active, reconnecting if necessary. To be placed
 * in the main loop.
 */
void ensureWiFiConnected() {
  unsigned long now = millis();
  if (now - lastWifiCheckTime > WIFI_TIMEOUT) {
    Serial.print("Checking WiFi... ");
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi connection lost. Reconnecting...");
      connectToWiFi();
    }
    lastWifiCheckTime = now;
  }
}

#endif  // _MY_WIFI_H_
