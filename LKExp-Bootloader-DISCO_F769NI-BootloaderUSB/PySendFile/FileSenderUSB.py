import serial
s = serial.Serial("/dev/ttyACM0")
s.write(open("testapp.bin","rb").read())