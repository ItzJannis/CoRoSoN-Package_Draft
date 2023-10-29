// -------------------------------------------------------------------
/*********************************************************************
* 
*      CoRoSoN
*         Co|mponents for 
*         Ro|bocupJunior
*         So|ccer
*          N|ewcomers
*
*      Project:     https://github.com/CoRoSoN-Kit/
*      Competition: https://www.robocup.org/
*
*      Created by BohleBots
*         https://www.bohlebots.de
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
  this->mPriv.pPixy = new Pixy2I2C();
  if(this->mPriv.pPixy->init(this->mPriv.Address) < 0) {
    DEBUG_ERRORS(CONNECT_FAILED);
    DEBUG_PRINT(Address);
    DEBUG_BLOCK("Pixy init failed", 1000);
  }
}

ERRORS Pixy::Update(void) {
  ERRORS r;
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
  //
  // Establish connection
  //
  this->mPriv.pPixy->ccc.getBlocks();
  if(this->mPriv.pPixy->ccc.numBlocks <= 0) {
    r |= CONNECT_FAILED | INVALID_ANSWER | ERROR_BREAK_OUT;
    DEBUG_ERRORS(r);
    return r;
  }
  //
  // Read data
  //
  for(int i = 0; i < this->mPriv.pPixy->ccc.numBlocks; i++) {
    Signature = this->mPriv.pPixy->ccc.blocks[i].m_signature;
    X         = this->mPriv.pPixy->ccc.blocks[i].m_x;
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