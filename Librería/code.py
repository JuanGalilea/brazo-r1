# import threading
# import Queue
from serial import Serial, EIGHTBITS
import random
import time

def run():
    port = '/dev/ttyUSB0'
    start = time.time()
    serial = Serial(port, 115200, timeout=0.1, bytesize=EIGHTBITS)
    speak(0, serial)
    for i in range(10,255):
        time_start = time.time()
        speak(i, serial)
        input_data = listen(serial)  
        time_end = time.time()
        print("Tiempo recibido-enviado: {}".format(time_end-time_start))
        time.sleep(.05)

    for i in range(255,-1, -1):
        time_start = time.time()
        speak(i, serial)
        input_data = listen(serial)  
        time_end = time.time()
        print("Tiempo recibido-enviado: {}".format(time_end-time_start))
        time.sleep(.1)
    serial.close()



def listen(port):
    for i in range(10):
        x = port.read()         # read one byte
        if x != b"":
            print(f"Listen = {x}:")
            for i in range(len(x)):
                print(f"<=>{x[i]}")
            return x



def speak(num, port):
    rand_num = num
    msg=bytearray()
    print(f"SpeakNum = {rand_num}, {chr(rand_num)}")
    msg.append(rand_num)
    print(f"Speak = {msg}")
    port.write(msg)


print("partiending")
run()
print("termina3")
