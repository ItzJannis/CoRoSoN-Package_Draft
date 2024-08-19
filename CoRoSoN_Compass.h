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
// File:    CoRoSoN_Compass.h
// Purpose: Providing a simpler, more beginner friendly interface
//          to work with a compass sensor
// -------------------------------------------------------------------
#ifndef COROSON_COMPASS_H
#define COROSON_COMPASS_H
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include "CoRoSoN_Util.h"
#include "CoRoSoN_I2C.h"
/*********************************************************************
* 
*  Types
*
*********************************************************************/
//
// Compass class
//
class Compass {
public:
  /************************************************************
  *
  * ? Compass()
  *   
  * * Description:
  *     Constructor
  *
  ************************************************************/
  Compass(unsigned short Address);

  /************************************************************
  *
  * ? SetHeading()
  *   
  * * Description:
  *     Sets the heading of the compass to the current state
  *
  ************************************************************/
  ERRORS SetHeading(void);

  /************************************************************
  *
  * ? HeadingDirection()
  *   
  * * Description:
  *     Returns the direction of the heading related 
  *     to the current state
  *     
  *     < 0 => heading left  / turned to the right
  *     > 0 => heading right / turned to the left 
  *
  ************************************************************/
  int HeadingAngle(void);
  
private:
  struct {
    unsigned short Address;
             int   Heading;
  } mPriv;
};
#endif // COROSON_COMPASS_H