#!/usr/bin/env python 
# -*- coding: utf8 -*-

# Led Strip Control
#
# Color wheel : Red Yellow Green Cyan Blue Magenta Red
#
# Author: Robin 

# Serial communication library
import serial
# Time manager
from time import *
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

done=-1
for i in range(11):
	if done==-1 and i<10:
		try:
			ser = serial.Serial(
			port='/dev/ttyUSB{0}'.format(i),
			baudrate=38400,
			)
			done=i

		except Exception:
			print("ttyACM{0} port erroné, test du suivant.".format(i))
	elif i==10:	
		ErrorScreen = Tk()
		ErrorScreen.title('Erreur')
		Label(ErrorScreen, text="Arduino non détecté").grid(padx=30, pady=10)
		ErrorScreen.mainloop() 
		raise SystemExit
	else:
		print("ttyACM{0} port connecté.".format(done))
		break


################################################
####		   GLOBAL VARIABLES				####
################################################

stopWheelThread = 0
stopSendThread = 0
valR=10
valV=10
valB=10

################################################
####			  	Threads					####
################################################

def wheelThread():
	# Beat in seconds
	global beat
	global lastTime
	# Colors values
	global valR, valV, valB
	global stopWheelThread

	while stopWheelThread==0:
		if time()-lastTime>beat/2048:
			lastTime=time()
			###########################
			# Color wheel acquisition #
			###########################

			# R -> Y
			if colorWheelValue.get()<1024:
				valR=1023
				valV=colorWheelValue.get()
				valB=0
			# Y -> G
			elif colorWheelValue.get()<2048:	
				valR=2047-colorWheelValue.get()
				valV=1023
				valB=0
			# G -> C
			elif colorWheelValue.get()<3072:
				valR=0
				valV=1023
				valB=colorWheelValue.get()-2048
			# C -> B
			elif colorWheelValue.get()<4096:
				valR=0
				valV=4095-colorWheelValue.get()
				valB=1023
			# B -> M
			elif colorWheelValue.get()<5120:
				valR=colorWheelValue.get()-4096
				valV=0
				valB=1023
			# M -> R
			else:
				valR=1023
				valV=0
				valB=6143-colorWheelValue.get()

			# Color bars acquisition
			#elif mode==colorBars:
			#valR=scaleR.get()
			#valV=scaleV.get()
			#valB=scaleB.get()
			
			if StopScale.get()==0:
				if colorWheelValue.get()<6143:
					colorWheelValue.set(colorWheelValue.get()+1)
				else:
					colorWheelValue.set(0)

def sendThread():
	# Colors values
	global valR, valV, valB
	global stopSendThread

	while stopSendThread==0:	

			string = "R{0}G{1}B{2}".format(valR,valV,valB)
			"""
									valR//(dimming.get()*dimming.get()/39+1),\
									valV//(dimming.get()*dimming.get()/39+1),\
									valB//(dimming.get()*dimming.get()/39+1))
			"""
			print(string)
			ser.write(string.encode())

def Key(event):
	#############
	# Beat keys #
	#############

	if (event.keysym=='k'):
		global lastBeat
		global beat
		beat=time()-lastBeat
		lastBeat=time()
		colorWheelValue.set(256)
	if (event.keysym=='l'):
		global lastBeat
		global beat
		beat=time()-lastBeat
		lastBeat=time()
		colorWheelValue.set(768)
	if (event.keysym=='m'):
		global lastBeat
		global beat
		beat=time()-lastBeat
		lastBeat=time()
		colorWheelValue.set(1280)


if __name__ == "__main__":


	################################################
	####			  	TK WINDOW				####
	################################################

	Mafenetre = Tk()
	Mafenetre.title('Led Strip Controller')
	# Time initialisation
	lastTime=time()
	# Beat initialisation
	lastBeat=time()
	beat=5
	# Bind keyboard to the window
	Mafenetre.bind('<Key>', Key)

	# Pause
	StopScale = Scale(Mafenetre,from_=0,to=1,resolution=1,\
	orient=HORIZONTAL,length=60,width=30, label="Pause")
	StopScale.set(1)
	StopScale.grid(row=0, column=0, padx=6, pady=20)

	# Dimming bar
	dimming = Scale(Mafenetre,from_=0,to=100,resolution=1,\
	orient=HORIZONTAL,length=512,width=20, label="Dimming")
	dimming.grid(row=1, column=0, padx=6, pady=20)

	# Color wheel bar
	colorWheelValue = Scale(Mafenetre,from_=0,to=6143,resolution=1,\
	orient=HORIZONTAL,length=512,width=30, label="Color Wheel")
	colorWheelValue.grid(row=2, column=0, padx=6, pady=20)

	"""
	# Individual colors bars
	scaleR = Scale(Mafenetre,from_=255,to=0,resolution=1,orient=VERTICAL,\
	length=256,width=30, label="R")
	scaleR.grid(row=0, column=0, padx=6)
	scaleV = Scale(Mafenetre,from_=255,to=0,resolution=1,orient=VERTICAL,\
	length=256,width=30, label="V")
	scaleV.grid(row=0, column=1, padx=6)
	scaleB = Scale(Mafenetre,from_=255,to=0,resolution=1,orient=VERTICAL,\
	length=256,width=30, label="B")
	scaleB.grid(row=0, column=2, padx=6)

	# Individual colors fields
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

	# Threads start
	Thread(None, wheelThread).start()
	Thread(None, sendThread).start()	

	# Tk window inifinite loop
	Mafenetre.mainloop() 

	# Exit everything on window closing
	stopWheelThread = 1
	Thread(None, wheelThread)._stop() 
	stopSendThread = 1
	Thread(None, sendThread)._stop() 
	ser.close()	

	raise SystemExit

