/**
 * MyDisplay.h
 * Benjamin Hartmann | 12/2025
 */

#ifndef _MY_DISPLAY_H_
#define _MY_DISPLAY_H_

#include "Arduino.h"
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

#define LCD_COLS 20
#define LCD_ROWS 4

class MyDisplay {
private:
  hd44780_I2Cexp lcd;
  String model;
  String version;

public:
  MyDisplay(const char *model, const char *version)
      : model(model), version(version) {}

  /**
   * @brief Initializes the LCD display. If initialization fails, the function
   * will not return and the onboard LED will blink an error code.
   */
  void begin() {
    int status = lcd.begin(LCD_COLS, LCD_ROWS);
    if (status) {
      hd44780::fatalError(status);
    }
  }

  /**
   * @brief Displays a status message on the LCD.
   *
   * @param status The status message to be displayed on the LCD.
   */
  void showStatus(const char *status) { showStatus(status, "", ""); }

  /**
   * @brief Displays a status message on the LCD with two additional lines of
   * text.
   *
   * @param status The status message to be displayed on the LCD.
   * @param line2 A second line of text to be displayed on the LCD.
   * @param line3 A third line of text to be displayed on the LCD.
   */
  void showStatus(const char *status, const char *line2, const char *line3) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(status);
    lcd.setCursor(0, 1);
    lcd.print(line2);
    lcd.setCursor(0, 2);
    lcd.print(line3);
    lcd.setCursor(0, 3);
    lcd.printf("%s %s", model.c_str(), version.c_str());
  }

  void showRaceStatus(int pos1, int pos2, int pos3) {
    lcd.setCursor(10, 0);
    lcd.print(pos1);
    lcd.setCursor(10, 1);
    lcd.print(pos2);
    lcd.setCursor(10, 2);
    lcd.print(pos3);
  }
};
#endif // _MY_DISPLAY_H_
