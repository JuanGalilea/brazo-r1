#define encoder0PinA  31
#define encoder0PinB  32

volatile long encoder0Pos = 0;
volatile long encoder1Pos = 0;
int count = -1;
bool mov = false;
long fullRotation = 20*175784;
long newposition0;
long oldposition0 = 0;
long newposition1;
long oldposition1 = 0;
unsigned long newtime;
float vel0;
float vel1;
bool aux = true;
char inByte;


void setup()
{
  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoder0A, CHANGE);  
  attachInterrupt(digitalPinToInterrupt(encoder0PinB), doEncoder0B, CHANGE);  
  
  Serial.begin(9600);
  Serial.println("Starting...");
  encoder0Pos = 0;
}

void doEncoder0A()
{
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    encoder0Pos++;
  } else {
    encoder0Pos--;
  }
}

void doEncoder0B()
{
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    encoder0Pos--;
  } else {
    encoder0Pos++;
  }
}
//500*43 = 21500
//1024*43 = 44032 -> 176128
//10000*43 = 430000
void loop() {
  while (Serial.available()>0) {
    inByte = Serial.read();
    Serial.println(++inByte);
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
