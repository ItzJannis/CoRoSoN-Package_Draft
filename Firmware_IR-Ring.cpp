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
  int    aRawValues        [NUM_SENSORS];
  double aBlurredValues    [NUM_SENSORS];
  double aExpandedValues   [NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR];
  double TotalSum;
  int    iDir;
  int    Width50Percent;
  double Sum;

  _CheckInit();
  ZEROMEM(aRawValues);
  ZEROMEM(aBlurredValues);
  ZEROMEM(aExpandedValues);
  //
  // Read sensors
  //
  t = 0;
  while (t < 5) {// 5ms equal 6 runs of 833us each
    for (int i = 0; i < ARRAY_LENGTH(aRawValues); i++) {
      aRawValues[i] += 1 - digitalRead(SENSOR_PINS[i]);                         
    }
  }
  //
  // Blur values
  //
  for (int i = 0; i < ARRAY_LENGTH(aRawValues) ; i++) {
    int iLeft  = (i - 1 + ARRAY_LENGTH(aRawValues)) % ARRAY_LENGTH(aRawValues);
    int iRight = (i + 1) % ARRAY_LENGTH(aRawValues);
    aBlurredValues[i]  = aRawValues[i]      * BLUR_ORIGINAL_VALUE_WEIGHT;
    aBlurredValues[i] += aRawValues[iLeft ] * ((1.0 - BLUR_ORIGINAL_VALUE_WEIGHT) / 2.0);
    aBlurredValues[i] += aRawValues[iRight] * ((1.0 - BLUR_ORIGINAL_VALUE_WEIGHT) / 2.0);
  }
  //
  // Expand values / increase resolution
  //
  for(int i = 0; i < ARRAY_LENGTH(aBlurredValues); i++) {
    int iNext = (i + 1) % ARRAY_LENGTH(aBlurredValues);
    double Delta = aBlurredValues[iNext] - aBlurredValues[i];
    for(int j = 0; j < EXPAND_FACTOR_PER_SENSOR; j++) { // linear interpolation
      int iCurrent = ((i * EXPAND_FACTOR_PER_SENSOR) + j);
      iCurrent += (EXPAND_FACTOR_PER_SENSOR / 2); // make sure middle is taken instead of most left
      iCurrent  = iCurrent % ARRAY_LENGTH(aExpandedValues); // index wrap around
      double Weight = (double)j / (double)EXPAND_FACTOR_PER_SENSOR;
      aExpandedValues[iCurrent] = aBlurredValues[i] + (Weight * Delta);
    }
  }
  //
  // Get direction by highest value
  //
  iDir = 0;
  for (int i = 1; i < ARRAY_LENGTH(aExpandedValues); i++) {
    if (aExpandedValues[i] > aExpandedValues[iDir]) {
        iDir = i;
    }
  }
  _LastBallDir = iDir;
  //
  // Calculate distribution by how wide 50% of the integral are spread around direction
  //
  Sum = aExpandedValues[iDir];
  if(Sum < MIN_VALUE_TO_DETECT) {
    _LastBallDist = 0;
    return;
  }
  Width50Percent = 0;
  if(Sum < (0.5 * TotalSum)) {
    for (int i = 1; i < ARRAY_LENGTH(aExpandedValues); i++) {
      if(Sum > (0.5 * TotalSum)) {
          Width50Percent = 2 * i;
          break;
      }
      int iLeft  = (iDir - i + ARRAY_LENGTH(aExpandedValues)) % ARRAY_LENGTH(aExpandedValues);
      int iRight = (iDir + i) % ARRAY_LENGTH(aExpandedValues);
      Sum += aExpandedValues[iLeft] + aExpandedValues[iRight];
    }
  }
  _LastBallDist = ARRAY_LENGTH(aExpandedValues) - Width50Percent;
}