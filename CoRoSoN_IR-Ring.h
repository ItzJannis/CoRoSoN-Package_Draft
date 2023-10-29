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
// File:    CoRoSoN_IR-Ring.h
// Purpose: Main header for the ir ring provided by CoRoSoN kit board
// -------------------------------------------------------------------
#ifndef COROSON_IR_RING_H
#define COROSON_IR_RING_H
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include <Arduino.h>
#include "CoRoSoN_Util.h"
#include "CoRoSoN_I2C.h"
#include "CoRoSoN_IR-Ring.h"
/*********************************************************************
* 
*  Types
*
*********************************************************************/
class IRRing {
public:
  /************************************************************
  *
  * ? IRRing()
  *   
  * * Description:
  *     Constructor
  *
  ************************************************************/
  IRRing(unsigned short Address);
  /************************************************************
  *
  * ? Update()
  *   
  * * Description:
  *     Reads the new data
  *
  ************************************************************/
  ERRORS Update(void);
  /************************************************************
  *
  * ? SeesBall()
  *   
  * * Description:
  *     Returns if the ball is seen
  *
  ************************************************************/
  bool SeesBall(void);
  /************************************************************
  *
  * ? BallDirection()
  *   
  * * Description:
  *     Returns the direction of the ball as follows:
  *       == 0: in front of robot
  *        > 0: to the right of robot in 22.5° steps
  *        < 0: to the left  of robot in 22.5° steps
  * 
  * ! Additional information:
  *     Returns -8 if ball is not seen, 
  *     therefore 180° or right behind robot is always +8
  * 
  * TODO: maybe change later with better code running on IR hardware
  *
  ************************************************************/
  int  BallDirection(void);
  /************************************************************
  *
  * ? BallDistance()
  *   
  * * Description:
  *     Returns the distance of the ball to the robot
  *     1 = closest, 15 = furthest
  * 
  * ! Additional information:
  *     Returns 0 if ball is not seen
  * 
  * TODO: maybe change later with better code running on IR hardware
  *
  ************************************************************/
  int  BallDistance(void);
private:
  struct {
    unsigned short Address;
             bool  SeesBall;
             int   BallDirection;
             int   BallDistance;
  } mPriv;
};
#endif // COROSON_IR_RING_H