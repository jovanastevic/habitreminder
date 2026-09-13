#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include "globals.h"

void drawScreenIdle()
{
    int yOffset = (int)(sin(millis() / 400.0) * 3);

    display.setTextSize(2); // Große Schrift (12 Pixel breit pro Buchstabe)

    // "HABIT" hat 5 Buchstaben -> 60px breit. (128 - 60) / 2 = X: 34
    display.setCursor(34, 15 + yOffset);
    display.println("HABIT");

    // "COMPANION" hat 9 Buchstaben -> 108px breit. (128 - 108) / 2 = X: 10
    display.setCursor(10, 35 + yOffset);
    display.println("COMPANION");
}

void drawScreenMenu()
{
    if (habitCount == 0)
    {
        display.setCursor(0, 20);
        display.println("Keine Habits!");
        display.setCursor(0, 40);
        display.println("Handy verbinden.");
    }
    else
    {
        Habit &h = habits[currentHabitIndex];
        display.setCursor(0, 0);
        display.print("Habit ");
        display.print(currentHabitIndex + 1);
        display.print("/");
        display.println(habitCount);
        display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

        display.setTextSize(2);
        display.setCursor(0, 20);
        display.println(h.name);

        display.setTextSize(1);
        display.setCursor(0, 45);
        display.println(h.completedToday ? "[X] Schon erledigt" : "[ ] Druecke Button!");
    }
}

void drawScreenCountdown()
{
    display.setCursor(0, 10);
    display.println("Bald ist es soweit!");
    display.setTextSize(2);
    display.setCursor(0, 25);
    display.println(habits[countdownHabitIndex].name);
    display.setTextSize(1);
    display.setCursor(0, 50);
    display.print("In ");
    display.print(countdownMinutes);
    display.println(" Min");
}

void drawScreenReminder()
{
    display.setTextSize(2);
    display.setCursor(0, 10);
    display.println("ZEIT FUER:");
    display.setCursor(0, 35);
    display.println(habits[currentHabitIndex].name);
}

void drawScreenCompleted()
{
    display.setTextSize(2);
    display.setCursor(20, 15);
    display.println("STARK!");
    display.setTextSize(1);
    display.setCursor(20, 40);
    display.print("Streak: ");
    display.print(habits[currentHabitIndex].currentStreak);
    display.println(" Tage");
}

inline void updateDisplay()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    switch (currentState)
    {
    case STATE_IDLE:
        drawScreenIdle();
        break;
    case STATE_MENU:
        drawScreenMenu();
        break;
    case STATE_COUNTDOWN:
        drawScreenCountdown();
        break;
    case STATE_REMINDER:
        drawScreenReminder();
        break;
    case STATE_COMPLETED:
        drawScreenCompleted();
        break;
    default:
        break;
    }
    display.display();
}

#endif // DISPLAY_MANAGER_H