#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
//#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

//SW Version
#define SWNAME "Whirlie-Control"
#define VERSION "0.3"

#define PERMSTORE   0

// Portsdefinition
#define DHEATING     2  //D4
#define DAIR         5  //D1
#define DPUMP1       4  //D2
#define DPUMP2       0  //D3
#define DFILTER      13 //D7
#define DLAMP        12 //D6 

// Butondefinition
#define HEATBTN     7
#define LAMPBTN     8
#define PUMP1BTN    9
#define PUMP2BTN    10
#define AIRBTN      11

#define A0          1

// Textdefinitions
#define TXT_HEATING "Heizung"
#define TXT_PUMP1   "Pumpe1"
#define TXT_PUMP2   "Pumpe2"
#define TXT_AIR     "Blubber"
#define TXT_LIGHT   "Licht"
#define TXT_FILTER  "Filter"

/* ----------------------------------- ----------------------------------- */

int HeatingState;
int LightState;
int Pump1State;
int Pump2State;
int FilterState;
int AirState;
String header;
int Timer;
float Temperature;
int TargetTemp;
String HostIp;

void
HandleSensors();

void
HandleBrowserIO();

struct StoreStruct {
    float fliesskommazahl;
};
StoreStruct storage = {1.0};


/* ----------------------------------- ----------------------------------- */
int
InitializeData();

int
SetupPorts();

void ShowForm(
    WiFiClient              client
    );

void
InitializeDisplay();

void
ShowWaterTemp(
    const float             Temperature
    );

void
DisplayButton(
    WiFiClient              client,
    String                  Name,
    const unsigned int      IsOn
    );
