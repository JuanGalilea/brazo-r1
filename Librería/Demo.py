from ARMR1 import *
from serial import Serial, EIGHTBITS
import random
import time

from code import serialLineReader, serialCharReader, speak, listen

def run():
    port = '/dev/ttyUSB0'
    start = time.time()
    serial = Serial(port, 115200, timeout=0.1, bytesize=EIGHTBITS)
    global portReader
    global portLineReader
    portReader = serialCharReader(serial)
    portLineReader = serialLineReader(serial)

    print("Reset Arduino to Sync...")
    input_data = b"a"
    while input_data != b'S':
        speak(129,serial)
        input_data = next(portReader)

    print("Starting Demo....")
    clearSerial()
    
    opt = "asdf"
    while opt != "0":
        print("MENU:")
        print("1 .- Change Hip Reference")
        print("2 .- Change Shoulder Reference")
        print("3 .- Change Elbow Reference")
        print("4 .- Excecute New References")
        print("5 .- Read Hip Position")
        print("6 .- Read Shoulder Position")
        print("7 .- Read Elbow Position")
        print("0 .- Exit Demo")
        opt = input(">>  ")
        if opt == "1":
            sendWithPayload(serial, CHANGE_HIP_REFERENCE, int(input("Ref? ")))
        if opt == "2":
            sendWithPayload(serial, CHANGE_SHOULDER_REFERENCE, int(input("Ref? ")))
        if opt == "3":
            sendWithPayload(serial, CHANGE_ELBOW_REFERENCE, int(input("Ref? ")))
        if opt == "4":
            print(str(send(serial, UPDATE_ALL_REFERENCES)))
        if opt == "5":
            print(send(serial, READ_HIP_POSITION)[0] * 2.815, "°")
        if opt == "6":
            print(send(serial, READ_SHOULDER_POSITION)[0] * 2.815, "°")
        if opt == "7":
            print(send(serial, READ_ELBOW_POSITION)[0] * 2.815, "°")


def sendWithPayload(serial, order, value):
    speak(order, serial)
    print(next(portReader))
    speak(value // 1024, serial)
    print(next(portReader))
    speak((value % 1024) // 8, serial)
    print(next(portReader))

def send(serial, order):
    speak(order, serial)
    return next(portReader)

def clearSerial():
    a = b"a"
    while a != b"":
        a = next(portReader)


run()