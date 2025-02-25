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
// File:    CoRoSoN_Pixy.h
// Purpose: Providing a simpler, more beginner friendly interface
//          to work with a Pixy cam
// -------------------------------------------------------------------
#ifndef COROSON_PIXY_H
#define COROSON_PIXY_H
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include <Arduino.h>
#include <Pixy2I2C.h>
#include "../Shared/CoRoSoN_Util.h"
/*********************************************************************
* 
*  Types
*
*********************************************************************/
//
// Pixy class
//
class Pixy {
public:
  /************************************************************
  *
  * ? Pixy()
  *   
  * * Description:
  *     Constructor
  *
  ************************************************************/
  Pixy(unsigned short Address, int SignatureGoal, int SignatureOwnGoal);

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
  * ? SeesGoal()
  *   
  * * Description:
  *     Returns if the goal is seen
  *
  ************************************************************/
  bool SeesGoal(void);

  /************************************************************
  *
  * ? SeesOwnGoal()
  *   
  * * Description:
  *     Returns if the own goal is seen
  *
  ************************************************************/
  bool SeesOwnGoal(void);
  
  /************************************************************
  *
  * ? DirectionGoal()
  *   
  * * Description:
  *     Returns the goals directions in terms of pixels
  * 
  *     < 0 => goal to the left
  *     > 0 => goal to the right
  *
  ************************************************************/
  int DirectionGoal(void);
  
  /************************************************************
  *
  * ? DirectionOwnGoal()
  *   
  * * Description:
  *     Returns the own goals directions in terms of pixels
  * 
  *     < 0 => own goal to the left
  *     > 0 => own goal to the right
  *
  ************************************************************/
  int DirectionOwnGoal(void);
  
private:
  struct {
    Pixy2I2C*      pPixy;
    unsigned short Address;
             int   SignatureGoal;
             int   SignatureOwnGoal;
             bool  SeesGoal;
             bool  SeesOwnGoal;
             int   DirectionGoal;
             int   DirectionOwnGoal;
  } mPriv;
};
#endif // COROSON_PIXY_H