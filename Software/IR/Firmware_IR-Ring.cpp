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
#include <math.h>
#include <Wire.h>
/*********************************************************************
* 
*  Config Check
*
*********************************************************************/
//
// BLUR_ORIGINAL_VALUE_WEIGHT_PERCENTAGE
#if (BLUR_ORIGINAL_VALUE_WEIGHT_PERCENTAGE <= 0)
  #error "Config-Error: BLUR_ORIGINAL_VALUE_WEIGHT_PERCENTAGE must be greater than 0"
#elif (BLUR_ORIGINAL_VALUE_WEIGHT_PERCENTAGE > 100)
  #error "Config-Error: BLUR_ORIGINAL_VALUE_WEIGHT_PERCENTAGE must be less than than or equal to 100"
#endif
//
// EXPAND_FACTOR_PER_SENSOR
#if (EXPAND_FACTOR_PER_SENSOR <= 0)
  #error "Config-Error: EXPAND_FACTOR_PER_SENSOR must be greater than 0"
#elif (EXPAND_FACTOR_PER_SENSOR > 16)
  #error "Config-Error: EXPAND_FACTOR_PER_SENSOR must be less than 16"
#endif
//
// VECTOR_ADDITION_SENSOR_COUNT
#if (VECTOR_ADDITION_SENSOR_COUNT <= 0)
  #error "Config-Error: VECTOR_ADDITION_SENSOR_COUNT must be greater than 0"
#elif (VECTOR_ADDITION_SENSOR_COUNT > (NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR))
  #error "Config-Error: VECTOR_ADDITION_SENSOR_COUNT must be less than or equal to (NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR)"
#endif
//
// EMA_ALPHA_PERCENTAGE
#if (EMA_ALPHA_PERCENTAGE <= 0)
  #error "Config-Error: EMA_ALPHA_PERCENTAGE must be greater than 0"
#elif (EMA_ALPHA_PERCENTAGE > 100)
  #error "Config-Error: EMA_ALPHA_PERCENTAGE must be less than or equal to 100"
#endif
//
// MIN_VALUE_TO_DETECT
#if (MIN_VALUE_TO_DETECT <= 0)
  #error "Config-Error: MIN_VALUE_TO_DETECT must be greater than 0"
#endif
//
// RUNNING_MEDIAN_DIRECTION_LENGTH
#if (RUNNING_MEDIAN_DIRECTION_LENGTH <= 0)
  #error "Config-Error: RUNNING_MEDIAN_DIRECTION_LENGTH must be greater than 0"
#endif
//
// RUNNING_MEDIAN_DISTANCE_LENGTH
#if (RUNNING_MEDIAN_DISTANCE_LENGTH <= 0)
  #error "Config-Error: RUNNING_MEDIAN_DISTANCE_LENGTH must be greater than 0"
#endif
/*********************************************************************
* 
*  Implementations
*
*********************************************************************/
static byte _BallDir = 0;
static byte _BallDist = 0;
static bool _SetupSuccessfull = false;
static long double _aFactorX[NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR]; // X: left to right in robot view
static long double _aFactorY[NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR]; // Y: behind to front in robot view
static long double _AngleStep_rad = 2.0 * PI / (long double)(NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR);
static double _AlphaEMA = (double)EMA_ALPHA_PERCENTAGE / 100.0;

static void _OnReceive(int n) {
  (void)n; // not needed
  while(Wire.available()) {
    Wire.read(); // read and forget
  }
}

static void _OnRequest(void) {
  Wire.write(_BallDir);
  Wire.write(_BallDist);
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

static void _Merge_int(int Arr[], int iLeft, int iMiddle, int iRight) {
  int aLeft [iMiddle - iLeft + 1];
  int aRight[iRight - iMiddle];

  memcpy(aLeft, Arr + iLeft, sizeof(aLeft));
  memcpy(aRight, Arr + iMiddle + 1, sizeof(aRight));
  int i = 0;
  int j = 0;
  int k = iLeft;
  while ((i < ARRAY_LENGTH(aLeft)) && (j < ARRAY_LENGTH(aRight))) {
    if (aLeft[i] <= aRight[j]) {
     Arr[k] = aLeft[i];
     i++;
    } else {
      Arr[k] = aRight[j];
      j++;
    }
    k++;
  }
  while (i < ARRAY_LENGTH(aLeft)) {
    Arr[k] = aLeft[i];
    i++;
    k++;
  }
  while (j < ARRAY_LENGTH(aRight)) {
    Arr[k] = aRight[j];
    j++;
    k++;
  }
}

static void _MergeSort_int(int Arr[], int iLeft, int iRight) {
  if (iLeft < iRight) {
    int iMiddle = iLeft + (iRight - iLeft) / 2;
    _MergeSort_int(Arr, iLeft, iMiddle);
    _MergeSort_int(Arr, iMiddle + 1, iRight);
    _Merge_int(Arr, iLeft, iMiddle, iRight);
  }
}

static void _Merge_double(double Arr[], int iLeft, int iMiddle, int iRight) {
  double aLeft [iMiddle - iLeft + 1];
  double aRight[iRight - iMiddle];

  memcpy(aLeft, Arr + iLeft, sizeof(aLeft));
  memcpy(aRight, Arr + iMiddle + 1, sizeof(aRight));
  int i = 0;
  int j = 0;
  int k = iLeft;
  while ((i < ARRAY_LENGTH(aLeft)) && (j < ARRAY_LENGTH(aRight))) {
    if (aLeft[i] <= aRight[j]) {
     Arr[k] = aLeft[i];
     i++;
    } else {
      Arr[k] = aRight[j];
      j++;
    }
    k++;
  }
  while (i < ARRAY_LENGTH(aLeft)) {
    Arr[k] = aLeft[i];
    i++;
    k++;
  }
  while (j < ARRAY_LENGTH(aRight)) {
    Arr[k] = aRight[j];
    j++;
    k++;
  }
}

static void _MergeSort_double(double Arr[], int iLeft, int iRight) {
  if (iLeft < iRight) {
    int iMiddle = iLeft + (iRight - iLeft) / 2;
    _MergeSort_double(Arr, iLeft, iMiddle);
    _MergeSort_double(Arr, iMiddle + 1, iRight);
    _Merge_double(Arr, iLeft, iMiddle, iRight);
  }
}

void Loop() {
  static int    aRawValues     [NUM_SENSORS];
  static double aEMAValues     [NUM_SENSORS];
  static int    aBlurredValues [NUM_SENSORS];
  static int    aExpandedValues[NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR];
  static int    TotalSum;
  static int    iMaximum;
  static double xDir;
  static double yDir;
  static double signedDir;
  static int    iDir;
  static int    Width50Percent;
  static double Sum;
  static double aDirXHistory[RUNNING_MEDIAN_DIRECTION_LENGTH];
  static double aDirYHistory[RUNNING_MEDIAN_DIRECTION_LENGTH];
  static int    aDistHistory[RUNNING_MEDIAN_DISTANCE_LENGTH];
  elapsedMillis t;

  _CheckSetup();
  ZEROMEM(aRawValues);
  ZEROMEM(aBlurredValues);
  ZEROMEM(aExpandedValues);
  //
  // Read sensors
  //
  t = 0;
  while (t < 5) { // 5ms equal 6 runs of 833us each
    for (int i = 0; i < ARRAY_LENGTH(aRawValues); i++) {
      aRawValues[i] += 1 - digitalRead(SENSOR_PINS[i]);                         
    }
  }
  //
  // Smooth values with Exponential Moving Average (EMA)
  //
  for (int i = 0; i < ARRAY_LENGTH(aEMAValues); i++) {
    aEMAValues[i] = (_AlphaEMA * (double)aRawValues[i]) + ((1.0 - _AlphaEMA) * aEMAValues[i]);                    
  }
  //
  // Blur values (with scaling factor of 100)
  //
  for (int i = 0; i < ARRAY_LENGTH(aEMAValues); i++) {
    int iLeft  = (i - 1 + ARRAY_LENGTH(aEMAValues)) % ARRAY_LENGTH(aEMAValues);
    int iRight = (i + 1) % ARRAY_LENGTH(aEMAValues);
    aBlurredValues[i]  = (double)aEMAValues[i]      * (double)BLUR_ORIGINAL_VALUE_WEIGHT_PERCENTAGE;
    aBlurredValues[i] += (double)aEMAValues[iLeft ] * (double)(100 - BLUR_ORIGINAL_VALUE_WEIGHT_PERCENTAGE) / 2.0;
    aBlurredValues[i] += (double)aEMAValues[iRight] * (double)(100 - BLUR_ORIGINAL_VALUE_WEIGHT_PERCENTAGE) / 2.0;
  }//
  // Expand values by cubic interpolation:
  //    Given: 
  //      y0 := f (0), y1 := f (1),
  //      d0 := f'(0), d1 := f'(1)
  //    Task: 
  //      Find coefficients a, b, c, d of 3rd degree polynomial f
  //    Solution:
  //      a =  2y0 - 2y1 +  d0 + d1
  //      b = -3y0 + 3y1 - 2d0 - d1
  //      c = d0
  //      d = y0
  //
  TotalSum = 0;
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
    for(int j = 0; j < EXPAND_FACTOR_PER_SENSOR; j++) { // expand values
      int iCurrent = (((i+1) * EXPAND_FACTOR_PER_SENSOR) + j - 1) % ARRAY_LENGTH(aExpandedValues);
      double Percentage = (double)j / (double)EXPAND_FACTOR_PER_SENSOR;
      aExpandedValues[iCurrent]  = a * Percentage * Percentage * Percentage;
      aExpandedValues[iCurrent] += b * Percentage * Percentage;
      aExpandedValues[iCurrent] += c * Percentage;
      aExpandedValues[iCurrent] += d;
      if (aExpandedValues[iCurrent] < 0) { // negative values might mess up vector addition
        aExpandedValues[iCurrent] = 0;
      }
      TotalSum += aExpandedValues[iCurrent];
    }
  }
  //
  // Get direction by vector addition in the cone around highest value
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
  if (VECTOR_ADDITION_SENSOR_COUNT % 2 == 0 ) { // ensure added vectors are centered around maximum
    int iLeft  = (iMaximum - (VECTOR_ADDITION_SENSOR_COUNT / 2) + ARRAY_LENGTH(aExpandedValues)) % ARRAY_LENGTH(aExpandedValues);
    int iRight = (iMaximum + (VECTOR_ADDITION_SENSOR_COUNT / 2)) % ARRAY_LENGTH(aExpandedValues);
    xDir += (0.5 * aExpandedValues[iLeft] * _aFactorX[iLeft]) + (0.5 * aExpandedValues[iRight] * _aFactorX[iRight]);
    yDir += (0.5 * aExpandedValues[iLeft] * _aFactorY[iLeft]) + (0.5 * aExpandedValues[iRight] * _aFactorY[iRight]);
  }
  signedDir = atan2(xDir, yDir) / _AngleStep_rad; // robot's x and y is swapped compared to math's
  iDir = (int)(signedDir + ((signedDir < 0) ? -0.5 : 0.5)) + (ARRAY_LENGTH(aExpandedValues) / 2) - 1; // +/- 0.5 to round to next int
  //
  // Calculate distance by how wide 50% of the integral area spread around direction
  //
  Sum = aExpandedValues[iDir];
  if(Sum < MIN_VALUE_TO_DETECT) {
    _BallDir = 0;
    _BallDist = 0;
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
  //
  // Update running medians: Y_i = a * X_i + (1-a) * Y_(i-1) with a in ]0;1]
  //
  //    To avoid messups when ball lies between array ends (behind robot, very slightly left)
  //    the direction is divided into x and y component, averaged and reassembled.
  //
  // Update X history
  for (int i = 0; i < ARRAY_LENGTH(aDirXHistory) - 1; i++) {
    aDirXHistory[i] = aDirXHistory[i+1];
  }
  aDirXHistory[ARRAY_LENGTH(aDirXHistory) - 1] = _aFactorX[iDir];
  // Update Y history
  for (int i = 0; i < ARRAY_LENGTH(aDirYHistory) - 1; i++) {
    aDirYHistory[i] = aDirYHistory[i+1];
  }
  aDirYHistory[ARRAY_LENGTH(aDirYHistory) - 1] = _aFactorY[iDir];
  // Update Dist history
  for (int i = 0; i < ARRAY_LENGTH(aDistHistory) - 1; i++) {
    aDistHistory[i] = aDistHistory[i+1];
  }
  aDistHistory[ARRAY_LENGTH(aDistHistory) - 1] = (ARRAY_LENGTH(aExpandedValues) - Width50Percent);
  //
  // Sort arrays
  //
  double aDirXHistory_sorted[RUNNING_MEDIAN_DIRECTION_LENGTH];
  double aDirYHistory_sorted[RUNNING_MEDIAN_DIRECTION_LENGTH];
  int    aDistHistory_sorted[RUNNING_MEDIAN_DISTANCE_LENGTH];
  memcpy(aDirXHistory_sorted, aDirXHistory, sizeof(aDirXHistory_sorted));
  memcpy(aDirYHistory_sorted, aDirYHistory, sizeof(aDirYHistory_sorted));
  memcpy(aDistHistory_sorted, aDistHistory, sizeof(aDistHistory_sorted));
  _MergeSort_double(aDirXHistory_sorted, 0, ARRAY_LENGTH(aDirXHistory_sorted) - 1);
  _MergeSort_double(aDirYHistory_sorted, 0, ARRAY_LENGTH(aDirYHistory_sorted) - 1);
  _MergeSort_int   (aDistHistory_sorted, 0, ARRAY_LENGTH(aDistHistory_sorted) - 1);
  //
  // Get median values
  //
  double DirXMedian = aDirXHistory_sorted[(ARRAY_LENGTH(aDirXHistory_sorted) - 1) / 2];
  double DirYMedian = aDirYHistory_sorted[(ARRAY_LENGTH(aDirYHistory_sorted) - 1) / 2];
  int    DistMedian = aDistHistory_sorted[(ARRAY_LENGTH(aDistHistory_sorted) - 1) / 2];
  //
  // Calc final direction
  //
  signedDir = atan2(DirXMedian, DirYMedian) / _AngleStep_rad; // robot's x and y is swapped compared to math's
  _BallDir = (int)(signedDir + ((signedDir < 0) ? -0.5 : 0.5)) + (ARRAY_LENGTH(aExpandedValues) / 2) - 1; // +/- 0.5 to round to next int
  _BallDist = DistMedian;
}
