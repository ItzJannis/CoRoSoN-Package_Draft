#include "CoRoSoN_Main.h"
#include "CoRoSoN_IR-Ring.h"
#include "CoRoSoN_Pixy.h"
#include <elapsedMillis.h>

IRRing irRing(I2C_ADD_IR);
Pixy pixy(I2C_ADD_PIXY, 1, 2);
bool IsInGame = false;
elapsedMillis buttonDeadTime = 0;

void setup() {
  CoRoSoN_Init();
  pixy.Init();

  delay(200);
  CoRoSoN_SetBoardLEDColor(LEFT , GREEN);
  CoRoSoN_SetBoardLEDColor(RIGHT, GREEN);
  delay(200);
  CoRoSoN_SetBoardLEDColor(LEFT , RED);
  CoRoSoN_SetBoardLEDColor(RIGHT, RED);
  delay(200);
  CoRoSoN_SetBoardLEDColor(LEFT , YELLOW);
  CoRoSoN_SetBoardLEDColor(RIGHT, YELLOW);
  delay(200);
  CoRoSoN_SetBoardLEDColor(LEFT , BLUE);
  CoRoSoN_SetBoardLEDColor(RIGHT, BLUE);
  delay(200);
  CoRoSoN_SetBoardLEDColor(LEFT , CYAN);
  CoRoSoN_SetBoardLEDColor(RIGHT, CYAN);
  delay(200);
  CoRoSoN_SetBoardLEDColor(LEFT , MAGENTA);
  CoRoSoN_SetBoardLEDColor(RIGHT, MAGENTA);
  delay(200);
  CoRoSoN_SetBoardLEDColor(LEFT , WHITE);
  CoRoSoN_SetBoardLEDColor(RIGHT, WHITE);
  delay(200);
  CoRoSoN_SetBoardLEDColor(LEFT , OFF);
  CoRoSoN_SetBoardLEDColor(RIGHT, OFF);
}

bool lightbarrierTriggered() {
  return (analogRead(ANALOG_IN_4) > 1000);
}

void drive(int speed, int rotation) {
  int MaxSpeed = ABS(speed) + ABS(rotation);
  if (MaxSpeed > 100) {
    speed = (speed * 100) / MaxSpeed;
    rotation = (rotation * 100) / MaxSpeed;
  }
  int spd1 = rotation - speed;
  int spd2 = rotation + speed;
  DIRECTION dir1 = (spd1 >= 0 ? FORWARD : BACKWARD);
  DIRECTION dir2 = (spd2 >= 0 ? FORWARD : BACKWARD);
  CoRoSoN_SetMotor(MOTOR_1, dir1, spd1);
  CoRoSoN_SetMotor(MOTOR_2, dir2, spd2);
}  

void standby() {
  //
  // Status LED Lightbarrier
  //
  if (lightbarrierTriggered()) {
    CoRoSoN_SetBoardLEDColor(RIGHT, GREEN);
  } else {
    CoRoSoN_SetBoardLEDColor(RIGHT, OFF);
  }
  //
  // Status LED ball
  //
  if (irRing.BallSeen()) {
    if (ABS(irRing.BallDirection()) < 3) {
      CoRoSoN_SetBoardLEDColor(LEFT, GREEN);
    } else if (ABS(irRing.BallDirection()) < 10) {
      CoRoSoN_SetBoardLEDColor(LEFT, CYAN);
    } else {
      CoRoSoN_SetBoardLEDColor(LEFT, BLUE);
    }
  } else {
    CoRoSoN_SetBoardLEDColor(LEFT, RED);
  }
  //
  // Button left => drive test
  //
  if (CoRoSoN_ButtonIsPressed(I2C_MODULE_1, LEFT)) {
    drive(50, 0);
  } else {
    drive(0, 0);
  }
  //
  // Button right => enter game
  //
  CoRoSoN_SetI2CLEDColor(I2C_MODULE_1, RIGHT, GREEN);
  if (CoRoSoN_ButtonIsPressed(I2C_MODULE_1, RIGHT)) {
    if (buttonDeadTime > 500) {
      IsInGame = true;
      buttonDeadTime = 0;
    }
  }
}

void game() {
  //
  // Shut off all LEDs
  //
  CoRoSoN_SetBoardLEDColor(RIGHT, OFF);
  CoRoSoN_SetI2CLEDColor(I2C_MODULE_1, LEFT, OFF);
  CoRoSoN_SetI2CLEDColor(I2C_MODULE_1, RIGHT, OFF);
  //
  // Game strategy
  //
  /*
    TODO: Insert your game strategy...
  */
}

void loop() {
  if (IsInGame) {
    game();
    //
    // Button right => enter game
    //
    if (CoRoSoN_ButtonIsPressed(I2C_MODULE_1, LEFT) || CoRoSoN_ButtonIsPressed(I2C_MODULE_1, RIGHT)) {
      if (buttonDeadTime > 500) {
        IsInGame = false;
        buttonDeadTime = 0;
        CoRoSoN_SetMotor(MOTOR_4, FORWARD, 0);
      }
    }
  } else {
    standby();
  }
  //
  // Debug prints
  //
  DEBUG_PRINT(irRing.BallSeen());
  DEBUG_PRINT(irRing.BallDirection());
  DEBUG_PRINT(irRing.BallDistance());
  DEBUG_PRINT(pixy.GoalSeen());
  DEBUG_PRINT(pixy.GoalDirection());
  DEBUG_PRINT(pixy.OwnGoalSeen());
  DEBUG_PRINT(pixy.OwnGoalDirection());
  //
  // Update
  //
  irRing.Update();
  pixy.Update();
  delay(5);
}