// -------------------------------------------------------------------
/*********************************************************************
* 
* !    LICENSE
*         GNU GPL 3.0
*
*********************************************************************/
// -------------------------------------------------------------------
/*********************************************************************
* 
* *     CoRoSoN
* *       Co|mponents for 
* *       Ro|bocupJunior
* *       So|ccer
* *        N|ewcomers
*
* *    Project:     https://github.com/CoRoSoN-Kit/
* *    Competition: https://www.robocup.org/
*
* *    Created by BohleBots:
* *       https://www.bohlebots.de
* *       info@bohlebots.de
* 
*********************************************************************/
// -------------------------------------------------------------------
// File:    CoRoSoN_I2C.cpp
// Purpose: Implementations for CoRoSoN_I2C.h
// -------------------------------------------------------------------
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include "CoRoSoN_I2C.h"
/*********************************************************************
* 
*  Implementations
*
*********************************************************************/
static bool _InitSuccessfull = false;

static void _CheckInit(void) {
  if(!_InitSuccessfull) {
    DEBUG_BLOCK("I2C_Init() was not called to set data and clock pin before using I2C", 1000);
  }
}

ERRORS I2C_Init(unsigned short SerialDataPin, unsigned short SerialClockPin) {
  ERRORS r;

  r = OKAY;
  if(!Wire.begin(SerialDataPin, SerialClockPin)) {
    r |= CONNECT_FAILED | ERROR_BREAK_OUT;
    DEBUG_ERRORS(r);
    DEBUG_BLOCK("I2C-Bus konnte nicht gestartet werden", 1000);
  } else {
    _InitSuccessfull = true;
  }
  return r;
}

ERRORS I2C_TestConnection(unsigned short Address) {
  ERRORS r;

  r = OKAY;
  _CheckInit();
  Wire.beginTransmission(Address);
  if(Wire.endTransmission()) {
    r |= CONNECT_FAILED;
    DEBUG_ERRORS(r);
    DEBUG_PRINT(Address);
  }
  return r;
}

ERRORS I2C_Write(unsigned short Address, byte aMessageBytes[], unsigned int NumBytes) {
  ERRORS r;
  int    MessageLength;

  _CheckInit();
  r = I2C_TestConnection(Address);
  if(r) {
    r |= ERROR_BREAK_OUT;
    DEBUG_ERRORS(r);
    DEBUG_PRINT(Address);
    return r;
  }
  Wire.beginTransmission(Address);
  MessageLength = Wire.write(aMessageBytes, NumBytes);
  if(MessageLength != NumBytes) {
    r |= INVALID_ANSWER;
    DEBUG_ERRORS(r);
    DEBUG_PRINT(MessageLength);
    DEBUG_PRINT(NumBytes);
  }
  if(Wire.endTransmission()) {
    r |= CONNECT_FAILED;
    DEBUG_ERRORS(r);
    DEBUG_PRINT(Address);
  }
  return r;
}

ERRORS I2C_ReadBlocking(unsigned short Address, byte aAnswerBytes[], unsigned int NumBytes) {
  ERRORS r;
  int    AnswerLength;

  r = OKAY;
  _CheckInit();
  r = I2C_TestConnection(Address);
  if(r) {
    r |= ERROR_BREAK_OUT;
    DEBUG_ERRORS(r);
    DEBUG_PRINT(Address);
    return r;
  }
  Wire.requestFrom(Address, NumBytes);
  while(Wire.available() < NumBytes) {
    continue; // Busy waiting
  }
  AnswerLength = Wire.readBytes(aAnswerBytes, NumBytes);
  if(AnswerLength != NumBytes) {
    r |= INVALID_ANSWER;
    DEBUG_ERRORS(r);
    DEBUG_PRINT(AnswerLength);
    DEBUG_PRINT(NumBytes);
    return r;
  }
  return r;
}