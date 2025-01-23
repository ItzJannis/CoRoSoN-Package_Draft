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
// File:    Firmware_IR-Ring.h
// Purpose: Providing an all-in-one solution for the IR ring 
//          to convert the TSSP sensor values to usefull 
//          direction and distance of the ball available via I2C
// -------------------------------------------------------------------
#ifndef FIRMWARE_IR_RING_H
#define FIRMWARE_IR_RING_H
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include <Arduino.h>
#include "../Shared/CoRoSoN_Util.h"
#include "../Shared/CoRoSoN_Config.h"
/*********************************************************************
* 
*  Config
*
*********************************************************************/
//
// Software Config
//
#define NUM_SENSORS (16)
#define BLUR_ORIGINAL_VALUE_WEIGHT_PERCENTAGE (10) // ! keep inside ]0;100[ 
#define EXPAND_FACTOR_PER_SENSOR (4) // ! keep inside [1;16] so that NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR is inside [NUM_SENSORS; 256]
#define VECTOR_ADDITION_SENSOR_COUNT (NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR / 4) // ! keep inside [1;NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR]
#define MIN_VALUE_TO_DETECT (50)
#define EMA_ALPHA_DIRECTION_PERCENTAGE (20) // ! keep inside ]0;100], essentially this is proportion a new value gets in the median
#define EMA_ALPHA_DISTANCE_PERCENTAGE  (20) // ! keep inside ]0;100], essentially this is proportion a new value gets in the median
//
// Hardware Config
//
// "Direction":                                  -7  -6  -5  -4  -3  -2  -1   0   1   2   3   4   5   6   7   8
const unsigned short SENSOR_PINS[NUM_SENSORS] = {27, 26, 25, 33, 32, 35, 34, 19, 18, 05, 17, 04, 02, 15, 13, 14};
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
