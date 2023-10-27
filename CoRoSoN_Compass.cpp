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
// File:    CoRoSoN_Compass.cpp
// Purpose: Implementations for CoRoSoN_Compass.h
// -------------------------------------------------------------------
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include "CoRoSoN_Compass.h"
/*********************************************************************
* 
*  Implementations
*
*********************************************************************/
static byte _aRequestMessage[1] = { 0b00000001 };

Compass::Compass(unsigned short Address) {
  ERRORS r;

  ZEROMEM(this->mPriv);
  this->mPriv.Address = Address;
  r = I2C_TestConnection(Address);
  if(r) {
    DEBUG_ERRORS(r);
    DEBUG_PRINT(Address);
    DEBUG_BLOCK("Compass init failed", 1000);
  }
}

ERRORS Compass::SetHeading() {
  ERRORS        r;
  unsigned char HighByte;
  unsigned char LowByte;
  unsigned char Angle8Bit;
  unsigned int  Angle16Bit;
  byte          aAnswer[3];
  
  r = OKAY;
  ZEROMEM(aAnswer);
  r = I2C_Write(this->mPriv.Address, _aRequestMessage, ARRAY_LENGTH(_aRequestMessage));
  if(r) {
    r |= CONNECT_FAILED | ERROR_BREAK_OUT;
    DEBUG_ERRORS(r);
    return r;
  }
  //
  // Read answer
  //
  r = I2C_ReadBlocking(this->mPriv.Address, aAnswer, ARRAY_LENGTH(aAnswer));
  if(r) {
    r |= INVALID_ANSWER | ERROR_BREAK_OUT;
    DEBUG_ERRORS(r);
    return r;
  }
  Angle8Bit = aAnswer[0]; (void)Angle8Bit;
  HighByte  = aAnswer[1];
  LowByte   = aAnswer[2];
  //
  // Encode answer
  //
  Angle16Bit = HighByte;
  Angle16Bit <<= 8;
  Angle16Bit += LowByte;
  //
  // Update Heading
  //
  this->mPriv.Heading = Angle16Bit / 10;
  return r;
}

int Compass::HeadingAngle() {
  ERRORS        r;
  unsigned char HighByte;
  unsigned char LowByte;
  unsigned char Angle8Bit;
  unsigned int  Angle16Bit;
  byte          aAnswer[3];
  int           HeadingAngle;
  
  ZEROMEM(aAnswer);
  r = I2C_Write(this->mPriv.Address, _aRequestMessage, ARRAY_LENGTH(_aRequestMessage));
  if(r) {
    r |= CONNECT_FAILED | ERROR_BREAK_OUT;
    DEBUG_ERRORS(r);
  }
  //
  // Read answer
  //
  r = I2C_ReadBlocking(this->mPriv.Address, aAnswer, ARRAY_LENGTH(aAnswer));
  if(r) {
    r |= INVALID_ANSWER | ERROR_BREAK_OUT;
    DEBUG_ERRORS(r);
  }
  Angle8Bit = aAnswer[0]; (void)Angle8Bit;
  HighByte  = aAnswer[1];
  LowByte   = aAnswer[2];
  //
  // Encode answer
  //
  Angle16Bit = HighByte;
  Angle16Bit <<= 8;
  Angle16Bit += LowByte;
  //
  // Calculate angle of heading
  //
  HeadingAngle  = Angle16Bit / 10;
  HeadingAngle -= this->mPriv.Heading; // get offset to heading
  HeadingAngle += 180; // make values positive
  HeadingAngle += 360; // taken from BohleBots, i don"t know why so far
  HeadingAngle %= 360; // clip to [0; 359]
  HeadingAngle -= 180; // get back to [-180; 179]
  HeadingAngle *=  -1; // instead of 45° rotated right return -45 for heading is 45° to the left
  return HeadingAngle;
}