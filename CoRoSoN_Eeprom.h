// -------------------------------------------------------------------
/*********************************************************************
* 
*      CoRoSoN
*         Co|mponents for 
*         Ro|bocupJunior
*         So|ccer
*          N|ewcomers
*
*      Project:     https://github.com/CoRoSoN-Kit/
*      Competition: https://www.robocup.org/
*
*      Created by BohleBots
*         https://www.bohlebots.de
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
#include "CoRoSoN_Util.h"
/*********************************************************************
* 
*  Config
*
*********************************************************************/
#define EEPROM_RW_ADD 0 // Address to read from and write to
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
* * Beschreibung:
*     Initializes the ESP32's eeprom
*
************************************************************/
ERRORS EEPROM_Init();
/************************************************************
*
* ? EEPROM_Read()
*   
* * Beschreibung:
*     reads the currently in the eeprom stored EEPROM_DATA
*
************************************************************/
EEPROM_DATA EEPROM_Read();
/************************************************************
*
* ? EEPROM_Write()
*   
* * Beschreibung:
*     Schreibt die uebergebenen Daten in das EEPROMs
*
************************************************************/
ERRORS EEPROM_Write(const EEPROM_DATA& Data);
#endif // COROSON_EEPROM_H