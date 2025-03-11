# CoRoSoN-Package Draft
>**Co**|mponents for \
>**Ro**|bocupJunior \
>**So**|ccer \
> **N**|ewcomers \
> Project: [CoRoSoN-Kit](https://github.com/CoRoSoN-Kit/CoRoSoN-Kit) \
> Competition: [RoboCup](https://www.robocup.org) \
> Author: [BohleBots](https://www.bohlebots.de)



# Description
This project offers a draft version to the official [CoRoSoN-Kit](https://github.com/CoRoSoN-Kit/CoRoSoN-Kit) to provide the fundamental building blocks for a fully functional robot participating in the Soccer Junior division of the [RoboCup](https://www.robocup.org). \
The purpose is to allow for an newcomer friendly entry to the competition by wrapping the essential functions with easy to use APIs. Board designs are designed to be rebuild easily and affordable. \
\
Feel free to reach out to us for questions, feedback, suggestions or constructive critism! \
See you in the competition!

## Disclaimer
The software and the hardware designs are developed to the best of our knowledge to serve the intented purpose, but may still contain faults. Our software and hardware are provided "as is" and do not provide any warranty. The risk of using it and all neccessary servicing, repair and correction as well as their cost is with you.

# Makefile
Using the Arduino IDE or arduino-cli requires all source for a .ino sketch to be inside the sketch folder or inside the libaries directory of the Arduino installation. \
Therefore source files inside Software/Shared or Software/Peripheral, that are not located directly next to a .ino sketch, cause problems. \
To allow for easy compatibility of this project with Arduino the Makefile was created. \
It automates the generation of valid Arduino sketch folders, such that they can be opened in the Arduino IDE or used with arduino-cli. \
\
The Makefile does not require all source files to be inside the sketch folder, but it requires SketchName.ino to be located inside Software/SketchName/.
\
Run `make` inside the home directory of this repo to see all the options.

# Software
## Requirements
1. [Arduino](https://www.arduino.cc/)
2. [Espressif ESP32 Board Information](https://github.com/espressif/arduino-esp32)
3. [elapsedMillis](https://github.com/pfeerick/elapsedMillis/wiki)
4. [PixyI2C](https://pixycam.com/downloads-pixy1/)
5. [Pixy2I2C](https://pixycam.com/downloads-pixy2/)


## Shared
Software that is not build for a specific board but is shared across different board's software.

### CoRoSoN_Util
Providing usefull utility definitions for error handling, initializing data and better debugging. \
The `enum ERROR_CODE` provides bitflags for uniform error handling while `DEBUG_PRINT(Variable)`, `DEBUG_ERRORS(Errors)` and `DEBUG_BLOCK(Message, NumMillis)` provide easier debugging via the serial connection and the `MIN(X,Y)`, `MAX(X,Y)`, `ABS(X)`, `ARRAY_LENGTH(Arr)`, `ZEROMEM(Obj)` provide function-like macros to handle data without tedious reimplementation of often used functionalities.

### CoRoSoN_I2C
Providing a simpler, more beginner friendly interface to work with the I2C bus system used in this project. \
`I2C_Init()`, `I2C_TestConnection()`, `I2C_Write()` and `I2C_ReadBlocking()` provide all the functionality a newcomer robot for the competition needs. \
**Attention**: \
`I2C_ReadBlocking()` waits for the answers and is therefore **blocking** the execution of the program. \
Make sure that NumBytes equals the length of the passed byte array!

### CoRoSoN_Vector
Providing `class Vector` to easily compute vectorial calculations with functions and operator overloading.

### CoRoSoN_Config
This file defines the hardware configuration (namely the pins) for the software.


## Main
Software that is supposed to be used on the mainboard.
### CoRoSoN_Main
The definitions of `enum COLOR`, `enum SIDE`, `enum I2C_ADD_BTNLED_MODULE`, `enum MOTOR` and `enum DIRECTION` are used to firstly make software better readable for newcomers and secondly somehow restrict inputs to reduce buggy behavior due to occasionally wrong inputs. \
`CoRoSoN_Init()`, `CoRoSoN_SetLEDColor()`, `CoRoSoN_ButtonIsPressed()` and `CoRoSoN_SetMotor()` provide a beginner friendly API to setting the mainboard up in the `setup()` of an arduino program as well as reading and writing the mainboards pins.

## Peripheral
Software that is supposed to be used on the mainboard to make use of the peripheral devices.
### CoRoSoN_Eeprom
Providing an interface to the ESP32's eeprom which is easy to use.\
The eeprom is very usefull to save backup data about the current program state (i.e. the robot is enabled to drive) in case of a shutdown or a reset for whatever reason.\
The `struct EEPROM_DATA` can be filled with whatever you want to read / write from the eeprom.
`EEPROM_Init()`, `EEPROM_Read()` and `EEPROM_Write()` provide an easy interface that only deals with instances of `EEPROM_DATA`.

### CoRoSoN_Pixy
Providing `class Pixy` with its `Update()`, `SeesGoal()`, `SeesOwnGoal()`, `DirectionGoal()` and `DirectionOwnGoal()` to make use of the Pixy camera for locating the two goals on the playing field. \
In `CoRoSoN_Pixy.h` set `PIXY_VERSION` to 1 or 2 according to the version of the Pixy cam in use. \
For the directions: <0 represents left, >0 represents right

### CoRoSoN_IR-Ring
Providing `class IRRing` with its `Update()`, `SeesBall()`, `BallDirection()` and `BallDistance()` to make use of the infrared sensor ring with the firmware running on it (also provided).
For the directions: <0 represents left, >0 represents right, 0 represents front, -32 represents no ball seen
For the distance: 1...32 represents closest...furthest, 0 represents no ball seen


## IR
Software that is supposed to be used on the infrared sensor ring.
### Firmware_IR-Ring
Providing the firmware that is designed to be loaded onto the infrared sensor ring and making it ready to be used in a plug-and-play manner. \
#### Software Process / Algorithm
1. Read all physical signals
2. Remove minimum value from all sensor values (ensuring at least one is 0)
3. Use exponential moving average (EMA) for each sensor - the lower `EMA_ALPHA_PERCENTAGE`, the more the values are smoothed
4. Gaussian blur on each sensors value with its 2 neighbouring values - `BLUR_ORIGINAL_VALUE_WEIGHT_PERCENTAGE` defines the weight the middle sensor gets
5. Exaggerate differences by squaring percentage of maximum value for each sensor
6. Expand values to higher resolution by using cubic interpolation
7. Giving the sensors close to the last output direction a boost of up to 5% of their current value, to reduce fluctuations
8. Find highest value
9. Exaggerate differences by squaring percentage of maximum value for each sensor
10. Vector addition around highest value for more precise direction - `VECTOR_ADDITION_SENSOR_COUNT` defines the number of vectors involved
11. Determine distance by numeric integration
12. Compute final direction and distance with running median - `RUNNING_MEDIAN_DIRECTION_LENGTH` and `RUNNING_MEDIAN_DISTANCE_LENGTH` define the history length for the running medians

# Hardware
## Main
Contains the [KiCad](https://www.kicad.org/download/) project of the Main PCB. 

## IR-Ring
Contains the [KiCad](https://www.kicad.org/download/) project of the IR-Ring PCB. 