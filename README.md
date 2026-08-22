# Semesterprojekt SS26 PC & TUI: Habit Companion

Habit Companion ist ein ESP32 Projekt, welches dabei hilft, Ziele effizienter zu erreichen durch aktives Nachgehen von Gewohnheiten. Der Habit Companion unterstützt dabei den Benutzer über eine Weboberfläche, wo Habits angelegt und gelöscht werden können und die Streak kann miteingesehen werden.

## Eingebaute Hardware Komponenten

| Komponente          | Funktion                                                                         | Pin Belegung                   |
| ------------------- | -------------------------------------------------------------------------------- | ------------------------------ |
| **OLED-Display**    | Anzeige der Habit Übersicht und des aktuellen Status                             | `SDA: 32` / `SCL: 33`          |
| **Drehencoder**     | Navigation durch Drehen und Auswahl durch Drücken                                | `CLK: 18` / `DT: 19` / `SW: 5` |
| **Passiver Buzzer** | Akustische Erinnerung mit individuell auswählbaren Melodien (derzeit 5 Optionen) | `Signal: 13`                   |
| **RGB-LED**         | Visuelle Darstellung des aktuellen Status durch unterschiedliche Farben          | `R: 25` / `G: 26` / `B: 27`    |

## Funktionsweise

Der ESP32 wird als Access Point freigeschalten und der Benutzer kann sich per WLAN mit dem Gerät verbinden. Die zugehörige Webseite kann unter **_192.168.4.1_** im Browser aufgerufen werden. Der Nutzer kann dort anschließend Habits erstellen und in einer Liste hinzufügen.

Die Habits bestehen aus _Titel, Uhrzeit, Melodie_. Wird beispielsweise 14:00 eingetragen bei einer Habit, dann wird um 14:00 die Erinnerungsmelodie abgespielt, um den Nutzer daran zu erinnern, seine Gewohnheit nachzugehen. 15 Minuten vorher ändert sich die Displayanzeige und zeigt einen Countdown an, wann die nächste Habit gemacht werden soll.

Oftmals kommt allerdings der Alltag dazwischen und man ist nicht in der Lage, jeden Tag um XY Uhr bspw. Sport zu machen. Deshalb hat man bis Mitternacht die Möglichkeit, die Habit zu erledigen, um etwas Flexibilität zu ermöglichen. Um die Regelmäßigkeit des Ausführens interessanter zu gestalten, gibt es für jede Habit ein Streaksystem. Wenn eine Habit z.B. 3 Tage in Folge erledigt wurde, werden 3 Flammen angezeigt bei der Handy Übersicht.

## Principles of Discoverability

### Afforanzen

Der Rotary Encoder bietet dabei zwei getrennte Interaktionsmöglichkeiten: **Drehen** zum wechseln der Displayübersicht und **Drücken** zum erledigen einer Habit.

### Signifier

Die Anzeige auf dem OLED Display zeigt die aktuell ausgewählte Habit und deren Status an. Dadruch wird ersichtlich, welche Habit gerade ausgewählt ist und welche Aktion mit dem Drücken ausgelöst wird.

### Constraint

Die Bedienung ist bewusst auf ein Bedienelement reduziert. Dadurch bleiben die möglichen Aktionen überschaubar. Der relativ schwergängige Drückpunkt des Encoders reduziert außerdem das Risiko einer unbeabsichtigten Bestätigung.

### Mapping

Das Drehen des Encoders entspricht dem Durchblättern der Habits, während das Drücken der Aktion "erledigt" zugeordnet ist. Die Reihenfolge der anzeigten Habits entspricht dabei der Reihenfolge, durch die mit dem Encoder navigiert wird.

### Feedback

Eine blaues Leuchten der LED signalisiert, dass eine Habit aktuell fällig ist. Beim erfolgreichen Erledigen wechselt die LED auf Grün. Zusätzlich geben eine Erinnerungsmelodie, welche individuell eingestellt werden kann je nach Habit, und eine Erfolgsmelodie akustisches Feedback.

# TODOS

- [ ] Knob für Rotary Encoder drucken
- [ ] 3D Modell - Einkerbung für Rotary Encoder und LED und Display lassen
- [ ] Modell drucken
- [ ] Showreel eintragen mit Video
