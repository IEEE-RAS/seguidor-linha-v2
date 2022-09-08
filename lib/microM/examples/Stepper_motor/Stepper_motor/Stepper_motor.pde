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
  int stepdirection=1;                       // choose the direction of rotation
  byte stepmode=0;                           // choose half or full steps

  microM.Step(stepdirection,stepmode);       // step the motor to next position
  
  /*                                         // if your motor is not under load then add these lines to save power.
  delay(1);                                  // wait for the motor to move to new position
  microM.Step(0,0);                          // turn off power to the motor 
  */
  delay(20);                                 // do not step the motor too quickly or it will stall
}


