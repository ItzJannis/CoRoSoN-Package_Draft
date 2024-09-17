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
// File:    CoRoSoN_Config.h
// Purpose: Header for pin defines and other general hardware config
// -------------------------------------------------------------------
#ifndef COROSON_CONFIG_H
#define COROSON_CONFIG_H
/*********************************************************************
* 
*  Configuration
*
*********************************************************************/
//
// I2C Bus
//
#define I2C_SDA (21)
#define I2C_SCL (22)
//
// I2C Devices
//
#define I2C_ADD_PIXY        (0x54)
#define I2C_ADD_IR          (0x55)
#define I2C_ADD_CMPS        (0x60)
#define I2C_BASEADD_BTNLEDS (0x20)
#define I2C_BTNLED_CODE_1 (1) // change to match binary number that is encoded by soldering on button and led module
#define I2C_BTNLED_CODE_2 (2) // change to match binary number that is encoded by soldering on button and led module
//
//  Motors
//
#define MOTOR_ENA   ( 2)
#define MOTOR_1_DIR (13)
#define MOTOR_2_DIR (14)
#define MOTOR_3_DIR (25)
#define MOTOR_4_DIR (18)
#define MOTOR_1_PWM (12)
#define MOTOR_2_PWM (27)
#define MOTOR_3_PWM (26)
#define MOTOR_4_PWM (19)
//
// Analog Inputs
//
#define ANALOG_IN_1 (35)
#define ANALOG_IN_2 (34)
#define ANALOG_IN_3 (39)
#define ANALOG_IN_4 (36)
//
// Digital Outputs
//
#define DIGITAL_OUT_1 (15)
#define DIGITAL_OUT_2 (23)
#endif // COROSON_CONFIG_H