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
static int _Angle8 = 1;

Compass::Compass(unsigned short Address) {
  ZEROFILL(this->mPriv);
  this->mPriv.Address = Address;

  Wire.beginTransmission(this->mPriv.Address);
  if(Wire.endTransmission()) {
    DEBUG_ERRORS(CONNECT_FAILED);
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
  
  r = OKAY;
  Wire.beginTransmission(this->mPriv.Address);
  Wire.write(_Angle8);
  //
  // Check if connect failed
  //
  if(Wire.endTransmission()) {
    r = (CONNECT_FAILED | ERROR_BREAK_OUT);
    DEBUG_ERRORS(r);
    DEBUG_PRINT(this->mPriv.Address);
    return r;
  }
  //
  // Read answer
  //
  Wire.requestFrom(this->mPriv.Address, 3);
  while (Wire.available() < 3) {
    ; // wait for 3 byte
  }
  Angle8Bit = Wire.read(); (void)Angle8Bit; // ignore first byte
  HighByte  = Wire.read();
  LowByte   = Wire.read();
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
  unsigned char HighByte;
  unsigned char LowByte;
  unsigned char Angle8Bit;
  unsigned int  Angle16Bit;
  int           HeadingAngle;
  
  Wire.beginTransmission(this->mPriv.Address);
  Wire.write(_Angle8);
  //
  // Check if connect failed
  //
  if(Wire.endTransmission()) {
    DEBUG_ERRORS(CONNECT_FAILED | ERROR_BREAK_OUT);
    DEBUG_PRINT(this->mPriv.Address);
  }
  //
  // Read answer
  //
  Wire.requestFrom(this->mPriv.Address, 3);
  while (Wire.available() < 3) {
    ; // wait for 3 byte
  }
  Angle8Bit = Wire.read(); (void)Angle8Bit; // ignore first byte
  HighByte  = Wire.read();
  LowByte   = Wire.read();
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