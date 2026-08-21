#ifndef MELODIES_H
#define MELODIES_H

#include <Arduino.h>

// --- Noten-Frequenzen ---
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_D5 587
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_G5 784
#define NOTE_A5 880
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_D6 1175

// --- Melodie-Funktionen ---
// Das Schlüsselwort "inline" verhindert Compiler-Fehler bei mehrfacher Einbindung

inline void playSuccessMelody(uint8_t buzzerPin)
{
  int melodies[] = {NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6};
  int duration[] = {8, 8, 8, 2};

  for (int i = 0; i < 4; i++)
  {
    int noteDuration = 1000 / duration[i];
    tone(buzzerPin, melodies[i], noteDuration);
    int pause = noteDuration * 1.30;
    delay(pause); // TODO: Später auf millis() umbauen für die Note 1
    noTone(buzzerPin);
  }
}

inline void playErrorMelody(uint8_t buzzerPin)
{
  int melodies[] = {NOTE_G5, NOTE_C5};
  int duration[] = {4, 2};

  for (int i = 0; i < 2; i++)
  {
    int noteDuration = 1000 / duration[i];
    tone(buzzerPin, melodies[i], noteDuration);
    int pause = noteDuration * 1.30;
    delay(pause);
    noTone(buzzerPin);
  }
}

inline void playReminderMelody(uint8_t buzzerPin)
{
  // Kurzes, doppeltes Piepen (nervt nicht zu sehr)
  tone(buzzerPin, NOTE_C6, 100);
  delay(150);
  tone(buzzerPin, NOTE_C6, 100);
  delay(150);
  noTone(buzzerPin);
}

// Ruhiger, schwebender Glockenton - gut fürs Aufwachen/sanfte Reminder
// ~3.9s, Start (C5) und Ende (E5) liegen nah beieinander -> loopt sanft
inline void playSmoothMelody(uint8_t buzzerPin)
{
  int melodies[] = {NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6, NOTE_G5, NOTE_E5,
                    NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6, NOTE_G5, NOTE_E5};
  int duration[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

  for (int i = 0; i < 12; i++)
  {
    int noteDuration = 1000 / duration[i];
    tone(buzzerPin, melodies[i], noteDuration);
    int pause = noteDuration * 1.30;
    delay(pause);
    noTone(buzzerPin);
  }
}

// Verspielte, hüpfende Melodie mit Dur-Dreiklängen - für positive Events
// ~3.25s, endet auf C5, gleicher Ton wie Start -> loopt nahtlos
inline void playPlayfulMelody(uint8_t buzzerPin)
{
  int melodies[] = {NOTE_C5, NOTE_E5, NOTE_D5, NOTE_F5,
                    NOTE_E5, NOTE_G5, NOTE_F5, NOTE_A5,
                    NOTE_G5, NOTE_C5};
  int duration[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

  for (int i = 0; i < 10; i++)
  {
    int noteDuration = 1000 / duration[i];
    tone(buzzerPin, melodies[i], noteDuration);
    int pause = noteDuration * 1.30;
    delay(pause);
    noTone(buzzerPin);
  }
}

// Schnelles, dringliches Hin-und-Her - für "jetzt aber wirklich"-Reminder
// ~3.25s, alterniert zwischen zwei Tönen -> loopt problemlos
inline void playUrgentMelody(uint8_t buzzerPin)
{
  int melodies[] = {NOTE_C6, NOTE_G5, NOTE_C6, NOTE_G5,
                    NOTE_C6, NOTE_G5, NOTE_C6, NOTE_G5,
                    NOTE_C6, NOTE_G5, NOTE_C6, NOTE_G5,
                    NOTE_C6, NOTE_G5, NOTE_C6, NOTE_G5,
                    NOTE_C6, NOTE_G5, NOTE_C6, NOTE_G5};
  int duration[] = {8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
                    8, 8, 8, 8, 8, 8, 8, 8, 8, 8};

  for (int i = 0; i < 20; i++)
  {
    int noteDuration = 1000 / duration[i];
    tone(buzzerPin, melodies[i], noteDuration);
    int pause = noteDuration * 1.30;
    delay(pause);
    noTone(buzzerPin);
  }
}

// Düster-mysteriöse, absteigende Melodie in Moll-Anmutung
// ~3.9s, Start (C6) und Ende (C4) sind Oktaven voneinander -> loopt harmonisch
inline void playMysteriousMelody(uint8_t buzzerPin)
{
  int melodies[] = {NOTE_C6, NOTE_A5, NOTE_F5, NOTE_D5,
                    NOTE_C5, NOTE_A4, NOTE_F4, NOTE_D4,
                    NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4};
  int duration[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

  for (int i = 0; i < 12; i++)
  {
    int noteDuration = 1000 / duration[i];
    tone(buzzerPin, melodies[i], noteDuration);
    int pause = noteDuration * 1.30;
    delay(pause);
    noTone(buzzerPin);
  }
}

// Kleine Fanfare - für "Streak erreicht" oder Meilensteine
// ~4.55s, endet mit langem Schlusston, Start & Ende beide C5/C6-Familie -> loopt gut
inline void playFanfareMelody(uint8_t buzzerPin)
{
  int melodies[] = {NOTE_C5, NOTE_C5, NOTE_C5, NOTE_G5,
                    NOTE_E5, NOTE_G5, NOTE_C6};
  int duration[] = {8, 8, 8, 4, 4, 4, 2};

  for (int i = 0; i < 7; i++)
  {
    int noteDuration = 1000 / duration[i];
    tone(buzzerPin, melodies[i], noteDuration);
    int pause = noteDuration * 1.30;
    delay(pause);
    noTone(buzzerPin);
  }
}

#endif // MELODIES_H