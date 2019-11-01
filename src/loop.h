
void actionButtonForSettingsScreen(char button) {
  changeDisplay = 1;
  switch (button) {
    case 'U':
      selectedLine--;
      selectedLine = selectedLine % 3;
      break;
    case 'D':
      selectedLine++;
      selectedLine = selectedLine % 3;
      break;
    case 'R':
      if (selectedLine == 0) {
        params.stepCm++;
      } else if (selectedLine == 1) {
        params.turnSteps++;
      } else {
        params.lineSteps++;
      }
      break;
    case '+':
      if (selectedLine == 0) {
        params.stepCm += 10;
      } else if (selectedLine == 1) {
        params.turnSteps += 10;
      } else {
        params.lineSteps += 10;
      }
      break;
    case 'L':
      if (selectedLine == 0) {
        params.stepCm--;
      } else if (selectedLine == 1) {
        params.turnSteps--;
      } else {
        params.lineSteps--;
      }
      break;
    case '-':
      if (selectedLine == 0) {
        params.stepCm -= 10;
      } else if (selectedLine == 1) {
        params.turnSteps -= 10;
      } else {
        params.lineSteps -= 10;
      }
      break;
    case 's':
    case 'G':
      saveParams();
      selectedMenu = CTRL_MENU;
      break;
    case 'C':
      loadParams();
      selectedMenu = CTRL_MENU;
      break;
  }
}

void actionButtonForScreen(char button) {
  if (selectedMenu == START_MENU) {
    selectedMenu = CTRL_MENU;
    changeDisplay = 1;
  } else if (selectedMenu == CTRL_MENU) {
    changeDisplay = 1;
    Serial.print(F("New char : "));
    Serial.println(button);
    if ( button > 47  && button < 58) // it's a number
    {
      consecutive_numbers++;
      if (consecutive_numbers > MAX_CONSECUTIVE_NUMBERS)
      {
        Serial.println(F("number too hight"));
        return;
      }
      else
      {
        cmd[cmd_l++] = button;
        return;
      }
    }
    else
    {
      switch (button) {
        case '@':
          if (ledOn) {
            switchOffLED();
            ledOn=false;
          } else  {
            switchOnLED();
            ledOn=true;
          }
          break;
        case 'S':
          selectedMenu = SETTINGS_MENU;
          changeDisplay = 1;
          break;
        case 'G':
          stepDelay = 800;
          selectedMenu = RUNNING_MENU;
          isMoving = true;
          commandLaunched = 0;

          max_num_cmd = num_of_cmd;
          num_of_cmd = 0;
          launchNextCommand();

          break;
        case 'A':
          cmd_l = 0;
          commandLaunched = 0;
          num_of_cmd = 0;
          break;
        case 'C':
          while (cmd_l > 0  &&  cmd[cmd_l - 1] > 47  && cmd[cmd_l - 1]  < 58)
          {
            cmd_l--;  //remove all number;
          }
          if (cmd_l > 0) {
            cmd_l--;
            num_of_cmd--;
          }
          break;
        case 'B' :
        case 'a' :
        case 'c' : 
        case 'r' :
        case 'E' :
        case 'U' :
        case 'D' :
        case 'L' :
        case 'R' :
        case 'W' :
        case '!' :
        case '|' :
        case 'P' :
          if (cmd_l < MAX_COMMANDS) {
            cmd[cmd_l++] = button;
            num_of_cmd++;
          } else {
            Serial.println(F("too many commands"));
          }
          break;
        case 0 :
        case '+' :
        case '-' :
        case 's' :
          changeDisplay = 0;
          break;
        default:
          Serial.println(F("Unknown Command"));
#ifdef HAVE_BLUETOOTH
          BTSerie.println(F("Unknown Command"));
#endif
          break;
      }
      consecutive_numbers = 0;
    }
  } else if (selectedMenu == SETTINGS_MENU) {
    actionButtonForSettingsScreen(button);
  } else if (selectedMenu == RUNNING_MENU) {
    disableMotors();
    isMoving = false;
    lcd.clear();
    lcd.setBacklight(HIGH);
    lcd.print(F("Arret!"));
    delay(1000);
    selectedMenu = CTRL_MENU;
    changeDisplay = 1;
  } else if (selectedMenu == OFF_MENU) {
    selectedMenu = previousMenu;
    changeDisplay = 1;
  }
}





void loop() {
  long currentTime = millis();
  int buttonId = getPressedButton();
  char button = 0;
  if (buttonId >= 0) {
    button = buttonsMap[buttonId];
    if (button != 0) {
      actionButtonForScreen(button);
    }
  } else {
#ifdef HAVE_BLUETOOTH
    while (BTSerie.available()) {
      selectedMenu = CTRL_MENU;
      button = BTSerie.read();
      Serial.println(button);
      if (button != 0) {
        actionButtonForScreen(button);
      }
    }
#endif

    while (Serial.available()) {
      selectedMenu = CTRL_MENU;
      button = Serial.read();
      //Serial.println(button);
      if (button != 0) {
        actionButtonForScreen(button);
      }
    }
  }

  if (isMoving) {
    if (isCommandTerminated()) {
      Serial.println(F("step delay"));
      if (! launchNextCommand()) {
        Serial.println(F("program terminated"));
        isMoving = false;
        disableMotors();
        lcd.clear();
        lcd.setBacklight(HIGH);
        lcd.print(F("fin !"));
        
#ifdef HAVE_BLUETOOTH
        BTSerie.println(F("fin !"));
#endif
        delay(1000);
        selectedMenu = CTRL_MENU;
        changeDisplay = 1;
      }
    }
  }

  updateScreen();

  // Screen off after 4s
  if (currentTime > lastChangeDisplay + SCREEN_TIMEOUT * 1000) {
    if (selectedMenu != OFF_MENU) {
      previousMenu = selectedMenu;
      selectedMenu = OFF_MENU;
      changeDisplay = 1;
    }
  }

}
