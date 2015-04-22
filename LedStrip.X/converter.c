
#include <stdio.h>
#include <stdlib.h>
#include <p18f25k80.h>
#include "converter.h"

// color = R V ou B, dutycycle entre 0 et 1023
void pwm(char color, int dutycycle) {
    switch(color){
        case 'R':
            CCPR2L=dutycycle/4;
            DC2B1=(dutycycle/2)%1;
            DC2B0=dutycycle%1;
            break;
        case 'G':
            CCPR3L=dutycycle/4;
            DC3B1=(dutycycle/2)%1;
            DC3B0=0;
            break;
        case 'B':
            CCPR4L=dutycycle/4;
            DC4B1=(dutycycle/2)%1;
            DC4B0=dutycycle%1;
            break;
    }

}
