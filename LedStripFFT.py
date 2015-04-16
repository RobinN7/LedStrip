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

import pyaudio
import sys
import struct
import math
import numpy
import matplotlib.pyplot as plt


chunk = 2048
FORMAT = pyaudio.paFloat32
#FORMAT = pyaudio.paInt16
CHANNELS = 1

RATE = 44100

p = pyaudio.PyAudio()

stream = p.open(format = FORMAT,
				channels = CHANNELS,
				rate = RATE,
				input = True,
				output = True,
				frames_per_buffer = chunk)





	

################################################
####			SERIAL CONNECTION			####
################################################

done=-1
for i in range(11):
	if done==-1 and i<10:
		try:
			ser = serial.Serial(
			port='/dev/ttyACM{0}'.format(i),
			baudrate=115200,
			)
			done=i

		except Exception:
			print("ttyACM{0} port erroné, test du suivant.".format(i))
	elif i==10:	

		exit=1

		ErrorScreen = Tk()
		ErrorScreen.title('Erreur')

		def Continue():
			global ErrorScreen
			global exit
			exit=0
			ErrorScreen.destroy()

		Label(ErrorScreen, text="Arduino non détecté").grid(row=0, column=0, columnspan=2, padx=25, pady=5)
		Button(ErrorScreen, text ='Quitter', height=2, command = ErrorScreen.quit).grid(row=1, column=0, pady=5)
		Button(ErrorScreen, text ='Continuer\nen local', height=2, command = Continue).grid(row=1, column=1, pady=5)
		ErrorScreen.mainloop() 

		if exit==1:
			raise SystemExit



	else:
		print("ttyACM{0} port connecté.".format(done))
		break


################################################
####		   GLOBAL VARIABLES				####
################################################

stopSendThread = 0
stopPulseThread = 0
stopPlotThread = 0
valR=10
valV=10
valB=10
fft=numpy.zeros(chunk)
fftAvOrder=100

################################################
####			  	Threads					####
################################################

def pulseThread():
	global data, chunk
	# Colors values
	global valR, valV, valB
	global stopPulseThread
	global fft
	while stopPulseThread==0:
		try:
			data = stream.read(chunk)
		except:
			continue 

		decodedata=numpy.fromstring(data, 'Float32');
		fft=numpy.fft.fft(decodedata)*(sensitivity.get()/50)

		low=int(abs((fft[2*chunk/1024]+fft[3*chunk/1024]+fft[4*chunk/1024])*10/3))
		
		# Saturation
		if low>255:
			low=255
	
		valR=low

		med=int(abs((fft[99*chunk/1024]+fft[100*chunk/1024]+fft[101*chunk/1024])*45)/3)

		# Saturation
		if med>255:
			med=255

		valV=med

		high=int(abs((fft[199*chunk/1024]+fft[200*chunk/1024]+fft[201*chunk/1024])*68)/3)

		# Saturation
		if high>255:
			high=255

		valB=high
		
def sendThread():
	# Colors values
	global valR, valV, valB
	global stopSendThread
	errorMessageFlag=0
	while stopSendThread==0:	

			string = "{0}f{1}f{2}f".format(\
									valR+256,\
									valV+512,\
									valB+768)
			try:
				ser.write(string.encode())
			except:
				if errorMessageFlag==0:
					stopSendThread = 1
					Thread(None, sendThread)._stop() 
					errorMessageFlag=1


def plotThread():
	# Colors values
	global fft
	global stopPlotThread

	while stopPlotThread==0:
		plt.clf()
		plt.plot(abs(fft[0:chunk/2]))
		plt.axis([0, chunk/2-1, 0, 1])
		plt.show(block=False)
		plt.pause(0.005)




if __name__ == "__main__":

	################################################
	####			  	TK WINDOW				####
	################################################

	Mafenetre = Tk()
	Mafenetre.title('Led Strip Controller')

	# Sensitivity bar
	sensitivity = Scale(Mafenetre,from_=0,to=100,resolution=1,\
	orient=HORIZONTAL,length=256,width=20, label="Sensitivity")
	sensitivity.set(50)
	sensitivity.grid(row=1, column=0, padx=6, pady=20)


	# Threads start
	Thread(None, sendThread).start()
	Thread(None, pulseThread).start()	
	Thread(None, plotThread).start()	

	# Tk window inifinite loop
	Mafenetre.mainloop() 

	# Exit everything on window closing
	stopSendThread = 1
	Thread(None, sendThread)._stop() 
	stopPulseThread = 1
	Thread(None, pulseThread)._stop() 
	stopPlotThread = 1
	Thread(None, plotThread)._stop() 
	try:
		ser.close()
	except:
		pass	
	sleep(0.2)
	plt.ioff()
	plt.close()

	stream.close()
	p.terminate()

	raise SystemExit