// ============================================================
//  ESP32-A — Sensor Node (soldered board, WiFi broken)
//  Responsibilities: Read sensors + Control pump + LCD display
//  Communicates with ESP32-B via Serial2 (UART)
//  Pump: HIGH = ON, LOW = OFF
// ============================================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Adafruit_LTR390.h"
#include "DHT.h"
#include <ArduinoJson.h>

// ========== Pin Configuration ==========
#define FLOW_PIN   18
#define DHTPIN      4
#define DHTTYPE   DHT11
#define PUMP_PIN   23  // HIGH = ON, LOW = OFF

// ========== Serial2 (UART communication with ESP32-B) ==========
// ESP32-A GPIO16 (RX2) ← ESP32-B TX
// ESP32-A GPIO17 (TX2) → ESP32-B RX
#define UART_RX 16
#define UART_TX 17

// ========== Sensor Objects ==========
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_LTR390   ltr = Adafruit_LTR390();
DHT               dht(DHTPIN, DHTTYPE);

// ========== Global Variables ==========
volatile int  pulsCount    = 0;
float         flowRate     = 0.0;

unsigned long sensorLastTime  = 0;
unsigned long displayLastTime = 0;
unsigned long pumpStartTime   = 0;
unsigned long pumpDuration    = 0;  // ms

int  displayPage = 0;
bool pumpActive  = false;

// ========== Water Flow Interrupt ==========
void IRAM_ATTR pulsCounter() {
  pulsCount++;
}

// ========== Setup ==========
void setup() {
  Serial.begin(115200);

  // Pump relay pin — LOW = OFF on startup
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
  Serial.println("[A] Pump initialized (OFF)");

  // Serial2: communication with ESP32-B
  Serial2.begin(115200, SERIAL_8N1, UART_RX, UART_TX);

  Wire.begin(21, 22);

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Booting...");

  // DHT11
  dht.begin();

  // LTR390 UV sensor
  if (!ltr.begin()) {
    Serial.println("[A] LTR390 not found!");
    lcd.setCursor(0, 1);
    lcd.print("LTR390 Error!");
    while (1) delay(10);
  }
  ltr.setMode(LTR390_MODE_ALS);
  ltr.setGain(LTR390_GAIN_3);
  ltr.setResolution(LTR390_RESOLUTION_18BIT);

  // Water flow sensor
  pinMode(FLOW_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), pulsCounter, FALLING);

  sensorLastTime  = millis();
  displayLastTime = millis();

  lcd.clear();
  lcd.print("Waiting B...");
  Serial.println("[A] Setup complete. Waiting for ESP32-B...");
}

// ========== Handle incoming commands from ESP32-B ==========
void handleIncomingCommand() {
  if (!Serial2.available()) return;

  String msg = Serial2.readStringUntil('\n');
  msg.trim();
  if (msg.length() == 0) return;

  Serial.println("[A] Received from B: " + msg);

  // Parse incoming JSON command
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, msg);
  if (error) {
    Serial.println("[A] JSON parse error: " + String(error.c_str()));
    return;
  }

  String action = doc["action"] | "";

  // Parse duration safely with type check
  int duration = doc["duration_sec"].as<int>();
  if (duration <= 0) duration = 30;  // Fallback to 30s if missing or invalid
  Serial.println("[A] Action: " + action + " | Duration parsed: " + String(duration) + "s");

  if (action == "water") {
    pumpDuration  = (unsigned long)duration * 1000UL;
    pumpStartTime = millis();
    pumpActive    = true;

    digitalWrite(PUMP_PIN, HIGH);  // Pump ON
    Serial.println("[A] Pump ON — will run for " + String(duration) + "s (" + String(pumpDuration) + "ms)");
    Serial.println("[A] pumpStartTime: " + String(pumpStartTime));

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Watering...");
    lcd.setCursor(0, 1);
    lcd.print(String(duration) + " sec");
  }

  // Connection status notification from ESP32-B
  if (action == "status") {
    String status = doc["connected"] | "unknown";
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("AWS:");
    lcd.setCursor(0, 1);
    lcd.print(status);
  }
}

// ========== Loop ==========
void loop() {
  unsigned long now = millis();

  // Handle commands from ESP32-B
  handleIncomingCommand();

if (pumpActive) {
    unsigned long elapsed = millis() - pumpStartTime;

    // 每 1 秒更新一次 LCD 顯示剩餘時間 + flowRate
    static unsigned long pumpDisplayLast = 0;
    if (now - pumpDisplayLast >= 1000) {
      pumpDisplayLast = now;
      unsigned long remaining = (pumpDuration - elapsed) / 1000UL + 1;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Water ");
      lcd.print(remaining);
      lcd.print("s left");
      lcd.setCursor(0, 1);
      lcd.print("Flow:");
      lcd.print(flowRate, 1);
      lcd.print("L/m");
    }

    if (elapsed >= pumpDuration) {
      pumpActive = false;
      digitalWrite(PUMP_PIN, LOW);
      Serial.println("[A] Pump OFF — elapsed: " + String(elapsed) + "ms");
      Serial.println("[A] Watering complete.");

      lcd.clear();
      lcd.print("Watering Done!");
      delay(2000);
      lcd.clear();
      lcd.print("System Ready!");
    }
  }

  // Read sensors and send to ESP32-B every 5 seconds
  if (now - sensorLastTime >= 5000) {

    // Calculate water flow rate
    detachInterrupt(digitalPinToInterrupt(FLOW_PIN));
    flowRate  = (pulsCount > 0) ? ((float)pulsCount + 3.0) / 8.1 : 0.0;
    pulsCount = 0;
    sensorLastTime = now;
    attachInterrupt(digitalPinToInterrupt(FLOW_PIN), pulsCounter, FALLING);

    // Read UV index
    ltr.setMode(LTR390_MODE_UVS);
    delay(50);
    uint32_t uvRaw = ltr.readUVS();
    float uvIndex  = uvRaw / 100.0;

    // Switch back to ambient light mode
    ltr.setMode(LTR390_MODE_ALS);
    delay(50);

    // Read temperature and humidity
    float humidity    = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Debug output
    Serial.print("[A] Temp:"); Serial.print(temperature);
    Serial.print(" Hum:");    Serial.print(humidity);
    Serial.print(" UV:");     Serial.print(uvIndex);
    Serial.print(" Flow:");   Serial.println(flowRate);

    // Build JSON and send to ESP32-B
    StaticJsonDocument<256> telemetry;
    telemetry["device_id"]   = "Wateringesp32";
    telemetry["temperature"] = serialized(String(temperature, 1));
    telemetry["humidity"]    = serialized(String(humidity, 1));
    telemetry["moisture"]    = 0;
    telemetry["uv_index"]    = serialized(String(uvIndex, 2));
    telemetry["flow_rate"]   = serialized(String(flowRate, 2));

    String jsonOut;
    serializeJson(telemetry, jsonOut);
    Serial2.println(jsonOut);
    Serial.println("[A] Sent to B: " + jsonOut);

    // Update LCD display (only when pump is not running)
    if (!pumpActive && (now - displayLastTime >= 3000)) {
      displayLastTime = now;
      lcd.clear();
      if (displayPage == 0) {
        lcd.setCursor(0, 0);
        lcd.print("T:"); lcd.print(temperature, 1);
        lcd.print(" H:"); lcd.print(humidity, 0);
        lcd.setCursor(0, 1);
        lcd.print("UV: "); lcd.print(uvIndex, 1);
        displayPage = 1;
      } else {
        lcd.setCursor(0, 0);
        lcd.print("Flow:"); lcd.print(flowRate, 1); lcd.print("L/m");
        lcd.setCursor(0, 1);
        lcd.print("UART: Active");
        displayPage = 0;
      }
    }
  }
}
