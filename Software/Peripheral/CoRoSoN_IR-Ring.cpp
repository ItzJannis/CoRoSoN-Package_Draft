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
  ZEROMEM(this->mPriv);
  this->mPriv.Address       = Address;
  this->mPriv.BallDirection = -32;
  this->mPriv.BallDistance  = 0;
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
  this->mPriv.BallSeen      = false;
  this->mPriv.BallDirection = -32;
  this->mPriv.BallDistance  = aAnswer[1];
  if(this->mPriv.BallDistance > 0) {
    this->mPriv.BallSeen      = true;
    this->mPriv.BallDirection = aAnswer[0] - 31;
  }
  return r;
}

bool IRRing::BallSeen() {
  return this->mPriv.BallSeen;
}

int IRRing::BallDirection() {
  return this->mPriv.BallDirection;
}

int IRRing::BallDistance() {
  return this->mPriv.BallDistance;
}