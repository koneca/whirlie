// whirlie_setup.ino

int
InitializeData()
{
  HeatingState = 0;
  LightState = 0;
  Pump1State = 0;
  Pump2State = 0;
  AirState = 0;
  Timer = 0;

  digitalWrite(DHEATING, HeatingState);
  digitalWrite(DLAMP, LightState);
  digitalWrite(DPUMP1, Pump1State);
  digitalWrite(DPUMP2, HeatingState);
  digitalWrite(DAIR, Pump2State);
  digitalWrite(DFILTER, FilterState);

  LoadTargetTemp();
}

/* ----------------------------------- ----------------------------------- */
int
SetupPorts()
{
  pinMode(DHEATING, OUTPUT);
  pinMode(DLAMP, OUTPUT);
  pinMode(DPUMP1, OUTPUT);
  pinMode(DPUMP2, OUTPUT);
  pinMode(DAIR, OUTPUT);
  pinMode(DFILTER, OUTPUT);

  //  pinMode(HeatBtn, INPUT);
  //  pinMode(LampBtn, INPUT);
  //  pinMode(Pump1Btn, INPUT);
  //  pinMode(Pump2Btn, INPUT);
  //  pinMode(AirBtn, INPUT);
}

/* ----------------------------------- ----------------------------------- */
void
LoadTargetTemp()
{
    EEPROM.begin(sizeof(byte));
    TargetTemp = EEPROM.read(PERMSTORE);
    EEPROM.end();
    Serial.print("Got permTargetTemp ");
    Serial.println(TargetTemp);
}

/* ----------------------------------- ----------------------------------- */
void
InitializeDisplay()
{
    // Lcd.init();
    // Wire.begin(D2, D1);
    // Lcd.backlight();
    // Lcd.setCursor(2,0);
    // Lcd.print(SWNAME);
    // Lcd.setCursor(1,1);
    // Lcd.print("Version");
    // Lcd.setCursor(9,1);
    // Lcd.print(VERSION);

    

    delay(2000);
}
