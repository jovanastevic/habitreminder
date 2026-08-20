#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- Pin Definitionen ---
const int PIN_R = 25; // led
const int PIN_G = 26;
const int PIN_B = 27;
const int PIN_BUZZER = 13;  // macht sound um abartig zu nerven
const int PIN_ENC_CLK = 18; // rotary encoder
const int PIN_ENC_DT = 19;
const int PIN_ENC_SW = 5;
const int I2C_SDA = 32; // oled display
const int I2C_SCL = 33;
const int PIN_BUTTON = 4; // mal schauen ob ich das einbaue

// --- OLED Konfiguration ---
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_RESET = -1;

// --- State Machine ---
enum DeviceState
{
  STATE_INIT,
  STATE_IDLE,
  STATE_MENU,
  STATE_COUNTDOWN,
  STATE_REMINDER,
  STATE_FEEDBACK
};

// --- Habit Datenstruktur ---
// struct Habit {
//   long id;
//   String name;
//   String time;          // z.B. "14:30"
//   String melody;
//   bool completedToday;
//   int currentStreak;
//   long lastCompletedDay;
//   bool reminderTriggered;
// };

struct Habit
{
  long id;
  String name;
  String time; // z. B. "14:30" (für Anzeige & Web-API)
  int hour;    // einmalig aus "time" geparst -> spart Arbeit im loop()
  int minute;
  String melody;          // "Success", "Reminder", "Error"
  bool completedToday;    // wird um Mitternacht zurückgesetzt
  int currentStreak;      // wie viele Tage in Folge erledigt?
  long lastCompletedDay;  // Tag seit 1970 (siehe getCurrentDay())
  bool reminderTriggered; // wurde heute schon erinnert?
};

// --- habit limit ---
const int MAX_HABITS = 6;

#endif