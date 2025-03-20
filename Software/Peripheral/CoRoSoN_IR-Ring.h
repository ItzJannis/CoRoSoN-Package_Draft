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
* *    Project:     https://github.com/CoRoSoN-Kit/CoRoSoN-Kit
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
#include "../Shared/CoRoSoN_Util.h"
#include "../Shared/CoRoSoN_I2C.h"
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
  * ? BallSeen()
  *   
  * * Description:
  *     Returns if the ball is seen
  *
  ************************************************************/
  bool BallSeen(void);

  /************************************************************
  *
  * ? BallDirection()
  *   
  * * Description:
  *     Returns the direction of the ball as follows:
  *       == 0: in front of robot
  *        > 0: to the right
  *        < 0: to the left
  * 
  * ! Additional information:
  *     Returns -32 if ball is not seen, 
  *     therefore 180° or right behind robot is always +32
  *
  ************************************************************/
  int BallDirection(void);

  /************************************************************
  *
  * ? BallDistance()
  *   
  * * Description:
  *     Returns the distance of the ball to the robot
  *     1 = closest,..., 64 = furthest
  * 
  * ! Additional information:
  *     Returns 0 if ball is not seen
  *
  ************************************************************/
  int BallDistance(void);

private:
  struct {
    unsigned short Address;
             bool  BallSeen;
             int   BallDirection;
             int   BallDistance;
  } mPriv;
};
#endif // COROSON_IR_RING_H