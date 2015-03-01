#!/usr/bin/env python 
# -*- coding: utf8 -*-

# Led Strip Control
#
#
# Author: Robin 

# Serial communication library
import serial
# Time manager
from time import sleep
# Thread manager
from threading import Thread 
# Random integers
from random import randint

# GUI library
try:
	# for Python2
	from Tkinter import *
except ImportError:
	# for Python3
	from tkinter import *
	
################################################
####			SERIAL CONNECTION			####
################################################
try:
	ser = serial.Serial(
    port='/dev/ttyACM0',
    baudrate=115200,
    #parity=serial.PARITY_ODD,
    #stopbits=serial.STOPBITS_TWO,
    #bytesize=serial.SEVENBITS
)
except Exception:	
	ErrorScreen = Tk()
	ErrorScreen.title('Erreur')
	Label(ErrorScreen, text="Arduino non détecté").grid(padx=30, pady=10)
	ErrorScreen.mainloop() 
	raise SystemExit

################################################
####		   GLOBAL VARIABLES				####
################################################

stopThread = 0

################################################
####			  MAIN THREAD				####
################################################

def mainThread():
	valR=10
	valV=10
	valB=10
	signR=1
	signV=1
	signB=1
	global stopThread

	while stopThread==0:

		valR=scaleR.get()
		valV=scaleV.get()
		valB=scaleB.get()

		string = "{0}f{1}f{2}f".format(valR+256,valV+512,valB+768)
		ser.write(string.encode())
		sleep(0.05)

################################################
####			  	TK WINDOW				####
################################################

Mafenetre = Tk()
Mafenetre.title('Led Strip Controller')


scaleR = Scale(Mafenetre,from_=255,to=0,resolution=1,orient=VERTICAL,\
length=256,width=20, label="R")
scaleR.grid(row=0, column=0, padx=6)

scaleV = Scale(Mafenetre,from_=255,to=0,resolution=1,orient=VERTICAL,\
length=256,width=20, label="V")
scaleV.grid(row=0, column=1, padx=6)

scaleB = Scale(Mafenetre,from_=255,to=0,resolution=1,orient=VERTICAL,\
length=256,width=20, label="B")
scaleB.grid(row=0, column=2, padx=6)

"""
Label(Mafenetre, text="Rouge =").grid(row=0)
valR_field= Entry(Mafenetre, width=9)
valR_field.grid(row=0, column=1, pady=5)

Label(Mafenetre, text="Vert =").grid(row=1)
valV_field= Entry(Mafenetre, width=9)
valV_field.grid(row=1, column=1, pady=5)

Label(Mafenetre, text="Bleu =").grid(row=2)
valB_field= Entry(Mafenetre, width=9)
valB_field.grid(row=2, column=1, pady=5)
"""

# Thread start
Thread(None, mainThread).start()

# Tk window inifinite loop
Mafenetre.mainloop() 

# Exit everything on window closing
stopThread = 1
Thread(None, mainThread)._stop() 
raise SystemExit

