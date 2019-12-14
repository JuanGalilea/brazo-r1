from serial import Serial, EIGHTBITS
from time import sleep

class InvalidOrderException (Exception):pass

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

    def getPositions(self):
        hip = self.talk(READ_HIP_POSITION)
        shoulder = self.talk(READ_SHOULDER_POSITION)
        elbow = self.talk(READ_ELBOW_POSITION)
        return (hip, shoulder, elbow)

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
    def __serialCharReader(port):
        while True:
            yield port.read()
    
    def clearSerial(self):
        a = b"a"
        while a != b"":
            a = next(self.reader)

if __name__ == "__main__":
    a = ARMR1()