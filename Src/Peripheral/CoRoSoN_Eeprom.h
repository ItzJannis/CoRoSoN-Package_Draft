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
// File:    CoRoSoN_Eeprom.h
// Purpose: Providing usefull utility functions to make use of the 
//          ESP32's eeprom
//            i.e. save last robot state to restart into it after reset
// -------------------------------------------------------------------
#ifndef COROSON_EEPROM_H
#define COROSON_EEPROM_H
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include <Arduino.h>
#include <EEPROM.h>
#include "../Shared/CoRoSoN_Util.h"
/*********************************************************************
* 
*  Types
*
*********************************************************************/
//
// Modifiy this struct for the eeprom interactions 
// to read / write data however you like
//
struct EEPROM_DATA {
  bool IsInStandby;
  // add what you want to store here
};

/*********************************************************************
* 
*  Functions
*
*********************************************************************/
/************************************************************
*
* ! [SETUP]
* ? EEPROM_Init()
*   
* * Description:
*     Initializes the ESP32's eeprom
*
************************************************************/
ERRORS EEPROM_Init(unsigned short Address);

/************************************************************
*
* ? EEPROM_Read()
*   
* * Description:
*     Reads the currently in the eeprom stored EEPROM_DATA
*
************************************************************/
EEPROM_DATA EEPROM_Read();

/************************************************************
*
* ? EEPROM_Write()
*   
* * Description:
*     Writes the passed EEPROM_DATA to the eeprom
*
************************************************************/
ERRORS EEPROM_Write(const EEPROM_DATA& Data);
#endif // COROSON_EEPROM_H