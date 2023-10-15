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
// File:    CoRoSoN_Util.h
// Purpose: Providing usefull utility functions like macros to 
//          initialize data, verbose debugging or error handling
// -------------------------------------------------------------------
#ifndef COROSON_UTIL_H
#define COROSON_UTIL_H
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include <Arduino.h>
/*********************************************************************
* 
*  Config
*
*********************************************************************/
#define COROSON_DEBUG // uncomment to shut off debug functionality
/*********************************************************************
* 
*  Types
*
*********************************************************************/
#define ERRORS byte 
//
// use this as the default return type for functions 
// that a coding newbie would declare as void 
// to provide a way to check for errors and handle them
//
enum ERROR_CODE : ERRORS {
  OKAY              = 0b00000000, // no error occured
  CONNECT_FAILED    = 0b00000001, // connection to device could not be established
  INVALID_ANSWER    = 0b00000010, // answer of device does not match protocol
  INVALID_PARAMETER = 0b00000100, // parameters do not match requirements (i.e. speed > 100%)
  INVALID_CONFIG    = 0b00001000, // configuration does not match parameters request
  PROCESS_RUNNING   = 0b00010000, // internal process still running or internal timer not yet ready (i.e. kick right after kick => cooling time not over)
  ERROR_IGNORED     = 0b00100000, // error ignored, so no break out and no error handling happended
  ERROR_HANDLED     = 0b01000000, // error handled and function continued with handled error (i.e. speed > 100% ==> speed = 100%)
  ERROR_BREAK_OUT   = 0b10000000  // error could not be handled and lead to early exit of the function
}; 
/*
  Construct your return value by merging the enum values with bitwise or ('|' operator)
  Check for a specific error with bitwise and ('&' operator)
  because OKAY is 0 you can simply check for an error by using if
 
  Example function:
 
         ERRORS printRatio(int a, int b) {
           double ratio;
           [Check for division by 0]
           if(b == 0) {
             DEBUG_PRINT(b);
             return (INVALID_PARAMETER | ERROR_BREAK_OUT);
           }
           ratio = (double)a / (double)b;
           Serial.println("ratio between " + String(a) + " and " + String(b) + " is " + String(ratio));
           return OKAY;
         }
 
  Example for using a function with ERRORS return type:
 
         ERRORS errors = someFunction("test");
         if(errors) {
           if(errors & CONNECT_FAILED) {
             [what to do if connect failed?]
           }
           if(errors & INVALID_PARAMETER) {
             [what to do if the parameter was invalid?]
           }
           [how should the errors be handled?]
         }
         [rest of code]
*/
/*********************************************************************
* 
*  Functions
*
*********************************************************************/
#ifdef COROSON_DEBUG
  //
  // name of the current file as a native C String (char* / "text")
  #define FILENAME (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)
  //
  // String (arduino type) concatinating current filename and current line into a verbose origin string for debug outputs
  #define ORIGIN String(String(FILENAME) + " in " + String(__PRETTY_FUNCTION__) + " at line " + String(__LINE__) + " : ")
  //
  // Prints the value passed in a verbose string to the serial monitor inlcuding origin of print and variable name
  #define DEBUG_PRINT(Variable) Serial.println(ORIGIN + String(#Variable) + " = " + String(Variable))
  //
  // Prints the different ERROR_CODE values the passed error is build off
  #define DEBUG_ERRORS(Errors) _DEBUG_ERRORS(ORIGIN, Errors)
  //
  // blocks the programm in an endless loop by epeatadly prints the passed massage every numMillis milliseconds
  #define DEBUG_BLOCK(Message, NumMillis) do { Serial.println(ORIGIN + Message); delay(NumMillis); } while(1)
#else // COROSON_DEBUG
  // define all the debug functions as nothing if not in debug config
  #define FILENAME
  #define ORIGIN
  #define DEBUG_PRINT(Variable)
  #define DEBUG_ERRORS(Errors)
  #define DEBUG_BLOCK(Message, NumMillis)
#endif // COROSON_DEBUG
//
// Returns the smaller one of the two values
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
//
// Returns the bigger one of the two values
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
//
// Returns the absolute value of the passed argument (works with all number types)
#define ABS(X) ((X) < 0 ? -(X) : (X))
//
// Returns the number of elements in the passed array
#define ARRAY_LENGTH(Arr) (sizeof(Arr) / sizeof(*Arr))
//
// Fills the bytes at the adress of the passed offset with 0's
#define ZEROMEM(Obj) memset(&Obj, 0, sizeof(Obj))
// 
// Do not call these directly!
//
#ifdef COROSON_DEBUG
  void _DEBUG_ERRORS(String Origin, ERRORS Errors);
#endif // COROSON_DEBUG
#endif // COROSON_UTIL_H