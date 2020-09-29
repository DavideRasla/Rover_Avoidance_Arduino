#include <Servo.h>
#include <Wire.h>
#include "CMPS03.h"

CMPS03 cmps03;

Servo servoLeft;                                                // Declare left servo signal
Servo servoRight;                                               // Declare right servo signal
Servo servoSensor1;                                             // Declare right servo signalyy

const int servoLeftPin           = 13;                          // I/O Pin LeftServo
const int servoRightPin          = 12;                          // I/O Pin RightServo
const int servoTurretPin         = 11;                          // I/O Pin Turret1
const int pingPinOut             = 2;                           // Sensor Trigger
const int pingPinIn              = 3;                           // Sensor Listen
const int sequence[]             = {0, 750, 1500, 2250, 3000};  // Sequence of turret positions.

int MaxDistance;
int PosToGo;
int ScanValid;                                                  // Got a valid scan
int TurnedValid;                                                // Turnet completely
int isMoving;                                                   // Correct scanning, turning and moving
int count ;
unsigned int CompassPosition;
int distance = 0;                                               // last pinged distance

int DistObstacle [] = {3,3,3,3,3};


unsigned int ConvertCompass (unsigned int a){
 if( a >= 0 && a <= 600)
    return 1; //North
    
 if (a > 600 && a <= 900)
     return 2; //North-Est
     
 if (a > 900 && a <= 1400)
     return 3; //Est

 if (a > 1400 && a <= 1900)
    return 4; //Sud-Est

 if (a > 1900 && a <= 2200)
    return 5; //Sud
    
 if (a > 2200 && a <= 2600)
    return 6; //Sud-Ovest
    
 if (a > 2600 && a<=3100)
    return 7; //Ovest
    
 if (a > 3100)
    return 8; //Nord-Ovest  
}

/*Used to ping */
long ping()
{
  long duration;
  digitalWrite( pingPinOut, LOW );
  delayMicroseconds(2);
  digitalWrite( pingPinOut, HIGH );
  delayMicroseconds( 10 );
  digitalWrite( pingPinOut, LOW );
  duration = pulseIn( pingPinIn, HIGH);
  return duration;
}
/*this fuction does a ping and calls SendViaBt()*/
int cmDistance()
{

  int AvoidLooping = 0;                       // Used to not consider distance > 100 cm 
  do
  {
    delayMicroseconds(80);
    long us = ping();
    distance =  0.034 * us / 2;               // Convert to cm measurement
    Serial.print("Durata segnale: ");
    Serial.println(us);
    Serial.print("Distanza pingata: ");
    Serial.println(distance);
    if(AvoidLooping == 3){                    // After 3 measurement i assum the distance equivalent to 100 cm
        distance = 100;
    }
    else{
        AvoidLooping++;
    }
  }
  while (distance > 100 || distance == 0);
  delay(100);
  SendViaBT();                               // Send data via BT to Receiver
  return distance;
}

/*Used to Scan in 5 direction */
int Scan(int * MaxDist, int DistOb[]) {
  int distance = 0;
  int pos;
  int posToGo = 0;
  for (pos = 0; pos < 5; pos += 1)
  {
    servoSensor1.writeMicroseconds((int)sequence[pos]);
    delay(600);
    distance = cmDistance();
    DistOb[pos] = distance;
    if (pos == 0) {
      *MaxDist = distance;
      posToGo = sequence[pos];
    }
    else if (distance > *MaxDist) {
      *MaxDist = distance;
      posToGo = sequence[pos];
    }
    Serial.print("Posizione:");
    Serial.println(sequence[pos]);
    Serial.print("La distanza misurata dall'ostacolo ");
    Serial.print(pos);
    Serial.print(" è: "); 
    Serial.print(distance);
    Serial.print("cm");
    Serial.println();
  }
  return posToGo;
}

/*Used to move the rover forward*/
void GoOn(int speedLeft, int speedRight)
{
  servoRight.attach(servoRightPin);                  // Attach right signal to pin 12
  servoLeft.attach(servoLeftPin);                    // Attach left signal to pin 13
  servoLeft.writeMicroseconds(1500 + speedLeft);     // Set Left servo speed
  servoRight.writeMicroseconds(1500 - speedRight);   // Set right servo speed
}

/*Used to Change direction */
void ChangeDirection(int FreeWay) {

  switch (FreeWay) {
    case 0:
      Serial.print("giro a destra");
      Serial.println(FreeWay);
      servoLeft.writeMicroseconds(1700);        // Left wheel counterclockwise
      servoRight.writeMicroseconds(1700);       // Right wheel counterclockwise
      servoSensor1.writeMicroseconds(1500);
      delay(600); // ...for 0.6 seconds
      break;
    case 750:
      Serial.print("Destra di 45 gradi");
      Serial.println(FreeWay);
      servoLeft.writeMicroseconds(1700);       // Right wheel counterclockwise
      servoRight.writeMicroseconds(1700);      // Right wheel counterclockwise
      servoSensor1.writeMicroseconds(1500);
      delay(300); // ...for 0.3 seconds
      break;
    case 1500:
      Serial.print("Vado dritto");
      Serial.println(FreeWay);
      servoLeft.writeMicroseconds(1500);  
      servoRight.writeMicroseconds(1500);
      servoSensor1.writeMicroseconds(1500);
      delay(600); // ...for 0.6 seconds
      break;
    case 2250:
      Serial.print("Vado a sinistra di 45 gradi");
      Serial.println(FreeWay);
      servoLeft.writeMicroseconds(1300);       // Left wheel clockwise
      servoRight.writeMicroseconds(1300);      // Right wheel clockwise
      servoSensor1.writeMicroseconds(1500);
      delay(300); // ...for 0.3 seconds
    case 3000:
      Serial.print("Vado a sinistra di 90 gradi");
      Serial.println(FreeWay);
      servoLeft.writeMicroseconds(1300);       // Left wheel clockwise
      servoRight.writeMicroseconds(1300);      // Right wheel clockwise
      servoSensor1.writeMicroseconds(1500);
      delay(600); // ...for 0.6 seconds
      break;
    default:
      break;
  }
}

/*This function reads the value from the compass and send 10 bytes via BT */
void SendViaBT(){
  CompassPosition = ConvertCompass(cmps03.read());
  Serial2.print((String)"p" + (char)DistObstacle[0] + (char)DistObstacle[1] + (char)DistObstacle[2] + (char)DistObstacle[3] +(char) DistObstacle[4] + 'r' + (char)distance + (char)CompassPosition+ (char)'s' );
  Serial.print("Sent data via BT");
}

void setup() {

 Wire.begin();
 
  servoSensor1.attach(servoTurretPin);      // Attach front Servo to pin 11
  servoRight.attach(servoRightPin);         // Attach right Servo to pin 12
  servoLeft.attach(servoLeftPin);           // Attach left Servo to pin 13
  pinMode(pingPinOut, OUTPUT);
  pinMode(pingPinIn, INPUT);
 

  Serial2.begin(115200);                    // defaults to 115200bps
  Serial2.print("$");                       // Print three times individually
  Serial2.print("$");
  Serial2.print("$");                       // Enter command mode
  delay(100);                               // Short delay, wait for the Mate to send back CMD
  Serial2.begin(115200);                    // Start bluetooth serial at 9600
  Serial.begin(9600);						            // Start the serial for debugging 
  
  ScanValid    = 0;                         // Got a valid scan
  TurnedValid  = 0;                         // Turnet completely
  isMoving     = 0;                         // Is moving
}

void loop() {
  Serial.println("|||||----------------------------Task dummy--------------------|||||| ");
}

void loop1(200) { //task for scanning

  if (ScanValid == 0 && isMoving == 0 && TurnedValid == 0  ) {                                        // i read these global variables to check if don't have a valid scan, turn and i am not moving

    Serial.println("|||||----------------------------Task 1 (Scan for a way)--------------------|||||| ");

    Serial.print("ScanValid:");
    Serial.println(ScanValid);
    Serial.print("isMoving:");

    PosToGo = Scan(&MaxDistance,DistObstacle );

    Serial.print("Distanza Più Lontana:");
    Serial.println(MaxDistance); 
    Serial.print("Direzione in cui andare");
    Serial.println(PosToGo); 

    ScanValid = 1;
  }
}

void loop2(250) { //task for change direction
  if ( ScanValid == 1 && TurnedValid == 0 && isMoving == 0 ) {                                     // i read these global variables to check if  have a valid scan but i don't have turned and i am not moving

    Serial.println("|||||----------------------------Task 2 (Change Direction)--------------------|||||| ");

    Serial.print("ScanValid:");
    Serial.println(ScanValid); 
    Serial.print("isMoving:");
    Serial.println(isMoving);
    Serial.print("TurnedValid:");
    Serial.println(TurnedValid); 
    Serial.println("Cambio direzione"); 

   
    servoRight.attach(servoRightPin);           // Attach right signal to pin 12
    servoLeft.attach(servoLeftPin);             // Attach left signal to pin 13
    ChangeDirection(PosToGo);
    servoLeft.detach();                         // Stop sending servo signals
    servoRight.detach();

    TurnedValid = 1;
  }
}

void loop3(300) { //task for move and scan
  if (ScanValid == 1 && TurnedValid == 1 ) {                                                      // I read these global variables and move only if the rover has a valid scan and turned correctly

   Serial.println("|||||----------------------------Task 3 (Go on and scan)--------------------|||||| ");

   Serial.print("ScanValid:");
   Serial.println(ScanValid); /
   Serial.print("isMoving:");
   Serial.println(isMoving); 
   Serial.print("TurnedValid:");
   Serial.println(TurnedValid); 

    isMoving = 1;
    count = 0;
    
    GoOn(150, 150);
    
    while ( count < 2  ) {
      if ( cmDistance() < 30 ) {
        count += 1;                           // to avoid false positives i check the last ping twice
      } 
    }
    
    servoLeft.detach(); // Stop sending servo signals
    servoRight.detach();
    TurnedValid = 0;
    ScanValid   = 0;
    isMoving    = 0;
    Serial.println("DISTANZA INFERIORE A 30cm, scansiono!");
  }
}
