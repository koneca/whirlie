#include "whirlie.h"

LiquidCrystal_I2C   Lcd(0x27,20,4);
WiFiServer          Server(80);

void setup()
{
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

    Server.begin();

    // Serial.println("Statring Display");
    // InitializeDisplay();
    // Serial.println("Display started");

    SetupPorts();
    InitializeData();
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
                Serial.write(c);        // gebe es auf dem seriellen Monitor aus
                header += c;
                if (c == '\n')
                { // wenn das Byte eine Neue-Zeile Char ist
                    // wenn die aktuelle Zeile leer ist, kamen 2 in folge.
                    // dies ist das Ende der HTTP-Anfrage vom Client, also senden wir eine Antwort:
                    if (currentLine.length() == 0)
                    {
                        HandleSensors();
                        HandleBrowserIO();
                        HandleForm(Client);
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
        Serial.println("Client getrennt.");
        Serial.println("");
    }
    delay(1000);
}

/* ----------------------------------- ----------------------------------- */
void HandleBrowserIO()
{
    // Hier werden die GPIO Pins ein- oder ausgeschaltet
    if (header.indexOf("GET /heat/on") >= 0)
    {
        Serial.println("Heating on");
        HeatingState = 1;
        digitalWrite(HEATING, HIGH);
    }
    else if (header.indexOf("GET /heat/off") >= 0)
    {
        Serial.println("Heating off");
        HeatingState = 0;
        digitalWrite(HEATING, LOW);
    }
    else if (header.indexOf("GET /air/on") >= 0)
    {
        Serial.println(" Air on");
        AirState = 1;
        digitalWrite(AIR, HIGH);
    }
    else if (header.indexOf("GET /air/off") >= 0)
    {
        Serial.println("Air off");
        AirState = 0;
        digitalWrite(AIR, LOW);
    }
    else if (header.indexOf("GET /lamp/on") >= 0)
    {
        Serial.println(" Lamp on");
        LampState = 1;
        digitalWrite(LAMP, HIGH);
    }
    else if (header.indexOf("GET /lamp/off") >= 0)
    {
        Serial.println("Lamp off");
        LampState = 0;
        digitalWrite(LAMP, LOW);
    }
    else if (header.indexOf("GET /pump1/on") >= 0)
    {
        Serial.println(" Pump1 on");
        Pump1State = 1;
        digitalWrite(PUMP1, HIGH);
    }
    else if (header.indexOf("GET /pump1/off") >= 0)
    {
        Serial.println("Pump1 off");
        Pump1State = 0;
        digitalWrite(PUMP1, LOW);
    }
    else if (header.indexOf("GET /pump2/on") >= 0)
    {
        Serial.println(" Pump2 on");
        Pump2State = 1;
        digitalWrite(PUMP2, HIGH);
    }
    else if (header.indexOf("GET /pump2/off") >= 0)
    {
        Serial.println(" Pump2 off");
        Pump2State = 0;
        digitalWrite(PUMP2, LOW);
    }
     else if (header.indexOf("GET /filter/on") >= 0)
    {
        Serial.println(" Filter on");
        FilterState = 1;
        digitalWrite(FILTER, HIGH);
    }
    else if (header.indexOf("GET /filter/off") >= 0)
    {
        Serial.println(" filter off");
        FilterState = 0;
        digitalWrite(FILTER, LOW);
    }
}

/* ----------------------------------- ----------------------------------- */
void StoreTargetTemp(
    const float             Temp             
)
{
    EEPROM.begin(sizeof(float));
    for (unsigned int t=0; t<sizeof(storage); t++)
		EEPROM.write(PERMSTORE + t, *((char*)&storage + t));
    EEPROM.end();
}

/* ----------------------------------- ----------------------------------- */
void HandleSensors()
{
    unsigned int            t;

    Serial.println("2222222");
    double AnalogValue = analogRead(A0);
    // convert the analog signal to voltage
    // the ESP2866 A0 reads between 0 and ~3 volts, producing a corresponding value
    // between 0 and 1024. The equation below will convert the value to a voltage value.
    double AnalogVolts = (AnalogValue * 3.03) / 1024;
    Serial.println(AnalogVolts);
    Temperature = (AnalogVolts)*100; //converting from 10 mv per degree wit 500 mV offset
                                     //to degrees ((voltage - 500mV) times 100)

    StoreTargetTemp(Temperature);

}

/* ----------------------------------- ----------------------------------- */
void HandleForm(
    WiFiClient              client
    )
{
    // HTTP-Header fangen immer mit einem Response-Code an (z.B. HTTP/1.1 200 OK)
    // gefolgt vom Content-Type damit der Client weiss was folgt, gefolgt von einer Leerzeile:
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
    // Hier wird nun die HTML Seite angezeigt:
    client.println("<!DOCTYPE html><html>");
    client.print("<title>");
    client.print(SWNAME);
    client.println("</title>");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<link rel=\"icon\" href=\"data:,\">");
    // Es folgen der CSS-Code um die Ein/Aus Buttons zu gestalten
    // Hier können Sie die Hintergrundfarge (background-color) und Schriftgröße (font-size) anpassen
    client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
    client.println("div.table { display: table; width: auto; margin: auto; padding: 5px;}");
    client.println("div.spalte { display: table-cell; border: none; width: 200px; padding: 5px; }");
    client.println(".button { background-color: #b7263b ; color: white; padding: 16px 40px;");
    client.println("text-decoration: none; font-size: 25px; margin: 2px; cursor: pointer;}");
    client.println(".button2 {background-color:  #26b72c;}</style></head>");

    // Webseiten-Überschrift
    client.print("<body><h1>");
    client.print(SWNAME);
    client.print(" ");
    client.print(VERSION);
    client.println("</h1>");

    client.print("<h3>Aktuelle Wassertemp.: ");
    client.print(Temperature);
    client.println(" &deg;C</h3>");

    client.print("<p><h3>Zieltemp.: &deg;C</h3>");
    client.print("<input type='range' name='TEMPERATURE' min='20' max='40' value='15'>");
    client.println("</p>");

    /* ------------------------------- Row 1 ------------------------------- */
    client.println("<div class='table'>");
    client.println("<div class='table-row'>");
    client.println("<div class='spalte'>");
        if (0 == FilterState)
        {
            client.println("<a href=\"/filter/on\"><button class=\"button\">Filter<br>EIN</button></a>");
        }
        else
        {
            client.println("<a href=\"/filter/off\"><button class=\"button button2\">Filter<br>AUS</button></a>");
        }
    client.print("</div><div class='spalte'>");
        if (0 == Pump1State)
        {
            client.println("<p><a href=\"/pump1/on\"><button class=\"button\">Pumpe1<br>EIN</button></a></p>");
        }
        else
        {
            client.println("<p><a href=\"/pump1/off\"><button class=\"button button2\">Pumpe1<br>AUS</button></a></p>");
        }
    client.println("</div></div>");

    /* ------------------------------- Row 2 ------------------------------- */
    client.print("<div class='table-row'>");
    client.print("<div class='spalte'>");
        if (0 == LampState)
        {
            client.println("<p><a href=\"/lamp/on\"><button class=\"button\">Licht<br>EIN</button></a></p>");
        }
        else
        {
            client.println("<p><a href=\"/lamp/off\"><button class=\"button button2\">Licht<br>AUS</button></a></p>");
        }
    client.print("</div><div class='spalte'>");
        if (0 == Pump2State)
        {
            client.println("<p><a href=\"/pump2/on\"><button class=\"button\">Pumpe2<br>EIN</button></a></p>");
        }
        else
        {
            client.println("<p><a href=\"/pump2/off\"><button class=\"button button2\">Pumpe2<br>AUS</button></a></p>");
        }
    client.println("</div></div>");

    /* ------------------------------- Row 3 ------------------------------- */
    client.print("<div class='table-row'>");
    client.print("<div class='spalte'>");
        if (0 == AirState)
        {
            client.println("<p><a href=\"/air/on\"><button class=\"button\">Luft<br>EIN</button></a></p>");
        }
        else
        {
            client.println("<p><a href=\"/air/off\"><button class=\"button button2\">Luft<br>AUS</button></a></p>");
        }
    client.print("</div><div class='spalte'>");
        if (HeatingState == 0)
        {
            client.println("<p><a href=\"/heat/on\"><button class=\"button\">Heizung<br>EIN</button></a></p>");
        }
        else
        {
            client.println("<p><a href=\"/heat/off\"><button class=\"button button2\">Heizung<br>AUS</button></a></p>");
        }
    client.println("</div></div>");

    client.println("</div>");
    client.println("</body></html>");

    // Die HTTP-Antwort wird mit einer Leerzeile beendet
    client.println();
}

void
ShowWaterTemp(
    const float          Temperature
    )
{
    Serial.print("actual Temp.: ");
    Serial.print(Temperature);
    Serial.println(" C");

    Lcd.setCursor(3,0);
    Lcd.print("Water temp.:");
    Lcd.setCursor(5,1);
    Lcd.print(Temperature);
    Lcd.setCursor(10,1);
    Lcd.print((char)223);
    Lcd.print("C");
}
    