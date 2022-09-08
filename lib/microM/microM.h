/******************************************
 *                                        *
 *               microM.h                 *
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


#ifndef microM_h
#define microM_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <inttypes.h>

class MicroM
{
private:
    int vibration;
    int oldx, oldy, oldz;
    
    int leftSpeed,rightSpeed;
    byte leftBrake,rightBrake;
    
    int stepdirection;
    byte stepmode;
    

       

public:
    volatile int tiflag,irflag;
    volatile byte ircommand;
    volatile byte irbitvalue,irbitcount,irdata,newbit;
    int devibrate,sensitivity,magnitude,deltx,delty,deltz,xaxis,yaxis,zaxis;
    byte irpin;

    MicroM();
    void Setup();
    void Impact(); 
    void Motors(int leftSpeed, int rightSpeed, byte leftBrake, byte rightBrake);
    void Step(int stepdirection,byte stepmode);
	
};

extern MicroM microM;

#endif