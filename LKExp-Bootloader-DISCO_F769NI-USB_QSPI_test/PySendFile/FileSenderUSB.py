import serial
s = serial.Serial('/dev/ttyACM0')
s.write(open('test.txt','rb').read())