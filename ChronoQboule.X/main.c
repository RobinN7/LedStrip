/*
 * File:   main.c
 * Author: 7Robot
 *
 * Created on 15 septembre 2014, 20:06
 */

//configuration

#pragma config XINST = OFF, SOSCSEL = DIG
#pragma config FCMEN=ON, IESO=ON , WDTEN = OFF , EBTRB = OFF

#include <stdio.h>
#include <stdlib.h>
#include <p18f25k80.h>
#include <xc.h>
#include <timers.h>
#include <portb.h>
#include "converter.h"

////////////////////////////////////DEFINES/////////////////////////////////    
                                                                            
#define Afficheur1 PORTAbits.RA1
                                          

////////////////////////////////VARIABLES GLOBALES//////////////////////////

unsigned int time;

char balayage = 0;
char front_montant = 0;
char old_state = 0;
char stance = 1;
char valeur = 0;

void rafraichissement(int);
void initialisation(void);


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
}

int main(int argc, char** argv) {

    initialisation();

    while (1) {

        if ((PORTCbits.RC0 == 1) && (old_state == 0)) {
            old_state = 1;

            if (stance != 1) {
                stance++;
            }
            for (int delay = 0; delay < 10000; delay++) {
            }

        }

        if ((PORTCbits.RC0 == 0) && (old_state == 1)) {
            old_state = 0;

            if (stance == 1) {
                stance++;
            }
            for (int delay = 0; delay < 10000; delay++) {
            }

        }


        switch (stance) {
            case 1: //affichage d'accueil en attente d'un appui
            {
                TMR0ON = 0;
                segments(0);
                for (int razdisplay = 0; razdisplay < 5; razdisplay++) {
                    rafraichissement(razdisplay);
                }
            }
                break; 
            case 2: //départ du chrono
            {
                TMR0ON = 1; //Now start the timer!
            }
                break; 
            case 3: //arret du chrono
            {
                TMR0ON = 0;
                time = 0;
                for (long int delay = 0; delay < 120000; delay++) { }
                stance = 0;
            }
                break; 
            default:;
        }


    }

    return (EXIT_SUCCESS);
}

void rafraichissement(int afficheur) {

    switch (afficheur) {

        case 0:
            segments(time % 10);
            PORTA = 0b00000001;
            break;
        case 1:
            segments((time / 10) % 10);
            PORTA = 0b00000010;
            break;
        case 2:
            segments((time / 100) % 10);
            PORTA = 0b00000100;
            break;
        case 3:
            segments((time / 1000) % 10);
            PORTA = 0b00001000;
            break;
        case 4:
            segments((time / 10000) % 10);
            PORTA = 0b00100000;
            break;
        default:;
    }
}

//intcon tmr4IP IPRX

void initialisation(void) {
    //unsigned char config=0;
    ///////////////////////CONFIGURATION////////////////////////////////////////
    RCONbits.IPEN = 1;
    INTCON = 0b11111000;
    ADCON1 = 0x00;
    TRISA = 0; // met les ports A en sortie
    TRISB = 0b11111111; // met le port B en entrée (bouton sur RB0)
    TRISC = 0b00000001; // met le port C en sortie sauf RC0 (pin qui sert à rien)
    ///////////////////////INITIALISATION///////////////////////////////////////
    TMR0ON = 0;
    segments(0);

    for (int razdisplay = 0; razdisplay < 5; razdisplay++) {
        rafraichissement(razdisplay);
    }
    ////////////////////////////////////////////////////////////////////////////


    ///////////////////INTERRUPTION PORT B /////////////////////////////////////
    ANCON0 = 0;
    INTEDG0 = 1; //INTEDG0: External Interrupt 0 Edge Select bit
    //1 = Interrupt on rising edge
    //0 = Interrupt on falling edge
    //INTCON2=0b0





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

