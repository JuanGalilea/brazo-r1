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


void setup()
{
  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoder0A, CHANGE);  
  attachInterrupt(digitalPinToInterrupt(encoder0PinB), doEncoder0B, CHANGE);  
  
  Serial.begin(9600);
  Serial2.begin(9600);      
  delay(500);
  Serial.println("Starting...");
  Serial2.println("M1: 0");
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
  if ((encoder0Pos < count * fullRotation) and (not mov)){
    Serial2.println("M1: 1200");
    mov = true;
  }
  if (encoder0Pos >= count * fullRotation) {
    Serial2.println("M1: 0");
    mov = false;
    Serial.println(count);
    count++;
    delay(10000);
  }
//    if(encoder0Pos>176128 and aux){
//      Serial2.println("M1: -300");
//    }
//    if(encoder0Pos<-176128 and aux){
//      Serial2.println("M1: 0");
//      //Serial.println(0);
//      aux = false;
//      Serial.print("DONE");}
}
