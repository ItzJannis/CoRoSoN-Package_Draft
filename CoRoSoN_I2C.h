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
// File:    CoRoSoN_I2C.h
// Purpose: Providing a simpler, more beginner friendly interface
//          to work with the I2C bus system used in CoRoSoN kit
// -------------------------------------------------------------------
#ifndef COROSON_I2C_H
#define COROSON_I2C_H
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include "CoRoSoN_Util.h"
/*********************************************************************
* 
*  Functions
*
*********************************************************************/
/************************************************************
*
* ? I2C_Init()
*   
* * Description:
*     Initialises the I2C bus on the board with 
*     the given data and clock pin
*
************************************************************/
ERRORS I2C_Init(unsigned int SerialDataPin, unsigned int SerialClockPin);
/************************************************************
*
* ? I2C_TestConnection()
*   
* * Description:
*     Tests if connection to the given address is possible
*
************************************************************/
ERRORS I2C_TestConnection(int Address);
/************************************************************
*
* ? I2C_Write()
*   
* * Description:
*     Writes the passed Data byte per byte to the given address
*
************************************************************/
ERRORS I2C_Write(int Address, byte aMessageBytes[], unsigned int NumBytes);
/************************************************************
*
* ? I2C_ReadBlocking()
*   
* * Description:
*     Reads the passed number of bytes from the passed 
*     address and stores the answer in the passed byte array
* ! Attention:
*     This functions waits for the answers and is therefore 
*     blocking
*     Make sure that NumBytes equals the length of the 
*     passed byte array
*
************************************************************/
ERRORS I2C_ReadBlocking(int Address, byte aAnswerBytes[], unsigned int NumBytes);
#endif // COROSON_I2C_H