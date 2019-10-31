import threading
import Queue
import random
import time


""" def run():
    input_data = listen()  # asumo len(input_data) == 8 (1 byte)
    
    to_Arduino = threading.Thread(target=speak)
    listen_Arduino = threading.Thread(target=listen)

    to_Arduino.start()
    listen_Arduino.start()


def listen():
    while True:
        input_data = # input

        if  len([i for i in input_data if i == "1"]) == 8: # header 1111 1111
            start_cycle()


def speak():
    while True:
 """


def run():
    port = '/dev/ttyUSB0'
    while True:
        time_start = time.time()
        input_data = listen(port)  
        speak(input_data)
        time_end = time.time()
        print("Tiempo recibido-enviado: {}".format(time_end-time_start))



def listen(port):
    while True:
        with serial.Serial(port, 19200, timeout=1) as ser:
            x = ser.read()          # read one byte
        if x:
            return x



def speak(input_data):
    ser = serial.Serial(port)
    number = str(random.randint(0, 255))
    ser.write(bytes(number))

