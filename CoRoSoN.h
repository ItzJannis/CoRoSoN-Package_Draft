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
/*********************************************************************
* 
*  Configuration
*
*********************************************************************/
//
// I2C Bus
//
#define I2C_SDA (21)
#define I2C_SCL (22)
//
// I2C Devices
//
#define I2C_ADD_PIXY        (0x54)
#define I2C_ADD_IR          (0x55)
#define I2C_ADD_CMPS        (0x60)
#define I2C_BASEADD_BTNLEDS (0x20)
#define I2C_BTNLED_CODE_1 (1) // change to match binary number that is encoded by soldering on button and led module
#define I2C_BTNLED_CODE_2 (2) // change to match binary number that is encoded by soldering on button and led module
//
//  Motors
//
#define MOTOR_ENA   ( 2)
#define MOTOR_1_DIR (13)
#define MOTOR_2_DIR (14)
#define MOTOR_3_DIR (25)
#define MOTOR_4_DIR (18)
#define MOTOR_1_PWM (12)
#define MOTOR_2_PWM (27)
#define MOTOR_3_PWM (26)
#define MOTOR_4_PWM (19)
//
// Analog Inputs
//
#define ANALOG_IN_1 (35)
#define ANALOG_IN_2 (34)
#define ANALOG_IN_3 (39)
#define ANALOG_IN_4 (36)
//
// Digital Outputs
//
#define DIGITAL_OUT_1 (15)
#define DIGITAL_OUT_2 (23)
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