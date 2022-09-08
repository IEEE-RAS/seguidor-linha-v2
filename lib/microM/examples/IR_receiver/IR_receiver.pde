/*
 This program demonstrates the use of the Micro Magician control board IR decoding function.
 This function decodes Sony IR Codes (SIRC) and returns the 7bit command as a byte. 
 
 Included in the library are instructions for configuring the universal remote sold by DAGU.
 
 Written by Russell Cameron
 */

#include <Arduino.h>
#include <microM.h>


void setup()
{ 
  microM.Setup();                                // this must be called to initialize timer 2
  //microM.irpin=4;                              // only required for external IR receivers. Defaults to 4 for onboard IR receiver

  Serial.begin(9600);                            // initialize serial monitor
  Serial.println("Ready to receive IR commands");
}


void loop()
{
  if(microM.ircommand>0)                         // display command from IR receiver
  {
    Serial.print("\tIR command:");
    Serial.println(microM.ircommand,DEC);        // use the DEC option to convert a byte to a decimal number
    microM.ircommand=0;                          // prevents display repeating the same values
  } 
}




/*
=================================================== Available Global Variables =====================================================
 
 microM.irpin
 A byte value specifying the digital pin used by IR receiver (default value is 4).
 Only required if using an external IR receiver.
 
 */








