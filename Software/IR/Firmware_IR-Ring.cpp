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
static float _aFactorX[NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR]; // X: left to right in robot view
static float _aFactorY[NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR]; // Y: behind to front in robot view
static float _AngleStep_rad = 2.0f * PI / (float)(NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR);
static float _AlphaEMA = (float)EMA_ALPHA_PERCENTAGE / 100.0f;

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
    DEBUG_BLOCK("Failed to initialize I2C", 1000);
  } else {
    _SetupSuccessfull = true;
  }
  for(int i = 0; i < ARRAY_LENGTH(_aFactorX); i++) {
    _aFactorX[i] = sin(_AngleStep_rad * (float)(i - (ARRAY_LENGTH(_aFactorX) / 2) + 1)); // robot's x is math's y
  }
  for(int i = 0; i < ARRAY_LENGTH(_aFactorY); i++) {
    _aFactorY[i] = cos(_AngleStep_rad * (float)(i - (ARRAY_LENGTH(_aFactorY) / 2) + 1)); // robot's y is math's x
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

static void _MergeSort_int(int aNumbers[], int iLeft, int iRight) {
  if (iLeft < iRight) {
    int iMiddle = iLeft + (iRight - iLeft) / 2;
    _MergeSort_int(aNumbers, iLeft, iMiddle);
    _MergeSort_int(aNumbers, iMiddle + 1, iRight);
    _Merge_int(aNumbers, iLeft, iMiddle, iRight);
  }
}

static void _Merge_float(float aNumbers[], int iLeft, int iMiddle, int iRight) {
  float aLeft [iMiddle - iLeft + 1];
  float aRight[iRight - iMiddle];

  memcpy(aLeft, aNumbers + iLeft, sizeof(aLeft));
  memcpy(aRight, aNumbers + iMiddle + 1, sizeof(aRight));
  int i = 0;
  int j = 0;
  int k = iLeft;
  while ((i < ARRAY_LENGTH(aLeft)) && (j < ARRAY_LENGTH(aRight))) {
    if (aLeft[i] <= aRight[j]) {
     aNumbers[k] = aLeft[i];
     i++;
    } else {
      aNumbers[k] = aRight[j];
      j++;
    }
    k++;
  }
  while (i < ARRAY_LENGTH(aLeft)) {
    aNumbers[k] = aLeft[i];
    i++;
    k++;
  }
  while (j < ARRAY_LENGTH(aRight)) {
    aNumbers[k] = aRight[j];
    j++;
    k++;
  }
}

static void _MergeSort_float(float aNumbers[], int iLeft, int iRight) {
  if (iLeft < iRight) {
    int iMiddle = iLeft + (iRight - iLeft) / 2;
    _MergeSort_float(aNumbers, iLeft, iMiddle);
    _MergeSort_float(aNumbers, iMiddle + 1, iRight);
    _Merge_float(aNumbers, iLeft, iMiddle, iRight);
  }
}

void Loop() {
  static int   aRawValues[NUM_SENSORS];
  static int   MinRawValue;
  static float aEMAValues    [NUM_SENSORS];
  static float aBlurredValues[NUM_SENSORS];
  static float MaxBlurredValue;
  static float aExpandedValues[NUM_SENSORS * EXPAND_FACTOR_PER_SENSOR];
  static float MaxExpandedValue;
  static float TotalSum;
  static float DistPercentage;
  static int   iMaximum;
  static float xDir;
  static float yDir;
  static float signedDir;
  static int   iDir;
  static float aDirXHistory[RUNNING_MEDIAN_DIRECTION_LENGTH];
  static float aDirYHistory[RUNNING_MEDIAN_DIRECTION_LENGTH];
  static int   aDistHistory[RUNNING_MEDIAN_DISTANCE_LENGTH];
  elapsedMillis t;

  _CheckSetup();
  ZEROMEM(aRawValues);
  ZEROMEM(aBlurredValues);
  ZEROMEM(aExpandedValues);
  //
  // Read sensors
  //
  t = 0;
  while (t < 10) { // 10ms equal 12 runs of 833us each
    for (int i = 0; i < ARRAY_LENGTH(aRawValues); i++) {
      aRawValues[i] += 1 - digitalRead(SENSOR_PINS[i]);                    
    }
  }
  //
  // Filter out background noice by subtracting minimum sensor value from all other
  //
  MinRawValue = 1000000;
  for (int i = 0; i < ARRAY_LENGTH(aRawValues); i++) {
    if (aRawValues[i] < MinRawValue) {
      MinRawValue = aRawValues[i];
    }
  }
  for (int i = 0; i < ARRAY_LENGTH(aRawValues); i++) {
    aRawValues[i] -= MinRawValue;                 
  }
  //
  // Smooth values with Exponential Moving Average (EMA)
  //
  for (int i = 0; i < ARRAY_LENGTH(aEMAValues); i++) {
    aEMAValues[i] = (_AlphaEMA * (float)aRawValues[i]) + ((1.0f - _AlphaEMA) * aEMAValues[i]);                    
  }
  //
  // Blur values (with scaling factor of 100)
  //
  MaxBlurredValue = 0.0f;
  for (int i = 0; i < ARRAY_LENGTH(aEMAValues); i++) {
    int iLeft  = (i - 1 + ARRAY_LENGTH(aEMAValues)) % ARRAY_LENGTH(aEMAValues);
    int iRight = (i + 1) % ARRAY_LENGTH(aEMAValues);
    aBlurredValues[i]  = aEMAValues[i]      * 0.01f * (float)BLUR_ORIGINAL_VALUE_WEIGHT_PERCENTAGE;
    aBlurredValues[i] += aEMAValues[iLeft ] * 0.01f * (float)(100 - BLUR_ORIGINAL_VALUE_WEIGHT_PERCENTAGE) / 2.0f;
    aBlurredValues[i] += aEMAValues[iRight] * 0.01f * (float)(100 - BLUR_ORIGINAL_VALUE_WEIGHT_PERCENTAGE) / 2.0f;
    if (aBlurredValues[i] > MaxBlurredValue) {
      MaxBlurredValue = aBlurredValues[i];
    }
  }
  //
  // Abort if highest value lower than minimum value for detecting a ball
  //
  if(MaxBlurredValue + MinRawValue < MIN_VALUE_TO_DETECT) {
    _BallDir = 0;
    _BallDist = 0;
    return;
  }
  //
  // Exaggerate differences for more significant peaks
  //
  for (int i = 0; i < ARRAY_LENGTH(aBlurredValues); i++) {
    float p = aBlurredValues[i] / MaxBlurredValue;
    aBlurredValues[i] = p * p * MaxBlurredValue;
  }
  //
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
  for(int i = 0; i < ARRAY_LENGTH(aBlurredValues); i++) {
    int iNext     = (i + 1) % ARRAY_LENGTH(aBlurredValues);
    int iNextNext = (i + 2) % ARRAY_LENGTH(aBlurredValues);
    float y0 = aBlurredValues[i];
    float y1 = aBlurredValues[iNext];
    float d0 = aBlurredValues[iNext] - aBlurredValues[i];
    float d1 = aBlurredValues[iNextNext] - aBlurredValues[iNext];
    float a =   (2.0f * y0) - (2.0f * y1) +         d0  + d1;
    float b = - (3.0f * y0) + (3.0f * y1) - (2.0f * d0) - d1;
    float c = d0;
    float d = y0;
    for(int j = 0; j < EXPAND_FACTOR_PER_SENSOR; j++) { // expand values
      int iCurrent = (((i+1) * EXPAND_FACTOR_PER_SENSOR) + j - 1) % ARRAY_LENGTH(aExpandedValues);
      float Percentage = (float)j / (float)EXPAND_FACTOR_PER_SENSOR;
      aExpandedValues[iCurrent]  = a * Percentage * Percentage * Percentage;
      aExpandedValues[iCurrent] += b * Percentage * Percentage;
      aExpandedValues[iCurrent] += c * Percentage;
      aExpandedValues[iCurrent] += d;
      if (aExpandedValues[iCurrent] < 0.0f) { // negative values might mess up vector addition
        aExpandedValues[iCurrent] = 0.0f;
      }
      float Dist = MIN(ABS(i - _BallDir), ARRAY_LENGTH(aExpandedValues) - ABS(i - _BallDir));
      if (Dist <= VECTOR_ADDITION_SENSOR_COUNT / 2) { 
        // take into account last output, to scale nearby values up
        // this reduces noisy fluctuations while significant changes will be kept 
        // due to relative increase and squared difference exeggeration afterwards
        float p = (float)Dist * 2.0f / (float)VECTOR_ADDITION_SENSOR_COUNT;
        aExpandedValues[i] *= 1.05f - (0.05f * p);
      }
    }
  }
  //
  // Find maximum
  //
  iMaximum = 0;
  MaxExpandedValue = 0;
  for (int i = 1; i < ARRAY_LENGTH(aExpandedValues); i++) {
    if (aExpandedValues[i] > MaxExpandedValue) {
      iMaximum = i;
      MaxExpandedValue = aExpandedValues[i];
    }
  }
  //
  // Exaggerate differences for more significant peaks
  //
  TotalSum = 0.0f;
  for (int i = 0; i < ARRAY_LENGTH(aExpandedValues); i++) {
    float p = aExpandedValues[i] / (float)MaxExpandedValue;
    aExpandedValues[i] = p * p * (float)MaxExpandedValue;
    TotalSum += aExpandedValues[i];
  }
  //
  // Get direction by vector addition in the cone around highest value
  //
  xDir = aExpandedValues[iMaximum] * _aFactorX[iMaximum];
  yDir = aExpandedValues[iMaximum] * _aFactorY[iMaximum];
  for (int i = 1; i < (VECTOR_ADDITION_SENSOR_COUNT - 1) / 2; i++) {
    int iLeft  = (iMaximum - i + ARRAY_LENGTH(aExpandedValues)) % ARRAY_LENGTH(aExpandedValues);
    int iRight = (iMaximum + i) % ARRAY_LENGTH(aExpandedValues);
    xDir += (aExpandedValues[iLeft] * _aFactorX[iLeft]) + (aExpandedValues[iRight] * _aFactorX[iRight]);
    yDir += (aExpandedValues[iLeft] * _aFactorY[iLeft]) + (aExpandedValues[iRight] * _aFactorY[iRight]);
  }
  if (VECTOR_ADDITION_SENSOR_COUNT % 2 == 0 ) { // ensure added vectors are centered around maximum
    int iLeft  = (iMaximum - (VECTOR_ADDITION_SENSOR_COUNT / 2) + ARRAY_LENGTH(aExpandedValues)) % ARRAY_LENGTH(aExpandedValues);
    int iRight = (iMaximum + (VECTOR_ADDITION_SENSOR_COUNT / 2)) % ARRAY_LENGTH(aExpandedValues);
    xDir += (0.5f * aExpandedValues[iLeft] * _aFactorX[iLeft]) + (0.5f * aExpandedValues[iRight] * _aFactorX[iRight]);
    yDir += (0.5f * aExpandedValues[iLeft] * _aFactorY[iLeft]) + (0.5f * aExpandedValues[iRight] * _aFactorY[iRight]);
  }
  signedDir = atan2(xDir, yDir) / _AngleStep_rad; // robot's x and y is swapped compared to math's
  iDir = (int)(signedDir + ((signedDir < 0) ? -0.5f : 0.5f)) + (ARRAY_LENGTH(aExpandedValues) / 2) - 1; // +/- 0.5 to round to next int
  //
  // Calculate distance
  //
  DistPercentage = TotalSum / (aExpandedValues[iDir] * ARRAY_LENGTH(aExpandedValues) / 2.0f);
  DistPercentage = MIN(MAX(0.0f, 1.0 - DistPercentage), 1.0f);
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
  aDistHistory[ARRAY_LENGTH(aDistHistory) - 1] = (DistPercentage * (float)ARRAY_LENGTH(aExpandedValues)) + 0.5f; // + 0.5f to round to next int
  //
  // Sort arrays
  //
  float aDirXHistory_sorted[RUNNING_MEDIAN_DIRECTION_LENGTH];
  float aDirYHistory_sorted[RUNNING_MEDIAN_DIRECTION_LENGTH];
  int   aDistHistory_sorted[RUNNING_MEDIAN_DISTANCE_LENGTH];
  memcpy(aDirXHistory_sorted, aDirXHistory, sizeof(aDirXHistory_sorted));
  memcpy(aDirYHistory_sorted, aDirYHistory, sizeof(aDirYHistory_sorted));
  memcpy(aDistHistory_sorted, aDistHistory, sizeof(aDistHistory_sorted));
  _MergeSort_float(aDirXHistory_sorted, 0, ARRAY_LENGTH(aDirXHistory_sorted) - 1);
  _MergeSort_float(aDirYHistory_sorted, 0, ARRAY_LENGTH(aDirYHistory_sorted) - 1);
  _MergeSort_int  (aDistHistory_sorted, 0, ARRAY_LENGTH(aDistHistory_sorted) - 1);
  //
  // Get median values
  //
  float DirXMedian = aDirXHistory_sorted[(ARRAY_LENGTH(aDirXHistory_sorted) - 1) / 2];
  float DirYMedian = aDirYHistory_sorted[(ARRAY_LENGTH(aDirYHistory_sorted) - 1) / 2];
  int   DistMedian = aDistHistory_sorted[(ARRAY_LENGTH(aDistHistory_sorted) - 1) / 2];
  //
  // Calc final direction
  //
  signedDir = atan2(DirXMedian, DirYMedian) / _AngleStep_rad; // robot's x and y is swapped compared to math's
  _BallDir = (int)(signedDir + ((signedDir < 0) ? -0.5f : 0.5f)) + (ARRAY_LENGTH(aExpandedValues) / 2) - 1; // +/- 0.5 to round to next int
  _BallDist = DistMedian;
}