#include <DHT.h>

// ---- Door State Enum ---- //
typedef enum Doorstatus {
  STATUS_DISABLED = 0,
  DOOR_CLOSED    = 1,
  DOOR_OPENED    = 2,
  INTRUDER       = 3
} Doorstatus;

// ---- Pins ---- //
#define RESETPIN   4
#define ENABLEPIN  21
#define DOORPIN    18
#define PIRPIN     19
#define DHTPIN     17
#define DHTTYPE    DHT22

// ---- Door Security Class ---- //
class DoorSecurity {
private:
  int resetpin;
  int pirpin;
  int enablepin;
  int doorpin;

  bool pirpinstate      = 0;
  bool enablepinstate   = 0;
  bool doorpinstate     = 0;
  bool resetpinstate    = 0;
  bool prevresetpinstate = 0;
  bool resetflag        = false;

  Doorstatus MyDoorStatus = STATUS_DISABLED;

  String msg[5] = {
    "System is Disabled.....",
    "Door is closed...",
    "Door is opened",
    "Intruder has trespassed....",
    "System is reseted...."
  };

public:
  DoorSecurity(int resetpin, int pirpin, int enablepin, int doorpin) {
    this->resetpin  = resetpin;
    this->pirpin    = pirpin;
    this->enablepin = enablepin;
    this->doorpin   = doorpin;

    pinMode(resetpin,  INPUT);
    pinMode(pirpin,    INPUT);
    pinMode(enablepin, INPUT);
    pinMode(doorpin,   INPUT);
  }

  void readinputs() {
    pirpinstate    = digitalRead(pirpin);
    enablepinstate = digitalRead(enablepin);
    doorpinstate   = digitalRead(doorpin);
    resetpinstate  = digitalRead(resetpin);
  }

  void updateresetstate() {
    prevresetpinstate = resetpinstate;
  }

  void checkreset() {
    resetflag = (prevresetpinstate != resetpinstate);
    Serial.print("reset pin: "); Serial.println(resetpinstate);
    Serial.print("prev reset pin: "); Serial.println(prevresetpinstate);
    Serial.print("reset flag: "); Serial.println(resetflag);
  }

  void setDoorstatus() {
    if (enablepinstate == 0) {
      MyDoorStatus = STATUS_DISABLED;
      Serial.println(msg[0]);
    }
    else if (enablepinstate == 1 && resetflag == true) {
      MyDoorStatus = STATUS_DISABLED;
      Serial.println(msg[4]);  // System reset
    }
    else {
      if (doorpinstate == 0) {
        MyDoorStatus = DOOR_CLOSED;
        Serial.println(msg[1]);
      }
      else if (doorpinstate == 1 && pirpinstate == 0) {
        MyDoorStatus = DOOR_OPENED;
        Serial.println(msg[2]);
      }
      else if (doorpinstate == 1 && pirpinstate == 1) {
        MyDoorStatus = INTRUDER;
        Serial.println(msg[3]);
      }
    }
  }
};

// ---- Home Security Class ---- //
class HomeSecurity {
private:
  DoorSecurity MyDoorSecurity;
  DHT Mydht;
  float temp = 0.00;
  float humidity = 0.00;

public:
  HomeSecurity(int resetpin, int pirpin, int enablepin, int doorpin)
    : MyDoorSecurity(resetpin, pirpin, enablepin, doorpin)
    , Mydht(DHTPIN, DHTTYPE)
  {
    Mydht.begin();
    Serial.println("Our HomeSecurity System is monitoring......");
  }

  void monitoring() {
    MyDoorSecurity.setDoorstatus();
    Serial.print("Temp of the system is:");
    Serial.print(temp);
    Serial.println(" C");
    Serial.print("Humidity of the system is:");
    Serial.print(humidity);
    Serial.println(" g/m^3");
  }

  void checkinputs() {
    MyDoorSecurity.readinputs();
    //MyDoorSecurity.checkreset();
    temp = Mydht.readTemperature();
    humidity = Mydht.readHumidity();
  }

  void processreset() {
   MyDoorSecurity.checkreset();
  }

  void setprevreset() {
    MyDoorSecurity.updateresetstate();
  }
};

HomeSecurity MyHomeSecurity(RESETPIN, PIRPIN, ENABLEPIN, DOORPIN);

void setup() {
  Serial.begin(115200);
  Serial.println("System is initialized.....");
}

void loop() {
  MyHomeSecurity.setprevreset();   // save previous value
  MyHomeSecurity.checkinputs();    // read new values
  MyHomeSecurity.processreset();   // compare previous vs new
  MyHomeSecurity.monitoring();     // update FSM

  delay(2000);
}