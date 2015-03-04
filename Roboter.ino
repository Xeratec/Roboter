//********************************** ROBOTER SKETCH **************************************************//
//                                       V. 1.3                                                       //
//                                    01.03.2015                                                      //
//                           Bang Ngyuen Khanh & Philip Wiese                                         //
//                                                                                                    //
//                                                                                                    //
//                                                                                                    //
//                                                                                                    //
//                                                                                                    //
//                                                                                                    //
//                                                                                                    //
//****************************************************************************************************//


#include <Servo.h> 
// Config //
const int timeout = 1000;            // WiFi Connection Timeout
const int joyX = 4;                  // Achse X des Joystick: Pin A4
const int joyY = 5;                  // Achse Y des Joystick: Pin A5
const int pinSpeed = 6;             // Potentiometer Speed
const int SW = 52;                   // Taster, um Kontrollmodus umzustellen
const int ST = 50;                   // Staus LED
const int geradeSpeed=50;

int maxSpeed=100;                    // Maximale Geschwindigkeit
int controlMode = 0 ;                // Kontroll Modus 0: Joystick, 1: XBee
int servoVal, motorVal, rightVal, leftVal, forwardSpeed, backwardSpeed;
int serialServo, serialMotor;
Servo servoLB, servoLF, servoRF, servoRB;
double percentDiff, backwardSpeedL, forwardSpeedL, backwardSpeedR, forwardSpeedR;
long t1_1,t1_2,t2_1,t2_2;            // Timervariablen
String readString,num1,num2;
float b;
boolean hardBlock = false;           // Manuelle Bedinung, wenn hardBlock = true, ist die automatische Erkennung der XBeeSteuerung deaktiviert

void setup() {
  // Servos initialisieren
  servoLF.attach(40, 900, 2100);
  servoLB.attach(41, 900, 2100);
  servoRF.attach(42, 900, 2100);
  servoRB.attach(43, 900, 2100);
  //Set-up Handbremse & Richtung des Motors
  pinMode(22, OUTPUT); //Dir motor LF
  pinMode(23, OUTPUT); //Hand-brake motor LF
  pinMode(24, OUTPUT); //Dir motor LB
  pinMode(25, OUTPUT); //Hand-brake motor LB
  pinMode(26, OUTPUT); //Dir motor RF
  pinMode(27, OUTPUT); //Hand-brake motor RF
  pinMode(28, OUTPUT); //Dir motor RB
  pinMode(29, OUTPUT); //Hand-brake motor RB
  pinMode(ST, OUTPUT);
  pinMode(SW, INPUT);
  digitalWrite(SW, HIGH);
  digitalWrite(ST,HIGH);
  t1_1 = millis();
  t2_1 = millis();
  Serial1.begin(115200);
  Serial.begin(115200);
}

void loop() {
  control();
  t1_2 = millis();
  if (controlMode==0){ 

    servoVal = analogRead(joyX);
    motorVal = analogRead(joyY);
    if (hardBlock) {
      if (t1_2-t1_1>250) {
        digitalWrite(ST,!digitalRead(ST));
        t1_1=t1_2;
      }
    }else {
      digitalWrite(ST,HIGH);
    }
  }else if(controlMode==1) {
    t2_2 = millis();
    //Status Led blinkt
    if (t1_2-t1_1>500) {
      digitalWrite(ST,!digitalRead(ST));
      t1_1=t1_2;
    }
    if (t2_2-t2_1>timeout){
      controlMode=0;
      servoVal = motorVal = 500;
    }
  }
  if (servoVal < 420) {// nach Links
    /* Der Wert von servoVal umskaliert von 420 bis 0 nach 0 bis 4 (proportional):
    Winkel für die innere Räder: 0 = 0°; 1 = 10°; 2 = 20°; 3 = 30°; 4 = 40°
    Winkel für die äussere: 0 = 0°; 1 = 4.03°; 2 = 8.06°; 3 = 12.09°; 4 = 16.12° */
    leftVal = map(servoVal, 420, 0, 0, 4);
    servoLF.writeMicroseconds(map(leftVal, 0, 4, 1405, 1824));
    servoLB.writeMicroseconds(map(leftVal, 0, 4, 1515, 1688));
    servoRF.writeMicroseconds(map(leftVal, 0, 4, 1435, 1045));
    servoRB.writeMicroseconds(map(leftVal, 0, 4, 1380, 1205));
    //Wenn motorVal kleiner als 420 ist -> rückwärt fahren
    if (motorVal < 420) {
      brakeOff();
      backward();
      /* leftVal und motorVal Umskalieren
      innere Räder 43% langsamer als die äussere Räder, je nach der Wert von leftVal
      Max. Geschwindigkeit 50*/
      b =  maxSpeed/100*80;
      backwardSpeed = map(motorVal, 420, 0, 0, (int) b);
      percentDiff = map(leftVal, 0, 4, 100, 43);
      backwardSpeedL = backwardSpeed * percentDiff / 100;
      analogWrite(7, backwardSpeedL); // Pin 7, “backwardSpeedL” schreiben
      analogWrite(6, backwardSpeedL);
      analogWrite(5, backwardSpeed);
      analogWrite(4, backwardSpeed);
    }
    if (motorVal > 603) {// vorwärts fahren
      brakeOff();
      forward();
      b =  maxSpeed/100*80;
      forwardSpeed = map(motorVal, 603, 1023, 0, (int) b);
      percentDiff = map(leftVal, 0, 4, 100, 43);
      forwardSpeedL = forwardSpeed * percentDiff / 100;
      analogWrite(7, forwardSpeedL);
      analogWrite(6, forwardSpeedL);
      analogWrite(5, forwardSpeed);
      analogWrite(4, forwardSpeed);
    }else brakeOn;
  }
  if (servoVal > 420 & servoVal < 603) { // standby - geradeaus
    servoLF.writeMicroseconds(1405);
    servoLB.writeMicroseconds(1515);
    servoRF.writeMicroseconds(1435);
    servoRB.writeMicroseconds(1380);
    if (motorVal > 603) {
      brakeOff();
      forward();
      b =  maxSpeed/100*geradeSpeed;
      forwardSpeed = map(motorVal, 603, 1023, 0, (int) b);
      analogWrite(7, forwardSpeed);
      analogWrite(6, forwardSpeed);
      analogWrite(5, forwardSpeed);
      analogWrite(4, forwardSpeed);
    }
    if (motorVal < 420) {
      brakeOff();
      backward();
      b = maxSpeed/100*geradeSpeed;
      backwardSpeed = map(motorVal, 420, 0, 0, (int) b);
      analogWrite(7, backwardSpeed);
      analogWrite(6, backwardSpeed);
      analogWrite(5, backwardSpeed);
      analogWrite(4, backwardSpeed);
     }
  }
  if (servoVal > 603) {// nach rechts
    rightVal = map(servoVal, 603, 1023, 0, 4);
    servoLF.writeMicroseconds(map(rightVal, 0, 4, 1405, 1219));
    servoLB.writeMicroseconds(map(rightVal, 0, 4, 1515, 1090));
    servoRF.writeMicroseconds(map(rightVal, 0, 4, 1435, 1605));
    servoRB.writeMicroseconds(map(rightVal, 0, 4, 1380, 1803));
    if (motorVal < 420){
      brakeOff();
      backward();
      b = maxSpeed/100*80;
      backwardSpeed = map(motorVal, 420, 0, 0, (int) b);
      percentDiff = map(leftVal, 0, 4, 100, 43);
      backwardSpeedR = backwardSpeed * percentDiff / 100;
      analogWrite(7, backwardSpeed);
      analogWrite(6, backwardSpeed);
      analogWrite(5, backwardSpeedR);
      analogWrite(4, backwardSpeedR);
    }
    if (motorVal > 603) {
      brakeOff();
      forward();
      b = maxSpeed/100*80;
      forwardSpeed = map(motorVal, 603, 1023, 0, (int) b);
      percentDiff = map(leftVal, 0, 4, 100, 43);
      forwardSpeedR = forwardSpeed * percentDiff / 100;
      analogWrite(7, forwardSpeed);
      analogWrite(6, forwardSpeed);
      analogWrite(5, forwardSpeedR);
      analogWrite(4, forwardSpeedR);
    }
  }
  if (motorVal > 420 & motorVal < 603) {
    analogWrite(7, 0);
    analogWrite(6, 0);
    analogWrite(5, 0);
    analogWrite(4, 0);
    brakeOn();
  }
}
  
void brakeOff() {
  digitalWrite(23, LOW);
  digitalWrite(25, LOW);
  digitalWrite(27, LOW);
  digitalWrite(29, LOW);
}

void brakeOn() {
  digitalWrite(23, HIGH);
  digitalWrite(25, HIGH);
  digitalWrite(27, HIGH);
  digitalWrite(29, HIGH);
}

void forward() {
  digitalWrite(22, LOW);
  digitalWrite(24, LOW);
  digitalWrite(26, LOW);
  digitalWrite(28, LOW);
}

void backward(){
  digitalWrite(22, HIGH);
  digitalWrite(24, HIGH);
  digitalWrite(26, HIGH);
  digitalWrite(28, HIGH);
}

void serialEvent1() {
  readString="";
  while (Serial1.available()) {
    delay(3);  //delay to allow buffer to fill 
    if (Serial1.available() >0) {
      char c = Serial1.read();  //gets one byte from serial buffer
      readString += c; //makes the string readString
    } 
  }

  if(readString.equals("ST;")) {
    if (!hardBlock) {
      controlMode=1;
      t2_1 = millis();
    }
  }else{
    if(controlMode==1) {
      readString=readString.substring(0,readString.indexOf(";"));
      if (readString.length() >0) {
        int delimiter = readString.indexOf(":");
        num1 = readString.substring(0, delimiter); //get the first four characters
        num2 = readString.substring(delimiter+1, readString.length()); //get the next four characters 
  
        serialServo = num1.toInt();
        serialMotor = num2.toInt();
        
        serialServo = map(serialServo,100,-100,0,1023);
        serialMotor = map(serialMotor,-100,100,0,1023);
        
        servoVal = serialServo;
        motorVal = serialMotor;
        
        Serial.print(readString);
      } 
    }
  }
}
 
void control(){
   //Entprellen des Tasters
   maxSpeed=map(analogRead(pinSpeed),1023,0,0,100);
   byte value_1,value_2=0;
   value_1=digitalRead(SW);
   if(!value_1){
     delay(50);
     value_2=digitalRead(SW);
     if(!value_2){
       analogWrite(7, 0);
       analogWrite(6, 0);
       analogWrite(5, 0);
       analogWrite(4, 0);
       brakeOn();
       if (hardBlock) {
         hardBlock = false;
         controlMode=1;
         t2_1 = millis();
       }else if (!hardBlock && controlMode==1){
         hardBlock = true;
         digitalWrite(ST,HIGH);
         controlMode=0;
       } 
      do{
       }while(!digitalRead(SW)); 
     }
   }
}
