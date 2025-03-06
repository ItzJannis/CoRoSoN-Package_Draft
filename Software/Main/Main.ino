#include "CoRoSoN_Main.h"
#include "../Peripheral/CoRoSoN_IR-Ring.h"
#include "../Peripheral/CoRoSoN_Pixy.h"

IRRing irRing(I2C_ADD_IR);
Pixy pixy(I2C_ADD_PIXY, 1, 2);

void setup() {
  CoRoSoN_Init();
  pixy.Init();
}

void loop() {
  //
  // Set LEDs based on buttons
  //
  if (CoRoSoN_ButtonIsPressed(I2C_MODULE_1, LEFT)) {
    CoRoSoN_SetBoardLEDColor(LEFT, BLUE);
    CoRoSoN_SetI2CLEDColor(I2C_MODULE_1, LEFT, BLUE);
    CoRoSoN_SetMotor(MOTOR_1, FORWARD, 20);
  } else {
    CoRoSoN_SetBoardLEDColor(LEFT, OFF);
    CoRoSoN_SetI2CLEDColor(I2C_MODULE_1, LEFT, OFF);
    CoRoSoN_SetMotor(MOTOR_1, FORWARD, 0);
  }
  if (CoRoSoN_ButtonIsPressed(I2C_MODULE_1, RIGHT)) {
    CoRoSoN_SetBoardLEDColor(RIGHT, BLUE);
    CoRoSoN_SetI2CLEDColor(I2C_MODULE_1, RIGHT, BLUE);
    CoRoSoN_SetMotor(MOTOR_2, FORWARD, 20);
  } else {
    CoRoSoN_SetBoardLEDColor(RIGHT, OFF);
    CoRoSoN_SetI2CLEDColor(I2C_MODULE_1, RIGHT, OFF);
    CoRoSoN_SetMotor(MOTOR_2, FORWARD, 0);
  }
  //
  // Debug print ir
  //
  DEBUG_PRINT(irRing.SeesBall());
  DEBUG_PRINT(irRing.BallDirection());
  DEBUG_PRINT(irRing.BallDistance());
  DEBUG_PRINT(pixy.SeesGoal());
  DEBUG_PRINT(pixy.DirectionGoal());
  DEBUG_PRINT(pixy.SeesOwnGoal());
  DEBUG_PRINT(pixy.DirectionOwnGoal());
  //
  // Update
  //
  irRing.Update();
  pixy.Update();
  delay(5);
}