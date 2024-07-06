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
// File:    Firmware_IR-Ring.h
// Purpose: Main header for the firmware to load onto ir ring
// -------------------------------------------------------------------
#ifndef FIRMWARE_IR_RING_H
#define FIRMWARE_IR_RING_H
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include <Arduino.h>
#include "CoRoSoN.h"
/*********************************************************************
* 
*  Config
*
*********************************************************************/
#define NUM_SENSORS (16)
#define BLUR_ORIGINAL_VALUE_WEIGHT (0.6) // ! keep inside ]0;1[ 
#define EXPAND_FACTOR_PER_SENSOR (4) // ! keep inside [1;16] so that NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR is inside [NUM_SENSORS; 256]
#define MIN_VALUE_TO_DETECT 50 //(TODO SET MIN_VALUE_TO_DETECT AFTER TESTING)
// "Direction":                                  -7  -6  -5  -4  -3  -2  -1   0   1   2   3   4   5   6   7   8
const unsigned short SENSOR_PINS[NUM_SENSORS] = {13, 15, 02, 04, 17, 05, 18, 19, 34, 35, 32, 33, 25, 26, 27, 14};
/*********************************************************************
* 
*  Functions
*
*********************************************************************/
/************************************************************
*
* ? Setup()
*   
* * Description:
*     Call this function in arduino's setup on ir ring to
*     setup everything correctly 
*
************************************************************/
ERRORS Setup(void);
/************************************************************
*
* ? Loop()
*   
* * Description:
*     Call this function in arduino's loop to read the 
*     current sensor situation
*
************************************************************/
void Loop(void);
#endif // FIRMWARE_IR_RING_H