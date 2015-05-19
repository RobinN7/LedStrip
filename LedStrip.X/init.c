#include <stdio.h>
#include <stdlib.h>
#include <p18f25k80.h>
#include "init.h"

void initialisation(void) {

    //////////////////////////////// PWM ///////////////////////////////////////

    initPWM();

    //////////////////////////////// UART //////////////////////////////////////

    initComms();

    //////////////////////////////// ADC ///////////////////////////////////////

    initADC();

    ////////////////////////////// TIMER0 //////////////////////////////////////

    //initTimer0();

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
    PIE3bits.RC2IE   = 1;   // ENABLE USART receive interrupt
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

void initTimer0(void)
{
    //Setup Timer0
    T0PS0 = 0; //Prescaler is divide by 256
    T0PS1 = 1;
    T0PS2 = 0;
    PSA = 0; //Timer Clock Source is from Prescaler
    T0CS = 0; //Prescaler gets clock from FCPU
    T08BIT = 1; //8 BIT MODE
    TMR0IE = 1; //Enable TIMER0 Interrupt
    PEIE = 1; //Enable Peripheral Interrupt
    GIE = 1; //Enable INTs globally
}