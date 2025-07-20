// logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include "cli.h"
#include "web_server.h"
#include "file_manager.h"

enum LogOutput {
  SERIAL,
  WEB_UI,
  LOG_FILE,
  // Add more output options here...
};

class Logger {
public:
  Logger(CLI& cli, WebServer& webServer, FileManager& fileManager);
  void logMessage(LogOutput output, const char* message);

private:
  CLI& cli;
  WebServer& webServer;
  FileManager& fileManager;
};

#endif
