// HBridge1 S1 goes in TX2 (pin 16) ¡connect to hip and elbow!
// HBridge1 S2 goes in RX2 (pin 17)
// 0 = H = Hip, 1 = S = Shoulder, 2 = E = Elbow
#define hipEncA  30
#define hipEncB  31

#define shoulderEncA  32
#define shoulderEncB  33

#define elbowEncA  34
#define elbowEncB  35

#define hipEOT      40
#define shoulderEOT 41
#define elbowEOT    42

long fullRotation = 175784;
bool isZeroed     = false;
char histPointer  = 0;

char inByte;
bool awaitingCommand;

bool exec = true;
long timeHist[5];

volatile bool hipZeroed = false;
volatile long hipPos    = 0;
long  hipPosHist[5]        ;
long  hipTarget         = 0;
long  hipNextTarget     = 0;
long  hipRef            = 0;
float hipP              = 0;
float hipI              = 0;
float hipD              = 0;

volatile bool shoulderZeroed  = false;
volatile long shoulderPos     = 0;
long  shoulderPosHist[5]         ;
long  shoulderTarget          = 0;
long  shoulderNextTarget      = 0;
long  shoulderRef             = 0;
float shoulderP               = 0;
float shoulderI               = 0;
float shoulderD               = 0;

volatile bool elbowZeroed = false;
volatile long elbowPos    = 0;
long  elbowPosHist[5]        ;
long  elbowTarget         = 0;
long  elbowNextTarget     = 0;
long  elbowRef            = 0;
float elbowP              = 0;
float elbowI              = 0;
float elbowD              = 0;

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
  Serial.println("huasisi");
  sendRefToMotor(0,0);  // turn off hip
  sendRefToMotor(1,0);  // turn off shoulder
  sendRefToMotor(2,0);  // turn off elbow

  // setear aca valores de "inicio" de los distintos wnes, pa pillar los 0 con los final de carrera
}

void letsZero() {
  if (not hipZeroed) {sendRefToMotor(0,40);}
  if (not shoulderZeroed) {sendRefToMotor(1,40);}
  if (not elbowZeroed) {sendRefToMotor(2,40);}
  if (hipZeroed & shoulderZeroed & elbowZeroed) {isZeroed = true;}
}

void loop() {
  // Check zeroing de las articulaciones
  updatePosHist();
  if (exec) {
    sendRefToMotor(2,controlElbow());
    // if (shoulderPos > 20000) {sendRefToMotor(1,300);}
    // else {sendRefToMotor(1,600);}
    // if (shoulderPos > 40000) {exec = false;}
  }
  else {
    // if (shoulderPos > 40000) {sendRefToMotor(1,-100);}
    // else if (shoulderPos > 30000) {sendRefToMotor(1,0);}
    // else if (shoulderPos > 2000) {sendRefToMotor(1,80);}
    // else if (shoulderPos > 1000) {sendRefToMotor(1,300);}
    // else {sendRefToMotor(0,0);}
  }
  
  // if (isZeroed) {
  // }
  // else {
  //   letsZero();
  // }
  // a todas mandarle como 20?? hasta que topen???

}

void doHipEncA() {
  if (digitalRead(hipEncA) == digitalRead(hipEncB)) {
    hipPos++;
  } 
  else {
    if (digitalRead(hipEOT)) {hipPos = 0;}
    else {hipPos--;}
  }
}

void doHipEncB() {
  if (digitalRead(hipEncA) == digitalRead(hipEncB)) {
    if (digitalRead(hipEOT)) {hipPos = 0;}
    else {hipPos--;}
  } 
  else {
    hipPos++;
  }
}

void doShoulderEncA() {
  if (digitalRead(shoulderEncA) == digitalRead(shoulderEncB)) {
    if (digitalRead(shoulderEOT)) {shoulderPos = 0;}
    else {shoulderPos--;}
  } 
  else {
    shoulderPos++;
  }
}

void doShoulderEncB() {
  if (digitalRead(shoulderEncA) == digitalRead(shoulderEncB)) {
    shoulderPos++;
  } 
  else {
    if (digitalRead(shoulderEOT)) {shoulderPos = 0;}
    else {shoulderPos--;}
  }
}

void doElbowEncA() {
  if (digitalRead(elbowEncA) == digitalRead(elbowEncB)) {
    if (digitalRead(elbowEOT)) {elbowPos = 0;}
    else {elbowPos--;}
  } 
  else {
    elbowPos++;
  }
}

void doElbowEncB() {
  if (digitalRead(elbowEncA) == digitalRead(elbowEncB)) {
    elbowPos++;
  } 
  else {
    if (digitalRead(elbowEOT)) {elbowPos = 0;}
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

int controlHip () {
  double p = (hipRef - hipPos) * hipP;
  double d = hipVel() * hipD;
  return map(constrain(p + d, -fullRotation, fullRotation), -fullRotation, fullRotation, -1000, 1000);
}

int controlShoulder () {
  double p = (shoulderRef - shoulderPos) * shoulderP;
  double d = shoulderVel() * shoulderD;
  return map(constrain(p + d, -fullRotation, fullRotation), -fullRotation, fullRotation, -1000, 1000);
}

int controlElbow () {
  double p = (elbowRef - elbowPos) * elbowP;
  double d = elbowVel() * elbowD;
  return map(constrain(p + d, -fullRotation, fullRotation), -fullRotation, fullRotation, -1000, 1000);
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
    Serial2.println(ref ,DEC);
    break;
  default:
    break;
}

} // EOF
