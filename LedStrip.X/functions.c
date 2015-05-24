#include "main.h"

extern unsigned int amplitude1;
extern unsigned int amplitude2;
extern unsigned int amplitude3;

extern long millis;

void setRGB(int r, int g, int b)
{
    pwm('R',r);
    pwm('G',g);
    pwm('B',b);
}

void strobe()
{
    if (millis%((int)(400*(float)amplitude2/65520.))<(int)((400*(float)amplitude3/65520.)*(float)amplitude2/65520.))
        setRGB( (int)(1023*(float)amplitude1/65520.),
                (int)(1023*(float)amplitude1/65520.),
                (int)(1023*(float)amplitude1/65520.));
    else
        setRGB(0,0,0);
}
