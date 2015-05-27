#include "main.h"

extern float amplitude1;
extern float amplitude2;
extern float amplitude3;

extern long millis;

void setRGB(int r, int g, int b)
{
    pwm('R',r);
    pwm('G',g);
    pwm('B',b);
}

void strobe()
{
    if (millis%((int)(1000*(1 - amplitude2*0.96)))<(int)(1000*(1-amplitude2*0.96)*(amplitude3*0.94+0.06)))
        setRGB( (int)(1023*amplitude1),
                (int)(1023*amplitude1),
                (int)(1023*amplitude1));
    else
        setRGB(0,0,0);
}
