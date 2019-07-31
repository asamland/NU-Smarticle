# Xbee_test.py
# Alex Samland
# 07/30/19

import serial
import time
import threading
import math

def go():
    ser.write(b"GO!\n")

def stop():
    ser.write(b"STOP!\n")

def move(angle):
    ser.write(bytes("{}\n".format(angle), 'utf-8'))

def serial_listener():
    global refill_flag
    i=0
    mat = [100,100,100]
    while True:
        test_str = ser.readline().strip().decode('utf-8')
        # print(test_str)
ser = serial.Serial('/dev/tty.usbserial-14310', 9600)
listener = threading.Thread(target=serial_listener)
listener.start()
# th = 0
# while True:
#     angle = 90+int(90*math.sin(th))
#     move(angle)
#     print(angle)
#     th+=0.1
#     time.sleep(0.8)
