// HBridge S1 goes in TX2 (pin 16)
// HBridge S2 goes in RX2 (pin 17)
// 0 = H = Hip, 1 = S = Shoulder, 2 = E = Elbow
#define hipEncA  30
#define hipEncB  31

#define shoulderEncA  32
#define shoulderEncB  33

#define elbowEncA  34
#define elbowEncB  35

long fullRotation = 175784;
bool isZeroed     = false;

volatile bool hipZeroed = false;
volatile long hipPos    = 0;
long  hipTarget         = 0;
long  hipNextTarget     = 0;
long  hipRef            = 0;
float hipP              = 0;
float hipI              = 0;
float hipD              = 0;

volatile bool shoulderZeroed  = false;
volatile long shoulderPos     = 0;
long  shoulderTarget          = 0;
long  shoulderNextTarget      = 0;
long  shoulderRef             = 0;
float shoulderP               = 0;
float shoulderI               = 0;
float shoulderD               = 0;

volatile bool elbowZeroed = false;
volatile long elbowPos    = 0;
long  elbowTarget         = 0;
long  elbowNextTarget     = 0;
long  elbowRef            = 0;
float elbowP              = 0;
float elbowI              = 0;
float elbowD              = 0;

char inByte;
bool awaitingCommand;


void setup() 
{
  pinMode(hipEncA, INPUT);
  attachInterrupt(digitalPinToInterrupt(hipEncA), doHipEncA, CHANGE);
  pinMode(hipEncB, INPUT);
  attachInterrupt(digitalPinToInterrupt(hipEncB), doHipEncB, CHANGE);

  pinMode(shoulderEncA, INPUT);
  attachInterrupt(digitalPinToInterrupt(shoulderEncA), doShoulderEncA, CHANGE);
  pinMode(shoulderEncB, INPUT);
  attachInterrupt(digitalPinToInterrupt(shoulderEncB), doShoulderEncB, CHANGE);

  pinMode(elbowEncA, INPUT);
  attachInterrupt(digitalPinToInterrupt(elbowEncA), doElbowEncA, CHANGE);
  pinMode(elbowEncB, INPUT);
  attachInterrupt(digitalPinToInterrupt(elbowEncB), doElbowEncB, CHANGE);

  Serial.begin(115200);
  Serial2.begin(9600);
  Serial3.begin(9600);

  sendRefToMotor(0,0);  // turn off hip
  sendRefToMotor(1,0);  // turn off shoulder
  sendRefToMotor(2,0);  // turn off elbow

  // setear aca valores de "inicio" de los distintos wnes, pa pillar los 0 con los final de carrera
}

void loop() {
  // Check zeroing de las articulaciones
  if (isZeroed) {
  }
  else {
    letsZero();
  }
  // a todas mandarle como 20?? hasta que topen???

}

void doHipEncA() {
  if (digitalRead(hipEncA) == digitalRead(hipEncB)) {
    hipPos++;
  } 
  else {
    hipPos--;
  }
}

void doHipEncB() {
  if (digitalRead(hipEncA) == digitalRead(hipEncB)) {
    hipPos--;
  } 
  else {
    hipPos++;
  }
}

void doShoulderEncA() {
  if (digitalRead(shoulderEncA) == digitalRead(shoulderEncB)) {
    shoulderPos++;
  } 
  else {
    shoulderPos--;
  }
}

void doShoulderEncB() {
  if (digitalRead(shoulderEncA) == digitalRead(shoulderEncB)) {
    shoulderPos--;
  } 
  else {
    shoulderPos++;
  }
}

void doElbowEncA() {
  if (digitalRead(elbowEncA) == digitalRead(elbowEncB)) {
    elbowPos++;
  } 
  else {
    elbowPos--;
  }
}

void doElbowEncB() {
  if (digitalRead(elbowEncA) == digitalRead(elbowEncB)) {
    elbowPos--;
  } 
  else {
    elbowPos++;
  }
}

void sendRefToMotor(int motor, long ref) {
  switch (motor) {
  case 0: // move Hip according to ref
    Serial2.print("M1: ");
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
}

void letsZero() {
  if (not hipZeroed) {sendRefToMotor(0,20);}
  if (not shoulderZeroed) {sendRefToMotor(1,20);}
  if (not elbowZeroed) {sendRefToMotor(2,20);}
  if (hipZeroed & shoulderZeroed & elbowZeroed) {isZeroed = true;}
}

} // EOF