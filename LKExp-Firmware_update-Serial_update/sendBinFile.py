import serial
import binascii
import time
import os

print("Starting script to send bin file\n")

serial = serial.Serial(port='/dev/tty.usbmodem14203', baudrate = 115200)
time.sleep(1)

len_data = serial.in_waiting
s = serial.read(len_data)

# print(serial.isOpen())

file = open('APPLICATION/BUILD/DISCO_F769NI/GCC_ARM/APPLICATION_application.bin','rb')
size = os.path.getsize('APPLICATION/BUILD/DISCO_F769NI/GCC_ARM/APPLICATION_application.bin')
print("File size in byte: ", size)
print((size).to_bytes(4, byteorder='big'))
print("File size in hex : ", hex(size))

# Send size first
serial.write((size).to_bytes(4, byteorder='big'))

counter = 0
s = serial.read(1)
while s != b'K':
    s = serial.read(1)
    # print (s)
    time.sleep(0.01)
text = b'K'
while counter < size - 1:
    text = file.read(8)
    counter += 8

    serial.write(text)

    s = serial.read(1) # Wait ACK
    # serial.flushInput()
    # serial.flushOutput()
    # time.sleep(.0001)
    progression = 100 * counter / size
    # print ("Return:", s, " Byte sent:", counter)
    print(f"Progression: {progression:.2f}%")
    if s == b'F':
        break

if s == b'F' :
    print("Error occurs, try again")
else:
    print("Finished send")
serial.close()

file.close()
