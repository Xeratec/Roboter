void brakeOff() {
  digitalWrite(hb_motor_left_front, LOW);
  digitalWrite(hb_motor_left_back, LOW);
  digitalWrite(hb_motor_right_front, LOW);
  digitalWrite(hb_motor_right_back, LOW);
}

void brakeOn() {
  digitalWrite(hb_motor_left_front, HIGH);
  digitalWrite(hb_motor_left_back, HIGH);
  digitalWrite(hb_motor_right_front, HIGH);
  digitalWrite(hb_motor_right_back, HIGH);
}

void forward() {
  digitalWrite(dir_motor_left_front, LOW);
  digitalWrite(dir_motor_left_back, LOW);
  digitalWrite(dir_motor_right_front, LOW);
  digitalWrite(dir_motor_right_back, LOW);
}

void backward() {
  digitalWrite(dir_motor_left_front, HIGH);
  digitalWrite(dir_motor_left_back, HIGH);
  digitalWrite(dir_motor_right_front, HIGH);
  digitalWrite(dir_motor_right_back, HIGH);
}

void serialEvent1() {
  stringRead = "";
  while (Serial1.available()) {
    delay(3);  //delay to allow buffer to fill
    if (Serial1.available() > 0) {
      char c = Serial1.read();  //gets one byte from serial buffer
      stringRead += c; //makes the string readString
    }
  }

  if (stringRead.equals("ST;")) {
    controlMode = 1;
    t2_1 = millis();
  } else {
    if (controlMode == 1) {
      stringRead = stringRead.substring(0, stringRead.indexOf(";"));
      if (stringRead.length() > 0) {
        int delimiter = stringRead.indexOf(":");
        num1 = stringRead.substring(0, delimiter); //get the first four characters
        num2 = stringRead.substring(delimiter + 1, stringRead.length()); //get the next four characters

        servoVal = num1.toInt();
        motorVal = num2.toInt();
      }
    }
  }
}

void checkControlMode() {
  t1_2 = millis();
  if (controlMode == 0) {
    digitalWrite(status_led, HIGH);
    if (value[2] == 1) {
      servoVal = map(analogRead(joystickX), 0, 1023, -100, 100);
      motorVal = map(analogRead(joystickY), 0, 1023, -100, 100);
      if (motorVal > -5 & motorVal < 5) motorVal = 0;
      if (servoVal > -5 & servoVal < 5) servoVal = 0;
    }
  } else if (controlMode == 1) {
    t2_2 = millis();
    //Status Led blinkt
    if (t1_2 - t1_1 > 500) {
      digitalWrite(status_led, !digitalRead(status_led));
      t1_1 = t1_2;
    }
    if (t2_2 - t2_1 > timeout) {
      controlMode = 0;
      servoVal = motorVal = 0;
    }
  }
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
        case 0:                           // Main Menü Menüpunkt wechseln
          refresh = true;
          if (menuItem == items - 1) {
            menuItem = 0;
          } else {
            menuItem++;
          }

          break;
        case 1:                          // 1. Untermenü
          switch (menuItem) {
            case 0:                      // "Info"
              break;
            case 1:                      // "Einstellungen"
              refresh = true;
              if (menuSubItem == subItems - 1) {
                menuSubItem = 0;
              } else {
                menuSubItem++;
              }
              break;
          }
          break;
        case 2:                          // 2. Untermenü
          switch (menuItem) {
            case 0:
              break;
            case 1:                             // "Einstellugen" Wert erhöhen
              if (menuSubItem < 2) {            // Geschwinditkeit in 5er Schritten
                if (value[menuSubItem] == 100) {
                  value[menuSubItem] = 5;
                } else if (value[menuSubItem] < 100) value[menuSubItem] += 5;
              } else if (menuSubItem == 2) {    // Joystick nur 0 oder 1
                if (value[menuSubItem] == 1) {
                  value[menuSubItem] = 0;
                } else value[menuSubItem] = 1;
              } else if (menuSubItem == 5) {    // Bremse nur 0 oder 1
                if (value[menuSubItem] == 1) {
                  value[menuSubItem] = 0;
                } else value[menuSubItem] = 1;
              } else                 {          // Stufen in 1er Schritten
                if (value[menuSubItem] == 100) {
                  value[menuSubItem] = 1;
                } else if (value[menuSubItem] < 100) value[menuSubItem] += 1;
              }
              for (int i = 0 ; i <= subItems ; i++) {
                EEPROM.write(i, value[i]);
              }
              refresh = true;
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
        case 0:                           // Main Menü Menüpunkt wechseln
          refresh = true;
          if (menuItem == 0) {
            menuItem = items - 1;
          } else {
            menuItem--;
          }
          break;
        case 1:                          // 1. Untermenü
          switch (menuItem) {
            case 0:                      // "Info"
              break;
            case 1:                      // "Einstellungen"
              refresh = true;
              if (menuSubItem == 0) {
                menuSubItem = subItems - 1;
              } else {
                menuSubItem--;
              }
              break;
          }
          break;
        case 2:                          // 2. Untermenü
          switch (menuItem) {
            case 0:                     // "Info"
              break;
            case 1:                     // "Einstellugen" Wert verkleinern
              if (menuSubItem < 2) {    // Geschwinditkeit in 5er Schritten
                if (value[menuSubItem] == 5) {
                  value[menuSubItem] = 100;
                } else if (value[menuSubItem] > 5) value[menuSubItem] -= 5;
              } else if (menuSubItem == 2) {    // Joystick nur 0 oder 1
                if (value[menuSubItem] == 1) {
                  value[menuSubItem] = 0;
                } else value[menuSubItem] = 1;
              } else if (menuSubItem == 5) {    // Bremse nur 0 oder 1
                if (value[menuSubItem] == 1) {
                  value[menuSubItem] = 0;
                } else value[menuSubItem] = 1;
              } else {                // Stufen in 1er Schritten
                if (value[menuSubItem] == 1) {
                  value[menuSubItem] = 100;
                } else if (value[menuSubItem] > 1) value[menuSubItem] -= 1;
              }
              for (int i = 0 ; i <= subItems ; i++) {
                EEPROM.write(i, value[i]);
              }
              refresh = true;
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
        case 0:                           // Main Menü Backlight an-/ausschalten
          if (bLight) {
            lcd.noBacklight();
            bLight = false;
          } else {
            lcd.backlight();
            bLight = true;
          }
          break;
        case 1:                          // 1. Untermenü ins Main Menu wechseln
          sub = 0;
          refresh = true;
          break;
        case 2:                          // 2. Untermenü Menü ins 1. Untermenü wechseln
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
        case 0:                          // Main Menuins 1. Untermenü wechseln
          sub = 1;
          lcd.clear();
          lcd.print("->" + menu[menuItem]);
          if (menuItem == 2) {
            delay(250);
            value[0] = default_max_speed;
            value[1] = default_kurve_speed;
            value[2] = default_joystick;
            value[3] = default_stufenMotor;
            value[4] = default_stufenReader;
            value[5] = default_bremse;
            menuItem = 0;
            sub = 0;
          }
          refresh = true;
          break;
        case 1:                          // 1. Untermenü ins 1. Untermenü wechseln
          if (menuItem == 1) {           // Einstellunen
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
