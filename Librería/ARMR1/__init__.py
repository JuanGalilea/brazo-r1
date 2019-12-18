from serial import Serial, EIGHTBITS
from time import sleep
import math as m

class InvalidOrderException (Exception):pass
class OutOfRangeReference   (Exception):pass

fullRotation = 175784

NO_COMMAND                      = 129
GOODBYE                         = 130
GO_HOME                         = 131
UPDATE_ALL_REFERENCES           = 132

READ_HIP_POSITION               = 192
UPDATE_HIP_REFERENCE            = 193
CHANGE_HIP_REFERENCE            = 194
CHANGE_HIP_REFERENCE_50         = 195

READ_SHOULDER_POSITION          = 208
UPDATE_SHOULDER_REFERENCE       = 209
CHANGE_SHOULDER_REFERENCE       = 210
CHANGE_SHOULDER_REFERENCE_50    = 211

READ_ELBOW_POSITION             = 224
UPDATE_ELBOW_REFERENCE          = 225
CHANGE_ELBOW_REFERENCE          = 226
CHANGE_ELBOW_REFERENCE_50       = 227

VALID_COMMANDS =    [ NO_COMMAND
                    , GOODBYE
                    , GO_HOME
                    , UPDATE_ALL_REFERENCES
                    , READ_HIP_POSITION
                    , UPDATE_HIP_REFERENCE
                    , CHANGE_HIP_REFERENCE
                    , CHANGE_HIP_REFERENCE_50
                    , READ_SHOULDER_POSITION
                    , UPDATE_SHOULDER_REFERENCE
                    , CHANGE_SHOULDER_REFERENCE
                    , CHANGE_SHOULDER_REFERENCE_50
                    , READ_ELBOW_POSITION
                    , UPDATE_ELBOW_REFERENCE
                    , CHANGE_ELBOW_REFERENCE
                    , CHANGE_ELBOW_REFERENCE_50
                    ]

class ARMR1:

    def __init__(self
                , serialPort    = '/dev/ttyUSB0'
                , baudrate      = 115200
                , timeout       = 0.1
                , bytesize      = EIGHTBITS):
        self.connection      = Serial(serialPort, baudrate, timeout = timeout, bytesize = bytesize)
        self.reader     = ARMR1.__serialCharReader(self.connection)
        self.lineReader = ARMR1.__serialLineReader(self.connection)

        print("Reset Arduino to Sync...")
        input_data = b"a"
        while input_data != b'S':
            self._speak(NO_COMMAND)
            input_data = next(self.reader)
        self.clearSerial()

    def getPositions(self): # Entrega angulos a resolución de 1.536° (no confiar mucho en negativos)
        hip = ARMR1.parsePosition(self.talk(READ_HIP_POSITION))
        shoulder = ARMR1.parsePosition(self.talk(READ_SHOULDER_POSITION))
        elbow = ARMR1.parsePosition(self.talk(READ_ELBOW_POSITION))

        return (hip, shoulder, elbow)

    def closeToWaypoint(self, angles, threshold = 5):
        (hip, shoulder, elbow) = self.getPositions()
        if abs(hip - angles[0]) < threshold:
            if abs(shoulder - angles[1]) < threshold:
                if abs(elbow - angles[2]) < threshold:
                    return True
        return False

    def talk(self, msg):
        self._speak(msg)
        return next(self.reader)

    def _speak(self, msg):
        if (0 <= msg < 128) or msg in VALID_COMMANDS:
            aux = bytearray()
            aux.append(msg)
            self.connection.write(aux)
        else:
            raise InvalidOrderException("Command Not Allowed")

    def angleToOrder(self, x):
        return x * fullRotation / 360

    def sendToXYZ(self, pos, execute = True):
        angles          = self.cartesian2angles(pos)
        return self.sendToABY(angles, execute= execute)

    def sendToABY(self, angles, execute = True):
        hipAngle        = self.angleToOrder(angles[0])
        shoulderAngle   = self.angleToOrder(angles[1])
        elbowAngle      = self.angleToOrder(angles[2])
                                                                # speak(order, serial)
                                                                #     print(next(portReader))
                                                                #     speak(value // 1024, serial)
                                                                #     print(next(portReader))
                                                                #     speak((value % 1024) // 8, serial)
                                                                #     print(next(portReader))
        if max([hipAngle // 1024, shoulderAngle // 1024, elbowAngle // 1024]) > 127:
            raise OutOfRangeReference("Angle given exceeds range possible to be sent")
        try:
            self.talk(CHANGE_ELBOW_REFERENCE)
            self.talk(hipAngle // 1024)
            self.talk(hipAngle % 1024)
            
            self.talk(CHANGE_SHOULDER_REFERENCE)
            self.talk(shoulderAngle // 1024)
            self.talk(shoulderAngle % 1024)
            
            self.talk(CHANGE_ELBOW_REFERENCE)
            self.talk(elbowAngle // 1024)
            self.talk(elbowAngle % 1024)

            if execute:
                self.talk(UPDATE_ALL_REFERENCES)
        except InvalidOrderException:
            print("Error: POSITION OUT OF REACH")
        
    def executeNextReference(self):
        self.talk(UPDATE_ALL_REFERENCES)

    @staticmethod
    def parsePosition(pos):
        aux = 1.536 * int(pos[0])
        if aux > 300:
            return aux - 360
        return aux

    @staticmethod
    def __serialLineReader(port):
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

    @staticmethod
    def messageSplitter(x):
        if x // 1024 > 127:
            raise OutOfRangeReference("Number too high to be sent to robot")
        return (x // 1024, (x % 1024) // 8)

    @staticmethod
    def __serialCharReader(port):
        while True:
            yield port.read()
    
    def clearSerial(self):
        a = b"a"
        while a != b"":
            a = next(self.reader)

    def cartesian2angles(self, xs):  # esta funcion toma un punto en el espacio de tarea y lo s tranforma al espacio de estados
        d1 = 84 / 1000      # sp.var('d_1')
        d2 = 225 / 1000     # sp.var('d_2')
        d3 = 225 / 1000     # sp.var('d_3')

        q1 = m.atan2(xs[1], xs[0])

        Dy = xs[2] - d1
        Dx = m.sqrt(xs[0] ** 2 + xs[1] ** 2)

        R1, R2 = d2, d3

        D = m.sqrt(Dx ** 2 + Dy ** 2)

        chorddistance = (R1 ** 2 - R2 ** 2 + D ** 2) / (2 * D)
        # distance from 1st circle's centre to the chord between intersects
        halfchordlength = m.sqrt(R1 ** 2 - chorddistance ** 2)
        chordmidpointx = (chorddistance * Dx) / D
        chordmidpointy = (chorddistance * Dy) / D

        interseccion = (chordmidpointx - (halfchordlength * Dy) / D, chordmidpointy + (halfchordlength * Dx) / D)
        theta2 = m.atan2(interseccion[1], interseccion[0])

        q2 = - theta2 + m.pi / 2
        q3 = m.pi / 2 - m.atan2(Dy - interseccion[1], Dx - interseccion[0]) - q2

        return [q1, q2 + 90, q3]

if __name__ == "__main__":
    a = ARMR1(serialPort='/dev/ttyUSB0')
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
            (big,smol) = ARMR1.messageSplitter(int(input("Ref? ")))
            print(a.talk(CHANGE_HIP_REFERENCE))
            print(a.talk(big))
            print(a.talk(smol))
            # sendWithPayload(serial, CHANGE_HIP_REFERENCE, int(input("Ref? ")))
        if opt == "2":
            (big,smol) = ARMR1.messageSplitter(int(input("Ref? ")))
            a.talk(CHANGE_SHOULDER_REFERENCE)
            a.talk(big)
            a.talk(smol)
            # sendWithPayload(serial, CHANGE_SHOULDER_REFERENCE, int(input("Ref? ")))
        if opt == "3":
            (big,smol) = ARMR1.messageSplitter(int(input("Ref? ")))
            a.talk(CHANGE_ELBOW_REFERENCE)
            a.talk(big)
            a.talk(smol)
            # sendWithPayload(serial, CHANGE_ELBOW_REFERENCE, int(input("Ref? ")))
        if opt == "4":
            a.talk(UPDATE_ALL_REFERENCES)
            # print(str(send(serial, UPDATE_ALL_REFERENCES)))
        if opt == "5":
            print(int(a.talk(READ_HIP_POSITION)[0])*2.815, "°")
            # print(send(serial, READ_HIP_POSITION)[0] * 2.815, "°")
        if opt == "6":
            print(int(a.talk(READ_SHOULDER_POSITION)[0])*2.815, "°")
            # print(send(serial, READ_SHOULDER_POSITION)[0] * 2.815, "°")
        if opt == "7":
            print(int(a.talk(READ_ELBOW_POSITION)[0])*2.815, "°")
            # print(send(serial, READ_ELBOW_POSITION)[0] * 2.815, "°")