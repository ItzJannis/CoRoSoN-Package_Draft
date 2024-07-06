#include "CoRoSoN.h"

void setup() {
  Serial.begin(115200);
  //DEBUG_BLOCK("test", 1000); // uncomment this line to block the processor with periodic printing
}

void loop() {
  ERRORS Errors = OKAY; // start with everything okay
  
  DEBUG_PRINT(Errors);
  // create bitmap to show which errors occured
  Errors = ERROR_BREAK_OUT | PROCESS_RUNNING | INVALID_CONFIG | CONNECT_FAILED;
  // print errors because one occured
  DEBUG_PRINT(Errors);
  DEBUG_ERRORS(Errors);
  // block cpu for ever with "test" printed every 2 seconds
  DEBUG_BLOCK("test", 2000);
}