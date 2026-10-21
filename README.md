# Habit Companion
***
Habit Companion is an ESP32 project that helps achieve goals more efficiently by actively tracking habits. The Habit Companion supports the user via a web interface where habits can be created and deleted, and the streak can be viewed.

## Built-in Hardware Components
***
| Component           | Function                                                                         | Pin Assignment                   |
| ------------------- | -------------------------------------------------------------------------------- | ------------------------------ |
| **OLED-Display**    | Displays the habit overview and current status                                   | `SDA: 32` / `SCL: 33`          |
| **Rotary Encoder**  | Navigation by turning and selection by pressing                                  | `CLK: 18` / `DT: 19` / `SW: 5` |
| **Passive Buzzer**  | Acoustic reminder with individually selectable melodies (currently 5 options)    | `Signal: 13`                   |
| **RGB-LED**         | Visual representation of the current status using different colors.              | `R: 25` / `G: 26` / `B: 27`    |

## Funktionsweise
***
The ESP32 is set up as an Access Point, allowing the user to connect to the device via Wi-Fi. The corresponding website can be accessed in the browser at **_192.168.4.1._** The user can then create habits there and add them to a list.

The habits consist of a _title_, _time_, and _melody_. For example, if 14:00 is entered for a habit, the reminder melody will play at 14:00 to remind the user to perform their habit. 15 minutes beforehand, the display changes to show a countdown until the next habit is due.

However, everyday life often gets in the way, and it may not be possible to exercise at a specific time every day, for instance. Therefore, you have until midnight to complete the habit, allowing for some flexibility. To make the regularity of execution more engaging, there is a streak system for each habit. For example, if a habit is completed 3 days in a row, 3 flames are displayed on the mobile overview.
