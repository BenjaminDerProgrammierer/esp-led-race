# Open LED Race

## Old Code Snippets

`readme.txt`

```plaintext
04.01.2025
read_me zu LED_Race_v4
bis zu 3 Spieler können Open LED Race spielen. (rot, grün, blau)



const int PIX_PIN    = 0;           // GPIO 0 funktioniert
const int BUZZER_PIN = 15;
const int START_PIN  = 12;
const int P1_PIN     = 13;  mit Interrupt-Einstellung
const int P2_PIN     = 14;  mit Interrupt-Einstellung
const int P3_PIN     = 2;  mit Interrupt-Einstellung

// NeoPixel
const int NUMPIXELS = 85;      // Anzahl der Leds auf Neopixel
const int NEOPIXEL_TYPE = 60;   // 60 pro m
const int CAR  = 4;            // 4 Led sind ein Player
const int START_POS = 10;      // erste LED auf der Strecke ist Nr. 10

const int brightness = 50;

//---- MQTT Broker settings
const char* mqtt_server   = htl_mqtt_broker;
const char* mqtt_username = htl_mqtt_username;
const char* mqtt_password = htl_mqtt_password;
const int   mqtt_port     = htl_mqtt_port;

const char* mqtt_main_Topic   = "Open_LED_Race/";
const char* mqtt_LWT_Topic    = "Open_LED_Race/WiFiStatus";   // online, offline
//const char* mqtt_Race_Setting = "Open_LED_Race/RaceSetting"; // wird nicht verwendet
const char* mqtt_Status_Topic = "Open_LED_Race/RaceStatus";   // Prepare4Race, Running, Finish, Stopped
const char* mqtt_Race_Topic   = "Open_LED_Race/RaceRunning";  // JSON Ausgabe von Spielzeit und Spieler-Schritte
const char* mqtt_Winner_Topic = "Open_LED_Race/RaceWinner";  // Anzeige vom Gewinner
const char* mqtt_Winner_Time  = "Open_LED_Race/RaceWinnerTime";  // Anzeige der Gewinner Zeit
const char* mqtt_Error_Msg    = "Open_LED_Race/Error_Message"; // Anzeige Verbindungsabbruch und reconnect

******************************************************************************************************************
setup:

Wifi Verbindung wird hergestellt  Verbindungsdaten in myWifi_Init.h
MQTT Verbindung wird hergestellt  Verbindungsdaten in myWifi_Init.h
LCD Anzeige wird aktiviert

MQTT Aktualisierung
 Open_LED_Race/RaceRunning JSON
 Open_LED_Race/RaceStatus Finish
 Open_LED_Race/WiFiStatus";   // online, offline wird als Last-Will-Testament aktiviert

******************************************************************************************************************
main loop:
 
 Wird die Start-Taste gedrückt:
  Es läuft die Startsequenz
  Open_LED_Race/RaceStatus Prepare4Race
  dann startet das Spiel und die Zeit läuft
  MQTT Aktualisierung
   Open_LED_Race/RaceStatus Running
   alle 2 Sekunden werden die Spielerdaten übermittelt
    Open_LED_Race/RaceRunning 
      out["P1_Time_ms"] = player1Time; Zeit in ms
      out["P2_Time_ms"] = player2Time;
      out["P3_Time_ms"] = player3Time;

      out["P1_Steps"] = player1_steps; registrierte Steps
      out["P2_Steps"] = player2_steps;
      out["P3_Steps"] = player3_steps;


 Wenn der erste Spieler das Ziel (die vorgegebenen Steps) erricht, dann
  wird die Spielerzeit gespeichert
  das Spiel wird gestoppt 
  die LCD aktualisiert
  MQTT Aktualisierung
   Open_LED_Race/RaceStatus Finish
   Open_LED_Race/RaceRunning letzte Spielerdaten
   Open_LED_Race/RaceWinner der Sieger wird zusätzlich ausgegeben
   Open_LED_Race/RaceWinnerTime die Siegerzeit wird zusätzlich ausgegeben


 Wird die Start-Taste während dem Spiel gedrückt:
  das Spiel wird gestoppt
  LCD aktualisiert
  Spiel zurücksetzen
  MQTT Aktualisierung
   Open_LED_Race/RaceStatus Stopped
   
  Beim erneuten drücken der Start-Taste wird die Startsequenz gestartet.
  
******************************************************************************************************************
```

## Secrets

You'll need to create a `MqttCredentials.h` and `WiFiCredentials.h` file in the `include` folder with the following content:

```cpp
// MqttCredentials.h
#define MY_MQTT_BROKER "test.mosquitto.org"
#define MY_MQTT_PORT 1883
#define MY_MQTT_USERNAME ""
#define MY_MQTT_PASSWORD ""

// WiFiCredentials.h
#define MY_SSID "ssid"
#define MY_PASSWORD "password"
```
