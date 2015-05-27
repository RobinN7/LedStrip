/* 
 * File:   main.h
 * Author: robin
 *
 * Created on April 18, 2015, 1:59 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <p18f25k80.h>
#include "init.h"
#include "interrupts.h"
#include "converter.h"
#include "functions.h"

#define SWITCH1 RC0
#define SWITCH2 RA7
#define SWITCH3 RA6

void writeStringToUART (const char* msg);
unsigned int readADC(char);
void readAllADC(void);
void changeModeDetect(void);

#endif	/* MAIN_H */

