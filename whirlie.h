#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
//#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>

//SW Version
#define SWNAME "Whirlie-Control"
#define VERSION "0.3.3"

#define PERMSTORE   0

// Portsdefinition
#define DHEATING     2  //D4
#define DAIR         5  //D1
#define DPUMP1       4  //D2
#define DPUMP2       0  //D3
#define DFILTER      13 //D7
#define DLAMP        12 //D6 

#define A0          1

// Textdefinitions
#define TXT_HEATING "Heizung"
#define TXT_PUMP1   "Pumpe1"
#define TXT_PUMP2   "Pumpe2"
#define TXT_AIR     "Blubber"
#define TXT_LIGHT   "Licht"
#define TXT_FILTER  "Filter"

#define BTN_LIGHT   0
#define BTN_PUMP1   1
#define BTN_PUMP2   2
#define BTN_AIR     3
#define BTN_FILTER  4
#define BTN_HEATING 5
#define INPUT_MASK  0x001f

#define LED_LIGHT   8
#define LED_PUMP1   9
#define LED_PUMP2   10
#define LED_AIR     11
#define LED_FILTER  12
#define LED_HEATING 13
#define OUTPUT_MASK  0x1f00

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
byte TargetTemp;
String HostIp;

uint16_t    PanelState;

void
HandleSensors();

void
HandleBrowserIO();

// struct StoreStruct {
//     float fliesskommazahl;
// };
// StoreStruct storage = {1.0};

void
LoadTargetTemp();

void
ToggleControlPanel();


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
