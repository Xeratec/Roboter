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
  digitalWrite(dir_motor_left_front, HIGH);
  digitalWrite(dir_motor_left_back, HIGH);
  digitalWrite(dir_motor_right_front, HIGH);
  digitalWrite(dir_motor_right_back, HIGH);
}

void backward() {
  digitalWrite(dir_motor_left_front, LOW);
  digitalWrite(dir_motor_left_back, LOW);
  digitalWrite(dir_motor_right_front, LOW);
  digitalWrite(dir_motor_right_back, LOW);
}

void serialEvent1() {
  stringRead = "";
  while (Serial1.available()) {
    delay(3);
    if (Serial1.available() > 0) {
      char c = Serial1.read();
      stringRead += c;
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
        num1 = stringRead.substring(0, delimiter);
        num2 = stringRead.substring(delimiter + 1, stringRead.length());

        servoVal = num1.toInt();
        motorVal = num2.toInt();
      }
    }
  }
}

void checkControlMode() {
  t1_2 = millis();
  if (controlMode == 0) {
    digitalWrite(led_yellow, HIGH);
    if (value[2] == 1) {
      servoVal = -1 * map(analogRead(joystickX), 0, 1023, -100, 100);
      motorVal = -1 * map(analogRead(joystickY), 0, 1023, -100, 100);
      if (motorVal > -5 & motorVal < 5) motorVal = 0;
      if (servoVal > -5 & servoVal < 5) servoVal = 0;
    }
  } else if (controlMode == 1) {
    t2_2 = millis();
    //Status Led blinkt
    if (t1_2 - t1_1 > 500) {
      digitalWrite(led_yellow, !digitalRead(led_yellow));
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
      t_backlight = millis();
      if (!bLight) {
        lcd.backlight();
        bLight = true;
      }

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
                if (value[menuSubItem] == 255) {
                  value[menuSubItem] = 5;
                } else if (value[menuSubItem] < 255) value[menuSubItem] += 5;
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
      t_backlight = millis();
      if (!bLight) {
        lcd.backlight();
        bLight = true;
      }

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
                  value[menuSubItem] = 255;
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
      t_backlight = millis();
      if (!bLight) {
        lcd.backlight();
        bLight = true;
      }

      switch (sub) {
        case 0:                           // Main Menü Backlight an-/ausschalten
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
      t_backlight = millis();
      if (!bLight) {
        lcd.backlight();
        bLight = true;
      }

      switch (sub) {
        case 0:                          // Main Menuins 1. Untermenü wechseln
          sub = 1;
          lcd.clear();
          lcd.print("->" + menu[menuItem]);
          if (menuItem == 2) {          // Funktions Test
            delay(250);
            funktionsTest();
            menuItem = 0;
            sub = 0;
          }
          if (menuItem == 3) {          // Reset
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
  //Entprellen des Tasters BACKLIGHT
  value_1, value_2 = 0;
  value_1 = digitalRead(button_backlight);
  if (!value_1) {
    delay(50);
    value_2 = digitalRead(button_backlight);
    if (!value_2) {
      t_backlight = millis();

      if (bLight) {
        lcd.noBacklight();
        bLight = false;
      } else {
        lcd.backlight();
        bLight = true;
      }
      do { } while (!digitalRead(button_backlight));
    }
  }
  //Entprellen des Tasters ENTER (Debug)
  value_1, value_2 = 0;
  value_1 = digitalRead(button_enter);
  if (!value_1) {
    t_backlight = millis();
    if (!bLight) {
      lcd.backlight();
      bLight = true;
    }
    
    value_2 = digitalRead(button_enter);
    if (!value_2) {
      do { } while (!digitalRead(button_enter));
    }
  }
}

void funktionsTest() {
  brakeOff();

  digitalWrite(led_red, HIGH);
  lcd.clear();
  lcd.print("Motor Test");
  delay(1000);
  lcd.clear();
  lcd.print("Zw. den Schritten");
  lcd.setCursor(0, 1);
  lcd.print("Enter druecken");
  delay(3000);

  lcd.clear();
  lcd.print("Links Hinten");
  lcd.setCursor(0, 1);
  lcd.print("Vorwaerts");
  forward();
  analogWrite(g_motor_left_front, 20);
  delay(1500);
  analogWrite(g_motor_left_front, 0);
  lcd.clear();
  lcd.print("Links Hinten");
  lcd.setCursor(0, 1);
  lcd.print("Rueckwaerts");
  delay(1000);
  backward();
  analogWrite(g_motor_left_front, 20);
  delay(1500);
  analogWrite(g_motor_left_front, 0);

  do { } while (digitalRead(button_enter));

  lcd.clear();
  lcd.print("Links Vorne");
  lcd.setCursor(0, 1);
  lcd.print("Vorwaerts");
  forward();
  analogWrite(g_motor_left_back, 20);
  delay(1500);
  analogWrite(g_motor_left_back, 0);
  lcd.clear();
  lcd.print("Links Vorne");
  lcd.setCursor(0, 1);
  lcd.print("Rueckwaerts");
  delay(1000);
  backward();
  analogWrite(g_motor_left_back, 20);
  delay(1500);
  analogWrite(g_motor_left_back, 0);

  do { } while (digitalRead(button_enter));

  lcd.clear();
  lcd.print("Rechts Vorne");
  forward();
  lcd.setCursor(0, 1);
  lcd.print("Vorwaerts");
  analogWrite(g_motor_right_front, 20);
  delay(1500);
  analogWrite(g_motor_right_front, 0);
  lcd.clear();
  lcd.print("Rechts Vorne");
  lcd.setCursor(0, 1);
  lcd.print("Rueckwaerts");
  delay(1000);
  backward();
  analogWrite(g_motor_right_front, 20);
  delay(1500);
  analogWrite(g_motor_right_front, 0);

  do { } while (digitalRead(button_enter));

  lcd.clear();
  lcd.print("Rechts Hinten");
  forward();
  lcd.setCursor(0, 1);
  lcd.print("Vorwaerts");
  analogWrite(g_motor_right_back, 20);
  delay(1500);
  analogWrite(g_motor_right_back, 0);
  lcd.clear();
  lcd.print("Rechts Hinten");
  lcd.setCursor(0, 1);
  lcd.print("Rueckwaerts");
  delay(1000);
  backward();
  analogWrite(g_motor_right_back, 20);
  delay(1500);
  analogWrite(g_motor_right_back, 0);

  brakeOn();
  do { } while (digitalRead(button_enter));

  lcd.clear();
  lcd.print("Servo Test");

  digitalWrite(led_red, LOW);
  delay(200);
  digitalWrite(led_red, HIGH);
  delay(200);
  digitalWrite(led_red, LOW);
  delay(200);
  digitalWrite(led_red, HIGH);

  lcd.clear();
  lcd.print("Links Vorne");
  lcd.setCursor(0, 1);
  lcd.print("Links");
  servoLF.writeMicroseconds(map(100, 0, 100, 1405, 1824));
  delay(1500);
  servoLF.writeMicroseconds(map(0, 0, 100, 1405, 1824));
  delay(1000);
  lcd.clear();
  lcd.print("Links Vorne");
  lcd.setCursor(0, 1);
  lcd.print("Rechts");
  servoLF.writeMicroseconds(map(100, 0, 100, 1405, 1219));
  delay(1500);
  servoLF.writeMicroseconds(map(0, 0, 100, 1405, 1219));

  do { } while (digitalRead(button_enter));

  lcd.clear();
  lcd.print("Links Hinten");
  lcd.setCursor(0, 1);
  lcd.print("Links");
  servoLB.writeMicroseconds(map(100, 0, 100, 1435, 1045));
  delay(1500);
  servoLB.writeMicroseconds(map(0, 0, 100, 1435, 1045));
  delay(1000);
  lcd.clear();
  lcd.print("Links Hinten");
  lcd.setCursor(0, 1);
  lcd.print("Rechts");
  servoLB.writeMicroseconds(map(100, 0, 100, 1435, 1605));
  delay(1500);
  servoLB.writeMicroseconds(map(0, 0, 100, 1435, 1605));

  do { } while (digitalRead(button_enter));

  lcd.clear();
  lcd.print("Rechts Vorne");
  lcd.setCursor(0, 1);
  lcd.print("Rechts");
  servoRF.writeMicroseconds(map(100, 0, 100, 1515, 1090));
  delay(1500);
  servoRF.writeMicroseconds(map(0, 0, 100, 1515, 1090));
  delay(1000);
  lcd.clear();
  lcd.print("Rechts Vorne");
  lcd.setCursor(0, 1);
  lcd.print("Links");
  servoRF.writeMicroseconds(map(100, 0, 100, 1515, 1688));
  delay(1500);
  servoRF.writeMicroseconds(map(0, 0, 100, 1515, 1688));

  do { } while (digitalRead(button_enter));

  lcd.clear();
  lcd.print("Rechts Hinten");
  lcd.setCursor(0, 1);
  lcd.print("Rechts");
  servoRB.writeMicroseconds(map(100, 0, 100, 1380, 1803));
  delay(1500);
  servoRB.writeMicroseconds(map(0, 0, 100, 1380, 1803));
  delay(1000);
  lcd.clear();
  lcd.print("Rechts Hinten");
  lcd.setCursor(0, 1);
  lcd.print("Links");
  servoRB.writeMicroseconds(map(100, 0, 100, 1380, 1205));
  delay(1500);
  servoRB.writeMicroseconds(map(0, 0, 100, 1380, 1205));

  delay(200);
  digitalWrite(led_red, LOW);
  delay(200);
  digitalWrite(led_red, HIGH);
  delay(200);
  digitalWrite(led_red, LOW);
  delay(200);
  digitalWrite(led_red, HIGH);
  delay(200);
  digitalWrite(led_red, LOW);
  delay(200);
  digitalWrite(led_red, HIGH);
  delay(200);
  digitalWrite(led_red, LOW);
  delay(200);

  lcd.clear();
  refresh = true;
}

