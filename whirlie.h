#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

//SW Version
#define SWNAME "Whirlie-Control"
#define VERSION "0.1"

// Portsdefinition
#define HEATING 12
#define LAMP 13
#define PUMP1 14
#define PUMP2 15
#define AIR 16

// Butondefinition
#define HEATBTN 7
#define LAMPBTN 8
#define PUMP1BTN 9
#define PUMP2BTN 10
#define AIRBTN 11

#define A0 1

/* ----------------------------------- ----------------------------------- */

int HeatingState;
int LampState;
int Pump1State;
int Pump2State;
int AirState;
String header;
int Timer;
float Temperature;

void HandleForm(WiFiClient client);
void HandleSensors();
void HandleBrowserIO();
WiFiServer server(80);

/* ----------------------------------- ----------------------------------- */
int
InitializeData();

int
SetupPorts();
