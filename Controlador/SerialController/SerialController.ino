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
#define HipPosControl   0b11000100
#define HipVelControl   0b11000101
#define HipTakeVRef     0b11000110
#define HipTakeVRef50   0b11000111
#define HipGiveVel      0b11001000
#define HipUpdateV      0b11001001
#define HipGiveCurrent  0b11001010

// Instrucciones Hombro
#define ShoulderGivePos     0b11010000
#define ShoulderUpdate      0b11010001
#define ShoulderTakeRef     0b11010010
#define ShoulderTakeRef50   0b11010011
#define ShoulderPosControl  0b11010100
#define ShoulderVelControl  0b11010101
#define ShoulderTakeVRef    0b11010110
#define ShoulderTakeVRef50  0b11010111
#define ShoulderGiveVel     0b11011000
#define ShoulderUpdateV     0b11011001
#define ShoulderGiveCurrent 0b11011010

// Instrucciones Codo
#define ElbowGivePos      0b11100000
#define ElbowUpdate       0b11100001
#define ElbowTakeRef      0b11100010
#define ElbowTakeRef50    0b11100011
#define ElbowPosControl   0b11100100
#define ElbowVelControl   0b11100101
#define ElbowTakeVRef     0b11100110
#define ElbowTakeVRef50   0b11100111
#define ElbowGiveVel      0b11101000
#define ElbowUpdateV      0b11101001
#define ElbowGiveCurrent  0b11101010

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

long fullRotation = 175784;       // Rotación completa del motor (Rotación del extremo efector = 240000)
float integrativeLoss = 0.8;      // Perdida de acumulación del factor integral en cada paso
bool isZeroing    = false;        
int  histPointer  = 0;            // "Puntero" de la posicion actual en la lista circular de posiciones
long startTime    = 0;
long lastTime     = 0;

char LastInByte;
char inByte;
char LastCommand;

bool exec = true;
long timeHist[5];                 // Lista circular de ultimos tiempos de ejecución

volatile bool hipZeroed = false;  
volatile long hipPos    = 60000;  // Posición actual de la cadera
long  hipPosHist[5]        ;      // Lista circular de las ultimas posiciones de la cadera
long  hipHome           = 0;      
long  hipNextTarget     = 60000;  // Proxima referencia a cargar
long  hipRef            = 60000;  // Referencia actual de la cadera
bool  hipVelControl     = 0;      // Valor a entregar al motor cuando se controla por velocidad
long  hipVelNextTarget  = 0;      // Proxima referencia a cargar en control por velocidad
long  hipVelRef         = 0;      // Referencia actual en control por velocidad
int   hipMotorT         = 0;      // Valor a entregar al motor cuando se controla por posicion
float hipP              = 4;      // Kp de la cadera
float hipI              = 1.5;    // Ki de la cadera
float hipD              = 40;     // Kd de la cadera
float hipVP             = 0;      // Kp de la cadera en control por velocidad
float hipVI             = 0;      // Ki de la cadera en control por velocidad
float hipVD             = 0;      // Kd de la cadera en control por velocidad
long  hipAccError       = 0;      // Error acumulado de la cadera (control integrativo)
int   hipIConstrain     = 5000;   // Limite de acumulación de la cadera

volatile bool shoulderZeroed  = false;
volatile long shoulderPos     = 0;
long  shoulderPosHist[5]         ;
long  shoulderHome            = 4000;
long  shoulderNextTarget      = 0;
long  shoulderRef             = 0;
bool  shoulderVelControl      = 0;
long  shoulderVelNextTarget   = 0;
long  shoulderVelRef          = 0;
int   shoulderMotorT          = 0;
float shoulderP               = 4;
float shoulderI               = 1.5;
float shoulderD               = 20;
float shoulderVP              = 0;
float shoulderVI              = 0;
float shoulderVD              = 0;
long  shoulderKg              = 18; // MAS ALTO = MAS DÉBIL (16 antes)
float shoulderFeedForward     = 0;
long  shoulderAccError        = 0;
int   shoulderIConstrain      = 5000;

volatile bool elbowZeroed = false;
volatile long elbowPos    = 0;
long  elbowPosHist[5]        ;
long  elbowHome           = 2000;
long  elbowNextTarget     = 0;
long  elbowRef            = 0;
bool  elbowVelControl     = 0;
long  elbowVelNextTarget  = 0;
long  elbowVelRef         = 0;
int   elbowMotorT         = 0;
float elbowP              = 4;
float elbowI              = 0.9;
float elbowD              = 10;
float elbowVP             = 0;
float elbowVI             = 0;
float elbowVD             = 0;
long  elbowKg             = 25;
long  elbowAccError       = 0;
int   elbowIConstrain     = 5000;

void setup() 
{
  // Inicializamos los pines a utilizar en input y les preparamos las interrupciones
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

  // Inicializamos comunicación serial
  Serial.begin(115200); // Computador (Maestro)
  Serial2.begin(9600);  // PuenteH 1 (Cadera y Codo)
  Serial3.begin(9600);  // PuenteH 2 (Hombro)

  sendRefToMotor(0,0);  // turn off hip
  sendRefToMotor(1,0);  // turn off shoulder
  sendRefToMotor(2,0);  // turn off elbow

  // Medimos tiempos iniciales
  startTime = millis();
  lastTime = millis();
}



void loop() {
  // Limitamos a funcionar con una separacion mínima de 20 ms entre ciclos
  if (millis() - lastTime > 20) {
    lastTime = millis();
    handleComms();                // Revisamos la comunicación con el maestro y vemos que se necesita hacer
    updatePosHist();              // Actualizamos valores en listas circulares y de tiempo (usado en control derivativo o por V)

    // Controlamos las distintas articulaciones según el tipo de control que tengan activo (Posición o velocidad)
    if (elbowVelControl) {
      controlElbowV();
    }
    else {
      controlElbow();
    }

    if (shoulderVelControl) {
      controlShoulderV();
    }
    else{
      controlShoulder();
    }
    
    if (hipVelControl) {
      controlHipV();
    }
    else {
      controlHip();
    }

    // Entregamos valores a los motores 
    sendRefToMotor(2,elbowMotorT);
    sendRefToMotor(1,shoulderMotorT);
    sendRefToMotor(0,hipMotorT);

    if (inByte != NoData) {
      LastInByte = inByte;
    }
  }
}

void handleComms () {
  if (Serial.available() > 0) {                       // Si hay algo en el puerto lo leemos si no no hacemos nada
    inByte = Serial.read();
    if ((inByte & TagMask) > 0) {                     // Lectura de comandos (primer bit es 1 en comandos)
      switch(inByte) {
        case Nothing:                                 // Utilizado por el maestro para confirmar la conexión
          Serial.write("S");                          // "S" de Start
          break;
        case GoodBye:                                 // Termina la comunicación y manda a "Home" al brazo
          setHipReference (hipHome);
          setShoulderReference (shoulderHome);
          setElbowReference (elbowHome);
          Serial.write('B');                          // "B" de Bye
          Serial.end();
          break;
        case GoHome:                                  // Solo envía a home al brazo
          setHipReference (hipHome);
          setShoulderReference (shoulderHome);
          setElbowReference (elbowHome);
          Serial.write('A');                          // "A" de Acknowledge
          break;
        case UpdateAll:                               // Actualiza todas las referencias por las cargadas en los nextTarget
          setHipReference (hipNextTarget);
          setShoulderReference (shoulderNextTarget);
          setElbowReference (elbowNextTarget);
          Serial.write('A');                          // "A" de Acknowledge
          break;
                  // Ordenes de la Cadera
        case HipGivePos:                              // Entregar posicion actual de la cadera
          Serial.write((char)((hipPos & ShortenLong)>>10));   // 8 bits mas significativos de la posicion
          break;
        case HipUpdate:                               // Actualizar referencia
          hipRef = hipNextTarget;
          Serial.write('A');
          break;
        case HipTakeRef:                              // Cargar nueva referencia en NextTarget
          Serial.write('A');
          break;
        case HipPosControl:                           // Cambiar a control por Posición
          hipVelControl = false;
          Serial.write('A');
          break;
        case HipVelControl:                           // Cambiar a control por velocidad
          hipVelControl = true;
          Serial.write('A');
          break;
        case HipTakeVRef:                             // Cargar nueva referencia de velocidad en NextVelTarget
          Serial.write('A');
          break;
        case HipGiveVel:                              // Entregar velocidad actual
          Serial.write('T');                          // No implementado
          break;
        case HipUpdateV:                              // Actualizar referencia de velocidad
          hipVelRef = hipNextTarget;
          Serial.write('A');
          break;
        case HipGiveCurrent:                          // Entrega de consumo actual de corriente
          Serial.write('T');                          // No implementado
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
        case ShoulderPosControl:
          shoulderVelControl = false;
          Serial.write('A');
          break;
        case ShoulderVelControl:
          shoulderVelControl = true;
          Serial.write('A');
          break;
        case ShoulderTakeVRef:
          Serial.write('A');
          break;
        case ShoulderGiveVel:
          Serial.write('T');
          break;
        case ShoulderUpdateV:
          shoulderVelRef = shoulderNextTarget;
          Serial.write('A');
          break;
        case ShoulderGiveCurrent:
          Serial.write('T');
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
        case ElbowPosControl:
          elbowVelControl = false;
          Serial.write('A');
          break;
        case ElbowVelControl:
          elbowVelControl = true;
          Serial.write('A');
          break;
        case ElbowTakeVRef:
          Serial.write('A');
          break;
        case ElbowGiveVel:
          Serial.write('T');
          break;
        case ElbowUpdateV:
          elbowVelRef = elbowNextTarget;
          Serial.write('A');
          break;
        case ElbowGiveCurrent:
          Serial.write('T');
          break;
        default:                                  
          Serial.write('E');                        // "E" de Error (comando no reconocido)
          break;
      }
      LastCommand = inByte;
    }
    else {                                    // Lectura de Literales
      switch (LastCommand) {
      case HipTakeRef:                        // Lectura del primer 50% del literal de la nueva referencia de la cadera
        Serial.write('5');                    // "5" de recibido el 50%
        hipNextTarget = emptyLong ^ ((inByte & payloadMask) << 10);
        LastCommand = HipTakeRef50;
        break;
      case HipTakeRef50:                      // Lectura de la segunda mitad de la nueva referencia
        Serial.write('A');                    // "A" de Acknowledge (recepción completa del literal)
        hipNextTarget = hipNextTarget ^ ((inByte & payloadMask) << 3);
        LastCommand = Nothing;
        break;
      case HipTakeVRef:
        Serial.write('5');
        hipVelNextTarget = emptyLong ^ ((inByte & payloadMask) << 10);
        LastCommand = HipTakeVRef50;
        break;
      case HipTakeVRef50:
        Serial.write('A');
        hipVelNextTarget = hipVelNextTarget ^ ((inByte & payloadMask) << 3);
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
      case ShoulderTakeVRef:
        Serial.write('5');
        shoulderVelNextTarget = emptyLong ^ ((inByte & payloadMask) << 10);
        LastCommand = ShoulderTakeVRef50;
        break;
      case ShoulderTakeVRef50:
        Serial.write('A');
        shoulderVelNextTarget = shoulderVelNextTarget ^ ((inByte & payloadMask) << 3);
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
      case ElbowTakeVRef:
        Serial.write('5');
        elbowVelNextTarget = emptyLong ^ ((inByte & payloadMask) << 10);
        LastCommand = ElbowTakeVRef50;
        break;
      case ElbowTakeVRef50:
        Serial.write('A');
        elbowVelNextTarget = elbowVelNextTarget ^ ((inByte & payloadMask) << 3);
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

void doHipEncA() { // Interrupciones de los encoders y chequeo de finales de carrera
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
    if (readFromReg(shoulderEOTShift)) {shoulderPos --;}
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
    if (readFromReg(shoulderEOTShift)) {shoulderPos --;}
    else {shoulderPos--;}
  }
}

void doElbowEncA() {
  if (readFromReg(elbowEncAShift) == readFromReg(elbowEncBShift)) {
    if (readFromReg(elbowEOTShift)) {elbowPos = 0;}
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
    if (readFromReg(elbowEOTShift)) {elbowPos = 0;}
    else {elbowPos--;}
  }
}

void doHipEOT() { // Interrupcion de los finales de carrera
  hipZeroed = true;
  if (hipVelControl) {
    hipRef = 0;
    hipVelControl = false;
  }
  hipPos = 0;
}

void doShoulderEOT() {
  shoulderZeroed = true;
  if (shoulderVelControl) {
    shoulderRef = 0;
    shoulderVelControl = false;
  }
  shoulderPos = 0;
}

void doElbowEOT() {
  elbowZeroed = true;
  if (elbowVelControl) {
    elbowRef = 0;
    elbowVelControl = false;
  }
  elbowPos = 0;
}

void updatePosHist () {                       // Actualiza valores en listas circulares de posicion y tiempo
  hipPosHist[histPointer] = hipPos;
  shoulderPosHist[histPointer] = shoulderPos;
  elbowPosHist[histPointer] = elbowPos;
  timeHist[histPointer] = lastTime;
  if (++histPointer >= 5) {histPointer = 0;}  // Mueve el "Puntero" de la ultima posicion cargada
}

bool readFromReg(int shift) {
  return (inputREG -> PIO_PDSR >> shift) & lastPinMask; // Carga por registro de pines digitales (Ignorar Warning que pueda generar esta linea)
}

int to05 (int entrada) {                      // Corrección de indices en lista circular
  if (0 <= entrada and entrada < 5) {
    return entrada;
  }
  else if (5 <= entrada) {
    return entrada % 5;
  }
  else {
    return entrada + ((abs(entrada) / 5) +1 ) * 5;
  }
  
}

double hipVel () {    // Medicion del promedio de velocidades de los ultimos 4 instantes de tiempo
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
  double speed1 = (elbowPosHist[to05(histPointer + 5)] - elbowPosHist[to05(histPointer + 4)]) 
                  / (timeHist[to05(histPointer + 5)] - timeHist[to05(histPointer + 4)]);
  double speed2 = (elbowPosHist[to05(histPointer + 4)] - elbowPosHist[to05(histPointer + 3)]) 
                  / (timeHist[to05(histPointer + 4)] - timeHist[to05(histPointer + 3)]);
  double speed3 = (elbowPosHist[to05(histPointer + 3)] - elbowPosHist[to05(histPointer + 2)]) 
                  / (timeHist[to05(histPointer + 3)] - timeHist[to05(histPointer + 2)]);
  double speed4 = (elbowPosHist[to05(histPointer + 2)] - elbowPosHist[to05(histPointer + 1)]) 
                  / (timeHist[to05(histPointer + 2)] - timeHist[to05(histPointer + 1)]);
  return (speed1 + speed2 + speed3 + speed4) / 4;
}

void setHipReference (double ref) {       // Actualiza Referencia (existe para legibilidad)
  hipRef = ref;
}

void setShoulderReference (double ref) {
  shoulderRef = ref;
}

void setElbowReference (double ref) {
  elbowRef = ref;
}

void controlHip () {                      // Ejecuta control por posicion sobre la cadera
  hipAccError = constrain(integrativeLoss * hipAccError + hipRef - hipPos, -hipIConstrain, hipIConstrain); // Actualizamos error acumulado
  double p = (hipRef - hipPos) * hipP;    // Calculamos valor de P
  double d = hipVel() * (-hipD);          // Calculamos valor de D
  long   i = hipAccError * hipI;          // Calculamos valor de I
  hipMotorT = map(constrain(p + i + d, -fullRotation, fullRotation), -fullRotation, fullRotation, -1000, 1000);
  // Limitado el output del motor entre -1000 y 1000 (1000 es caleta)
}

void controlHipV () {                     // Control P sobre velocidad
  double p = (hipVelRef - hipVel()) * hipVP;
  hipMotorT = map(constrain(p, -fullRotation, fullRotation), -fullRotation, fullRotation, -1000, 1000);
}

void controlShoulder () {                 // Control de posicion sobre el Hombro
  shoulderAccError = constrain(integrativeLoss * shoulderAccError + shoulderRef - shoulderPos, -shoulderIConstrain, shoulderIConstrain);
  double p = (shoulderRef - shoulderPos) * shoulderP;   // Todo esto ya se ha visto en la cadera
  double d = shoulderVel() * (-shoulderD);
  long   i = shoulderAccError * shoulderI;

  double aux = map(shoulderPos, 5000, 135000,0,180);    // termino auxiliar para el calculo del termino G
  if (aux > 180)  {aux = 360 - aux;}
  if (aux < 0)    {aux = abs(aux);}
  double g = ((aux - 90)*abs(aux - 90)) / -shoulderKg;  // Compensación de la gravedad según Posición (Cuadrática)

  shoulderMotorT = elbowMotorT * shoulderFeedForward + g + map(constrain(p + i + d, -fullRotation, fullRotation), -fullRotation, fullRotation, -1000, 1000);
  // FeedForward del torque que hace el codo (el hombro tambien lo tiene que hacer para mantenerse "quieto")
  // Termino de la gravedad se agrega al control normal
  // Ninguno de estos está en la parte saturada ya que no tratan con fuerzas "externas" (cargas o interacciones con el ambiente)
}

void controlShoulderV () {
  double p = (shoulderVelRef - shoulderVel()) * shoulderVP;
  shoulderMotorT = map(constrain(p, -fullRotation, fullRotation), -fullRotation, fullRotation, -1000, 1000);
}


void controlElbow () {
  elbowAccError = constrain(integrativeLoss * elbowAccError + elbowRef - elbowPos, -elbowIConstrain, elbowIConstrain);
  double p = (elbowRef - elbowPos) * elbowP;
  double d = elbowVel() * (-elbowD);
  long   i = elbowAccError * elbowI;

  double aux = map(shoulderPos -  elbowPos, -65000, 56000,-90,90);
  if (aux > 90)   {aux = 180 - aux;}
  if (aux < -90)  {aux = -180 - aux;}
  double g = ((aux) * abs(aux)) / elbowKg;

  elbowMotorT = g + map(constrain(p + i + d, -fullRotation, fullRotation), -fullRotation, fullRotation, -1000, 1000);
//   return map(constrain(p + i + d, -fullRotation, fullRotation), -fullRotation, fullRotation, -1000, 1000);
}

void controlElbowV () {
  double p = (elbowVelRef - elbowVel()) * elbowVP;
  elbowMotorT = map(constrain(p, -fullRotation, fullRotation), -fullRotation, fullRotation, -1000, 1000);
}

void sendRefToMotor(int motor, int ref) {   // Envia la referencia indicada al motor indicado
  switch (motor) {
  case 0: // move Hip according to ref
    Serial2.print("M1: ");
    Serial2.println(-ref ,DEC); // Gira para el otro lado
    break;
  case 1: // move Shoulder according to ref
    Serial3.print("M1: ");
    Serial3.println(ref ,DEC);  
    break;
  case 2: // move Elbow according to ref
    Serial2.print("M2: ");
    Serial2.println(-ref ,DEC); // Tambien gira para el otro lado
    break;
  default:
    break;
  }
}