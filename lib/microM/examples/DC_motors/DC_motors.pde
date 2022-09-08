/*
This program demonstrates the use of the Micro Magician dual H bridge.
The DC motor control function can independantly control 2 small DC motors.

This program will gradually increase the speed of the 2 motors and then
engage the brake when they are at full speed.
The motors will then reverse direction and repeate the process.

Written by Russell Cameron
*/
#include <Arduino.h>
#include <microM.h>


void setup()
{ 
  
}


void loop()
{
  static int leftDir=2,rightDir=-2;                            // set rate of acceleration
  static int leftSpeed,rightSpeed;                             // motor control variables
  static byte leftBrake, rightBrake;

  leftSpeed+=leftDir;                                          // accelerate/deccelerate motor
  if(leftSpeed<-254 || leftSpeed>254) 
  {
    leftDir=-leftDir;                                          // if peak speed reached then reverse direction
    leftBrake=1;                                               // if peak speed reached then brake motor
  }
  if(leftSpeed==0) leftBrake=0;                                // release brake when motor speed = 0
  
  rightSpeed+=rightDir;                                        // accelerate/deccelerate motor
  if(rightSpeed<-254 || rightSpeed>254)                        
  {
    rightDir=-rightDir;                                        // if peak speed reached then reverse direction
    rightBrake=1;                                              // if peak speed reached then brake motor
  }
  if(rightSpeed==0) rightBrake=0;                              // release brake when motor speed = 0
  
  delay(10);                                                
  microM.Motors(leftSpeed,rightSpeed,leftBrake,rightBrake);    // update motor controller
}
