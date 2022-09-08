/*  This program demonstrates how easy it is to drive a stepper motor with the Micro Magician
    
    "stepdirection" can be any integer. 
    All positive values step the motor clockwise.
    All negative values step the motor anti-clockwise.
    A value of 0 will turn off power to the motor to prevent it overheating.
    
    "stepmode" is a byte.
    A value of 0 causes the motor to step in full steps.
    Any other value generates half steps.
    
    Written by Russell Cameron 
*/   




#include <microM.h>                          // Include the micro.M library

void setup()
{ 
  
}

void loop()
{
  int stepdirection=0;                       // choose the direction of rotation
  byte stepmode=0;                           // choose half or full steps

  microM.Step(stepdirection,stepmode);       // step the motor
  delay(100);                                // do not step the motor too quickly or it will stall
}


