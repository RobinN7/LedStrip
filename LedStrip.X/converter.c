#include "main.h"

// color = R V ou B, dutycycle entre 0 et 1023
void pwm(char color, int dutycycle) {
    switch(color){
        case 'B':
            CCPR2L=dutycycle/4;
            DC2B1=(dutycycle/2)%1;
            DC2B0=dutycycle%1;
            break;
        case 'R':
            CCPR3L=dutycycle/4;
            DC3B1=(dutycycle/2)%1;
            DC3B0=0;
            break;
        case 'G':
            CCPR4L=dutycycle/4;
            DC4B1=(dutycycle/2)%1;
            DC4B0=dutycycle%1;
            break;
    }

}
