#include "main.h"

////////////////////////////// TIMER0 INTERRUPT ////////////////////////////////

long millis=0;

void interrupt low_priority Timer0_ISR(void)
{


    //static unsigned char millis=0;
    if (T0IE && T0IF)
    {
        T0IF=0;     //TMR0 interrupt flag must be cleared in software
                    //to allow subsequent interrupts
        TMR0=15;    // Offset de 15 pour avoir des ticks toutes les millisecondes

        millis++;   //increment the counter variable by 1
    }

}

////////////////////////////// USART2 INTERRUPT ////////////////////////////////

void interrupt high_priority RX2_ISR(void) {
    RC2IF = 0;
    // On baisse le FLAG
    extern unsigned int R[100];
    extern unsigned int G[100];
    extern unsigned int B[100];

    static char compteur=0;
    static char buffer[16]="";
    static char beginR=0;
    static char beginG=0;
    static char beginB=0;

    char input[2] = "";
    input[0] = RCREG2;      // Lecture UART

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