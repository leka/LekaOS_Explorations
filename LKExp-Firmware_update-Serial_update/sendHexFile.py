import serial
import binascii
import time

print("Starting script to send hex file\n")

serial = serial.Serial(port='/dev/tty.usbmodem14203', baudrate = 115200)
time.sleep(1)

len_data = serial.in_waiting
s = serial.read(len_data)
# print (s)

# print(serial.isOpen())

# file = open('test.hex','r')
# file = open('APPLICATION_application.hex','r')
file = open('APPLICATION/BUILD/DISCO_F769NI/GCC_ARM/APPLICATION_application.hex','r')
code = 0

serial.write(binascii.unhexlify("FF"))

s = serial.read(1)
while s != b'0':
    s = serial.read(1)
    # print (s)
    time.sleep(0.01)

while code != 1 :

    text = file.readline()[1:-1]
    line = binascii.unhexlify(text)

    serial.write(line)
    code = line[3]

    s = serial.read(1) # Wait ACK 
    # print (s)
    if s == b'F' :
        break

if s == b'F' :
    print("Error occurs, try again")
else:
    print("Finished send")
serial.close()

file.close()
