// Variable to store the HTTP request
String header;

//todo : replace the format to use the one from this page https://randomnerdtutorials.com/esp32-esp8266-input-data-html-form/

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void UI_loop() {
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    String currentLine = "";                // make a String to hold incoming data from the client

    // timeout vars
    currentTime = millis();
    previousTime = currentTime;


    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected

      //timeout var
      currentTime = millis();

      if (client.available()) {            // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:

          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();



            // buttons
            bool buttonpressed = true;
            if (header.indexOf("GET /Aeration") >= 0) {
              FeatureAvailable.Aeration = !FeatureAvailable.Aeration;
              if (FeatureAvailable.Aeration) {
                Aeration_on();
              } else {
                Aeration_off();
              }
            } else if (header.indexOf("GET /PhRead") >= 0) {
              FeatureAvailable.PhRead = !FeatureAvailable.PhRead;
            } else if (header.indexOf("GET /WaterWheel") >= 0) {
              FeatureAvailable.WaterWheel = !FeatureAvailable.WaterWheel;
            } else if (header.indexOf("GET /WaterLevel") >= 0) {
              FeatureAvailable.WaterLevel = !FeatureAvailable.WaterLevel;
            } else if (header.indexOf("GET /DensityRead") >= 0) {
              FeatureAvailable.DensityRead = !FeatureAvailable.DensityRead;
            } else if (header.indexOf("GET /Heating") >= 0) {
              if (FeatureAvailable.Heating) {
                FeatureAvailable.Heating = false;
                Temps.MinTreshold = 0;
                Temps.MaxTreshold = 1;
                heating_off();
              } else {
                FeatureAvailable.Heating = true;
                if (!SystemStates.NightTime) {
                  Temps.MinTreshold = Temps.DayMin;
                  Temps.MaxTreshold = Temps.DayMax;
                } else {
                  Temps.MinTreshold = Temps.NightMin;
                  Temps.MaxTreshold = Temps.NightMax;
                }
              }
            } else if (header.indexOf("GET /Lighting") >= 0) {
              if (FeatureAvailable.Lighting) {
                FeatureAvailable.Lighting = false;
                lightSwitch_off();
              } else {
                FeatureAvailable.Lighting = true;
                lightSwitch_on();
              }

            } else if (header.indexOf("GET /CalpH7") >= 0) {
              Set_pH_Calibrate_7();
            } else if (header.indexOf("GET /CalpH4") >= 0) {
              Set_pH_Calibrate_4();
            } else if (header.indexOf("GET /CalpH10") >= 0) {
              Set_pH_Calibrate_10();
            }else {
              buttonpressed=false;
            }

            //todo add button for extraction density set 



            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");


            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");


            // Buttons display style

            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>Spirulina Bioreactor</h1>");


            client.println("<p>Temperature : " + String(LastValue.TempRead) + "</p>");
            client.println("<p>pH :  " + String(LastValue.PhRead) + "</p>");
            client.println("<p>Water Level : " + String(LastValue.WaterLevel) + "</p>");
            client.println("<p>Density : " + String(LastValue.PopulationRead) + "</p>");


            // todo : add condition to display all these buttons to return to the root address
            // Display current Button 1 states (text above the button)
            // lights button

            if (!buttonpressed){
              
            client.println("<p>Lights are " + String(FeatureAvailable.Lighting) + "</p>");
            client.println("<p><a href=\"/Lighting\"><button class=\"button\">Toggle</button></a></p>");

            // heating button
            client.println("<p>Heating is " + String(FeatureAvailable.Heating) + "</p>");
            client.println("<p><a href=\"/Heating\"><button class=\"button\">Toggle</button></a></p>");


            // Aeration button
            client.println("<p>Aeration is " + String(FeatureAvailable.Aeration) + "</p>");
            client.println("<p><a href=\"/Aeration\"><button class=\"button\">Toggle</button></a></p>");

            // PhRead button
            client.println("<p>Ph Sensor is " + String(FeatureAvailable.PhRead) + "</p>");
            client.println("<p><a href=\"/PhRead\"><button class=\"button\">Toggle</button></a></p>");

            // WaterWheel button
            client.println("<p>WaterWheel is " + String(FeatureAvailable.WaterWheel) + "</p>");
            client.println("<p><a href=\"/WaterWheel\"><button class=\"button\">Toggle</button></a></p>");

            // WaterLevel button
            client.println("<p>WaterLevel sensor is " + String(FeatureAvailable.WaterLevel) + "</p>");
            client.println("<p><a href=\"/WaterLevel\"><button class=\"button\">Toggle</button></a></p>");



            //pHcalibration buttons 7
            client.println("<p> pH 7 read value is " + String(Ph_Sensor_Variables.calibrate_7) + "</p>");
            client.println("<p><a href=\"/CalpH7\"><button class=\"button\">Toggle</button></a></p>");
            //pHcalibration buttons 4
            client.println("<p>pH 4 read value is " + String(Ph_Sensor_Variables.calibrate_4) + "</p>");
            client.println("<p><a href=\"/CalpH4\"><button class=\"button\">Toggle</button></a></p>");
            //pHcalibration buttons 10
            client.println("<p>pH 7 read value is 10 " + String(Ph_Sensor_Variables.calibrate_10) + "</p>");
            client.println("<p><a href=\"/CalpH10\"><button class=\"button\">Toggle</button></a></p>");


// adding entry
//            client.println("<form action=\"/get\">input1: <input type=\"text\" name=\"input1\"><input type=\"submit\" value=\"Submit\"></form>"); 
            
            
            }else{
              client.println("<p>action processed</p>");
              client.println("<p><a href=\"/\"><button class=\"button\">OK</button></a></p>");
            }


            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();

            // Break out of the while loop
            break;

          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }

    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
