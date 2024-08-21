# CoRoSoN-Package
>**Co**|mponents for \
>**Ro**|bocupJunior \
>**So**|ccer \
> **N**|ewcomers \
> Project:     https://github.com/CoRoSoN-Kit/ \
> Competition: https://www.robocup.org \
> Created by BohleBots: https://www.bohlebots.de

## CoRoSoN_Util
Providing usefull utility functions like macros to initialize data, verbose debugging or error handling

>### ERRORS and ERROR_CODE
ERRORS is a bitmap for ERROR_CODES for ease of use \
Available ERROR_CODES:
>#### OKAY
No error occured
>#### CONNECT_FAILED
Connection to device could not be established
>#### INVALID_ANSWER
Answer of device does not match protocol
>#### INVALID_PARAMETER
Parameters do not match requirements (i.e. speed > 100%)
>#### INVALID_CONFIG
Configuration does not match parameters request
>#### PROCESS_RUNNING
Internal process still running or internal timer not yet ready (i.e. kick right after kick => cooling time not over)
>#### ERROR_IGNORED
Error ignored, so no break out and no error handling happended \
Generally you should not ignore errors, but if they happen while doing the setup it might be useful to ignore the error to allow the setup function to finish i.e.
>#### ERROR_HANDLED
Error handled and function continued with handled error (i.e. speed > 100% ==> speed = 100%)
>#### ERROR_BREAK_OUT
Error could not be handled and lead to early exit of the function

### COROSON_DEBUG
If this is defined the verbose debugging functionsare compiled in.
>#### DEBUG_PRINT(Variable)
Prints the value passed in a verbose string to the serial monitor inlcuding origin of print and variable name
>#### DEBUG_BLOCK(Message, NumMillis)
Blocks the programm in an endless loop by epeatadly prints the passed massage every numMillis milliseconds

### Function macros
>#### MIN(X,Y)
Returns the smaller one of the two values
>#### MAX(X,Y)
Returns the bigger one of the two values
>#### ABS(X)
Returns the absolute value of the passed argument (works with all number types)
>#### ARRAY_LENGTH(Arr)
Returns the number of elements in the passed array (**Do not use with heap-allocated Arr**)
>#### ZEROMEM(Obj)
Sets all bytes of Obj to 0 (**Dont use with heap-allocated Obj**) 
>#### ZEROMEM_N(Obj, NumBytes)
Sets the first NumBytes bytes of Obj to 0

## CoRoSoN_Eeprom
Providing usefull utility functions to make use of the ESP32's eeprom \
i.e. save last robot state to restart into it after reset
>#### struct EEPROM_DATA
Modifiy this struct for the eeprom interactions to read / write data however you like. \
Every eeprom interaction will read or write 1 instance of this struct
>#### ERRORS EEPROM_Init(unsigned short Address)
Initializes the ESP32's eeprom at the passed address
>#### EEPROM_DATA EEPROM_Read()
Reads the currently in the eeprom stored EEPROM_DATA
>#### ERRORS EEPROM_Write(const EEPROM_DATA& Data)
Writes the passed EEPROM_DATA to the eeprom

## CoRoSoN_I2C
Providing a simpler, more beginner friendly interface to work with the I2C bus system used in CoRoSoN kit
>#### ERRORS I2C_Init(unsigned short SerialDataPin, unsigned short SerialClockPin)
Initialises the I2C bus on the board with the given data and clock pin
>#### ERRORS I2C_TestConnection(unsigned short Address)
Tests if connection to the given address is possible
>#### ERRORS I2C_Write(unsigned short Address, byte aMessageBytes[], unsigned int NumBytes)
Writes the passed Data byte per byte to the given address
>#### ERRORS I2C_ReadBlocking(unsigned short Address, byte aAnswerBytes[], unsigned int NumBytes)
Reads the passed number of bytes from the passed address and stores the answer in the passed byte array \
**Attention**: \
This functions waits for the answers and is therefore **blocking** \
Make sure that NumBytes equals the length of the passed byte array

## Main Board
The software described under this part is designed to create simple API's for controls that might be overwhelming for newcomers.

### CoRoSoN_IR-Ring
Main header for the ir ring provided by CoRoSoN kit board
>#### class IRRing
Providing a simpler, more beginner friendly interface to work with the IR-Ring
>##### IRRing(unsigned short Address)
Constructer for new IRRing instance with the passed I2C Address
>##### ERRORS Update(void)
Reads the new data
>##### bool SeesBall(void)
Returns if the ball is seen
>##### int BallDirection(void)
Returns the direction of the ball as follows: \
== 0: in front of robot \
 \> 0: to the right \
 < 0: to the left  \
**Additional information**: \
Returns -64 if ball is not seen therefore 180° or right behind robot is always +64
>##### int BallDistance(void)
Returns the distance of the ball to the robot \
1 = closest,..., 64 = furthest \
**Additional information**: \
Returns 0 if ball is not seen

### CoRoSoN_Pixy
Providing a simpler, more beginner friendly interface to work with a Pixy cam
>#### class Pixy
Providing a simpler, more beginner friendly interface to work with the Pixy camera
>##### Pixy(unsigned short Address, int SignatureGoal, int SignatureOwnGoal)
Constructer for new Pixy instance with the passed I2C Address and signatures for goal and own goal
>##### ERRORS Update(void)
Reads the new data
>##### bool SeesGoal(void)
Returns if the goal is seen
>##### bool SeesOwnGoal(void)
Returns if the own goal is seen
>##### int DirectionGoal(void)
Returns the goals directions in terms of pixels \
< 0 => goal to the left \
\> 0 => goal to the right
>##### int DirectionOwnGoal(void)
Returns the own goals directions in terms of pixels \
< 0 => goal to the left \
\> 0 => goal to the right

### CoRoSoN_Compass
>#### class Compass
Providing a simpler, more beginner friendly interface to work with a compass sensor
>##### Compass(unsigned short Address)
Constructer for new Compass instance with the passed I2C Address
>##### ERRORS SetHeading(void)
Sets the heading of the compass to the current state
>##### int HeadingAngle(void)
Returns the direction of the heading related to the current state \
< 0 => heading left  / turned to the right \
\> 0 => heading right / turned to the left 

### CoRoSoN
Including this file gives you everything CoRoSoN has to offer for your main board. \
Based on the content included by this file you should be able to built your first RoboCup Soccer Robot.

#### Configuration
##### I2C bus
>###### I2C_SDA
Serial pin for I2C data
>###### I2C_SCL
Serial pin for I2C clock
>###### I2C_ADD_PIXY
I2C address of pixy camera
>###### I2C_ADD_IR
I2C address of ir-ring
>###### I2C_ADD_CMPS
I2C address of compass sensor
>###### I2C_BASEADD_BTNLEDS
I2C base address for button and led modules
>###### I2C_BTNLED_CODE_1
Binary address encoded by soldering on the first module itself
>###### I2C_BTNLED_CODE_2
Binary address encoded by soldering on the second module itself
##### Motor
CoRoSoN supports driving with up to 4 motors. If less than 4 are used the once with the highest numbers are ignored.
>###### MOTOR_ENA
Enable pin for all motors
>###### MOTOR_1_DIR, MOTOR_2_DIR, MOTOR_3_DIR, MOTOR_4_DIR
Direction pins for the motors
>###### MOTOR_1_PWM, MOTOR_2_PWM, MOTOR_3_PWM, MOTOR_4_PWM
PWM pins for the motors
##### Analog Inputs
CoRoSoN supports 4 prefixed analog input pins
>###### ANALOG_IN_1, ANALOG_IN_2, ANALOG_IN_3, ANALOG_IN_4
##### Digital Outputs
CoRoSoN supports 2 prefixed digital output pins
>###### DIGITAL_OUT_1, DIGITAL_OUT_2

#### Types
>##### COLOR
Color enumaration to easily use LEDs
>###### OFF, GREEN, RED, YELLOW, BLUE, CYAN, MAGENTA, WHITE
>##### SIDE
Side enumaration to easily choose between left and right button or LED 
>###### LEFT, RIGHT
>##### I2C_ADD_BTNLED_MODULE
Button and LED module enumaration to easily choose between the two button and LED modules
>###### I2C_MODULE_1, I2C_MODULE_2
>##### MOTOR
Motor enumaration to easily choose a motor
>###### MOTOR_1, MOTOR_2, MOTOR_3, MOTOR_4
>##### DIRECTION
Direction enumaration to easily control motors
>###### BACKWARD, FORWARD

### Functions
>#### ERRORS CoRoSoN_Init(void)
Initializes all I2C, pins, serial, etc.
>#### ERRORS CoRoSoN_SetLEDColor(I2C_ADD_BTNLED_MODULE AddModule, SIDE Side, COLOR Color)
Sets the color of the LED
>#### bool CoRoSoN_ButtonIsPressed(I2C_ADD_BTNLED_MODULE AddModule, SIDE Side)
Returns if the button is pressed \
**Attention:** \
Returns false if the connect failed \
(Yes, there would be ways to implement return structs or returning by passed pointer, but that would be advanced and nothing for newcomers)
>#### ERRORS CoRoSoN_SetMotor(MOTOR Motor, DIRECTION Direction, unsigned int SpeedPercentage)
Sets the motor to the passed direction with the passed percentage of max speed \
**Attention:** \
SpeedPercentage > 100 will be changed to SpeedPercentage = 100


## IR Ring
The software described under this part aims to provide an all-in-one solution for the IR ring to convert the TSSP sensor values to usefull direction and distance of the ball available via I2C
### Firmware_IR-Ring
This firmware basically does:
1. Read all physical signals
2. Gaussian blur on each sensors value with its 2 neighbouring values
3. Expand values to higher resolution by using cubic interpolation
>#### NUM_SENSORS
Number of physical TSSP sensors for the rotational-symmetric evaluation
>#### BLUR_ORIGINAL_VALUE_WEIGHT
Weight for the original value in step 2 (gaussian blur) \
**Attention:** \
Must be a value greater than 0 and less than 1
>#### EXPAND_FACTOR_PER_SENSOR
Factor by which the values should be expanded in step 3 \
**Attention:** \
Must be an integer from 1 to 16 (both inclusive)
>#### const unsigned short SENSOR_PINS\[NUM_SENSORS\]
Pins for the NUM_SENSORS physical TSSP sensors
>#### ERRORS Setup(void)
Call this function in arduino's setup on ir ring to setup everything 
>#### void Loop(void)
Call this function in arduino's loop to read the current sensor situation