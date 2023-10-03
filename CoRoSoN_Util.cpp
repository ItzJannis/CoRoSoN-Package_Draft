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
// File:    CoRoSoN_Util.cpp
// Purpose: Implementations for CoRoSoN_Util.h
// -------------------------------------------------------------------
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include "CoRoSoN_Util.h"
/*********************************************************************
* 
*  Implementations
*
*********************************************************************/
#ifdef BB_DEBUG
  static const ERROR_CODE _aCodes[] = {
     OKAY             
    ,CONNECT_FAILED   
    ,INVALID_ANSWER   
    ,INVALID_PARAMETER
    ,INVALID_CONFIG   
    ,PROCESS_RUNNING  
    ,ERROR_IGNORED  
    ,ERROR_HANDLED    
    ,ERROR_BREAK_OUT  
  };

  static const char* _aNames[] = { // KEEP IN SYNC WITH Codes !!!
     "OKAY"             
    ,"CONNECT_FAILED"   
    ,"INVALID_ANSWER"   
    ,"INVALID_PARAMETER"
    ,"INVALID_CONFIG"   
    ,"PROCESS_RUNNING"  
    ,"ERROR_IGNORED"  
    ,"ERROR_HANDLED" 
    ,"ERROR_BREAK_OUT"  
  };

  void _DEBUG_ERRORS(String Origin, ERRORS Errors) {
    String OutputString;
    int    i;

    OutputString = Origin;
    if(!Errors) {
      OutputString += _aNames[0];
      goto Print; // Loop unnecessary if Errors are none
    }
    for(i = 0; i < ARRAY_LENGTH(_aCodes); i++) {
      if(Errors & _aCodes[i]) {
        OutputString += _aNames[i];
        OutputString += " | ";
      }
    }
  Print:
    Serial.println(OutputString);
  }
#endif // BB_DEBUG