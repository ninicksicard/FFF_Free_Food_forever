#include "time_sync.h"
#include "cli.h" 
void setupTimeSync() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  customPrintln("\nWaiting for time");
  while (!time(nullptr)) {
    customPrint(".");
    delay(1000);
  }
  time_t now = time(nullptr);
  customPrintln(ctime(&now));
}

void syncTime() {
  time_t now = time(nullptr);
  //customPrintln(ctime(&now));
}
