#ifndef MELODIES_H
#define MELODIES_H

#include <Arduino.h>

// --- Noten-Frequenzen ---
#define NOTE_C4  262
#define NOTE_G4  392
#define NOTE_C5  523
#define NOTE_E5  659
#define NOTE_G5  784
#define NOTE_C6  1047

// --- Melodie-Funktionen ---
// Das Schlüsselwort "inline" verhindert Compiler-Fehler bei mehrfacher Einbindung

inline void playSuccessMelody(uint8_t buzzerPin) {
  int melodies[] = {NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6};
  int duration[] = {8, 8, 8, 2};

  for (int i = 0; i < 4; i++) {
    int noteDuration = 1000 / duration[i];
    tone(buzzerPin, melodies[i], noteDuration);
    int pause = noteDuration * 1.30;
    delay(pause); // TODO: Später auf millis() umbauen für die Note 1
    noTone(buzzerPin);
  }
}

inline void playErrorMelody(uint8_t buzzerPin) {
  int melodies[] = {NOTE_G5, NOTE_C5};
  int duration[] = {4, 2};

  for (int i = 0; i < 2; i++) {
    int noteDuration = 1000 / duration[i];
    tone(buzzerPin, melodies[i], noteDuration);
    int pause = noteDuration * 1.30;
    delay(pause); 
    noTone(buzzerPin);
  }
}

inline void playReminderMelody(uint8_t buzzerPin) {
  // Kurzes, doppeltes Piepen (nervt nicht zu sehr)
  tone(buzzerPin, NOTE_C6, 100);
  delay(150);
  tone(buzzerPin, NOTE_C6, 100);
  delay(150);
  noTone(buzzerPin);
}

#endif // MELODIES_H