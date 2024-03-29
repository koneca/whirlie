// whirlie_ui.ino


/* ----------------------------------- ----------------------------------- */
void
DisplayButton(
    WiFiClient              client,
    String                  Name,
    const unsigned int      IsOn
    )
{
    String                  Text = "<a href=\"/";
    char                    TextBuffer[0x100];

//    sprintf(TextBuffer, "<a href=\"/%s/%s \"><button class=\"button\">%s<br>%s</button></a>", Name, IsOn? "off": "on", Name, IsOn? "AUS": "EIN" )
    Text += Name;
    Text += "/";
    Text += IsOn? "off": "on";
    Text += "\"><button class=\"button";
    Text += IsOn? " button2": "";
    Text += "\">";
    Text += Name;
    Text += "<br>";
    Text += IsOn? "AUS": "EIN";
    Text += "</button></a>";

    client.println(Text);
}

/* ----------------------------------- ----------------------------------- */
void HandleBrowserIO(
    WiFiClient              Client
    )
{
    String                  PostData = "";
    char                    c;
    
    Serial.println(header);
    if(0 <= header.indexOf("GET /"))
    {
        HandleForm();
    }
    else if(0 <= header.indexOf("POST /"))
    {
        Serial.println("got an post");
        while (Client.available())
        {
             c = Client.read();
             PostData += c;
        }
        HandleSlider(PostData);
        
    }
}

/* ----------------------------------- ----------------------------------- */
void
HandleSlider(
    String                  PostData
)
{
    unsigned int            Position = 0;

    if(0 <= PostData.indexOf("TargetTemp"))
    {
        Position = PostData.indexOf("=");
        TargetTemp = PostData.substring(++Position).toInt();
        Serial.println(TargetTemp);
        StoreTargetTemp(TargetTemp);
    }
}

/* ----------------------------------- ----------------------------------- */
void
HandleForm()
{
    bool                     SwitchOff = false;
    unsigned int             DPort = 0;
    bool                     PortSwitched = false;
    char                     DebugBuffer[100];

    Serial.println("got an get");

    if(0 <= header.indexOf("off"))
    {
        SwitchOff = true;
    }

    if (header.indexOf(TXT_HEATING) >= 0)
    {
        HeatingState = ! SwitchOff;
        DPort = DHEATING;
        PortSwitched = true;
        PanelState ^= 1UL << BTN_HEATING;
    }

    else if (header.indexOf(TXT_LIGHT) >= 0)
    {
        LightState = ! SwitchOff;
        DPort = DLAMP;
        PortSwitched = true;
        PanelState ^= 1UL << BTN_LAMP;
    }
    else if (header.indexOf(TXT_AIR) >= 0)
    {
        AirState = ! SwitchOff;
        DPort = DAIR;
        PortSwitched = true;
        PanelState ^= 1UL << BTN_AIR;
    }
    else if (header.indexOf(TXT_PUMP1) >= 0)
    {
        Pump1State = ! SwitchOff;
        DPort = DPUMP1;
        PortSwitched = true;
        PanelState ^= 1UL << BTN_PUMP1;
    }
    else if (header.indexOf(TXT_PUMP2) >= 0)
    {
        Pump2State = ! SwitchOff;
        DPort = DPUMP2;
        PortSwitched = true;
        PanelState ^= 1UL << BTN_PUMP2;
    }
    else if (header.indexOf(TXT_FILTER) >= 0)
    {
        FilterState = ! SwitchOff;
        DPort = DFILTER;
        PortSwitched = true;
        PanelState ^= 1UL << BTN_FILTER;
    }

        if(PortSwitched)
    {
        sprintf(DebugBuffer, "switched port %d %s", DPort, SwitchOff ? "Off" : "On");
        Serial.println(DebugBuffer);
        StateChangedByBrowser = true;

        digitalWrite(DPort, ! SwitchOff);
    }
}

/* ----------------------------------- ----------------------------------- */
void
ShowForm(
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
    client.println("<style>html { font-family: Verdana; display: inline-block; margin: 0px auto; text-align: center; background-color: #474747}");
    client.println(".heading { Background-color: #3FA5A9; padding: 2px; margin: 5px auto;}");
    client.println("div.table { display: table; width: auto; margin: 20px auto;}");
    client.println("div.spalte { display: table-cell; width: 49% }");
    client.println(".button { background-color: #F26B38; width: 190px; height: 130px; margin: 5% 10px; border: none; color: white;");
    client.println("text-decoration: none; font-size: 32px; cursor: pointer;}");
    client.println(".button2 {background-color:  #2F9599;}</style>");
    client.print("<script>function sendPosition(slider, value) { console.log(slider+': '+value);  var request = new XMLHttpRequest();  request.open('POST', 'http://");
    client.print(HostIp);
    client.println("/');");
    client.println("request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded'); request.send('TargetTemp'+'='+value);");
    client.println("setTimeout(location.reload(), 100); }");
    client.println("</script>");
    
    client.println("</head>");

    // Webseiten-Überschrift
    client.print("<body>");
    client.println("<div class='heading'>");
    client.println("<h1>");
    client.print(SWNAME);
    client.print(" ");
    client.print(VERSION);
    client.println("</h1>");

    client.print("<p>Aktuelle Wassertemp.: ");
    client.print(Temperature);
    client.println(" &deg;C</p>");

    client.print("<p>Zieltemp.: ");
    client.print(TargetTemp);
    client.print(".0 &deg;C</br>");
    client.print("<input type='range' id='TempSlider' min='20' max='40' value='");
    client.print(TargetTemp);
    client.print("' onchange='sendPosition(this.id, this.value);'>");
    client.println("</p>");
    client.println("</div>");

    /* ------------------------------- Row 1 ------------------------------- */
    client.println("<div class='table'>");
    client.println("<div class='table-row'>");
    client.println("<div class='spalte'>");

        DisplayButton(client, TXT_FILTER, FilterState);
    client.print("</div><div class='spalte'>");
        DisplayButton(client, TXT_PUMP1, Pump1State);

    client.println("</div></div>");

    /* ------------------------------- Row 2 ------------------------------- */
    client.print("<div class='table-row'>");
    client.print("<div class='spalte'>");

        DisplayButton(client, TXT_LIGHT, LightState);
    client.print("</div><div class='spalte'>");
        DisplayButton(client, TXT_PUMP2, Pump2State);
        
    client.println("</div></div>");

    /* ------------------------------- Row 3 ------------------------------- */
    client.print("<div class='table-row'>");
    client.print("<div class='spalte'>");
    
        DisplayButton(client, TXT_AIR, AirState);
    client.print("</div><div class='spalte'>");
        DisplayButton(client, TXT_HEATING, HeatingState);
        
    client.println("</div></div>");

    client.println("</div>");
    client.println("</body></html>");

    // Die HTTP-Antwort wird mit einer Leerzeile beendet
    client.println();
}

