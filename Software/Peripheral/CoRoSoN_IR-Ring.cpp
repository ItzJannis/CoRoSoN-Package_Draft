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
// File:    CoRoSoN_IR-Ring.cpp
// Purpose: Implementations for CoRoSoN_IR-Ring.h
// -------------------------------------------------------------------
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include "CoRoSoN_IR-Ring.h"
/*********************************************************************
* 
*  Implementations
*
*********************************************************************/

IRRing::IRRing(unsigned short Address) {
  ERRORS r;

  ZEROMEM(this->mPriv);
  this->mPriv.Address       = Address;
  this->mPriv.BallDirection = -64;
  this->mPriv.BallDistance  = 0;
  r = I2C_TestConnection(Address);
  if(r) {
    DEBUG_ERRORS(r);
    DEBUG_PRINT(Address);
    DEBUG_BLOCK("IR ring init failed", 1000);
  }
}

ERRORS IRRing::Update() {
  ERRORS r;
  byte   aAnswer[2];

  r = OKAY;
  ZEROMEM(aAnswer);
  r = I2C_ReadBlocking(this->mPriv.Address, aAnswer, ARRAY_LENGTH(aAnswer));
  if(r) {
    r |= INVALID_ANSWER | ERROR_BREAK_OUT;
    DEBUG_ERRORS(r);
    return r;
  }
  // Default: not seen
  this->mPriv.SeesBall      = false;
  this->mPriv.BallDirection = -32;
  this->mPriv.BallDistance  = aAnswer[1];
  if(this->mPriv.BallDistance != 0) {
    this->mPriv.SeesBall      = true;
    this->mPriv.BallDirection = aAnswer[0] - 32;
  }
  return r;
}

bool IRRing::SeesBall() {
  return this->mPriv.SeesBall;
}

int IRRing::BallDirection() {
  return this->mPriv.BallDirection;
}

int IRRing::BallDistance() {
  return this->mPriv.BallDistance;
}