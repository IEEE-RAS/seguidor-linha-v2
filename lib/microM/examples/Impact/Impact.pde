/*
This program demonstrates the use of the Micro Magician Impact function

Tapping the controller with your finger should display impact readings.
The X,Y and Z deltas will be positive or negative depending on direction of the impact.

The output of these functions are available as global variables to be used
anywhere in your program. Some variables are also available so that the code
can be adjusted to suit your robots physical design.

Written by Russell Cameron
*/


#include <microM.h>

void setup()
{ 
  microM.Setup();                        // this must be called first in your setup function
  microM.sensitivity=50;                 // if your robot vibrates or moves over a rough surface then increase this value to prevent false triggering
  microM.devibrate=50;                   // depends on the construction of your robot. Increase the value if you get additional readings after initial impact
  
  Serial.begin(9600);                    // initialize serial monitor
  Serial.println("Ready");
}


void loop()
{
  microM.Impact();                       // function must be called at least once every 2mS to work accurately
  if(microM.magnitude>0)                 // display results of impact
  {
    Serial.println(""); 
    Serial.print("\tMagnitude:");
    Serial.print(microM.magnitude);
    Serial.print("\tDelta X:");
    Serial.print(microM.deltx);
    Serial.print("\tDelta Y:");
    Serial.print(microM.delty);
    Serial.print("\tDelta Z:");
    Serial.println(microM.deltz);
    Serial.println(""); 
    microM.magnitude=0;                  // prevents display repeating the same values
  }
  
  /*                                   
  Serial.print("\tX axis:");             // realtime X,Y,Z axis readings       
  Serial.print(microM.xaxis);
  Serial.print("\tX axis:");
  Serial.print(microM.yaxis);
  Serial.print("\tX axis:");
  Serial.println(microM.zaxis);
  */ 
}

/*
============================== Available Global Variables ==================================

microM.magnitude                                                                              
An integer that is the magnitude vector of an impact.
Returns 0 when impacts are below the sensitivity threshold.

microM.deltx
An integer equal to the initial acceleration measured during impact on the X axis.            
Use this value to determine direction of impact. 

microM.xaxis
An integer that is the realtime reading of the accelerometer xaxis.
You can also read directly from analog input A0 but the program will run slower.

microM.delty
An integer equal to the initial acceleration measured during impact on the Y axis.
Use this value to determine direction of impact.

microM.yaxis
An integer that is the realtime reading of the accelerometer yaxis.
You can also read directly from analog input A1 but the program will run slower.

microM.deltz
An integer equal to the initial acceleration measured during impact on the Z axis.
Use this value to determine direction of impact.

microM.zaxis
An integer that is the realtime reading of the accelerometer zaxis.
You can also read directly from analog input A2 but the program will run slower.

microM.sensitivity
An interger specifying minimum impact value (default value 30).
Use this to differentiate between normal vibration and impact.

microM.devibrate
An integer specifying the number of readings to be ignored after the initial impact (default value 100).
Any object will vibrate after an impact. This can cause false triggering.

*/







