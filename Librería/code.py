# import threading
# import Queue
from serial import Serial, EIGHTBITS
import random
import time

def run():
    port = '/dev/ttyUSB0'
    start = time.time()
    serial = Serial(port, 115200, timeout=0.1, bytesize=EIGHTBITS)
    # for i in range(10,255):
    #     time_start = time.time()
    #     speak(i, serial)
    #     input_data = listen(serial)  
    #     time_end = time.time()
    #     print("Tiempo recibido-enviado: {}".format(time_end-time_start))
    #     time.sleep(.05)

    # for i in range(255,-1, -1):
    #     time_start = time.time()
    #     speak(i, serial)
    #     input_data = listen(serial)  
    #     time_end = time.time()
    #     print("Tiempo recibido-enviado: {}".format(time_end-time_start))
    #     time.sleep(.1)
    # portReader = serialLineReader(serial)
    portReader = serialCharReader(serial)

    input_data = b"a"
    while input_data != b'S':
        speak(129,serial)
        input_data = next(portReader)
    print("Starting")

    start = time.time()
    aux = b""
    this_time = time.time()
    maximum = 0
    last_time = time.time()
    counter = 0
    for j in range(1000):
        speak(194,serial)
        # print("-------------------------------------------")
        # print(next(portReader))
        aux = next(portReader)
        this_time = time.time()
        if this_time - last_time > maximum: maximum = this_time - last_time
        if this_time - last_time > 0.022: counter += 1
        last_time = this_time
        for i in range(0,2):
            tgt = random.randint(0,127)
            speak(tgt,serial)
            aux = next(portReader)
            this_time = time.time()
            if this_time - last_time > maximum: maximum = this_time - last_time
            if this_time - last_time > 0.022: counter += 1
            last_time = this_time
            # print(next(portReader))
            # time.sleep(0.01)
    print("Tiempo promedio ciclo Comm: {}".format((time.time()-start)/3000))
    print("Tiempo Máximo ciclo Comm: {}".format(maximum))
    print(counter)

    serial.close()



def listen(port):
    for i in range(10):
        x = port.read()         # read one byte
        if x != b"":
            print(f"Listen = {x}:")
            for i in range(len(x)):
                print(f"<=>{x[i]}")
            return x


def serialLineReader(port):
    while True:
        aux = bytearray()
        while True:
            x = port.read()
            if x == b"\n":
                break
            elif x!= b"":
                aux += x
            else:
                yield b""
        yield aux

def serialCharReader(port):
    while True:
        yield port.read()

def speak(num, port):
    rand_num = num
    msg=bytearray()
    # print(f"SpeakNum = {rand_num}, {chr(rand_num)}")
    msg.append(rand_num)
    # print(f"Speak = {msg}")
    port.write(msg)


print("partiending")
run()
print("termina3")
