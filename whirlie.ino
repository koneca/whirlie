// whirlie.ino

#include "whirlie.h"


//LiquidCrystal_I2C   Lcd(0x27,20,4);
WiFiServer          Server(80);
Adafruit_MCP23017   Expander;

void setup()
{
    Expander.begin();
    SetupPorts();
    InitializeData();
    Serial.begin(115200);

    Serial.print("Starting ");
    Serial.print(SWNAME);
    Serial.print(" ");
    Serial.println(VERSION);
    Serial.print("Setting soft-AP ... ");
    WiFiManager wifiManager;

    if(0 == wifiManager.autoConnect("WhirlpoolAP"))
    {
        Serial.println("Ready");
    }

    HostIp = WiFi.localIP().toString();
    Server.begin();

    // Serial.println("Statring Display");
    // InitializeDisplay();
    // Serial.println("Display started");
}

/* ------------------------------- Main loop ------------------------------- */
void loop()
{
    WiFiClient Client = Server.available();

//    ShowWaterTemp(Temperature);

    if (Client)
    {
        Serial.println("New Client.");
        String currentLine = ""; // erstelle einen String mit den eingehenden Daten vom Client
        while (Client.connected())
        {
        //    ShowWaterTemp(Temperature);
            
            // wiederholen so lange der Client verbunden ist
            if (Client.available())
            {                           // Fall ein Byte zum lesen da ist,
                char c = Client.read(); // lese das Byte, und dann
                header += c;
                if (c == '\n')
                { // wenn das Byte eine Neue-Zeile Char ist
                    // wenn die aktuelle Zeile leer ist, kamen 2 in folge.
                    // dies ist das Ende der HTTP-Anfrage vom Client, also senden wir eine Antwort:
                    if (currentLine.length() == 0)
                    {
  
                            HandleSensors();
                            HandleBrowserIO(Client);
                            ShowForm(Client);
                 
                        
                        // und wir verlassen mit einem break die Schleife
                        break;
                    }
                    else
                    { // falls eine neue Zeile kommt, lösche die aktuelle Zeile
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // wenn etwas kommt was kein Zeilenumbruch ist,
                    currentLine += c; // füge es am Ende von currentLine an
                }
            }
        }
        // Die Header-Variable für den nächsten Durchlauf löschen
        header = "";
        // Die Verbindung schließen
        Client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
    else
    {
        ToggleControlPanel();
    }
    
    delay(1000);
}

void
ToggleControlPanel()
{
    uint16_t    PanelStateRead = 0;
    uint16_t    BtnState = 0;
    uint16_t    NewState = 0;
    
    PanelStateRead = Expander.readGPIOAB();
    //ignore unised ports
    PanelStateRead &= INPUT_MASK;

    //Button Pressed
    if(0 != PanelStateRead)
    {
        // bitswitch to Outputmask
        BtnState = (0x00FF & PanelStateRead) << 8;
        BtnState &= OUTPUT_MASK;

        //Toggle ports, changed
        NewState = BtnState ^ PanelState;

        Expander.writeGPIOAB(NewState);
        PanelState = NewState;
    }

    if(PanelStateRead || StateChangedByBrowser)
    {
        //write all because we don't know if Mobile App switched one port
        digitalWrite(DHEATING,  PanelState & BTN_HEATING ? 1 : 0);
        digitalWrite(DAIR,      PanelState & BTN_AIR    ? 1 : 0);
        digitalWrite(DPUMP1,    PanelState & BTN_PUMP1  ? 1 : 0);
        digitalWrite(DPUMP2,    PanelState & BTN_PUMP2  ? 1 : 0);
        digitalWrite(DFILTER,   PanelState & BTN_FILTER ? 1 : 0);
        digitalWrite(DLAMP,     PanelState & BTN_LIGHT  ? 1 : 0);

        // clean up here
        HeatingState = PanelState & BTN_HEATING ? 1 : 0;
        AirState     = PanelState & BTN_AIR    ? 1 : 0;
        Pump1State   = PanelState & BTN_PUMP1  ? 1 : 0;
        Pump2State   = PanelState & BTN_PUMP2  ? 1 : 0;
        FilterState  = PanelState & BTN_FILTER ? 1 : 0;
        LightState   = PanelState & BTN_LIGHT  ? 1 : 0;

    }
}

/* ----------------------------------- ----------------------------------- */
void StoreTargetTemp(
    const float             Temp             
)
{
    Serial.println("storing Target Temp");
    EEPROM.begin(sizeof(byte));
    EEPROM.write(PERMSTORE, TargetTemp);
    // for (unsigned int t=0; t<sizeof(storage); t++)
	// 	EEPROM.write(PERMSTORE + t, *((char*)&storage + t));
    EEPROM.end();
}

/* ----------------------------------- ----------------------------------- */
void HandleSensors()
{
    unsigned int            t;

    double AnalogValue = analogRead(A0);
    // convert the analog signal to voltage
    // the ESP2866 A0 reads between 0 and ~3 volts, producing a corresponding value
    // between 0 and 1024. The equation below will convert the value to a voltage value.
    double AnalogVolts = (AnalogValue * 3.03) / 1024;
    Serial.println(AnalogVolts);
    Temperature = (AnalogVolts)*100; //converting from 10 mv per degree wit 500 mV offset
                                     //to degrees ((voltage - 500mV) times 100)
    Serial.print("the temperature ");
    Serial.println(Temperature);

}

/* ----------------------------------- ----------------------------------- */
void
ShowWaterTemp(
    const float          Temperature
    )
{
    Serial.print("actual Temp.: ");
    Serial.print(Temperature);
    Serial.println(" C");

    // Lcd.setCursor(3,0);
    // Lcd.print("Water temp.:");
    // Lcd.setCursor(5,1);
    // Lcd.print(Temperature);
    // Lcd.setCursor(10,1);
    // Lcd.print((char)223);
    // Lcd.print("C");
}
    
