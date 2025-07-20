#ifndef CLI_H
#define CLI_H

#include <Arduino.h>
#include "modes.h"




extern ModeManager modeManager;

template <typename T>
void customPrintln(T message);

template <typename T>
void customPrint(T message);

// Explicit instantiations for types you expect to use
extern template void customPrintln<String>(String message);
extern template void customPrint<String>(String message);
extern template void customPrintln<int>(int message);
extern template void customPrint<int>(int message);
extern template void customPrintln<char const*>(char const* message);
extern template void customPrint<char const*>(char const* message);
extern template void customPrintln<char*>(char* message);
extern template void customPrint<char*>(char* message);
extern template void customPrintln<touch_pad_t>(touch_pad_t);
extern template void customPrint<touch_pad_t>(touch_pad_t);
extern template void customPrint<unsigned short>(unsigned short);

void setupCLI();
void handleCLI();
void processCommand(String command);

#endif
