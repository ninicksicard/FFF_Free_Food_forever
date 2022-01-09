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
              FeatureEnable.Aeration = !FeatureEnable.Aeration;
              Aeration_on();
            }
            else if (header.indexOf("GET /WaterLevel") >= 0) {
              FeatureEnable.WaterLevel = !FeatureEnable.WaterLevel;
              waterPump_on();
            }
            
            else if (header.indexOf("GET /extraction") >= 0) {
              FeatureEnable.extraction = !FeatureEnable.extraction;
              extraction_on();
            }
            else if (header.indexOf("GET /Lighting") >= 0) {
              FeatureEnable.Lighting = !FeatureEnable.Lighting;
              lightSwitch_on();
            }
            else {
              buttonpressed = false;
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
            client.println("<p>Time : " + String(GetHour()) + "</p>");
            client.println("<p>Water Level : " + String(Variables.WaterLevel) + "</p>");

            // todo : add condition to display all these buttons to return to the root address
            // Display current Button 1 states (text above the button)
            // lights button

            if (!buttonpressed) {

              client.println("<p>Lights are " + String(FeatureEnable.Lighting) + "</p>");
              client.println("<p><a href=\"/Lighting\"><button class=\"button\">Toggle</button></a></p>");

              // Aeration button
              client.println("<p>water wheel is " + String(FeatureEnable.Aeration) + "</p>");
              client.println("<p><a href=\"/Aeration\"><button class=\"button\">Toggle</button></a></p>");


              // WaterLevel button
              client.println("<p>Water Level control is " + String(FeatureEnable.WaterLevel) + "</p>");
              client.println("<p><a href=\"/WaterLevel\"><button class=\"button\">Toggle</button></a></p>");

              // extraction button
              client.println("<p> extraction pump is " + String(FeatureEnable.extraction) + "</p>");
              client.println("<p><a href=\"/extraction\"><button class=\"button\">Toggle</button></a></p>");

              // client.println("<form action=\"/get\">input1: <input type=\"text\" name=\"input1\"><input type=\"submit\" value=\"Submit\"></form>");


            } else {
              client.println("<p>action processed</p>");
              client.println("<p><a href=\"/\"><button class=\"button\">OK</button></a></p>");
            }

            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();

            // Break out of the while loop
            break;
          }
          else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }

    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
//    Serial.println("Client disconnected.");
//    Serial.println("");
  }
}
