#include "whirlie.h"

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

    server.begin();

    SetupPorts();
    InitializeData();
}

/* ------------------------------- Main loop ------------------------------- */
void loop()
{
    WiFiClient client = server.available();

    if (client)
    {
        Serial.println("New Client.");
        String currentLine = ""; // erstelle einen String mit den eingehenden Daten vom Client
        while (client.connected())
        {
            // wiederholen so lange der Client verbunden ist
            if (client.available())
            {                           // Fall ein Byte zum lesen da ist,
                char c = client.read(); // lese das Byte, und dann
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
                        HandleForm(client);
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
        client.stop();
        Serial.println("Client getrennt.");
        Serial.println("");
    }
}

/* ----------------------------------- ----------------------------------- */
void HandleBrowserIO()
{
    // Hier werden die GPIO Pins ein- oder ausgeschaltet
    if (header.indexOf("GET /12/on") >= 0)
    {
        Serial.println("Heating on");
        HeatingState = 1;
        digitalWrite(HEATING, HIGH);
    }
    else if (header.indexOf("GET /12/off") >= 0)
    {
        Serial.println("Heating off");
        HeatingState = 0;
        digitalWrite(HEATING, LOW);
    }
    else if (header.indexOf("GET /16/on") >= 0)
    {
        Serial.println(" Air on");
        AirState = 1;
        digitalWrite(AIR, HIGH);
    }
    else if (header.indexOf("GET /16/off") >= 0)
    {
        Serial.println("Air off");
        AirState = 0;
        digitalWrite(AIR, LOW);
    }
    else if (header.indexOf("GET /13/on") >= 0)
    {
        Serial.println(" Air on");
        LampState = 1;
        digitalWrite(LAMP, HIGH);
    }
    else if (header.indexOf("GET /13/off") >= 0)
    {
        Serial.println("Air off");
        LampState = 0;
        digitalWrite(LAMP, LOW);
    }
    else if (header.indexOf("GET /14/on") >= 0)
    {
        Serial.println(" Pump1 on");
        Pump1State = 1;
        digitalWrite(PUMP1, HIGH);
    }
    else if (header.indexOf("GET /14/off") >= 0)
    {
        Serial.println("Pump1 off");
        Pump1State = 0;
        digitalWrite(PUMP1, LOW);
    }
    else if (header.indexOf("GET /15/on") >= 0)
    {
        Serial.println(" Pump2 on");
        Pump2State = 1;
        digitalWrite(PUMP2, HIGH);
    }
    else if (header.indexOf("GET /15/off") >= 0)
    {
        Serial.println(" Pump2 off");
        Pump2State = 0;
        digitalWrite(PUMP2, LOW);
    }
}

/* ----------------------------------- ----------------------------------- */
void HandleSensors()
{
    Serial.println("2222222");
    double AnalogValue = analogRead(A0);
    // convert the analog signal to voltage
    // the ESP2866 A0 reads between 0 and ~3 volts, producing a corresponding value
    // between 0 and 1024. The equation below will convert the value to a voltage value.
    double AnalogVolts = (AnalogValue * 3.03) / 1024;
    Serial.println(AnalogVolts);
    Temperature = (AnalogVolts)*100; //converting from 10 mv per degree wit 500 mV offset
                                     //to degrees ((voltage - 500mV) times 100)
}

/* ----------------------------------- ----------------------------------- */
void HandleForm(WiFiClient client)
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
    client.println(".button { background-color: #335544; border: none; color: white; padding: 16px 40px;");
    client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
    client.println(".button2 {background-color: #886677;}</style></head>");

    // Webseiten-Überschrift
    client.print("<body><h1>");
    client.print(SWNAME);
    client.print(" ");
    client.print(VERSION);
    client.println("</h1>");

    client.print("<h2>Watertemperature:");
    client.print(Temperature);
    client.println("</h2>");

    client.println("<input type='range' name='VOLUME' min='0' max='30' value='15'>");

    // Zeige den aktuellen Status, und AN/AUS Buttons for GPIO 5
    client.print("<p>Heating - State ");
    client.print(HeatingState ? "ON" : "OFF");
    client.println("</p>");
    // wenn HeatingState = off, zeige den EIN Button
    if (HeatingState == 0)
    {
        client.println("<p><a href=\"/12/on\"><button class=\"button\">EIN</button></a></p>");
    }
    else
    {
        client.println("<p><a href=\"/12/off\"><button class=\"button button2\">AUS</button></a></p>");
    }

    // Das gleiche für Air
    client.print("<p>Air - State ");
    client.print(AirState ? "ON" : "OFF");
    client.println("</p>");
    // Wenn AirState = off, zeige den EIN Button
    if (0 == AirState)
    {
        client.println("<p><a href=\"/16/on\"><button class=\"button\">EIN</button></a></p>");
    }
    else
    {
        client.println("<p><a href=\"/16/off\"><button class=\"button button2\">AUS</button></a></p>");
    }

    // Das gleiche für Pump1
    client.print("<p>Pump1 - State ");
    client.print(Pump1State ? "ON" : "OFF");
    client.println("</p>");
    // Wenn Pump1State = off, zeige den EIN Button
    if (0 == Pump1State)
    {
        client.println("<p><a href=\"/14/on\"><button class=\"button\">EIN</button></a></p>");
    }
    else
    {
        client.println("<p><a href=\"/14/off\"><button class=\"button button2\">AUS</button></a></p>");
    }

    // Das gleiche für Pump2
    client.print("<p>Pump2 - State ");
    client.print(Pump2State ? "ON" : "OFF");
    client.println("</p>");
    // Wenn Pump2State = off, zeige den EIN Button
    if (0 == Pump2State)
    {
        client.println("<p><a href=\"/15/on\"><button class=\"button\">EIN</button></a></p>");
    }
    else
    {
        client.println("<p><a href=\"/15/off\"><button class=\"button button2\">AUS</button></a></p>");
    }
    client.println("</body></html>");

    // Die HTTP-Antwort wird mit einer Leerzeile beendet
    client.println();
}
