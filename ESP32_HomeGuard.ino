#include <Arduino.h>
#include "BluetoothSerial.h"
#include <WiFi.h>
#define FIREBASE_DISABLE_ALL_LOGS
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// ---------------- WiFi Credentials ----------------
#define WIFI_SSID     "YOUR WIFI SSD"
#define WIFI_PASSWORD "YOUR WIFI PASSWORD"

// ---------------- Firebase Credentials ----------------
#define API_KEY "YOUR API_KEY"
#define DATABASE_URL "YOUR FIREBASE_URL"

// ---------------- Pin Configuration ----------------
#define PIR_PIN       32
#define UART_RX_PIN   16
#define UART_TX_PIN   17
#define ENABLE_PIN    25   // Active HIGH
#define RESET_PIN     26   // Active HIGH

// ---------------- Bluetooth Setup ----------------
BluetoothSerial BT;

// ---------------- Firebase Objects ----------------
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ---------------- System Variables ----------------
int pirState = 0;
float humidity = 0.0;
float temperature = 0.0;
int doorState = 0;
int alertFlag = 0;
bool systemEnabled = false;
bool resetState = false;
int overwriteVal = 0;
bool btResetActive = false;  // gives Bluetooth reset priority
unsigned long sendPrevMillis = 0;
unsigned long fetchPrevMillis = 0;

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);

  pinMode(PIR_PIN, INPUT_PULLUP);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);

  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(RESET_PIN, LOW);

  BT.begin("ESP32_Home_Security");
  Serial.println("✅ Bluetooth Started. Waiting for commands...");

  // ----- WiFi -----
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("🌐 Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Connected!");
  Serial.println(WiFi.localIP());

  // ----- Firebase -----
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  Firebase.signUp(&config, &auth, "", ""); // anonymous sign-in
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Serial.println("🔥 Firebase Initialized Successfully!");
}

// ---------------- Bluetooth Command Handler ----------------
void handleBTCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();
  Serial.println("📲 BT Command Received: " + cmd);

  if (cmd == "ON" || cmd == "1") {
    digitalWrite(ENABLE_PIN, HIGH);
    systemEnabled = true;
    Serial.println("🟢 System ENABLED (Active HIGH)");
  } 
  else if (cmd == "OFF" || cmd == "0") {
    digitalWrite(ENABLE_PIN, LOW);
    systemEnabled = false;
    Serial.println("🔴 System DISABLED");
  } 
  else if (cmd == "RESET" || cmd == "R") {
    digitalWrite(RESET_PIN, HIGH);
    resetState = true;
    btResetActive = true; // BT reset gets priority
    Serial.println("🔄 RESET pin set HIGH (BT)");
  } 
  else if (cmd == "RESET_OFF" || cmd == "RSTOP") {
    digitalWrite(RESET_PIN, LOW);
    resetState = false;
    btResetActive = false; // allow overwrite again
    Serial.println("✅ RESET pin set LOW (BT stopped)");
  }
}

// ---------------- Parse STM32 Data ----------------
void parseSTM32Line(const String &line) {
  int di = line.indexOf("DoorState:");
  int ti = line.indexOf("Temp:");
  int hi = line.indexOf("Hum:");
  if (di == -1 || ti == -1 || hi == -1) return;

  String ds = line.substring(di + 10, ti - 1);
  String ts = line.substring(ti + 5, hi - 1);
  String hs = line.substring(hi + 4);

  ds.trim(); ts.trim(); hs.trim();

  doorState = ds.toInt();
  temperature = ts.toFloat();
  humidity = hs.toFloat();

  Serial.println("📡 STM32 → Door: " + String(doorState) +
                 ", Temp: " + String(temperature) +
                 ", Hum: " + String(humidity));
}

// ---------------- Send Data to Bluetooth ----------------
void sendBTData() {
  if (!BT.connected()) return;

  String out = String(pirState) + "," +
               String(humidity, 2) + "," +
               String(temperature, 2) + "," +
               String(doorState) + "," +
               String(alertFlag);
  BT.println(out);
  Serial.println("📤 Sent to BT: " + out);
}

// ---------------- Send Data to Firebase ----------------
void sendFirebaseData() {
  if (millis() - sendPrevMillis > 3000) {  // every 3s
    sendPrevMillis = millis();

    if (Firebase.ready()) {
      String path = "/HomeSecurity";

      Firebase.RTDB.setInt(&fbdo, path + "/PIR", pirState);
      Firebase.RTDB.setFloat(&fbdo, path + "/Temperature", temperature);
      Firebase.RTDB.setFloat(&fbdo, path + "/Humidity", humidity);
      Firebase.RTDB.setInt(&fbdo, path + "/DoorState", doorState);
      Firebase.RTDB.setInt(&fbdo, path + "/AlertFlag", alertFlag);
      Firebase.RTDB.setBool(&fbdo, path + "/SystemEnabled", systemEnabled);

      Serial.println("☁️ Data Updated to Firebase!");
    }
  }
}

// ---------------- Fetch Overwrite from Firebase ----------------
void fetchOverwrite() {
  if (millis() - fetchPrevMillis > 2000) { // every 2s
    fetchPrevMillis = millis();

    if (Firebase.ready()) {
      if (Firebase.RTDB.getInt(&fbdo, "/status/Overwrite")) {
        overwriteVal = fbdo.intData();
        Serial.println("📥 Overwrite = " + String(overwriteVal));

        // Only act on overwrite if BT reset is NOT active
        if (!btResetActive) {
          if (overwriteVal == 1) {
            digitalWrite(RESET_PIN, HIGH);
            resetState = true;
            Serial.println("🔄 RESET latch HIGH (Face detected)");
          } else {
            digitalWrite(RESET_PIN, LOW);
            resetState = false;
            Serial.println("✅ RESET latch LOW (No known face)");
          }
        } else {
          Serial.println("🚫 Overwrite ignored due to BT RESET priority");
        }
      } else {
        Serial.println("⚠️ Failed to read Overwrite: " + fbdo.errorReason());
      }
    }
  }
}

// ---------------- Main Loop ----------------
void loop() {
  pirState = digitalRead(PIR_PIN);

  // --- Read STM32 UART ---
  if (Serial2.available()) {
    String line = Serial2.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      Serial.println("📥 From STM32: " + line);
      parseSTM32Line(line);
    }
  }

  // --- Handle Bluetooth Commands ---
  if (BT.available()) {
    String cmd = BT.readStringUntil('\n');
    if (cmd.length() > 0) handleBTCommand(cmd);
  }

  // --- Fetch Overwrite value ---
  fetchOverwrite();

  // --- Compute Alert ---
  if (systemEnabled) {
    if (doorState == 3) alertFlag = (pirState == HIGH) ? 1 : 0;
    else if (doorState == 2) alertFlag = 1;
    else alertFlag = 0;
  } else {
    alertFlag = 0;
  }

  // --- Send Updates ---
  sendBTData();
  sendFirebaseData();

  delay(1000);
}
