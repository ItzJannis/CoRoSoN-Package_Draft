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
// File:    CoRoSoN.h
// Purpose: Main header of CoRoSoN kit meant to run on the main board
// -------------------------------------------------------------------
#ifndef COROSON_H
#define COROSON_H
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include <Arduino.h>
#include "CoRoSoN_Util.h"
#include "CoRoSoN_Eeprom.h"
#include "CoRoSoN_I2C.h"
#include "CoRoSoN_IR-Ring.h"
#include "CoRoSoN_Pixy.h"
#include "CoRoSoN_Compass.h"
#include "CoRoSoN_Config.h"
/*********************************************************************
* 
*  Types
*
*********************************************************************/
//
// Color enumaration to easily use LEDs
//
enum COLOR : int {
   OFF = 0
  ,GREEN
  ,RED
  ,YELLOW
  ,BLUE
  ,CYAN
  ,MAGENTA
  ,WHITE
};
//
// Side enumaration to easily choose between left and right button or LED 
//
enum SIDE : int {
   LEFT = 0
  ,RIGHT
};
//
// Button and LED module enumaration to easily choose between the two button and LED modules
//
enum I2C_ADD_BTNLED_MODULE : int {
   I2C_MODULE_1 = I2C_BASEADD_BTNLEDS + I2C_BTNLED_CODE_1
  ,I2C_MODULE_2 = I2C_BASEADD_BTNLEDS + I2C_BTNLED_CODE_2
};
//
// Motor enumaration to easily choose a motor
//
enum MOTOR : int {
   MOTOR_1 = 1
  ,MOTOR_2
  ,MOTOR_3
  ,MOTOR_4
};
//
// Direction enumaration to easily control motors
//
enum DIRECTION : int {
   BACKWARD = LOW
  ,FORWARD  = HIGH
};
/*********************************************************************
* 
*  Functions
*
*********************************************************************/
/************************************************************
*
* ? CoRoSoN_Init()
*   
* * Description:
*     Initializes all I2C, pins, serial, etc.
*
************************************************************/
ERRORS CoRoSoN_Init(void);

/************************************************************
*
* ? CoRoSoN_SetLEDColor()
*   
* * Description:
*     Sets the color of the LED
*
************************************************************/
ERRORS CoRoSoN_SetLEDColor(I2C_ADD_BTNLED_MODULE AddModule, SIDE Side, COLOR Color);

/************************************************************
*
* ? CoRoSoN_ButtonIsPressed()
*   
* * Description:
*     Returns if the button is pressed
*
* ! Attention:
*     Returns false if the connect failed
*
************************************************************/
bool CoRoSoN_ButtonIsPressed(I2C_ADD_BTNLED_MODULE AddModule, SIDE Side);

/************************************************************
*
* ? CoRoSoN_SetMotor()
*   
* * Description:
*     Sets the motor to the passed direction 
*     with the passed percentage of max speed
*
* ! Attention:
*     SpeedPercentage > 100 will be changed to 
*     SpeedPercentage = 100
*
************************************************************/
ERRORS CoRoSoN_SetMotor(MOTOR Motor, DIRECTION Direction, unsigned int SpeedPercentage);
#endif // COROSON_H