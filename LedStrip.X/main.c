/*
 * File:   main.c
 * Author: Robin
 *
 * Created on 15 april 2015, 20:06
 */

//****************************************************************************************
//                    CONFIGURATION BITS PIC18F25K80
//****************************************************************************************

// Configuration register

// CONFIG1L
#pragma config RETEN = OFF      // VREG Sleep Enable bit (Ultra low-power regulator is Disabled (Controlled by REGSLP bit))
#pragma config INTOSCSEL = HIGH // LF-INTOSC Low-power Enable bit (LF-INTOSC in High-power mode during Sleep)
#pragma config SOSCSEL = DIG    // SOSC Power Selection and mode Configuration bits (Digital (SCLKI) mode)
#pragma config XINST = OFF      // Extended Instruction Set (Disabled)

// CONFIG1H
#pragma config FOSC = INTIO2    // Oscillator (Internal RC oscillator)
#pragma config PLLCFG = OFF     // PLL x4 Enable bit (Disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor (Disabled) (!)
#pragma config IESO = OFF       // Internal External Oscillator Switch Over Mode (Disabled) (!)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power Up Timer (Disabled)
#pragma config BOREN = SBORDIS  // Brown Out Detect (Enabled in hardware, SBOREN disabled)
#pragma config BORV = 3         // Brown-out Reset Voltage bits (1.8V)
#pragma config BORPWR = ZPBORMV // BORMV Power level (ZPBORMV instead of BORMV is selected)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer (WDT disabled in hardware; SWDTEN bit disabled)
#pragma config WDTPS = 1048576  // Watchdog Postscaler (1:1048576)

// CONFIG3H
#pragma config CANMX = PORTB    // ECAN Mux bit (ECAN TX and RX pins are located on RB2 and RB3, respectively)
#pragma config MSSPMSK = MSK7   // MSSP address masking (7 Bit address masking mode)
#pragma config MCLRE = OFF      // Master Clear Enable (MCLR Disabled, RG5 Enabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Overflow Reset (Enabled)
#pragma config BBSIZ = BB2K     // Boot Block Size (2K word Boot Block size)

// CONFIG5L
#pragma config CP0 = OFF        // Code Protect 00800-01FFF (Disabled)
#pragma config CP1 = OFF        // Code Protect 02000-03FFF (Disabled)
#pragma config CP2 = OFF        // Code Protect 04000-05FFF (Disabled)
#pragma config CP3 = OFF        // Code Protect 06000-07FFF (Disabled)

// CONFIG5H
#pragma config CPB = OFF        // Code Protect Boot (Disabled)
#pragma config CPD = OFF        // Data EE Read Protect (Disabled)

// CONFIG6L
#pragma config WRT0 = OFF       // Table Write Protect 00800-03FFF (Disabled)
#pragma config WRT1 = OFF       // Table Write Protect 04000-07FFF (Disabled)
#pragma config WRT2 = OFF       // Table Write Protect 08000-0BFFF (Disabled)
#pragma config WRT3 = OFF       // Table Write Protect 0C000-0FFFF (Disabled)

// CONFIG6H
#pragma config WRTC = OFF       // Config. Write Protect (Disabled)
#pragma config WRTB = OFF       // Table Write Protect Boot (Disabled)
#pragma config WRTD = OFF       // Data EE Write Protect (Disabled)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protect 00800-03FFF (Disabled)
#pragma config EBTR1 = OFF      // Table Read Protect 04000-07FFF (Disabled)
#pragma config EBTR2 = OFF      // Table Read Protect 08000-0BFFF (Disabled)
#pragma config EBTR3 = OFF      // Table Read Protect 0C000-0FFFF (Disabled)

// CONFIG7H
#pragma config EBTRB = OFF      // Table Read Protect Boot (Disabled)


///////////////////////////////// INCLUDES /////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <p18f25k80.h>
#include <xc.h>
#include <timers.h>
#include <portb.h>
#include "main.h"
#include "converter.h"
#include "init.h"

//////////////////////////////// GLOBAL VARS ///////////////////////////////////

char beginR=0;
char beginG=0;
char beginB=0;
unsigned int R[100]=0;
unsigned int G[100]=0;
unsigned int B[100]=0;

unsigned int millis=0;
////////////////////////////// TIMER0 INTERRUPT ////////////////////////////////

void interrupt low_priority Timer0_ISR(void)
{
    //static unsigned char millis=0;
    if (T0IE && T0IF)
    {
        T0IF=0; //TMR0 interrupt flag must be cleared in software
                //to allow subsequent interrupts
        TMR0=15; // Offset pour avoir des ticks toutes les millisecondes

        millis++; //increment the counter variable by 1
        if (millis==1000)
        {
            millis=0;
            pwm('R',1023);
            pwm('G',1023);
            pwm('B',1023);
            pwm('R',0);
            pwm('G',0);
            pwm('B',0);
        }
    }

}

void interrupt high_priority RX2_ISR(void) {
    RC2IF = 0;              // On baisse le FLAG

    static char compteur=0;
    static char buffer[16]="";

    char input[2] = "";
    input[0] = RCREG2;    // Lecture UART

    if (beginR==1 | beginG==1 | beginB==1)  // Trame lancee precedemment
    {
        if (input[0]!='R' & input[0]!='G' & input[0]!='B')  // Trame en cours
        {
            buffer[compteur]=input[0];
            compteur+=1;
        }

        else                                                // Fin d'une trame
        {
            if (beginR==1)  // Fin de trame rouge
                R[0]=atoi(buffer);
                beginR=0;
            if (beginG==1)  // Fin de trame vert
                G[0]=atoi(buffer);
                beginG=0;
            if (beginB==1)  // Fin de trame bleu
                B[0]=atoi(buffer);
                beginB=0;

            // RAZ compteur et buffer
            compteur=0;
            buffer[3]='\0';
            buffer[2]='\0';
            buffer[1]='\0';
            buffer[0]='\0';
        }
    }

    if (input[0]=='R')  // Debut trame rouge
        beginR=1;
    if (input[0]=='G')  // Debut trame vert
        beginG=1;
    if (input[0]=='B')  // Debut trame bleu
        beginB=1;
}


int main(int argc, char** argv) {

    long int delay=0;
    for(delay=0;delay<100000;delay++);
    initialisation();
    char msg[15]="";
 
    char mode=0;
    char old_SWITCH1=0;
    
    //sprintf(msg, "%d\n\r", i);
    //writeStringToUART(msg);
   

    unsigned int amplitude1=0;
    unsigned int amplitude2=0;
    unsigned int amplitude3=0;

    while (1) {
        for(delay=0;delay<100;delay++);
        
        amplitude1=readADC(1);
        amplitude2=readADC(2);
        amplitude3=readADC(3);

        // Detection de front montant sur le bouton du premier potentiometre
        if (SWITCH1==0 && old_SWITCH1==1) 
        {
            mode=!mode;
        }
        old_SWITCH1=SWITCH1;

        if (mode==1)
        {
            pwm('R',(int)( (float)R[0] * (float)amplitude1/65520.));
            pwm('G',(int)( (float)G[0] * (float)amplitude2/65520.));
            pwm('B',(int)( (float)B[0] * (float)amplitude3/65520.));
        }
        else
        {
            //pwm('R',(int)( 1023 * (float)amplitude1/65520.));
            //pwm('G',(int)( 1023 * (float)amplitude2/65520.));
            //pwm('B',(int)( 1023 * (float)amplitude3/65520.));
        }
    }

    return (EXIT_SUCCESS);
}




void writeStringToUART (const char *msg)
{
    while(*msg)    {
        while(PIR3bits.TX2IF == 0) {}
        TXREG2 = *msg++;
    }
}

unsigned int readADC(char channel)
{
    char tempo=0;
    switch (channel)
    {
        case 1:
        {
            CHS1=0;
            CHS0=1;
            break;
        }
        case 2:
        {
            CHS1=1;
            CHS0=0;
            break;
        }
        case 3:
        {
            CHS1=1;
            CHS0=1;
            break;
        }
    }
    for(tempo=0;tempo<100;tempo++);  // Capa charging
    GO_nDONE = 1; //Initializes A/D Conversion
    while(GO_nDONE); //Wait for A/D Conversion to complete

    return ((ADRESH<<8)+ADRESL);
}
