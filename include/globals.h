#ifndef GLOBALS_H
#define GLOBALS_H
#include "config.h"
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;
extern Habit habits[];
extern int habitCount;
extern int currentHabitIndex;
extern int countdownHabitIndex;
extern int countdownMinutes;
extern DeviceState currentState;

#endif