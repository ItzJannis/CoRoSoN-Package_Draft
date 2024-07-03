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
// File:    Firmware_IR-Ring.cpp
// Purpose: Implementations for Firmware_IR-Ring.h
// -------------------------------------------------------------------
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include "Firmware_IR-Ring.h"
#include <elapsedMillis.h>
/*********************************************************************
* 
*  Implementations
*
*********************************************************************/
static byte _LastBallDir = 0;
static byte _LastBallDist = 0;
static bool _InitSuccessfull = false;

static void _OnReceive(int n) {
  (void)n; // not needed
  while(Wire.available()) {
    Wire.read(); // read and forget
  }
}

static void _OnRequest(void) {
  Wire.write(_LastBallDir);
  Wire.write(_LastBallDist);
}

static void _CheckInit(void) {
  if(!_InitSuccessfull) {
    DEBUG_BLOCK("Setup() was not called to set data and clock pin before using I2C", 1000);
  }
}

ERRORS Setup() {
  ERRORS r = OKAY;

  Serial.begin(115200);
  for(int i = 0; i < NUM_SENSORS; i++) {
    pinMode(SENSOR_PINS[i], INPUT);
  }
  Wire.onRequest(_OnRequest);
  Wire.onReceive(_OnReceive);
  if(!Wire.begin(I2C_ADD_IR)) {
    r |= CONNECT_FAILED | ERROR_BREAK_OUT;
    DEBUG_ERRORS(r);
    DEBUG_BLOCK("I2C-Bus konnte nicht gestartet werden", 1000);
  } else {
    _InitSuccessfull = true;
  }
  return r;
}

void Loop() {
  elapsedMillis t;
  int    aRawValues    [NUM_SENSORS];
  double aBlurredValues[NUM_SENSORS];
  double TotalSum;
  int    iDir;
  int    Width50Percent;
  double Sum;

  _CheckInit();
  ZEROMEM(_aRawValues);
  //
  // Read sensors
  //
  t = 0;
  while (t < 5) {// 5ms equal 6 runs of 833us each
    for (int i = 0; i < NUM_SENSORS ; i++) {
      aRawValues[i] += 1 - digitalRead(SENSOR_PINS[i]);                         
    }
  }
  //
  // Blur values
  //
  TotalSum = 0.0;
  for (int i = 0; i < NUM_SENSORS ; i++) {
    int iLeft  = (i - 1 + NUM_SENSORS) % NUM_SENSORS;
    int iRight = (i + 1) % NUM_SENSORS;
    aBlurredValues[i]  = aRawValues[i]      * BLUR_ORIGINAL_VALUE_WEIGHT;
    aBlurredValues[i] += aRawValues[iLeft ] * ((1.0 - BLUR_ORIGINAL_VALUE_WEIGHT) / 2.0);
    aBlurredValues[i] += aRawValues[iRight] * ((1.0 - BLUR_ORIGINAL_VALUE_WEIGHT) / 2.0);
    TotalSum += aBlurredValues[i];
  }
  //
  // Get direction by highest value
  //
  iDir = 0;
  for (int i = 1; i < NUM_SENSORS ; i++) {
    if (aBlurredValues[i] > aBlurredValues[iDir]) {
        iDir = i;
    }
  }
  _LastBallDir = iDir - 7;
  //
  // Calculate distribution by how wide 50% of the integral are spread around direction
  //
  Sum = aBlurredValues[iDir];
  if(Sum < MIN_VALUE_TO_DETECT) {
    _LastBallDist = 0;
    return;
  }
  Width50Percent = 0;
  if(Sum > (0.5 * TotalSum)) {
    for (int i = 1; i < NUM_SENSORS; i++) {
      if(Sum > (0.5 * TotalSum)) {
          Width50Percent = 2 * i;
          break;
      }
      int iLeft  = (iDir - i + NUM_SENSORS) % NUM_SENSORS;
      int iRight = (iDir + i) % NUM_SENSORS;
      Sum += aBlurredValues[iLeft] + aBlurredValues[iRight];
    }
  }
  _LastBallDist = (NUM_SENSORS - Width50Percent) * (NUM_SENSORS - Width50Percent);
}