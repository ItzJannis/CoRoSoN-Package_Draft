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
#include <Wire.h>
/*********************************************************************
* 
*  Config Check
*
*********************************************************************/
//
// EMA_ALPHA_DIRECTION_PERCENTAGE
#if (EMA_ALPHA_DIRECTION_PERCENTAGE <= 0)
  #error "Config-Error: EMA_ALPHA_DIRECTION_PERCENTAGE must be greater than 0"
#elif (EMA_ALPHA_DIRECTION_PERCENTAGE > 100)
  #error "Config-Error: EMA_ALPHA_DIRECTION_PERCENTAGE must be less than or equal to 100"
#endif
//
// EMA_ALPHA_DISTANCE_PERCENTAGE
#if (EMA_ALPHA_DISTANCE_PERCENTAGE <= 0)
  #error "Config-Error: EMA_ALPHA_DISTANCE_PERCENTAGE must be greater than 0"
#elif (EMA_ALPHA_DISTANCE_PERCENTAGE > 100)
  #error "Config-Error: EMA_ALPHA_DISTANCE_PERCENTAGE must be less than or equal to 100"
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
static byte _BallDir = 0;
static byte _BallDist = 0;
static bool _SetupSuccessfull = false;
static double _AlphaEMA_Dir  = (double)EMA_ALPHA_DIRECTION_PERCENTAGE / 100.0;
static double _AlphaEMA_Dist = (double)EMA_ALPHA_DISTANCE_PERCENTAGE  / 100.0;
//
// Grid like image: X = left to right, Y = top to bottom
//
static const unsigned int _GRID_RANGE_X = 139; // Ratio approx. soccer field, max. distance approx. 250
static const unsigned int _GRID_RANGE_Y = 208; // Ratio approx. soccer field, max. distance approx. 250
static const unsigned int _GRID_WIDTH  = _GRID_RANGE_X + 13 + _GRID_RANGE_X;
static const unsigned int _GRID_HEIGHT = _GRID_RANGE_Y + 13 + _GRID_RANGE_Y;
//
// Sensor stencel:
//
// |--------- 13 ----------|
// . . . . . . X . . . . . . --
// . . . . X . . . X . . . .  |
// . . X . . . . . . . X . .  |
// . . . . . . . . . . . . .  |
// . X . . . . . . . . . X .  |
// . . . . . . . . . . . . .  |
// X . . . . . + . . . . . X  13
// . . . . . . . . . . . . .  |
// . X . . . . . . . . . X .  |
// . . . . . . . . . . . . .  |
// . . X . . . . . . . X . .  |
// . . . . X . . . X . . . .  |
// . . . . . . X . . . . . . --
//
static const int _aSensorX [NUM_SENSORS] = { -2, -4, -5, -6, -5, -4, -2,  0,  2,  4,  5,  6,  5,  4,  2,  0};
static const int _aSensorY [NUM_SENSORS] = {  5,  4,  2,  0, -2, -4, -5, -6, -5, -4, -2,  0,  2,  4,  5,  6};
static int _CenterX = _GRID_RANGE_X - 1 + 7;
static int _CenterY = _GRID_RANGE_Y - 1 + 7;
static float _aaGridTotal[_GRID_WIDTH][_GRID_HEIGHT];
static float _aaaSensorInfluence[NUM_SENSORS][_GRID_WIDTH][_GRID_HEIGHT];

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
  //
  // Init pins
  //
  for(int i = 0; i < ARRAY_LENGTH(SENSOR_PINS); i++) {
    pinMode(SENSOR_PINS[i], INPUT);
  }
  //
  // Init I2C
  //
  Wire.onRequest(_OnRequest);
  Wire.onReceive(_OnReceive);
  if(!Wire.begin(I2C_ADD_IR)) {
    r |= CONNECT_FAILED | ERROR_BREAK_OUT;
    DEBUG_ERRORS(r);
    DEBUG_BLOCK("I2C-Bus konnte nicht gestartet werden", 1000);
  } else {
    _SetupSuccessfull = true;
  }
  //
  // Init Grid
  //
  ZEROMEM(_aaaSensorInfluence);
  Vector vCenter(_CenterX, _CenterY);
  for (int i = 0; i < NUM_SENSORS; i++) {
    Vector vCenterToSensor(_aSensorX[i], _aSensorY[i]);
    //
    // Determine where a reflection would occure (center of beem)
    // Ray = vCenter + t * vCenterToSensor
    // => lowest t to intersect with edge of grid determines potential reflection point
    //
    float t_left   = ((_aSensorX[i] < 0) ? (- (float)_GRID_WIDTH  / (2.0f * (float)_aSensorX[i])) : __FLT_MAX__);
    float t_right  = ((_aSensorX[i] > 0) ? (  (float)_GRID_WIDTH  / (2.0f * (float)_aSensorX[i])) : __FLT_MAX__);
    float t_top    = ((_aSensorY[i] < 0) ? (- (float)_GRID_HEIGHT / (2.0f * (float)_aSensorY[i])) : __FLT_MAX__);
    float t_bottom = ((_aSensorY[i] > 0) ? (  (float)_GRID_HEIGHT / (2.0f * (float)_aSensorY[i])) : __FLT_MAX__);
    int xReflection = -1;
    int yReflection = -1;
    if ((t_left <= t_right) && (t_left <= t_top) && (t_left <= t_bottom)) {
      xReflection = 0;
      yReflection = ((float)_GRID_HEIGHT / 2.0f) + (t_left * (float)_aSensorY[i]) + 0.5f; // +0.5f for rounding to next int
    }
    if ((t_right <= t_left) && (t_right <= t_top) && (t_right <= t_bottom)) {
      xReflection = _GRID_WIDTH - 1;
      yReflection = ((float)_GRID_HEIGHT / 2.0f) + (t_right * (float)_aSensorY[i]) + 0.5f; // +0.5f for rounding to next int
    }
    if ((t_top <= t_left) && (t_top <= t_right) && (t_top <= t_bottom)) {
      xReflection = ((float)_GRID_WIDTH / 2.0f) + (t_top * (double)_aSensorX[i]) + 0.5f; // +0.5f for rounding to next int
      yReflection = 0;
    }
    if ((t_bottom <= t_left) && (t_bottom <= t_right) && (t_bottom <= t_top)) {
      xReflection = ((float)_GRID_WIDTH / 2.0f) + (t_bottom * (float)_aSensorX[i]) + 0.5f; // +0.5f for rounding to next int
      yReflection = _GRID_HEIGHT - 1;
    }
    //
    // Calculate influence factor for each cell
    //
    for (int x = 0; x < _GRID_WIDTH; x++) {
      for (int y = 0; y < _GRID_HEIGHT; y++) {
        // Determine if cell is in the 90 degree view angle of sensor
        Vector vSensorToCell = Vector(x, y) - (vCenter + vCenterToSensor);
        if (vCenterToSensor.angleOffsetTo(vSensorToCell) <= 0.5 * M_PI) {
          _aaaSensorInfluence[i][x][y] = 1.0f;
        } else {
          _aaaSensorInfluence[i][x][y] = 1.0f / (float)(((x - xReflection) * (x - xReflection)) + ((y - yReflection) * (y - yReflection)));
        }
      }
    }
  }
  return r;
}

void Loop() {
  static int aRawValues[NUM_SENSORS];
  static float MaxValue;
  static int MaxX;
  static int MaxY;
  static double EMA_DirX; 
  static double EMA_DirY;
  static double EMA_Dist;
  elapsedMillis t;

  _CheckSetup();
  ZEROMEM(aRawValues);
  ZEROMEM(_aaGridTotal);
  //
  // Read sensors
  //
  t = 0;
  while (t < 5) { // 5ms equal 6 runs of 833us each
    for (int i = 0; i < NUM_SENSORS; i++) {
      aRawValues[i] += 1 - digitalRead(SENSOR_PINS[i]);                         
    }
  }
  //
  // Create total grid
  //
  for (int i = 0; i < NUM_SENSORS; i++) {
    for (int x = 0; x < _GRID_WIDTH; x++) {
      for (int y = 0; y < _GRID_HEIGHT; y++) {
        _aaGridTotal[x][y] += (float)aRawValues[i] * _aaaSensorInfluence[i][x][y];
      }
    }
  }
  //
  // Find maximum
  //
  MaxValue = -1.0;
  MaxX = -1;
  MaxY = -1;
  for (int x = 0; x < _GRID_WIDTH; x++) {
    for (int y = 0; y < _GRID_HEIGHT; y++) {
      if (_aaGridTotal[x][y] > MaxValue) {
        MaxValue = _aaGridTotal[x][y];
        MaxX = x;
        MaxY = y;
      }
    }
  }
  if(MaxValue < (float)MIN_VALUE_TO_DETECT) {
    _BallDir = 0;
    _BallDist = 0;
    return;
  }
  //
  // Smoothing with Exponential Moving Average
  //
  EMA_DirX = (_AlphaEMA_Dir * (double)MaxX) + ((1.0 - _AlphaEMA_Dir) * EMA_DirX);
  EMA_DirY = (_AlphaEMA_Dir * (double)MaxY) + ((1.0 - _AlphaEMA_Dir) * EMA_DirY);
  //
  // Calc final vector to ball
  //  Note: vector x = bottom to top, y = left to right, such that angles are correct
  //  => See CoRoSoN_Vector.h for more info
  //  
  Vector vBall((double)(_GRID_HEIGHT - 1) - EMA_DirY, EMA_DirX); // Vector x = bottom to 
  double floatingDir = (vBall.getAngle() * 128.0 / M_PI) + 127.5;
  _BallDir = ((floatingDir < 0.0) ? 255 : (int)floatingDir);
  _BallDir = vBall.getRadius() + 0.5; // +0.5 for rounding to next int
}
