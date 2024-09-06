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
#include <Math.h>
/*********************************************************************
* 
*  Config Check
*
*********************************************************************/
//
// BLUR_ORIGINAL_VALUE_WEIGHT
#if (BLUR_ORIGINAL_VALUE_WEIGHT <= 0.0)
  #error "Config-Error: BLUR_ORIGINAL_VALUE_WEIGHT must be greater than 0.0"
#elif (BLUR_ORIGINAL_VALUE_WEIGHT >= 1.0)
  #error "Config-Error: BLUR_ORIGINAL_VALUE_WEIGHT must be less than 1.0"
#endif
//
// EXPAND_FACTOR_PER_SENSOR
#if (EXPAND_FACTOR_PER_SENSOR <= 0)
  #error "Config-Error: EXPAND_FACTOR_PER_SENSOR must be greater than 0"
#elif (EXPAND_FACTOR_PER_SENSOR > (NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR))
  #error "Config-Error: BLUR_ORIGINAL_VALUE_WEIGHT must not be greater than (NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR)"
#endif
//
// VECTOR_ADDITION_SENSOR_COUNT
#if (VECTOR_ADDITION_SENSOR_COUNT <= 0)
  #error "Config-Error: VECTOR_ADDITION_SENSOR_COUNT must be greater than 0"
#elif (VECTOR_ADDITION_SENSOR_COUNT >= 17)
  #error "Config-Error: BLUR_ORIGINAL_VALUE_WEIGHT must be less than 17"
#endif
//
// MIN_VALUE_TO_DETECT
#if (MIN_VALUE_TO_DETECT <= 0)
  #error "Config-Error: MIN_VALUE_TO_DETECT must be greater than 0"
#endif
/*********************************************************************
* 
*  Implementations
*
*********************************************************************/
static byte _LastBallDir = 0;
static byte _LastBallDist = 0;
static bool _SetupSuccessfull = false;
static long double _aFactorX[NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR]; // X: left to right in robot view
static long double _aFactorY[NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR]; // Y: behind to front in robot view
static long double _AngleStep_rad = 2.0 * PI / (long double)(NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR);

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

static void _CheckSetup(void) {
  if(!_SetupSuccessfull) {
    DEBUG_BLOCK("Setup() was not called to set data and clock pin before using I2C", 1000);
  }
}

ERRORS Setup() {
  ERRORS r;

  r = OKAY;
  Serial.begin(115200);
  for(int i = 0; i < ARRAY_LENGTH(SENSOR_PINS); i++) {
    pinMode(SENSOR_PINS[i], INPUT);
  }
  Wire.onRequest(_OnRequest);
  Wire.onReceive(_OnReceive);
  if(!Wire.begin(I2C_ADD_IR)) {
    r |= CONNECT_FAILED | ERROR_BREAK_OUT;
    DEBUG_ERRORS(r);
    DEBUG_BLOCK("I2C-Bus konnte nicht gestartet werden", 1000);
  } else {
    _SetupSuccessfull = true;
  }
  for(int i = 0; i < ARRAY_LENGTH(_aFactorX); i++) {
    _aFactorX[i] = sin(_AngleStep_rad * (i - (ARRAY_LENGTH(_aFactorX) / 2) + 1)); // robot's x is math's y
  }
  for(int i = 0; i < ARRAY_LENGTH(_aFactorY); i++) {
    _aFactorY[i] = cos(_AngleStep_rad * (i - (ARRAY_LENGTH(_aFactorY) / 2) + 1)); // robot's y is math's x
  }
  return r;
}

void Loop() {
  elapsedMillis t;
  int    aRawValues     [NUM_SENSORS];
  double aBlurredValues [NUM_SENSORS];
  double aExpandedValues[NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR];
  double TotalSum;
  int    iMaximum;
  double xDir;
  double yDir;
  double angleDir_rad;
  int    iDir;
  int    Width50Percent;
  double Sum;

  _CheckSetup();
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
    aBlurredValues[i]  = (double)aRawValues[i]      * BLUR_ORIGINAL_VALUE_WEIGHT;
    aBlurredValues[i] += (double)aRawValues[iLeft ] * ((1.0 - BLUR_ORIGINAL_VALUE_WEIGHT) / 2.0);
    aBlurredValues[i] += (double)aRawValues[iRight] * ((1.0 - BLUR_ORIGINAL_VALUE_WEIGHT) / 2.0);
  }
  //
  // Expand values by cubic interpolation:
  //    Given: 
  //      y0 := f(0), y1 := f(1), d0 := f'(0), d1 := f'(1)
  //    Task: 
  //      Find coefficients a, b, c, d of 3rd degree polynomial f
  //    Solution:
  //      a =  2y0 - 2y1 +  d0 + d1
  //      b = -3y0 + 3y1 - 2d0 - d1
  //      c = d0
  //      d = y0
  //
  TotalSum = 0.0;
  for(int i = 0; i < ARRAY_LENGTH(aBlurredValues); i++) {
    int iNext     = (i + 1) % ARRAY_LENGTH(aBlurredValues);
    int iNextNext = (i + 2) % ARRAY_LENGTH(aBlurredValues);
    double y0 = aBlurredValues[i];
    double y1 = aBlurredValues[iNext];
    double d0 = aBlurredValues[iNext] - aBlurredValues[i];
    double d1 = aBlurredValues[iNextNext] - aBlurredValues[iNext];
    double a =   (2 * y0) - (2 * y1) +      d0  + d1;
    double b = - (3 * y0) + (3 * y1) - (2 * d0) - d1;
    double c = d0;
    double d = y0;
    for(int j = 0; j < EXPAND_FACTOR_PER_SENSOR; j++) { // linear interpolation
      int iCurrent = ((i * EXPAND_FACTOR_PER_SENSOR) + j) % ARRAY_LENGTH(aExpandedValues);
      double Percentage = (double)j / (double)EXPAND_FACTOR_PER_SENSOR;
      aExpandedValues[iCurrent]  = a * Percentage * Percentage * Percentage;
      aExpandedValues[iCurrent] += b * Percentage * Percentage;
      aExpandedValues[iCurrent] += c * Percentage;
      aExpandedValues[iCurrent] += d;
      TotalSum += aExpandedValues[iCurrent];
    }
  }
  //
  // Get direction by vector addition in the quarter around highest value
  //
  iMaximum = 0;
  for (int i = 1; i < ARRAY_LENGTH(aExpandedValues); i++) {
    if (aExpandedValues[i] > aExpandedValues[iMaximum]) {
        iMaximum = i;
    }
  }
  xDir = aExpandedValues[iMaximum] * _aFactorX[iMaximum];
  yDir = aExpandedValues[iMaximum] * _aFactorY[iMaximum];
  for (int i = 1; i < (VECTOR_ADDITION_SENSOR_COUNT - 1) / 2; i++) {
    int iLeft = (iMaximum - i + ARRAY_LENGTH(aExpandedValues)) % ARRAY_LENGTH(aExpandedValues);
    int iRight = (iMaximum + i) % ARRAY_LENGTH(aExpandedValues);
    xDir += (aExpandedValues[iLeft] * _aFactorX[iLeft]) + (aExpandedValues[iRight] * _aFactorX[iRight]);
    yDir += (aExpandedValues[iLeft] * _aFactorY[iLeft]) + (aExpandedValues[iRight] * _aFactorY[iRight]);
  }
  if(VECTOR_ADDITION_SENSOR_COUNT % 2 == 0 ) { // ensure added vectors are centered around maximum
    int iLeft  = (iMaximum - (VECTOR_ADDITION_SENSOR_COUNT / 2) + ARRAY_LENGTH(aExpandedValues)) % ARRAY_LENGTH(aExpandedValues);
    int iRight = (iMaximum + (VECTOR_ADDITION_SENSOR_COUNT / 2)) % ARRAY_LENGTH(aExpandedValues);
    xDir += (0.5 * aExpandedValues[iLeft] * _aFactorX[iLeft]) + (0.5 * aExpandedValues[iRight] * _aFactorX[iRight]);
    yDir += (0.5 * aExpandedValues[iLeft] * _aFactorY[iLeft]) + (0.5 * aExpandedValues[iRight] * _aFactorY[iRight]);
  }
  angleDir_rad = atan2(xDir, yDir); // robot's x and y is swapped compared to math's
  iDir = (int)((angleDir_rad / _AngleStep_rad) + 0.5); // + 0.5 for rounded division
  iDir += (ARRAY_LENGTH(aExpandedValues) / 2) - 1;
  _LastBallDir = iDir;
  //
  // Calculate distribution by how wide 50% of the integral area spread around direction
  //
  Sum = aExpandedValues[iDir];
  if(Sum < MIN_VALUE_TO_DETECT) {
    _LastBallDist = 0;
    return;
  }
  Width50Percent = 1;
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
