#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <sys/time.h> // Uhr über settimeofday()/gettimeofday() stellen/lesen
#include <Preferences.h>
#include <LittleFS.h>

#include "config.h"
#include "secrets.h"
#include "melodies.h"

// =====================================================================
// Globale Objekte
// =====================================================================
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WebServer server(80);
Preferences prefs; // for smth

// =====================================================================
// State-Machine- und Habit-Daten
// =====================================================================
DeviceState currentState = STATE_INIT;

Habit habits[MAX_HABITS];
int habitCount = 0;
int currentHabitIndex = 0;
long lastKnownDay = 0; // TODO:

int countdownHabitIndex = -1;
int countdownMinutes = 0;

// Rotary Encoder / Taster
int lastCLK = HIGH;
bool lastButtonState = HIGH;
unsigned long lastActivityTime = 0;
unsigned long feedbackStartTime = 0;
unsigned long lastBlinkTime = 0;
int brightness = 0;
int fadeAmount = 5;

// Funktion um Farbe der LED anzuzeigen
void setColor(int r, int g, int b)
{
  analogWrite(PIN_R, r);
  analogWrite(PIN_G, g);
  analogWrite(PIN_B, b);
}

// =====================================================================
// Zeit
// =====================================================================
// Tage seit 1970 anhand der internen Chip-Uhr (per Handy gesetzt, siehe handleSyncTime)

long getCurrentDay()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  if (tv.tv_sec < 1000000000)
    return 0; // Uhr wurde noch nicht vom Handy gestellt
  return tv.tv_sec / 86400L;
}

// "HH:MM" -> hour/minute als int, damit loop() das nicht bei jedem
// Durchlauf für jeden Habit neu parsen muss.
void parseTime(Habit &h)
{
  h.hour = h.time.substring(0, 2).toInt();
  h.minute = h.time.substring(3, 5).toInt();
}

// =====================================================================
// Persistenz (Flash über Preferences, als JSON-String)
// =====================================================================
void saveHabits()
{
  JsonDocument doc;
  JsonArray array = doc.to<JsonArray>();
  for (int i = 0; i < habitCount; i++)
  {
    JsonObject obj = array.add<JsonObject>();
    obj["id"] = habits[i].id;
    obj["name"] = habits[i].name;
    obj["time"] = habits[i].time;
    obj["melody"] = habits[i].melody;
    obj["completedToday"] = habits[i].completedToday;
    obj["currentStreak"] = habits[i].currentStreak;
    obj["lastCompletedDay"] = habits[i].lastCompletedDay;
    obj["reminderTriggered"] = habits[i].reminderTriggered;
  }
  String jsonStr;
  serializeJson(doc, jsonStr);
  prefs.putString("data", jsonStr);
}

void loadHabits()
{
  String jsonStr = prefs.getString("data", "[]");
  JsonDocument doc;
  deserializeJson(doc, jsonStr);

  habitCount = 0;
  for (JsonObject obj : doc.as<JsonArray>())
  {
    if (habitCount >= MAX_HABITS)
      break;
    Habit &h = habits[habitCount];
    h.id = obj["id"];
    h.name = obj["name"].as<String>();
    h.time = obj["time"].as<String>();
    h.melody = obj["melody"].as<String>();
    h.completedToday = obj["completedToday"];
    h.currentStreak = obj["currentStreak"];
    h.lastCompletedDay = obj["lastCompletedDay"];
    h.reminderTriggered = obj["reminderTriggered"];
    parseTime(h);
    habitCount++;
  }
}

// =====================================================================
// Webserver-Handler
// =====================================================================
void handleGetHabits()
{
  // schlanker als saveHabits(): die Webseite braucht z.B. reminderTriggered
  // nicht, das ist reine Geräte-interne Buchhaltung.
  JsonDocument doc;
  JsonArray array = doc.to<JsonArray>();
  for (int i = 0; i < habitCount; i++)
  {
    JsonObject obj = array.add<JsonObject>();
    obj["id"] = habits[i].id;
    obj["name"] = habits[i].name;
    obj["time"] = habits[i].time;
    obj["melody"] = habits[i].melody;
    obj["completedToday"] = habits[i].completedToday;
    obj["currentStreak"] = habits[i].currentStreak;
  }
  String jsonResponse;
  serializeJson(doc, jsonResponse);
  server.send(200, "application/json", jsonResponse);
}

void handlePostHabit()
{
  if (habitCount >= MAX_HABITS)
  {
    server.send(400, "text/plain", "Max Habits reached");
    return;
  }

  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));

  Habit &h = habits[habitCount];
  h.id = doc["id"];
  h.name = doc["name"].as<String>();
  h.time = doc["time"].as<String>();
  h.melody = doc["melody"].as<String>();
  h.completedToday = false;
  h.currentStreak = 0;
  h.lastCompletedDay = 0;
  h.reminderTriggered = false;
  parseTime(h);

  habitCount++;
  saveHabits();
  server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleDeleteHabit()
{
  long id = server.arg("id").toInt();
  for (int i = 0; i < habitCount; i++)
  {
    if (habits[i].id == id)
    {
      for (int j = i; j < habitCount - 1; j++)
        habits[j] = habits[j + 1];
      habitCount--;
      saveHabits();
      break;
    }
  }
  server.send(200, "application/json", "{\"status\":\"ok\"}");
}

// Der Trick: Das Handy sendet uns die Uhrzeit, wenn die Webseite lädt
// (der Access Point hat kein Internet -> kein NTP möglich)
void handleSyncTime()
{
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));

  struct timeval tv;
  tv.tv_sec = doc["timestamp"];
  tv.tv_usec = 0;
  settimeofday(&tv, NULL);

  server.send(200, "text/plain", "Time Synced");
}

// =====================================================================
// Display
// =====================================================================
void drawScreen()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if (currentState == STATE_IDLE)
  {
    // Smiley
    display.drawCircle(64, 32, 20, SSD1306_WHITE);   // Gesicht
    display.fillCircle(56, 26, 2, SSD1306_WHITE);    // Auge links
    display.fillCircle(72, 26, 2, SSD1306_WHITE);    // Auge rechts
    display.drawPixel(64, 32, SSD1306_WHITE);        // Nase
    display.drawLine(54, 40, 74, 40, SSD1306_WHITE); // Lächeln
    display.drawLine(54, 40, 50, 36, SSD1306_WHITE);
    display.drawLine(74, 40, 78, 36, SSD1306_WHITE);
  }
  else if (currentState == STATE_MENU)
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
  else if (currentState == STATE_COUNTDOWN)
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
  else if (currentState == STATE_REMINDER)
  {
    display.setTextSize(2);
    display.setCursor(0, 10);
    display.println("ZEIT FUER:");
    display.setCursor(0, 35);
    display.println(habits[currentHabitIndex].name);
  }
  else if (currentState == STATE_FEEDBACK)
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

  display.display();
}

void setupWebServer()
{
  server.on("/", HTTP_GET, []()
            {
    File file = LittleFS.open("/index.html", "r");
    if(!file){
      server.send(500, "text/plain", "Fehler: index.html nicht gefunden");
      return;
    }
    server.streamFile(file, "text/html");
    file.close(); });

  server.on("/style.css", HTTP_GET, []()
            {
    File file = LittleFS.open("/style.css", "r");
    server.streamFile(file, "text/css");
    file.close(); });

  server.on("/script.js", HTTP_GET, []()
            {
    File file = LittleFS.open("/script.js", "r");
    server.streamFile(file, "application/javascript");
    file.close(); });

  // Fehlende Registrierungen ergänzt:
  server.on("/habits", HTTP_GET, handleGetHabits);
  server.on("/habits", HTTP_POST, handlePostHabit);
  server.on("/habits", HTTP_DELETE, handleDeleteHabit);
  server.on("/sync-time", HTTP_POST, handleSyncTime);
}

// =====================================================================
// Setup
// =====================================================================
void setup()
{
  Serial.begin(115200);
  Serial.println("\n Habit Reminder startet...");

  if (!LittleFS.begin(true))
  {
    Serial.println("Fehler beim Mounten von LittleFS");
    return;
  }

  Serial.println("LittleFS erfolgreich gestartet!");

  pinMode(PIN_ENC_CLK, INPUT_PULLUP);
  pinMode(PIN_ENC_DT, INPUT_PULLUP);
  pinMode(PIN_ENC_SW, INPUT_PULLUP);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  setColor(0, 0, 0);

  // Wire.begin() OHNE Pins würde die ESP32-Standardpins (21/22) statt
  // I2C_SDA/I2C_SCL (32/33) verwenden -> deshalb hier explizit angeben
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED Fehler!");
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("Starte Reminder...");
  display.display();

  prefs.begin("habitApp", false); // Flash-Speicher öffnen
  WiFi.softAP(ssid, password);
  Serial.print("Access Point gestartet. IP: ");
  Serial.println(WiFi.softAPIP()); // normalerweise 192.168.4.1

  setupWebServer();
  server.begin();
  Serial.println("Webserver läuft");

  loadHabits();
  currentState = STATE_IDLE;
}

// =====================================================================
// Loop
// =====================================================================
void loop()
{
  server.handleClient();
  unsigned long now = millis();
  long currentDay = getCurrentDay();

  // 1. Mitternachts-Reset & Streak-Bruch-Check
  if (currentDay > lastKnownDay && currentDay > 0)
  {
    for (int i = 0; i < habitCount; i++)
    {
      habits[i].completedToday = false;
      habits[i].reminderTriggered = false;
      if (habits[i].lastCompletedDay < currentDay - 1 && habits[i].currentStreak > 0)
        habits[i].currentStreak = 0; // Streak gerissen
    }
    saveHabits();
    lastKnownDay = currentDay;
  }

  // 2. Zeit prüfen (Countdown & Reminder) - während IDLE oder COUNTDOWN
  //    (läuft in COUNTDOWN weiter, sonst bleibt die Anzeige stehen und
  //    der Reminder wird nie ausgelöst)
  if (currentDay > 0 && habitCount > 0 &&
      (currentState == STATE_IDLE || currentState == STATE_COUNTDOWN))
  {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm timeinfo;
    // gmtime_r statt localtime_r: der Zeitstempel wurde vom Handy bereits
    // um die Zeitzone korrigiert (siehe webpage.h) - der ESP32 braucht also
    // keine eigene Zeitzonen-Umrechnung mehr vorzunehmen.
    gmtime_r(&tv.tv_sec, &timeinfo);
    int currentMins = timeinfo.tm_hour * 60 + timeinfo.tm_min;

    // Debug: alle 5s aktuelle Geräte-Zeit ausgeben, hilft beim Prüfen
    // ob der Zeit-Sync korrekt angekommen ist
    static unsigned long lastTimeDebug = 0;
    if (now - lastTimeDebug > 5000)
    {
      Serial.printf("Geraetezeit: %02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min);
      lastTimeDebug = now;
    }

    bool stillCounting = false;

    for (int i = 0; i < habitCount; i++)
    {
      if (habits[i].completedToday)
        continue;

      int diff = (habits[i].hour * 60 + habits[i].minute) - currentMins;

      if (diff == 0 && !habits[i].reminderTriggered)
      {
        currentState = STATE_REMINDER;
        currentHabitIndex = i;
        habits[i].reminderTriggered = true;
        saveHabits();
        playReminderMelody(PIN_BUZZER); // kurzer Piep
        break;
      }
      else if (diff > 0 && diff <= 15)
      {
        currentState = STATE_COUNTDOWN;
        countdownHabitIndex = i;
        countdownMinutes = diff;
        stillCounting = true;
      }
    }

    // kein Habit mehr im 15-Minuten-Fenster -> zurück zum Smiley
    if (currentState == STATE_COUNTDOWN && !stillCounting)
      currentState = STATE_IDLE;
  }

  // 3. Rotary Encoder lesen
  int clkState = digitalRead(PIN_ENC_CLK);
  if (clkState != lastCLK && clkState == HIGH)
  {
    if (digitalRead(PIN_ENC_DT) != clkState)
      currentHabitIndex++;
    else
      currentHabitIndex--;

    if (currentHabitIndex >= habitCount)
      currentHabitIndex = 0;
    if (currentHabitIndex < 0)
      currentHabitIndex = max(0, habitCount - 1);

    if (currentState == STATE_IDLE || currentState == STATE_COUNTDOWN)
      currentState = STATE_MENU;

    lastActivityTime = now;
  }
  lastCLK = clkState;

  // 4. Button lesen (Encoder-Taster)
  bool btnState = digitalRead(PIN_ENC_SW);
  if (btnState == LOW && lastButtonState == HIGH && now - lastActivityTime > 200) // Entprellen
  {
    if ((currentState == STATE_MENU || currentState == STATE_REMINDER) && habitCount > 0)
    {
      Habit &h = habits[currentHabitIndex];
      if (!h.completedToday)
      {
        h.completedToday = true;

        // Streak nur erhöhen, wenn heute noch nicht erledigt
        if (h.lastCompletedDay < currentDay)
        {
          h.currentStreak++;
          h.lastCompletedDay = currentDay;
        }
        saveHabits();
        currentState = STATE_FEEDBACK;
        feedbackStartTime = now;

        if (h.melody == "Success")
          playSuccessMelody(PIN_BUZZER);
      }
    }
    lastActivityTime = now;
  }
  lastButtonState = btnState;

  // 5. State-abhängige LED-Steuerung
  switch (currentState)
  {
  case STATE_MENU:
    setColor(0, 0, 0);
    if (now - lastActivityTime > 10000) // 10s Inaktivität -> zurück zum Smiley
      currentState = STATE_IDLE;
    break;

  case STATE_FEEDBACK:
    setColor(0, 255, 0); // Knallgrün
    if (now - feedbackStartTime > 3000)
    {
      currentState = STATE_IDLE;
      setColor(0, 0, 0);
    }
    break;

  case STATE_REMINDER:
    // sanft pulsierende blaue LED
    if (now - lastBlinkTime > 30)
    {
      brightness += fadeAmount;
      if (brightness <= 0 || brightness >= 255)
        fadeAmount = -fadeAmount;
      setColor(0, 0, brightness);
      lastBlinkTime = now;
    }
    break;

  case STATE_COUNTDOWN:
  case STATE_IDLE:
    setColor(0, 0, 0);
    break;

  default:
    break;
  }

  // Display max. alle 100ms neu zeichnen (verhindert Flimmern)
  static unsigned long lastDraw = 0;
  if (now - lastDraw > 100)
  {
    drawScreen();
    lastDraw = now;
  }
}