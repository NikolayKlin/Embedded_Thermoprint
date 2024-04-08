import serial
import  sys
import time

from sympy import true
#command=sys.argv[1]

#for i in range(2):

port = serial.Serial('/dev/ttyUSB0', 115200)
port.timeout=1

while true:
    command = input()
    port.write(command.encode('utf-8'))
    port.flush()
   

port.close()

