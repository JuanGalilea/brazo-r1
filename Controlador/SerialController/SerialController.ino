// HBridge1 S1 goes in TX2 (pin 16) ¡connect to hip and elbow!
// HBridge1 S2 goes in RX2 (pin 17)
// 0 = H = Hip, 1 = S = Shoulder, 2 = E = Elbow
#define inputREG PIOC
#define lastPinMask 0b00000000000000000000000000000001
#define ShortenLong 0b00000000000000111111111111111111
#define emptyLong   0b00000000000000000000000000000000

#define TagMask         0b10000000
#define payloadMask     0b01111111

// Instrucciones globales
#define NoData          0b10000000
#define Nothing         0b10000001
#define GoodBye         0b10000010
#define GoHome          0b10000011
#define UpdateAll       0b10000100

// Instrucciones Cadera
#define HipGivePos      0b11000000
#define HipUpdate       0b11000001
#define HipTakeRef      0b11000010
#define HipTakeRef50    0b11000011

// Instrucciones Hombro
#define ShoulderGivePos   0b11010000
#define ShoulderUpdate    0b11010001
#define ShoulderTakeRef   0b11010010
#define ShoulderTakeRef50 0b11010011

// Instrucciones Codo
#define ElbowGivePos      0b11100000
#define ElbowUpdate       0b11100001
#define ElbowTakeRef      0b11100010
#define ElbowTakeRef50    0b11100011

#define hipEncA  33
#define hipEncAShift 1
#define hipEncB  34
#define hipEncBShift 2

#define shoulderEncA  35
#define shoulderEncAShift 3
#define shoulderEncB  36
#define shoulderEncBShift 4

#define elbowEncA  37
#define elbowEncAShift 5
#define elbowEncB  38
#define elbowEncBShift 6

#define hipEOT      39
#define hipEOTShift 7
#define shoulderEOT 40
#define shoulderEOTShift 8
#define elbowEOT    41
#define elbowEOTShift 9

long fullRotation = 175784;
float integrativeLoss = 0.8;
bool isZeroed     = false;
char histPointer  = 0;
long startTime    = 0;
long lastTime     = 0;

char LastInByte;
char inByte;
char LastCommand;
bool awaitingCommand;

bool exec = true;
long timeHist[5];

volatile bool hipZeroed = false;
volatile long hipPos    = 0;
long  hipPosHist[5]        ;
long  hipHome           = 0;
long  hipTarget         = 0;
long  hipNextTarget     = 0;
long  hipRef            = 0;
int   hipMotorT         = 0;
float hipP              = 0;
float hipI              = 0;
float hipD              = 0;
long  hipAccError       = 0;
int   hipIConstrain     = 5000;

volatile bool shoulderZeroed  = false;
volatile long shoulderPos     = 0;
long  shoulderPosHist[5]         ;
long  shoulderHome            = 4000;
long  shoulderTarget          = 0;
long  shoulderNextTarget      = 0;
long  shoulderRef             = 0;
int   shoulderMotorT          = 0;
float shoulderP               = 4;
float shoulderI               = 1.5;
float shoulderD               = 20;
long  shoulderKg              = 16; // MAS ALTO = MAS DÉBIL
long  shoulderAccError        = 0;
int   shoulderIConstrain      = 5000;

volatile bool elbowZeroed = false;
volatile long elbowPos    = 0;
long  elbowPosHist[5]        ;
long  elbowHome           = 2000;
long  elbowTarget         = 0;
long  elbowNextTarget     = 0;
long  elbowRef            = 0;
int   elbowMotorT         = 0;
float elbowP              = 4;
float elbowI              = 0.9;
float elbowD              = 10;
long  elbowAccError       = 0;
int   elbowIConstrain     = 5000;

//void letsZero() {
//  if (not hipZeroed) {sendRefToMotor(0,40);}
//  if (not shoulderZeroed) {sendRefToMotor(1,40);}
//  if (not elbowZeroed) {sendRefToMotor(2,40);}
//  if (hipZeroed & shoulderZeroed & elbowZeroed) {isZeroed = true;}
//}

void setup() 
{
  pinMode(hipEncA, INPUT);
  attachInterrupt(digitalPinToInterrupt(hipEncA), doHipEncA, CHANGE);
  pinMode(hipEncB, INPUT);
  attachInterrupt(digitalPinToInterrupt(hipEncB), doHipEncB, CHANGE);
  pinMode(hipEOT, INPUT);
  attachInterrupt(digitalPinToInterrupt(hipEOT), doHipEOT, RISING);

  pinMode(shoulderEncA, INPUT);
  attachInterrupt(digitalPinToInterrupt(shoulderEncA), doShoulderEncA, CHANGE);
  pinMode(shoulderEncB, INPUT);
  attachInterrupt(digitalPinToInterrupt(shoulderEncB), doShoulderEncB, CHANGE);
  pinMode(shoulderEOT, INPUT);
  attachInterrupt(digitalPinToInterrupt(shoulderEOT), doShoulderEOT, RISING);

  pinMode(elbowEncA, INPUT);
  attachInterrupt(digitalPinToInterrupt(elbowEncA), doElbowEncA, CHANGE);
  pinMode(elbowEncB, INPUT);
  attachInterrupt(digitalPinToInterrupt(elbowEncB), doElbowEncB, CHANGE);
  pinMode(elbowEOT, INPUT);
  attachInterrupt(digitalPinToInterrupt(elbowEOT), doElbowEOT, RISING);

  Serial.begin(115200);
  Serial2.begin(9600);
  Serial3.begin(9600);

  sendRefToMotor(0,0);  // turn off hip
  sendRefToMotor(1,0);  // turn off shoulder
  sendRefToMotor(2,0);  // turn off elbow

  startTime = millis();
  lastTime = millis();
}



void loop() {
  if (millis() - lastTime > 20) {
    lastTime = millis();
    handleComms();
    updatePosHist();
    controlElbow();
    controlShoulder();
    controlHip();
    sendRefToMotor(2,elbowMotorT);
    sendRefToMotor(1,shoulderMotorT);
    sendRefToMotor(0,hipMotorT);
    if (inByte != NoData) {
      LastInByte = inByte;
    }
  }
}

void handleComms () {
  if (Serial.available() > 0) {
    inByte = Serial.read();
    if ((inByte & TagMask) > 0) {                     // Lectura de comandos
      switch(inByte) {
        case Nothing:
          Serial.write("S");
          break;
        case GoodBye:
          setHipReference (hipHome);
          setShoulderReference (shoulderHome);
          setElbowReference (elbowHome);
          Serial.write('B');
          Serial.end();
          break;
        case GoHome:
          setHipReference (hipHome);
          setShoulderReference (shoulderHome);
          setElbowReference (elbowHome);
          Serial.write('A');
          break;
        case UpdateAll:
          setHipReference (hipNextTarget);
          setShoulderReference (shoulderNextTarget);
          setElbowReference (elbowNextTarget);
          Serial.write('A');
          break;
                  // Ordenes de la Cadera
        case HipGivePos:
          Serial.write((char)((hipPos & ShortenLong)>>10));
          break;
        case HipUpdate:
          hipRef = hipNextTarget;
          Serial.write('A');
          break;
        case HipTakeRef:
          Serial.write('A');
          break;
                  // Ordenes del Hombro
        case ShoulderGivePos:
          Serial.write((char)((shoulderPos & ShortenLong)>>10));
          break;
        case ShoulderUpdate:
          shoulderRef = shoulderNextTarget;
          Serial.write('A');
          break;
        case ShoulderTakeRef:
          Serial.write('A');
          break;
                  // Ordenes del Codo
        case ElbowGivePos:
          Serial.write((char)((elbowPos & ShortenLong)>>10));
          break;
        case ElbowUpdate:
          elbowRef = elbowNextTarget;
          Serial.write('A');
          break;
        case ElbowTakeRef:
          Serial.write('A');
          break;
        default:
          Serial.write('E');
          break;
      }
      LastCommand = inByte;
    }
    else {                                    // Lectura de Literales
      switch (LastCommand) {
      case HipTakeRef:
        Serial.write('5');
        hipNextTarget = emptyLong ^ ((inByte & payloadMask) << 10);
        LastCommand = HipTakeRef50;
        break;
      case HipTakeRef50:
        Serial.write('A');
        hipNextTarget = hipNextTarget ^ ((inByte & payloadMask) << 3);
        LastCommand = Nothing;
        break;
      case ShoulderTakeRef:
        Serial.write('5');
        shoulderNextTarget = emptyLong ^ ((inByte & payloadMask) << 10);
        LastCommand = ShoulderTakeRef50;
        break;
      case ShoulderTakeRef50:
        Serial.write('A');
        shoulderNextTarget = shoulderNextTarget ^ ((inByte & payloadMask) << 3);
        LastCommand = Nothing;
        break;
      case ElbowTakeRef:
        Serial.write('5');
        elbowNextTarget = emptyLong ^ ((inByte & payloadMask) << 10);
        LastCommand = ElbowTakeRef50;
        break;
      case ElbowTakeRef50:
        Serial.write('A');
        elbowNextTarget = elbowNextTarget ^ ((inByte & payloadMask) << 3);
        LastCommand = Nothing;
        break;
      default:
        Serial.write('U');
        break;
      }
    }
  }
  else {
    inByte = NoData;
  }
  
}

void doHipEncA() {
  if (readFromReg(hipEncAShift) == readFromReg(hipEncBShift)) {
    hipPos++;
  } 
  else {
    if (readFromReg(hipEOTShift)) {hipPos = 0;}
    else {hipPos--;}
  }
}

void doHipEncB() {
  if (readFromReg(hipEncAShift) == readFromReg(hipEncBShift)) {
    if (readFromReg(hipEOTShift)) {hipPos = 0;}
    else {hipPos--;}
  } 
  else {
    hipPos++;
  }
}

void doShoulderEncA() {
  if (readFromReg(shoulderEncAShift) == readFromReg(shoulderEncBShift)) {
    if (readFromReg(shoulderEOTShift)) {shoulderPos = 0;}
    else {shoulderPos--;}
  } 
  else {
    shoulderPos++;
  }
}

void doShoulderEncB() {
  if (readFromReg(shoulderEncAShift) == readFromReg(shoulderEncBShift)) {
    shoulderPos++;
  } 
  else {
    if (readFromReg(shoulderEOTShift)) {shoulderPos = 0;}
    else {shoulderPos--;}
  }
}

void doElbowEncA() {
  if (readFromReg(elbowEncAShift) == readFromReg(elbowEncBShift)) {
    if (readFromReg(elbowEOTShift)) {elbowPos--;}
    // if (readFromReg(elbowEOTShift)) {elbowPos = 0;}
    else {elbowPos--;}
  } 
  else {
    elbowPos++;
  }
}

void doElbowEncB() {
  if (readFromReg(elbowEncAShift) == readFromReg(elbowEncBShift)) {
    elbowPos++;
  } 
  else {
    // if (readFromReg(elbowEOTShift)) {elbowPos = 0;}
    if (readFromReg(elbowEOTShift)) {elbowPos--;}
    else {elbowPos--;}
  }
}

void doHipEOT() {
  hipZeroed = true;
  hipPos = 0;
}

void doShoulderEOT() {
  shoulderZeroed = true;
  shoulderPos = 0;
}

void doElbowEOT() {
  elbowZeroed = true;
  elbowPos = 0;
}

void updatePosHist () {
  hipPosHist[histPointer] = hipPos;
  shoulderPosHist[histPointer] = shoulderPos;
  elbowPosHist[histPointer] = elbowPos;
  timeHist[histPointer] = millis();
  if (++histPointer >= 5) {histPointer = 0;}
}

bool readFromReg(int shift) {
  return (inputREG -> PIO_PDSR >> shift) & lastPinMask;
}

char to05 (char entrada) {
  if (0 <= entrada < 5) {
    return entrada;
  }
  else if (0 < entrada) {
    return to05(entrada - 5);
  }
  else {
    return to05(entrada + 5);
  }
}

double hipVel () {
  double speed1 = (hipPosHist[to05(histPointer - 1)] - hipPosHist[to05(histPointer - 2)]) 
                  / (timeHist[to05(histPointer - 1)] - timeHist[to05(histPointer - 2)]);
  double speed2 = (hipPosHist[to05(histPointer - 2)] - hipPosHist[to05(histPointer - 3)]) 
                  / (timeHist[to05(histPointer - 2)] - timeHist[to05(histPointer - 3)]);
  double speed3 = (hipPosHist[to05(histPointer - 3)] - hipPosHist[to05(histPointer - 4)]) 
                  / (timeHist[to05(histPointer - 3)] - timeHist[to05(histPointer - 4)]);
  double speed4 = (hipPosHist[to05(histPointer - 4)] - hipPosHist[to05(histPointer - 5)]) 
                  / (timeHist[to05(histPointer - 4)] - timeHist[to05(histPointer - 5)]);
  return (speed1 + speed2 + speed3 + speed4) / 4;
}

double shoulderVel () {
  double speed1 = (shoulderPosHist[to05(histPointer - 1)] - shoulderPosHist[to05(histPointer - 2)]) 
                  / (timeHist[to05(histPointer - 1)] - timeHist[to05(histPointer - 2)]);
  double speed2 = (shoulderPosHist[to05(histPointer - 2)] - shoulderPosHist[to05(histPointer - 3)]) 
                  / (timeHist[to05(histPointer - 2)] - timeHist[to05(histPointer - 3)]);
  double speed3 = (shoulderPosHist[to05(histPointer - 3)] - shoulderPosHist[to05(histPointer - 4)]) 
                  / (timeHist[to05(histPointer - 3)] - timeHist[to05(histPointer - 4)]);
  double speed4 = (shoulderPosHist[to05(histPointer - 4)] - shoulderPosHist[to05(histPointer - 5)]) 
                  / (timeHist[to05(histPointer - 4)] - timeHist[to05(histPointer - 5)]);
  return (speed1 + speed2 + speed3 + speed4) / 4;
}

double elbowVel () {
  double speed1 = (elbowPosHist[to05(histPointer - 1)] - elbowPosHist[to05(histPointer - 2)]) 
                  / (timeHist[to05(histPointer - 1)] - timeHist[to05(histPointer - 2)]);
  double speed2 = (elbowPosHist[to05(histPointer - 2)] - elbowPosHist[to05(histPointer - 3)]) 
                  / (timeHist[to05(histPointer - 2)] - timeHist[to05(histPointer - 3)]);
  double speed3 = (elbowPosHist[to05(histPointer - 3)] - elbowPosHist[to05(histPointer - 4)]) 
                  / (timeHist[to05(histPointer - 3)] - timeHist[to05(histPointer - 4)]);
  double speed4 = (elbowPosHist[to05(histPointer - 4)] - elbowPosHist[to05(histPointer - 5)]) 
                  / (timeHist[to05(histPointer - 4)] - timeHist[to05(histPointer - 5)]);
  return (speed1 + speed2 + speed3 + speed4) / 4;
}

void setHipReference (double ref) {
  hipRef = ref;
}

void setShoulderReference (double ref) {
  shoulderRef = ref;
}

void setElbowReference (double ref) {
  elbowRef = ref;
}

void controlHip () {
  hipAccError = constrain(integrativeLoss * hipAccError + hipRef - hipPos, -hipIConstrain, hipIConstrain);
  double p = (hipRef - hipPos) * hipP;
  double d = hipVel() * (-hipD);
  long   i = hipAccError * hipI;
  hipMotorT = map(constrain(p + i + d, -fullRotation, fullRotation), -fullRotation, fullRotation, -1000, 1000);
//   return map(constrain(p + i + d, -fullRotation, fullRotation), -fullRotation, fullRotation, -1000, 1000);
}

void controlShoulder () {
  shoulderAccError = constrain(integrativeLoss * shoulderAccError + shoulderRef - shoulderPos, -shoulderIConstrain, shoulderIConstrain);
  double p = (shoulderRef - shoulderPos) * shoulderP;
  double d = shoulderVel() * (-shoulderD);
  double aux = map(shoulderPos, 0, 110000,0,180);
  double g = ((aux - 90)*abs(aux - 90)) / -shoulderKg;
  long   i = shoulderAccError * shoulderI;
  shoulderMotorT = constrain(g + map(constrain(p + i + d, -fullRotation, fullRotation), -fullRotation, fullRotation, -1000, 1000),-1000,1000);
}

void controlElbow () {
  elbowAccError = constrain(integrativeLoss * elbowAccError + elbowRef - elbowPos, -elbowIConstrain, elbowIConstrain);
  double p = (elbowRef - elbowPos) * elbowP;
  double d = elbowVel() * (-elbowD);
  long   i = elbowAccError * elbowI;
  elbowMotorT = map(constrain(p + i + d, -fullRotation, fullRotation), -fullRotation, fullRotation, -1000, 1000);
//   return map(constrain(p + i + d, -fullRotation, fullRotation), -fullRotation, fullRotation, -1000, 1000);
}

void sendRefToMotor(int motor, int ref) {
  switch (motor) {
  case 0: // move Hip according to ref
    Serial2.print("M0: ");
    Serial2.println(ref ,DEC);
    break;
  case 1: // move Shoulder according to ref
    Serial3.print("M1: ");
    Serial3.println(ref ,DEC);
    break;
  case 2: // move Elbow according to ref
    Serial2.print("M2: ");
    Serial2.println(-ref ,DEC);
    break;
  default:
    break;
  }
}
