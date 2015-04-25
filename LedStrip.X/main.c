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

//////////////////////////////// GLOBAL VARS ///////////////////////////////////

char compteur=0;
char buffer[16]="";
char buffer1[16]="";
char beginR=0;
char beginG=0;
char beginB=0;
int R=10;
int G=10;
int B=10;

/*
//Main Interrupt Service Routine (ISR)
void interrupt low_interrupt() {
    //Check if it is TMR0 Overflow ISR
    if (TMR0IE && TMR0IF) {
        //TMR0 Overflow ISR
        TMR0 = 14; ///offset

        if (balayage == 4) {
            balayage = 0;
        } else {
            balayage++; //Increment Over Flow Counter
        }
        rafraichissement(balayage);
        time++;
        //Clear Flag
        TMR0IF = 0;
    }
}*/

void interrupt high_priority high_isr(void) {
    RC2IF = 0;              // On baisse le FLAG


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
                pwm('R',atoi(buffer));
                beginR=0;
            if (beginG==1)  // Fin de trame vert
                pwm('G',atoi(buffer));
                beginG=0;
            if (beginB==1)  // Fin de trame bleu
                pwm('B',atoi(buffer));
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
    initialisation();
    char msg[32]="";

    /*
    sprintf(msg, "%d\n\r", i);
    writeStringToUART(msg);
    */

    // ATTENTION RX UART DESACTIVÉ

    while (1) {

        
        for(delay=0;delay<60000;delay++);



        sprintf(msg, "%u\n\r", readADC(1));
        writeStringToUART(msg);

        readADC(2);

        sprintf(msg, "%u\n\r", readADC(2));
        writeStringToUART(msg);

        readADC(3);

        sprintf(msg, "%u\n\r", readADC(3));
        writeStringToUART(msg);

        writeStringToUART("\n\r");
    }

    return (EXIT_SUCCESS);
}


void initialisation(void) {

    //////////////////////////////// PWM ///////////////////////////////////////
    
    initPWM();

    //////////////////////////////// UART //////////////////////////////////////

    initComms();

    //////////////////////////////// ADC ///////////////////////////////////////

    initADC();

}


void initPWM()
{
    // FOSC = 8 MHz
    // Set 7812.5 Hz PWM frequency
    // PWM Period = [(PR2) + 1] * 4 * T OSC * (TMR2 Prescale Value)
    PR2=0xFF;

    // Clear Duty Cycle value
    // High State lenght= (CCPRxL:CCPxCON<5:4>) * T OSC * (TMR2 Prescale Value)
    CCPR2L=0b00000000;
    CCPR3L=0b00000000;      //  8 HSB of 10 bit duty cycle
    CCPR4L=0b00000000;
    DC2B1=0;
    DC2B0=0;
    DC3B1=0;    //  DCxB<1:0> = 2 LSB of 10 bit duty cycle
    DC3B0=0;
    DC4B1=0;
    DC4B0=0;

    // DUTY CYCLE = (CCPRxL:CCPxCON<5:4>)/(4*[(PR2) + 1])
    //            = (CCPRxL:CCPxCON<5:4>)/1024

    // CCP<2:4> pins as outputs
    TRISC=TRISC & 0b00111011;

    // TMR2 Prescale Value = 1
    T2CKPS1=0;
    T2CKPS0=0;

    // Enable Timer2
    TMR2ON = 1;

    // CCP<2:4> in PWM mode
    CCP2M2=1;
    CCP2M3=1;
    CCP3M2=1;
    CCP3M3=1;
    CCP4M2=1;
    CCP4M3=1;
}


void initComms()
{
    TRISBbits.TRISB7 = 1;           //RX2 pin = input
    TRISBbits.TRISB6 = 0;           //TX2 pin = output

    /* Serial port initialization */
    TXSTA2bits.BRGH = 1;            //High speed Baudrate
    TXSTA2bits.SYNC = 0;            //Asynchronous
    SPBRGH2 = 0;
    SPBRG2 = 51;                    //((FCY/16)/BAUD) - 1; // set baud to 38400  FCY=32000000
    BAUDCON2 = 0x08;                //BRGH16 = 1



    TXSTA2bits.TXEN = 1;            //Enables transmitter
    RCSTA2bits.CREN = 1;            //Enables receiver
    RCSTA2bits.SPEN = 1;            //Enable UART

    // USART interrupts configuration
    RCONbits.IPEN   = 1;    // ENABLE interrupt priority
    INTCONbits.GIE  = 1;    // ENABLE interrupts
    INTCONbits.PEIE = 1;    // ENABLE peripheral interrupts.
    PIE3bits.RC2IE   = 0;   // ENABLE USART receive interrupt
    PIE3bits.TX2IE   = 0;   // DISABLE USART TX interrupt

    RC2IF = 0;              // Clear flag
}

void initADC(void)
{
    TRIGSEL1=0;             // Special trigger from ECCP1
    TRIGSEL0=0;             //

    VCFG1=0;                // VREF+ = AVDD
    VCFG0=0;                //
    VNCFG=0;                // VREF+ = AVSS

                            // CHSN<2:0> = 000 => Analog Neg Chann = AVSS

    ANSEL3=1;               //
    ANSEL2=1;               //  AN1, AN2, AN3 as analog inputs
    ANSEL1=1;               //
        
    ADON=1;                 // Turn ON ADC

    CHS0=1;                 // Init ADC channel on AN1
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
