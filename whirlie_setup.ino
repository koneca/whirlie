
int
InitializeData()
{
  HeatingState = 0;
  LampState = 0;
  Pump1State = 0;
  Pump2State = 0;
  AirState = 0;
  Timer = 0;

  digitalWrite(HEATING, HeatingState);
  digitalWrite(LAMP, LampState);
  digitalWrite(PUMP1, Pump1State);
  digitalWrite(PUMP2, HeatingState);
  digitalWrite(AIR, Pump2State);
}

int
SetupPorts()
{
  pinMode(HEATING, OUTPUT);
  pinMode(LAMP, OUTPUT);
  pinMode(PUMP1, OUTPUT);
  pinMode(PUMP2, OUTPUT);
  pinMode(AIR, OUTPUT);

  //  pinMode(HeatBtn, INPUT);
  //  pinMode(LampBtn, INPUT);
  //  pinMode(Pump1Btn, INPUT);
  //  pinMode(Pump2Btn, INPUT);
  //  pinMode(AirBtn, INPUT);
}

void
InitializeDisplay()
{
    Lcd.init();
    Wire.begin(D2, D1);
    Lcd.backlight();
    Lcd.setCursor(2,0);
    Lcd.print(SWNAME);
    Lcd.setCursor(1,1);
    Lcd.print("Version");
    Lcd.setCursor(9,1);
    Lcd.print(VERSION);
    delay(2000);
}


