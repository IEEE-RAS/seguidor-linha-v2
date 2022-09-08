/******************************************
 *                                        *
 *              microM.cpp                *
 *                                        *
 *  a library to simplify the use of the  *
 *     DAGU Micro Magician Controller     *
 *                                        *
 *              written by                *
 *            Russell Cameron             *
 *                                        *
 *       with advice and help from        *
 *              LMR members               *
 *                                        *
 *                 Cr0c0                  *
 *                 Maus                   *
 *                 Mogul                  *
 *               RobotFreak               *
 *                                        *
 ******************************************/

#include "microM.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// Preinstantiate the object
MicroM microM = MicroM();

// class definitions follow

MicroM::MicroM()
{
    ircommand = 0;
    magnitude = 0;
    xaxis = yaxis = zaxis = 0;
}

/*
    TCNT2 values:
 
   7 = 1KHz    1000uS
 132 = 2KHz     500uS
 157 = 2.5KHz   400uS
 174 = 3KHz     333uS
 195 = 4KHz     250uS
 207 = 5KHz     200uS - slower sample rates will disable SIRC decoder
 230 = 8KHz     125uS  
 232 =10KHz     100uS
 */

//------------------------------------------------------------- Setup --------------------------------------------------------

void MicroM::Setup()
{
  cli();                                                     // disable all interrupts
  TIMSK2 &= ~(1<<TOIE2);                                     // disable the timer overflow interrupt
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));                      // configure timer2 in normal count mode
  TCCR2B &= ~(1<<WGM22);  
  ASSR   &= ~(1<<AS2);                                       // select clock source: internal I/O clock
  TIMSK2 &= ~(1<<OCIE2A);                                    // disable Compare Match A interrupt enable (only want overflow)

  // configure the prescaler to CPU clock divided by 32
  TCCR2B |= (1<<CS21)  | (1<<CS20);                          // set bits  
  TCCR2B &= ~(1<<CS22);                                      // clear bit   
  TCNT2 = 207;                                               // set frequency to 5KHz (sample every 200uS)
  TIMSK2 |= (1<<TOIE2);                                      // disable the timer overflow interrupt
  sei();                                                     // enable all interrupts
  
  sensitivity=30;                                            // default values
  devibrate=100;
  irpin=4;
}



ISR(TIMER2_OVF_vect)                                         // timer overflow service routine
{                                                    
  TCNT2 = 207;                                               // reload timer for 200uS intervals
  microM.tiflag++;                                           // increment tiflag used to call Impact function
  

  //----------------------------------------------------------- Sony IRC Decoder ------------------------------------------------------

  if(digitalRead(microM.irpin)==0)                           // check state of IR receiver output 
  {
    microM.irflag++;                                         // increment irflag used to measure ir pulse widths
    microM.newbit=1;                                         // recognizes a new bit being measured
  }
  else                                                       // IR receiver output high - end of bit
  {
    if(microM.newbit==1)				     // low->high transition (end of bit)
    {
      if(microM.irflag>4)                                    // bitwidth>4 = logic "1"
      {
        microM.irdata+=microM.irbitvalue;                    // increment data by bit value
      }
      microM.irbitvalue*=2;                                  // bitvalue updated for next bit (1,2,4,8,16,32,64,128)
      microM.irbitcount++;                                   // count the bits

      if(microM.irbitcount==7)                               // only want first 7 bits
      {
        microM.irbitvalue=0;                                 // additional bits have a value of 0
        microM.ircommand=microM.irdata+1;                    // make data available to user
      }

      if(microM.irflag>8)                                    // SIRC start bit resets decoder values
      {
        microM.irbitvalue=1;
        microM.irbitcount=0;
        microM.irdata=0;
      }

      microM.newbit=0;                                       // addional interrupts not new bit
    }
    microM.irflag=0;                                         // reset irflag when IR pulse ends  
  }
}


//------------------------------------------------------------- Impact Detection -----------------------------------------------------

void MicroM::Impact()          
{
  if(tiflag<10) return;					      // Interrupt multiplier - only check accelerometer every 2mS
  tiflag=0;

  int oldx=xaxis;                                             // local variables store previous axis readings for comparison
  int oldy=yaxis;
  int oldz=zaxis;

  vibration--;                                                // loop counter prevents false triggering 
  if(vibration<0) vibration=0;                                // as robot vibrates due to impact
  
  xaxis=analogRead(0);                                        // new axis readings are taken - very time consuming
  yaxis=analogRead(1);                                        // each analogRead takes aproximately 260uS
  zaxis=analogRead(2);
  if(vibration>0) return;                                     // until vibration has subsided no further calculations required

  deltx=xaxis-oldx;                                           // delta between old and new axis readings
  delty=yaxis-oldy;
  deltz=zaxis-oldz;
  magnitude=sqrt(sq(deltx)+sq(delty)+sq(deltz));              // magnitude of delta x,y,z using pythagorus

  if (magnitude>sensitivity)                                  // has a new impact occured
  {
    vibration=devibrate;                                      // reset anti-vibration counter
    return;
  }
  else
  {
    magnitude=0;                                              // no impact detected
  }
}


//-------------------------------------------------------------- Motor Control ------------------------------------------------------

void MicroM::Motors(int leftSpeed, int rightSpeed, byte leftBrake, byte rightBrake)

{
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
	
  if(leftBrake!=0)
  {
    digitalWrite(5,1);
    digitalWrite(7,1);
  }
  else
  {
    if(leftSpeed==0)
    {
      digitalWrite(5,0);
      digitalWrite(7,0);
    }
  
    if(leftSpeed>0)
    {
      analogWrite(5,leftSpeed);
      digitalWrite(7,0);
    }

    if(leftSpeed<0)
    {
      leftSpeed+=255;
      analogWrite(5,leftSpeed);
      digitalWrite(7,1);
    }
  }

  if(rightBrake!=0)
  {
    digitalWrite(6,1);
    digitalWrite(8,1);
  }
  else    
  {
    if(rightSpeed==0)
    {
      digitalWrite(6,0);
      digitalWrite(8,0);
    }
  
    if(rightSpeed>0)
    {
      analogWrite(6,rightSpeed);
      digitalWrite(8,0);
    }

    if(rightSpeed<0)
    {
      rightSpeed+=255;
      analogWrite(6,rightSpeed);
      digitalWrite(8,1);
    }
  }
}



//-------------------------------------------------------------- Stepper Motor Control ----------------------------------------------

void MicroM::Step(int stepdirection, byte stepmode)

{
  static byte stepstep=2;
  pinMode(5,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(8,OUTPUT);

  if(stepdirection==0)
  {
    digitalWrite(5,0);
    digitalWrite(7,0);
    digitalWrite(6,0);
    digitalWrite(8,0);
    return;
  }

  if(stepdirection>0)
  {
    stepstep=stepstep+1+(stepmode==0);
    if(stepstep>9) stepstep=2;
  }

  if(stepdirection<0)
  {
    stepstep=stepstep-1-(stepmode==0);
    if(stepstep<2) stepstep=9;
  }

  digitalWrite(5,(stepstep==2 || stepstep>7));
  digitalWrite(7,(stepstep>3  && stepstep<7));
  digitalWrite(6,(stepstep<5));
  digitalWrite(8,(stepstep>5  && stepstep<9));
}


/* 
   Step    |2|3|4|5|6|7|8|9
   --------+-+-+-+-+-+-+-+-
   Left  A |1|0|0|0|0|0|1|1
         B |0|0|1|1|1|0|0|0
   --------+-+-+-+-+-+-+-+-
   Right A |1|1|1|0|0|0|0|0
         B |0|0|0|0|1|1|1|0
*/

