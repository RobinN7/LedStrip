/* 
 * File:   main.h
 * Author: robin
 *
 * Created on April 18, 2015, 1:59 PM
 */

#ifndef MAIN_H
#define	MAIN_H

void initialisation(void);
void initComms(void);
void initPWM(void);
void initADC(void);

void writeStringToUART (const char* msg);
unsigned int readADC(char);

#endif	/* MAIN_H */

