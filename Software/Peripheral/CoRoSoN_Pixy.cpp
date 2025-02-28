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
// File:    CoRoSoN_Pixy.cpp
// Purpose: Implementations for CoRoSoN_Pixy.h
// -------------------------------------------------------------------
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include "CoRoSoN_Pixy.h"
/*********************************************************************
* 
*  Implementations
*
*********************************************************************/

Pixy::Pixy(unsigned short Address, int SignatureGoal, int SignatureOwnGoal) {
  ZEROMEM(this->mPriv);
  this->mPriv.Address          = Address;
  this->mPriv.SignatureGoal    = SignatureGoal;
  this->mPriv.SignatureOwnGoal = SignatureOwnGoal;
#if (PIXY_VERSION == 1)
  this->mPriv.pPixyV1 = new PixyI2C();
  if(this->mPriv.pPixyV1->init(this->mPriv.Address) < 0) {
    DEBUG_ERRORS(CONNECT_FAILED);
    DEBUG_PRINT(Address);
    DEBUG_BLOCK("Pixy init failed", 1000);
  }
#elif (PIXY_VERSION == 2)
  this->mPriv.pPixyV2 = new Pixy2I2C();
  if(this->mPriv.pPixyV2->init(this->mPriv.Address) < 0) {
    DEBUG_ERRORS(CONNECT_FAILED);
    DEBUG_PRINT(Address);
    DEBUG_BLOCK("Pixy init failed", 1000);
  }
#endif
}

ERRORS Pixy::Update(void) {
  ERRORS r;
  int    NumBlocks;
  int    Signature;
  int    X;
  bool   UpdatedGoal;
  bool   UpdatedOwnGoal;
  //
  // Init
  //
  r = OKAY;
  Signature = 0;
  X         = 0;
  UpdatedGoal    = false;
  UpdatedOwnGoal = false;
  NumBlocks = 0;
  //
  // Establish connection
  //
#if (PIXY_VERSION == 1)
  NumBlocks = this->mPriv.pPixyV1->getBlocks();
#elif (PIXY_VERSION == 2)
  NumBlocks = this->mPriv.pPixyV2->ccc.getBlocks();
#endif
  if(NumBlocks <= 0) {
    r |= CONNECT_FAILED | INVALID_ANSWER | ERROR_BREAK_OUT;
    DEBUG_ERRORS(r);
    return r;
  }
  //
  // Read data
  //
  for(int i = 0; i < NumBlocks; i++) {
#if (PIXY_VERSION == 1)
    Signature = this->mPriv.pPixyV1->blocks[i].signature;
    X         = this->mPriv.pPixyV1->blocks[i].x;
#elif (PIXY_VERSION == 2)
    Signature = this->mPriv.pPixyV2->ccc.blocks[i].m_signature;
    X         = this->mPriv.pPixyV2->ccc.blocks[i].m_x;
#endif
    if(Signature == this->mPriv.SignatureGoal) {
      UpdatedGoal          = true;
      this->mPriv.SeesGoal = true;
      this->mPriv.DirectionGoal = (X - 158) / 2;
    }
    if(Signature == this->mPriv.SignatureOwnGoal) {
      UpdatedOwnGoal          = true;
      this->mPriv.SeesOwnGoal = true;
      this->mPriv.DirectionOwnGoal = (X - 158) / 2;
    }
  }
  if(!UpdatedGoal) {
    this->mPriv.SeesGoal      = false;
    this->mPriv.DirectionGoal = 0;
  }
  if(!UpdatedOwnGoal) {
    this->mPriv.SeesOwnGoal      = false;
    this->mPriv.DirectionOwnGoal = 0;
  }
  return r;
}

bool Pixy::SeesGoal() {
  return this->mPriv.SeesGoal;
}

bool Pixy::SeesOwnGoal() {
  return this->mPriv.SeesOwnGoal;
}

int Pixy::DirectionGoal() {
  return this->mPriv.DirectionGoal;
}

int Pixy::DirectionOwnGoal() {
  return this->mPriv.DirectionOwnGoal;
}