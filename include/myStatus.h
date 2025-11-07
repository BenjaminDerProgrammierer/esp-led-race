/**
 * MyStatus.h
 * Benjamin Hartmann | 11/2025
 */

#ifndef _MY_STATUS_H_
#define _MY_STATUS_H_

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>


#define LCD_I2C_ADDR 0x27
#define LCD_COLUMNS 20
#define LCD_ROWS 4

LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLUMNS, LCD_ROWS);

/**
 * @brief Initializes the LCD
 */
void initLcd() {
  lcd.begin();
  lcd.backlight();
}

/**
 * @brief Displays status messages on the LCD
 *
 * @param firstLine The 3rd line of the LCD
 * @param secondLine The 4th line of the LCD
 */
void showStatus(String firstLine, String secondLine) {
  lcd.clear();
  lcd.home();
  lcd.print("     Welcome to     ");
  lcd.setCursor(0, 1);
  lcd.print("   Open-LED-Race!   ");
  lcd.setCursor(0, 2);
  lcd.print(firstLine);
  lcd.setCursor(0, 3);
  lcd.print(secondLine);
}


/**
 * @brief Displays the status when the race is stopped
 * 
 */
void showRaceStoppedStatus() {
  lcd.clear();
  lcd.home();
  lcd.print("   Open-LED-Race    ");
  lcd.setCursor(0, 1);
  lcd.print("    Race stopped    ");
  lcd.setCursor(0, 2);
  lcd.print(" Press Start Button ");
  // lcd.setCursor(0, 3);
  // lcd.print("                   ");
}

/**
 * @brief Displays the status when the race is being prepared
 * 
 */
void showPrepareRaceStatus() {
  lcd.clear();
  lcd.home();
  lcd.print("   Open-LED-Race   ");
  lcd.setCursor(0, 1);
  lcd.print("  Prepare for the  ");
  lcd.setCursor(0, 2);
  lcd.print("       Race!       ");
//   lcd.setCursor(0, 3);
//   lcd.print("                   ");
}


/**
 * @brief Displays the status during the game
 *
 * @param playerTime The elapsed time of the players
 * @param player1Steps The number of steps taken by player 1
 * @param player2Steps The number of steps taken by player 2
 * @param player3Steps The number of steps taken by player 3
 */
void showGameStatus(unsigned long playerTime, int player1Steps, int player2Steps,
                    int player3Steps) {
  lcd.clear();
  lcd.home();
  lcd.print("   Open-LED-Race   ");
  lcd.setCursor(0, 1);
  lcd.print(" Time: ");
  lcd.print(playerTime);
  lcd.print(" ms");
  lcd.setCursor(0, 2);
  lcd.print(" P1:");
  lcd.print(player1Steps);
  lcd.print(" P2:");
  lcd.print(player2Steps);
  lcd.setCursor(0, 3);
  lcd.print(" P3:");
  lcd.print(player3Steps);
}

/**
 * @brief Displays the winner of the race
 *
 * @param winnerNumber The number of the winning player
 */
void showWinner(int winnerNumber) {
  lcd.clear();
  lcd.home();
  lcd.print("   Open-LED-Race   ");
  switch (winnerNumber) {
    case 1:
      lcd.setCursor(0, 1);
      lcd.print("1: Winner!        ");
      lcd.setCursor(0, 2);
      lcd.print("2: Sorry, you lost.");
      lcd.setCursor(0, 3);
      lcd.print("3: Sorry, you lost.");
      break;
    case 2:
      lcd.setCursor(0, 1);
      lcd.print("1: Sorry, you lost.");
      lcd.setCursor(0, 2);
      lcd.print("2: Winner!        ");
      lcd.setCursor(0, 3);
      lcd.print("3: Sorry, you lost.");
      break;
    case 3:
      lcd.setCursor(0, 1);
      lcd.print("1: Sorry, you lost.");
      lcd.setCursor(0, 2);
      lcd.print("2: Sorry, you lost.");
      lcd.setCursor(0, 3);
      lcd.print("3: Winner!        ");
      break;
    default:
      lcd.setCursor(0, 1);
      lcd.print("No winner!       ");
      lcd.setCursor(0, 2);
      lcd.print("                 ");
      lcd.setCursor(0, 3);
      lcd.print("                 ");
      break;
  }
}

#endif  // _MY_STATUS_H_