#include "user_interface.h"
#include "cli.h"  // Include the CLI header to process commands

UserInterface::UserInterface()
  : server(80) {}

void UserInterface::setupUserInterface() {
  // Setup the server here
  server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (request->client()->connected()) {
      String html = "<html><body>";
      html += "<form id=\"commandForm\" action=\"/command\" method=\"post\">";
      html += "<label for=\"command\">Command:</label><br>";
      html += "<input type=\"text\" id=\"command\" name=\"command\"><br>";
      html += "<input type=\"submit\" value=\"Submit\">";
      html += "</form>";
      html += "<h2>Messages:</h2>";
      html += "<div id=\"messages\">" + this->messageBuffer + "</div>";
      html += "<div id=\"submittedCommands\"></div>";
      html += "<script>";
      html += "document.getElementById('commandForm').addEventListener('submit', function(event) {";
      html += "event.preventDefault();";
      html += "var command = document.getElementById('command').value;";
      html += "fetch('/command', {method: 'POST', body: new URLSearchParams('command=' + command)}).then(function(response) {";
      html += "if(response.ok) {";
      html += "return response.text();";
      html += "} else {";
      html += "throw new Error('Failed to send command');";
      html += "}";
      html += "}).then(function(text) {";
      html += "document.getElementById('submittedCommands').innerHTML += '<p>' + command + '</p>';";
      html += "document.getElementById('command').value = '';";
      html += "location.reload();";  // Add this line to reload the page
      html += "}).catch(function(error) {";
      html += "console.error('Failed to send command: ', error);";
      html += "});";
      html += "});";
      html += "</script>";
      html += "</body></html>";
      request->send(200, "text/html", html);
    } else {
      request->send(500, "text/plain", "Client disconnected");
    }
  });
  // Add a new endpoint for sending commands
  server.on("/command", HTTP_POST, [this](AsyncWebServerRequest *request) {
    if (request->client()->connected()) {
      this->handleCommand(request);
    } else {
      request->send(500, "text/plain", "Client disconnected");
    }
  });
  server.begin();
}




void UserInterface::handleCommand(AsyncWebServerRequest *request) {
  if (request->hasParam("command", true)) {                        // Check if the request has a "command" parameter in the body
    String command = request->getParam("command", true)->value();  // Get the command value
    processCommand(command);                                       // Process the command using the CLI's processCommand function
    request->send(200, "text/plain", "Command processed");
  } else {
    request->send(400, "text/plain", "No command provided");
  }
}

void UserInterface::printToWebln(String message) {
  messageBuffer += message + "<br>";
  if (messageBuffer.length() > 5000) {              // Limit the size of the message buffer to prevent it from getting too large
    messageBuffer = messageBuffer.substring(1000);  // Remove the oldest messages
  }
}

void UserInterface::printToWeb(String message) {
  messageBuffer += message + "<br>";
  if (messageBuffer.length() > 5000) {              // Limit the size of the message buffer to prevent it from getting too large
    messageBuffer = messageBuffer.substring(1000);  // Remove the oldest messages
  }
}

void UserInterface::clearMessageBuffer() {
  messageBuffer = "";
}
