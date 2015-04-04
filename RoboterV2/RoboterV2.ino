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
#define joyX 4
#define joyY 5
#define button_ControlMode 52
#define button_up 30
#define button_down 31
#define button_left 32
#define button_right 33
#define status_led 50
// Motor
#define dir_motor_left_front 22
#define hb_motor_left_front 23
#define dir_motor_left_back 24
#define hb_motor_left_back 25
#define dir_motor_right_front 26
#define hb_motor_right_front 27
#define dir_motor_right_back 28
#define hb_motor_right_back 29
// Servos
#define servo_left_front 40
#define servo_left_back 41
#define servo_right_front 42
#define servo_right_back 43
#define servo_max_pulse 2100
#define servo_min_pulse 900

//************************************* KONSTANTEN ****************************************************//
//LCD Menu
const String menu[] = {"Info", "Einstellungen"};
const String settings[] = {"Max. Speed", "Kurve Speed", "Stufen Motor", "Stufen Räder"};
const int items = 2;
const int subItems = 4;
const String v = "1.4";
//Roboter
const int timeout = 1000;                    // WiFi Connection Timeout

//************************************* VARIABELN ****************************************************//
//LCD Menu
LiquidCrystal_I2C lcd(0x27, 16, 2);
byte value[] {50, 80, 4, 4};
int menuItem = 0;
int menuSubItem = 0;
int sub = 0;
boolean bLight = true;
boolean refresh = true;

//Roboter
int controlMode = 0 ;                   // Kontroll Modus 0: Joystick, 1: XBee
long t1_1 , t1_2, t2_1, t2_2;           // Timervariablen
String stringRead, num1, num2;
int serialServo, serialMotor;
Servo servoLB, servoLF, servoRF, servoRB;

void setup() {
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
  //************************************** LCD MENU *****************************************************//
  pinMode(button_up, INPUT_PULLUP);
  pinMode(button_down, INPUT_PULLUP);
  pinMode(button_left, INPUT_PULLUP);
  pinMode(status_led, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.print("RoboBoy v" + v);
  delay(1000);
  lcd.clear();

  //Einstellungen aus dem Speicher laden
  if (EEPROM.read(0) != 255) {
    value[0] = EEPROM.read(0);
    value[1] = EEPROM.read(1);
    value[2] = EEPROM.read(2);
    value[3] = EEPROM.read(3);
  }
}

void loop() {
  checkButtons();
  roboter();
  showMenu();
  t1_2 = millis();
}

void showMenu() {
  
}

void roboter() {
  
}

void checkButtons() {
  //Entprellen des Tasters RIGHT
  byte value_1, value_2 = 0;
  value_1 = digitalRead(button_right);
  if (!value_1) {
    delay(50);
    value_2 = digitalRead(button_right);
    if (!value_2) {
      switch (sub) {
        case 0:
          refresh = true;
          if (menuItem == items - 1) {
            menuItem = 0;
          } else {
            menuItem++;
          }

          break;
        case 1:
          switch (menuItem) {
            case 0:
              break;
            case 1:
              refresh = true;
              if (menuSubItem == subItems - 1) {
                menuSubItem = 0;
              } else {
                menuSubItem++;
              }
              break;
            case 2:
              break;
          }
          break;
        case 2:
          switch (menuItem) {
            case 0:
              break;
            case 1:
              if (value[menuSubItem] == 100) {
                value[menuSubItem] = 5;
              } else if (value[menuSubItem] < 100) value[menuSubItem] += 5;
              EEPROM.write(0, value[0]);
              EEPROM.write(1, value[1]);
              refresh = true;
              break;
            case 2:
              break;
          }
          break;
      }
      do { } while (!digitalRead(button_right));
    }
  }

  //Entprellen des Tasters LEFT
  value_1, value_2 = 0;
  value_1 = digitalRead(button_left);
  if (!value_1) {
    delay(50);
    value_2 = digitalRead(button_left);
    if (!value_2) {
      switch (sub) {
        case 0:
          refresh = true;
          if (menuItem == 0) {
            menuItem = items - 1;
          } else {
            menuItem--;
          }
          break;
        case 1:
          switch (menuItem) {
            case 0:
              break;
            case 1:
              refresh = true;
              if (menuSubItem == 0) {
                menuSubItem = subItems - 1;
              } else {
                menuSubItem--;
              }
              break;
            case 2:
              break;
          }
          break;
        case 2:
          switch (menuItem) {
            case 0:
              break;
            case 1:
              if (value[menuSubItem] == 5) {
                value[menuSubItem] = 100;
              } else if (value[menuSubItem] > 5) value[menuSubItem] -= 5;
              EEPROM.write(0, value[0]);
              EEPROM.write(1, value[1]);
              refresh = true;
              break;
            case 2:
              break;
          }
          break;
      }
      do { } while (!digitalRead(button_left));
    }
  }

  //Entprellen des Tasters UP
  value_1, value_2 = 0;
  value_1 = digitalRead(button_up);
  if (!value_1) {
    delay(50);
    value_2 = digitalRead(button_up);
    if (!value_2) {
      switch (sub) {
        case 0:
          if (bLight) {
            lcd.noBacklight();
            bLight = false;
          } else {
            lcd.backlight();
             bLight = true;
          }
          break;
        case 1:
          sub = 0;
          refresh = true;
          break;
        case 2:
          sub = 1;
          refresh = true;
          break;
      }
      do { } while (!digitalRead(button_up));
    }
  }

  //Entprellen des Tasters DOWN
  value_1, value_2 = 0;
  value_1 = digitalRead(button_down);
  if (!value_1) {
    delay(50);
    value_2 = digitalRead(button_down);
    if (!value_2) {
      switch (sub) {
        case 0:
          sub = 1;
          lcd.clear();
          lcd.print("->" + menu[menuItem]);
          refresh = true;
          break;
        case 1:
          if (menuItem == 1) {
            lcd.clear();
            lcd.print("->" + menu[menuItem]);
            lcd.setCursor(0, 1);
            lcd.print("->" + settings[menuSubItem]);
            sub = 2;
            refresh = true;
          }
          break;
      }
      do { } while (!digitalRead(button_down));
    }
  }

}
