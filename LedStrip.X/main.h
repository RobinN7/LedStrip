/* 
 * File:   main.h
 * Author: robin
 *
 * Created on April 18, 2015, 1:59 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#define SWITCH1 RC0
#define SWITCH2 RA7
#define SWITCH3 RA6

void writeStringToUART (const char* msg);
unsigned int readADC(char);

#endif	/* MAIN_H */

