#include <Arduino.h>
#include <DHT.h>

// -------- Pin Definitions --------
#define ENABLEPIN   PA1
#define DOORPIN     PA0
#define RESETPIN    PA6
#define PIRPIN      PA4
#define LED_ALERT   PC13
#define DHT_PIN     PA5
#define DHT_TYPE    DHT22
#define RELAYPIN_1  PA11
#define RELAYPIN_2  PA12

// DHT Sensor
DHT dht(DHT_PIN, DHT_TYPE); 
float humidity = 0.0;
float temp = 0.0; 

// -------- Debounce Parameters --------
const unsigned long debounceDelay = 50;  // 50 ms debounce delay

// -------- Door States --------
typedef enum {
  STATE_DISABLED = 0,
  STATE_DOOR_CLOSED = 1,
  STATE_DOOR_OPEN = 2,
  STATE_INTRUDER = 3
} DoorState;

// -------- DoorSystem Class --------
class DoorSystem {
  public:
    DoorSystem(int enablePin, int doorPin, int resetPin, int pirPin) {
      this->enablePin = enablePin;
      this->doorPin = doorPin;
      this->resetPin = resetPin;
      this->pirPin = pirPin;

      pinMode(enablePin, INPUT);
      pinMode(doorPin, INPUT_PULLUP);
      pinMode(resetPin, INPUT_PULLUP);
      pinMode(pirPin, INPUT);

      currentState = STATE_DISABLED;
      lastEnableState = HIGH;
      lastResetState = HIGH;
      enableState = HIGH;
      resetState = HIGH;
    }

    void readInputs() {
      // ----- Enable Button Debounce -----
      bool readingEnable = digitalRead(enablePin);
      if (readingEnable != lastEnableState) lastEnableTime = millis();
      if ((millis() - lastEnableTime) > debounceDelay) enableState = readingEnable;
      lastEnableState = readingEnable;

      // ----- Reset Button Debounce -----
      bool readingReset = digitalRead(resetPin);
      if (readingReset != lastResetState) lastResetTime = millis();
      if ((millis() - lastResetTime) > debounceDelay) resetState = readingReset;
      lastResetState = readingReset;

      // Other inputs without debounce
      doorState = digitalRead(doorPin);
      pirState = digitalRead(pirPin);
    }

    void updateState() {
      switch (currentState) {
        case STATE_DISABLED:
          if (enableState == LOW) {  
            currentState = !doorState ? STATE_DOOR_CLOSED : STATE_DOOR_OPEN;
            Serial.println(!doorState ? "System Enabled: Door Closed" : "System Enabled: Door Open");
          }
          break;

        case STATE_DOOR_CLOSED:
          if (enableState == HIGH || resetState == LOW) {
            currentState = STATE_DISABLED;
            Serial.println("System Disabled or Reset");
          } else if (doorState) {
            currentState = STATE_DOOR_OPEN;
            Serial.println("Door Opened!");
          }
          break;

        case STATE_DOOR_OPEN:
          if (enableState == HIGH || resetState == LOW) {
            currentState = STATE_DISABLED;
            Serial.println("System Disabled or Reset");
          } else if (pirState) {
            currentState = STATE_INTRUDER;
            Serial.println("Intruder Detected!");
          } else if (!doorState) {
            currentState = STATE_DOOR_CLOSED;
            Serial.println("Door Closed!");
          }
          break;

        case STATE_INTRUDER:
          if (enableState == HIGH || resetState == LOW) {
            currentState = STATE_DISABLED;
            Serial.println("System Disabled or Reset");
          } else {
            Serial.println("Intruder! Check Immediately!");
          }
          break;
      }
    }

    DoorState getDoorState() { return currentState; }

  private:
    int enablePin, doorPin, resetPin, pirPin;
    bool enableState, doorState, resetState, pirState;
    bool lastEnableState, lastResetState;
    unsigned long lastEnableTime = 0;
    unsigned long lastResetTime = 0;
    DoorState currentState;
};

// -------- Global Object --------
DoorSystem myDoorSystem(ENABLEPIN, DOORPIN, RESETPIN, PIRPIN);

// -------- UART to ESP32 --------
HardwareSerial Serial2(PA3, PA2); // RX, TX for ESP32

// -------- Setup --------
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);       // UART2 to ESP32
  dht.begin();
  pinMode(LED_ALERT, OUTPUT);
  Serial.println("Magnetic Door System Initialized with UART2 for ESP32");
}

// -------- Loop --------
void loop() {
  // Read Inputs & Update FSM
  myDoorSystem.readInputs();
  myDoorSystem.updateState();

  // LED Alert
  if(myDoorSystem.getDoorState() == STATE_INTRUDER) digitalWrite(LED_ALERT, HIGH);
  else digitalWrite(LED_ALERT, LOW);

  // DHT Readings
  temp = dht.readTemperature();
  humidity = dht.readHumidity();

  if (isnan(temp) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Temperature: "); Serial.print(temp); Serial.print(" °C, ");
    Serial.print("Humidity: "); Serial.println(humidity);
  }

  // Prepare UART String
  String uartMsg = "DoorState:" + String(myDoorSystem.getDoorState())
                 + ",Temp:" + String(temp)
                 + ",Hum:" + String(humidity);

  Serial2.println(uartMsg);  // Send to ESP32
  Serial.println("Sent via UART2: " + uartMsg);

  delay(2000); // DHT needs ~2s
}

