// whirlie.ino

#include "whirlie.h"

//LiquidCrystal_I2C   Lcd(0x27,20,4);
WiFiServer          Server(80);

void setup()
{
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
    delay(1000);
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
    
