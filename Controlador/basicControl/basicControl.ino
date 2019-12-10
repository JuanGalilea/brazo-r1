// HBridge S1 goes in TX2 (pin 16)
// HBridge S2 goes in RX2 (pin 17)
// 0 = H = Hip, 1 = S = Shoulder, 2 = E = Elbow
#define encoder0PinA  30
#define encoder0PinB  31

#define hipEncA  30
#define hipEncB  31

#define shoulderEncA  32
#define shoulderEncB  33

#define elbowEncA  34
#define elbowEncB  35


volatile long encoder0Pos = 0;
volatile long encoder1Pos = 0;
int count = -1;
bool mov = false;
int vel = 0;
long fullRotation = 175784;
long target = fullRotation / 2;
long newposition0;
long oldposition0 = 0;
long newposition1;
long oldposition1 = 0;
unsigned long newtime;
float vel0;
float vel1;
bool aux = true;
char inByte;
char simplePos;



void setup()
{
  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoder0A, CHANGE);  
  attachInterrupt(digitalPinToInterrupt(encoder0PinB), doEncoder0B, CHANGE);  
  
  Serial.begin(115200);
  Serial2.begin(9600);
  Serial2.println("M1: 0");
  Serial.println("Input  : REF  :   POS : VEL");
  
  encoder0Pos = fullRotation / 2;
}

void doEncoder0A()
{
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    encoder0Pos++;
  } else {
    encoder0Pos--;
  }
  if (encoder0Pos >= fullRotation) {encoder0Pos = 0;}
  if (encoder0Pos <= 0) {encoder0Pos = fullRotation-1;}
}

void doEncoder0B()
{
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    encoder0Pos--;
  } else {
    encoder0Pos++;
  }
  if (encoder0Pos >= fullRotation) {encoder0Pos = 0;}
  if (encoder0Pos <= 0) {encoder0Pos = fullRotation-1;}
}
//500*43 = 21500
//1024*43 = 44032 -> 176128
//10000*43 = 430000
void loop() {
  if (abs(target - encoder0Pos) < 300) {vel = 0;}
  else if (abs(target - encoder0Pos) < 1000) {vel = 25;}
  else if (abs(target - encoder0Pos) < 4000) {vel = 50;}
  else if (abs(target - encoder0Pos) < 8000) {vel = 100;}
  else {vel = 500;}
  if (target > encoder0Pos) {
    Serial2.print("M1: -");
    Serial2.println(vel, DEC);
    }
  else {
    Serial2.print("M1: ");
    Serial2.println(vel, DEC);
    }
  while (Serial.available()>0) {
    inByte = Serial.read();
    target = map(inByte,0,255,0,fullRotation-1);
    Serial.print(inByte, DEC);
    Serial.print(" : ");
    Serial.print(target, DEC);
    Serial.print(" : ");
    Serial.print(encoder0Pos, DEC);
    Serial.print(" : ");
    Serial.println(vel, DEC);
    }
  }
//    if(encoder0Pos>176128 and aux){
//      Serial2.println("M1: -300");
//    }
//    if(encoder0Pos<-176128 and aux){
//      Serial2.println("M1: 0");
//      //Serial.println(0);
//      aux = false;
//      Serial.print("DONE");}
