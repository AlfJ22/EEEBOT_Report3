//Header file stores all flags and headers used in the main file
#ifndef U_MAP
#define U_MAP


#include <MPU6050_tockn.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Keypad.h>


#define ROW_NUM     4 // four rows
#define COLUMN_NUM  3 // three columns
#define I2C_SLAVE_ADDR 0x04    //  iic slave address 

#define key_0 0x30  //keypad numbers 0 to 9
#define key_1 0x31
#define key_2 0x32
#define key_3 0x33
#define key_4 0x34
#define key_5 0x35
#define key_6 0x36
#define key_7 0x37
#define key_8 0x38
#define key_9 0x39
#define key_x 0x2a  //keypad *
#define key_h 0x23  //keypad #


bool FORWARD = false;    //Forward Flag
bool BACKWARD = false;   //Backward Flag
bool ALEFT = false;      //Left Flag
bool RIGHT = false;    //Right Flag
bool STOP = false;   //Stop Flag
bool flag_send = false;   // Flag to send data

bool old_control = false;

bool CW_2R_5 = false; //clockwise 2 rotation pin 5
bool CCW_2R_0 = false;

//flags for left up, left down etc.
bool left_U = false;
bool left_D = false;
bool right_U = false;
bool right_D = false;

// flag for up and down
bool flag_D = false;
bool flag_U = false;






#endif
