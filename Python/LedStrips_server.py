import socket
import sys
import serial

HOST = ''		# Symbolic name, meaning all available interfaces
PORT = 22134	# Arbitrary non-privileged port

try:
	ftdi_file = "/dev/ttyUSB0"
	ftdi = serial.Serial(ftdi_file, 38400)
	print ('FTDI detected')
except Exception:
	print ('FTDI not detected')

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
print ('Socket created')



#Bind socket to local host and port
try:
	s.bind((HOST, PORT))
except socket.error as msg:
	print ('Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
	sys.exit()

print ('Socket bind complete')

#Start listening on socket
s.listen(10)
print ('Socket now listening')

conn, addr = s.accept()
print ('Connected with ' + addr[0] + ':' + str(addr[1]))

while 1:

	cmd = b''
	while not cmd:
		cmd = conn.recv(1)
	val = b''
	while not val:
		val = conn.recv(1)
	cmd, val = chr(cmd[0]), val[0]

	# Close socket command
	if cmd=='C':
		break;

	string = "{0}{1}".format(cmd,val*4)

	# Send to USB, or not if unplugged
	try:
		ftdi.write(string.encode())
	except Exception:
		pass

	# For future clean socket close
	#if data[0]==82:
	#	break

s.close()