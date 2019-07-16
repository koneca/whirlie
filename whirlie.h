#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

//SW Version
#define SWNAME "Whirlie-Control"
#define VERSION "0.2"

#define PERMSTORE   0

// Portsdefinition
#define HEATING     2  //D4
#define AIR         5  //D1
#define PUMP1       4  //D2
#define PUMP2       0  //D3
#define FILTER      13 //D7
#define LAMP        12 //D6 

// Butondefinition
#define HEATBTN     7
#define LAMPBTN     8
#define PUMP1BTN    9
#define PUMP2BTN    10
#define AIRBTN      11

#define A0          1

/* ----------------------------------- ----------------------------------- */

int HeatingState;
int LampState;
int Pump1State;
int Pump2State;
int FilterState;
int AirState;
String header;
int Timer;
float Temperature;
float TargetTemp;

void HandleForm(WiFiClient client);
void HandleSensors();
void HandleBrowserIO();

struct StoreStruct {
    float fliesskommazahl;
};
StoreStruct storage = {1.0};


/* ----------------------------------- ----------------------------------- */
int
InitializeData();

int
SetupPorts();

void
InitializeDisplay();

void
ShowWaterTemp(
    const float             Temperature
    );
