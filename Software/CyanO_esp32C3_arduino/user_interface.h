#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <ESPAsyncWebServer.h>

class UserInterface {
public:
    UserInterface();
    void setupUserInterface();
    void handleCommand(AsyncWebServerRequest *request);
    void printToWeb(String message);
    void printToWebln(String message);
    void clearMessageBuffer();
private:
    AsyncWebServer server;
    String messageBuffer;
};

extern UserInterface ui;

#endif