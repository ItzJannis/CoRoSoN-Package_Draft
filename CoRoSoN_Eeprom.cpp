// -------------------------------------------------------------------
/*********************************************************************
* 
* !    LICENSE
*         TODO: what kind of licens to use?
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
// File:    CoRoSoN_Eeprom.cpp
// Purpose: Implementations for CoRoSoN_Eeprom.h
// -------------------------------------------------------------------
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include "CoRoSoN_Eeprom.h"
/*********************************************************************
* 
*  Implementations
*
*********************************************************************/
unsigned int _Address = 0;



ERRORS EEPROM_Init(unsigned int Address) {
  _Address = Address;
  if (!EEPROM.begin(sizeof(EEPROM_DATA))) {
    return (CONNECT_FAILED | ERROR_BREAK_OUT);
  }
  return OKAY;
}

EEPROM_DATA EEPROM_Read() {
  EEPROM_DATA Data;
  ZEROMEM(Data); // Init data with 0s
  EEPROM.readBytes(_Address, &Data, sizeof(EEPROM_DATA));
  return Data;
}

ERRORS EEPROM_Write(const EEPROM_DATA& Data) {
  EEPROM.writeBytes(_Address, &Data, sizeof(EEPROM_DATA));
  if (!EEPROM.commit()) {
    return (CONNECT_FAILED | ERROR_BREAK_OUT);
  }
  return OKAY;
}