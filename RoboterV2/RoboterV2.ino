//********************************** ROBOTER SKETCH **************************************************//
//                                       V. 1.4                                                       //
//                                    03.04.2015                                                      //
//                           Bang Ngyuen Khanh & Philip Wiese                                         //
//                                                                                                    //
//                                      Experimental                                                  //
//                                                                                                    //
//                                                                                                    //
//                                                                                                    //
//                                                                                                    //
//                                                                                                    //
//****************************************************************************************************//

//************************************** LIBRARYS ****************************************************//
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

//*********************************** CONFIGURATION **************************************************//
//Buttons, Joystick & LED
#define joystickX 4
#define joystickY 5
#define button_ControlMode 52
#define button_up 32
#define button_down 35
#define button_left 36
#define button_right 39
#define status_led 50
// Motor
#define dir_motor_left_front 22
#define hb_motor_left_front 23
#define g_motor_left_front 7
#define dir_motor_left_back 24
#define hb_motor_left_back 25
#define g_motor_left_back 6
#define dir_motor_right_front 26
#define hb_motor_right_front 27
#define g_motor_right_front 5
#define dir_motor_right_back 28
#define hb_motor_right_back 29
#define g_motor_right_back 4
// Servos
#define servo_left_front 40
#define servo_left_back 41
#define servo_right_front 42
#define servo_right_back 43
#define servo_max_pulse 2100
#define servo_min_pulse 900
//Einstellugnen
#define default_max_speed 50
#define default_kurve_speed 80
#define default_joystick 1
#define default_stufenMotor 4
#define default_stufenReader 4
#define default_bremse 1

//************************************* KONSTANTEN ****************************************************//
//LCD Menu
const String menu[] = {"Info", "Einstellungen", "Reset"};
const String settings[] = {"Max. Speed", "Kurve Speed", "Joystick", "Stufen Motor", "Stufen Raeder", "Bremse" };
const int items = 3;
const int subItems = 6;
const String v = "1.4";
//Roboter
const int timeout = 1000;                    // WiFi Connection Timeout

//************************************* VARIABELN ****************************************************//
//LCD Menu
LiquidCrystal_I2C lcd(0x27, 16, 2);
byte value[] {default_max_speed, default_kurve_speed, default_joystick, default_stufenMotor, default_stufenReader, default_bremse};
int menuItem = 0;
int menuSubItem = 0;
int sub = 0;
boolean bLight = true;
boolean refresh = true;

//Roboter
int controlMode = 0 ;                   // Kontroll Modus 0: Joystick, 1: XBee
long t1_1 , t1_2, t2_1, t2_2;           // Timervariablen
String stringRead, num1, num2;
int servoVal, motorVal, motorSpeed, motorSpeedInner, servoAngle;
Servo servoLB, servoLF, servoRF, servoRB;

void setup() {
  //************************************** Debug *****************************************************//
  /*
  for (int i = 0 ; i <= subItems ; i++) {
    EEPROM.write(i, value[i]);
  }
  */
  //************************************** Debug *****************************************************//

  //*************************************** ROBOTER *****************************************************//
  servoLF.attach(servo_left_front, servo_min_pulse, servo_max_pulse);
  servoLB.attach(servo_left_back, servo_min_pulse, servo_max_pulse);
  servoRF.attach(servo_right_front, servo_min_pulse, servo_max_pulse);
  servoRB.attach(servo_right_back, servo_min_pulse, servo_max_pulse);

  pinMode(dir_motor_left_front, OUTPUT); //Dir motor LF
  pinMode(hb_motor_left_front, OUTPUT); //Hand-brake motor LF
  pinMode(dir_motor_left_back, OUTPUT); //Dir motor LB
  pinMode(hb_motor_left_back, OUTPUT); //Hand-brake motor LB
  pinMode(dir_motor_right_front, OUTPUT); //Dir motor RF
  pinMode(hb_motor_right_front, OUTPUT); //Hand-brake motor RF
  pinMode(dir_motor_right_back, OUTPUT); //Dir motor RB
  pinMode(hb_motor_right_back, OUTPUT); //Hand-brake motor RB
  pinMode(status_led, OUTPUT);

  digitalWrite(status_led, HIGH);
  t1_1 = millis();
  t2_1 = millis();

  Serial1.begin(115200);
  //************************************** LCD MENU *****************************************************//
  pinMode(button_up, INPUT_PULLUP);
  pinMode(button_down, INPUT_PULLUP);
  pinMode(button_left, INPUT_PULLUP);
  pinMode(button_right, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.print("RoboBoy v" + v);
  delay(1000);
  lcd.clear();

  //Einstellungen aus dem Speicher laden
  if (EEPROM.read(0) != 255) {
    for (int i = 0 ; i <= subItems ; i++) {
      value[i] = EEPROM.read(i);
    }
  }
}

void loop() {
  checkButtons();
  roboter();
  showMenu();
  t1_2 = millis();
}

void roboter() {
  checkControlMode();
  //delay(50);
  //************************************** Debug *****************************************************//
  //************************************** Debug *****************************************************//

  //**************************************** LENKUNG ***************************************************//
  //**************************************** Gerade ***************************************************//
  servoAngle = 100 /  value[4] * map(abs(servoVal), 0, 100, 0 , value[4]);
  if (servoVal == 0) {              //Gerade
    motorSpeed = value[0] /  value[3] * map(abs(motorVal), 0, 100, 0 , value[3]);
    servoLF.writeMicroseconds(1405);
    servoLB.writeMicroseconds(1515);
    servoRF.writeMicroseconds(1435);
    servoRB.writeMicroseconds(1380);
    if (motorVal > 0) {               //Vorwärts
      brakeOff();
      forward();
      analogWrite(g_motor_left_front, motorSpeed);
      analogWrite(g_motor_left_back, motorSpeed);
      analogWrite(g_motor_right_front, motorSpeed);
      analogWrite(g_motor_right_back, motorSpeed);
      delay(20);
    } else if (motorVal < 0) {        //Rückwärts
      brakeOff();
      backward();
      analogWrite(g_motor_left_front, motorSpeed);
      analogWrite(g_motor_left_back, motorSpeed);
      analogWrite(g_motor_right_front, motorSpeed);
      analogWrite(g_motor_right_back, motorSpeed);
      delay(20);
    }
    //**************************************** Links ***************************************************//
  } else if (servoVal < 0) {
    motorSpeed = value[1] /  value[3] * map(abs(motorVal), 0, 100, 0 , value[3]);
    motorSpeedInner = motorSpeed * 43 / 100;
    servoLF.writeMicroseconds(map(servoAngle, 0, 100, 1405, 1824));
    servoLB.writeMicroseconds(map(servoAngle, 0, 100, 1515, 1688));
    servoRF.writeMicroseconds(map(servoAngle, 0, 100, 1435, 1045));
    servoRB.writeMicroseconds(map(servoAngle, 0, 100, 1380, 1205));
    if (motorVal > 0) {               //Vorwärts
      brakeOff();
      forward();
      analogWrite(g_motor_left_front, motorSpeedInner);
      analogWrite(g_motor_left_back, motorSpeedInner);
      analogWrite(g_motor_right_front, motorSpeed);
      analogWrite(g_motor_right_back, motorSpeed);
      delay(20);
    } else if (motorVal < 0) {        //Rückwärts
      brakeOff();
      backward();
      analogWrite(g_motor_left_front, motorSpeedInner);
      analogWrite(g_motor_left_back, motorSpeedInner);
      analogWrite(g_motor_right_front, motorSpeed);
      analogWrite(g_motor_right_back, motorSpeed);
      delay(20);
    }
    //**************************************** Rechts ***************************************************//
  } else if (servoVal > 0) {
    motorSpeed = value[1] /  value[3] * map(abs(motorVal), 0, 100, 0 , value[3]);
    motorSpeedInner = motorSpeed * 43 / 100;
    servoLF.writeMicroseconds(map(servoAngle, 0, 100, 1405, 1219));
    servoLB.writeMicroseconds(map(servoAngle, 0, 100, 1515, 1090));
    servoRF.writeMicroseconds(map(servoAngle, 0, 100, 1435, 1605));
    servoRB.writeMicroseconds(map(servoAngle, 0, 100, 1380, 1803));
    if (motorVal > 0) {               //Vorwärts
      brakeOff();
      forward();
      analogWrite(g_motor_left_front, motorSpeed);
      analogWrite(g_motor_left_back, motorSpeed);
      analogWrite(g_motor_right_front, motorSpeedInner);
      analogWrite(g_motor_right_back, motorSpeedInner);
      delay(20);
    } else if (motorVal < 0) {        //Rückwärts
      brakeOff();
      backward();
      analogWrite(g_motor_left_front, motorSpeed);
      analogWrite(g_motor_left_back, motorSpeed);
      analogWrite(g_motor_right_front, motorSpeedInner);
      analogWrite(g_motor_right_back, motorSpeedInner);
      delay(20);
    }
  }
  //**************************************** Anhalten ***************************************************//
  if (motorVal == 0) {
    analogWrite(g_motor_left_front, 0);
    analogWrite(g_motor_left_back, 0);
    analogWrite(g_motor_right_front, 0);
    analogWrite(g_motor_right_back, 0);
    if (value[5]==1) brakeOn();
  }
}

void showMenu() {
  switch (sub) {
    case 0:                           // Main Menü darstellen
      if (refresh) {
        lcd.clear();
        lcd.print(menu[menuItem]);
        refresh = false;
      }
      break;

    case 1:                          // 1. Untermenü  darstellen
      if (menuItem == 0) {           // "Infos" anzeigen
        if (refresh) {
          lcd.clear();
          lcd.print("RoboBoy v" + v);
          lcd.setCursor(0, 1);
          lcd.print("By Philip & Bang");
          refresh = false;
        }
      } else {
        if (refresh) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("->" + menu[menuItem]);
        }
      }
      switch (menuItem) {
        case 0:                    // "Infos"
          break;
        case 1:                    // Einstellungen
          if (refresh) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("->" + menu[menuItem]);
            lcd.setCursor(0, 1);
            lcd.print(settings[menuSubItem]);
            refresh = false;
          }
          break;
      }
      break;
    case 2:                          // 2. Untermenü  darstellen
      switch (menuItem) {
        case 0:                      // "Infos"
          if (refresh) {
            refresh = false;
          }
          break;
        case 1:                     // Einstellungen Wert verändern
          if (refresh) {
            lcd.clear();
            lcd.print("->" + settings[menuSubItem] + ":");
            lcd.setCursor(0, 1);
            lcd.print(value[menuSubItem]);
            refresh = false;
          }
          break;
      }
      break;
  }
}
