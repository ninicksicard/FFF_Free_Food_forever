// logger.cpp
#include "logger.h"

Logger::Logger(CLI& cli, WebServer& webServer, FileManager& fileManager)
  : cli(cli), webServer(webServer), fileManager(fileManager) {}

void Logger::logMessage(LogOutput output, const char* message) {
  switch (output) {
    case SERIAL:
      cli.println(message);
      break;
    case WEB_UI:
      webServer.sendMessage(message);
      break;
    case LOG_FILE:
      fileManager.writeLog(message);
      break;
    // Handle other output options here...
  }
}
