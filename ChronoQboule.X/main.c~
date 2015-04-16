/*
 * File:   main.c
 * Author: 7Robot
 *
 * ChronoQboule 2.0
 *
 * Created on 15 septembre 2014, 20:06
 */

//configuration

#pragma config XINST = OFF
//#pragma config FOSC = INTIO2



#include <stdio.h>
#include <stdlib.h>
#include <p18f25k80.h>
#include <xc.h>
#include "converter.h"
//#include <timers.h>
//#include <p18cxxx.h>

////////////////////////////////////DEFINE /////////////////////////////////

#define display1 PORTAbits_t.RA0
#define display2 PORTAbits_t.RA1
#define display3 PORTAbits_t.RA2
#define display4 PORTAbits_t.RA3
#define display5 PORTAbits_t.RA4


////////////////////////////////VARIABLES GLOBALES////////////////////////////////////////////

//////UNITES//////
char milliSec = 0;
char centiSec = 0;
char deciSec  = 0;
char Sec      = 0;
char decaSec  = 0;
//////////////////

char scanning = 0;


void refresh (int);




//Main Interrupt Service Routine (ISR)
void interrupt low_interrupt()
{
   //Check if it is TMR0 Overflow ISR
   if(TMR0IE && TMR0IF)
   {
      //TMR0 Overflow ISR
       if (scanning ==4)
       {
           scanning = 0;
       }
       else
       {
           scanning++;  //Increment Over Flow Counter
       }

       refresh(scanning);
      //Clear Flag
      TMR0IF=0;
   }
}



int main(int argc, char** argv) {

    ///////////////////////CONFIGURATION///////////////////////////////////////////
    INTCON  =  0b11100000;
    TRISB = 0; // mets les ports B en sortie
    TRISA = 0; // mets les ports A en sortie


       //Setup Timer0 la persistance retinienne
   T0PS0=1; //Prescaler is divide by 256
   T0PS1=1;
   T0PS2=1;
   PSA=0;      //Timer Clock Source is from Prescaler
   T0CS=0;     //Prescaler gets clock from FCPU (5MHz)
   T08BIT=1;   //8 BIT MODE
   TMR0IE=1;   //Enable TIMER0 Interrupt
   PEIE=1;     //Enable Peripheral Interrupt
   GIE=1;      //Enable INTs globally
   TMR0ON=1;      //Now start the timer!

       //setup Timer2

    while (1) {
    }

    return (EXIT_SUCCESS);
}


void refresh (int display)
{
    switch(display)
    {
        case 0 :    PORTA =0b00000001; break;
        case 1 :    PORTA =0b00000010; break;
        case 2 :    PORTA =0b00000100; break;
        case 3 :    PORTA =0b00001000; break;
        case 4 :    PORTA =0b00010000; break;
        default : ;
    }
}

//intcon tmr4IP IPRX