#include <Arduino.h>

/*
   OoRoBoT code

   Get the following libraries (using the Arduino IDE library manager)
  - **AccelStepper** v1.57.1 by Mike McCauley
  - **LiquidCrystal I2C** v1.1.2 by Frank de Brabander
 
*/
#include <Wire.h>
#include <Servo.h>
#include "charset.h"
#include "buttons.h"


#define OOROBOT_VERSION "1.1.3"

#define SCREEN_TIMEOUT 25


// define this symbol to invert the motors direction
#define MAX_COMMANDS 512
#define MAX_LOOPS 10

// define this symbol to add bluetooth support
#define HAVE_BLUETOOTH

#ifdef HAVE_BLUETOOTH
# include <SoftwareSerial.h>
# define RxD 12
# define TxD 13
SoftwareSerial BTSerie(RxD, TxD);
#endif


// Menu index
#define START_MENU 0
#define SETTINGS_MENU 1
#define CTRL_MENU 2
#define RUNNING_MENU 3
#define OFF_MENU 4

#define MAX_STEPPER_SPEED 900
#define MIN_STEPPER_SPEED 200
#define WHEEL_SPACING_MM 132



int stepperSpeed = MIN_STEPPER_SPEED;


Servo penServo;

#include "lcd.h"
LCD lcd; // The I2C LCD object


int stepDelay = 800;
int steps1 = 0; // keep track of the step count for motor 1
int steps2 = 0; // keep track of the step count for motor 2
int isMoving = false;

char buttonsMap[] = {
  'L', 'G', 's', 'C', 0, 'R', '@', 'U', 'P', 'D', '|', '!',
  '-',  0, 'S', 'A', 0, '+', 0,  0,   0,   0,  0, 0
};

int previousMenu = CTRL_MENU;
int selectedMenu = START_MENU;
char cmd[MAX_COMMANDS + 1] = {};
int loopCounter[MAX_LOOPS] = {};
int loopPointer[MAX_LOOPS] = {};
int loopIndex=0;
int currentRadius=0;
boolean reverseOrientation=false;

unsigned char cmd_l = 0;
int changeDisplay = 1;
long lastChangeDisplay = 0;
unsigned int selectedLine = 0;
short commandLaunched = 0;
short consecutive_numbers = 0;
#define MAX_CONSECUTIVE_NUMBERS 3
short num_of_cmd = 0;
short max_num_cmd = 0;
long startMovement=0;
boolean ledOn=false;


void moveServo(int angle) {
  penServo.attach(3);
  penServo.write(angle);
  delay(200);
  penServo.detach();
}

#include "params.h"
#include "move.h"

#include "setup.h"

void switchOnLED() {
  Serial.println(F("switch on LED")); 
}


void switchOffLED() {
  Serial.println(F("switch off LED"));
}

char commandToDisplay(char c) {
  switch (c) {
    case 'U':
      return 1;
      break;
    case 'D':
      return 2;
      break;
    case 'L':
      return 3;
      break;
    case 'R':
      return 4;
      break;
    case 'P':
      return 5;
      break;
    default:
      return c;
  }
}


void updateScreen() {
  if (changeDisplay) {
    if (selectedMenu == START_MENU) {
      //lcd.setBacklight(HIGH);
      lcd.display();
      lcd.setCursor(0, 0);
      lcd.print(" OoRoBoT  " OOROBOT_VERSION);
      lcd.setCursor(0, 1);
      lcd.print(F("Pret \7 demarrer!"));
#ifdef HAVE_BLUETOOTH
      BTSerie.println("OoRoBoT " OOROBOT_VERSION);
      BTSerie.println(F("En attente de commandes"));
#endif
      previousMenu = CTRL_MENU;
      selectedMenu = CTRL_MENU;
    } else if (selectedMenu == CTRL_MENU) {
      lcd.setBacklight(HIGH);
      cmd[cmd_l] = 0;
#ifdef HAVE_BLUETOOTH
      BTSerie.println(cmd);
#endif
      lcd.clear();
      lcd.setCursor(0, 0);
      for (char i = 0 ; i < cmd_l ; i++) {
        if (i == 16) {
          lcd.setCursor(0, 1);
        }
        if (i<32) {
          lcd.print(commandToDisplay(cmd[i]));
        }
      }
    } else if (selectedMenu == SETTINGS_MENU) {
      lcd.setBacklight(HIGH);
      lcd.clear();
      int cm = params.stepCm / 10;
      int mm = params.stepCm - 10 * cm;
      int currentLine=selectedLine;
      int stepIdx=0;
      int turnIdx=1;
      int lineIdx=2;
      if (selectedLine==2) {
        stepIdx=2;
        turnIdx=2;
        lineIdx=0;  
      }
      if (stepIdx<2) {
        lcd.setCursor(0, stepIdx);
        lcd.print(F(" Distance:"));
        lcd.print(cm);
        lcd.print(F("."));
        lcd.print(mm);
        lcd.print(F("cm"));
      }      
      if (turnIdx<2) {
        lcd.setCursor(0, turnIdx);
        lcd.print(F(" 1/4Tour:"));
        lcd.print(params.turnSteps);
        lcd.print(F("pas"));
      }
      if (lineIdx<2) {
        lcd.setCursor(0, lineIdx);
        lcd.print(F(" 1cm:"));
        lcd.print(params.lineSteps);
        lcd.print(F("pas"));
        lcd.setCursor(0, lineIdx+1);
        lcd.print(F(" "));
        lcd.print(params.btName);
      }
      
      lcd.setCursor(0, selectedLine%2);
      lcd.print("\6");
#ifdef HAVE_BLUETOOTH
      if (selectedLine == 0) {
        BTSerie.print(F("Distance:"));
        BTSerie.print(cm);
        BTSerie.print(F("."));
        BTSerie.print(mm);
        BTSerie.println(F("cm"));
      } else {
        BTSerie.print(F("1/4Tour:"));
        BTSerie.print(params.turnSteps);
        BTSerie.println(F("pas"));
      }
#endif

    } else if (selectedMenu == CTRL_MENU) {
      lcd.setBacklight(HIGH);
    } else if (selectedMenu == OFF_MENU) {
      lcd.setBacklight(LOW);
    }
    lastChangeDisplay = millis();
  }
  changeDisplay = 0;
}


int startLoop=-1;
int remainingLoop=0;

short getStepSize(char* cmd,  short* commandLaunched)
{
  char command = cmd[*commandLaunched];
  int stepsize = 0;
  for (short i = 0; i < MAX_CONSECUTIVE_NUMBERS ; i++)
  {
    if (*commandLaunched + 1 < MAX_COMMANDS - 1 )
    {
      if (cmd[*commandLaunched + 1] > 47 && cmd[*commandLaunched + 1] < 58)
      {
        stepsize = stepsize * 10 + cmd[*commandLaunched + 1 ] - 48;
        *commandLaunched = *commandLaunched + 1;
      }
    }
  }

  if (stepsize == 0)
  {
    switch (command) {
      case 'W':
        stepsize = stepDelay;
        break;
      case 'U':
      case 'D':
        stepsize = params.stepCm; //10cm
        break;
      case 'L':
      case 'R':
        stepsize = 90;  // 90°;
        break;
      case 'P':
        stepsize=stepDelay*2;
        break;
    }
  }

  return stepsize;

}



boolean launchNextCommand() {
  if (commandLaunched >= cmd_l) {
    return false;
  } else {
    disableMotors();
    lcd.clear();
    if (stepDelay>100) {
      lcd.setBacklight(HIGH);
      lcd.print((num_of_cmd + 1));
      lcd.print(F(" sur "));
      lcd.print(max_num_cmd);
      lcd.print(F(" : "));
      lcd.print(commandToDisplay(cmd[commandLaunched]));
    }
#ifdef HAVE_BLUETOOTH
    BTSerie.println(cmd[commandLaunched]);
#endif
    Serial.print(F("etape "));
    Serial.print((num_of_cmd + 1));
    Serial.print(F(" sur "));
    Serial.print(max_num_cmd);
    Serial.print(F(" : "));
    Serial.println(cmd[commandLaunched]);

    num_of_cmd++;
    delay(stepDelay);
    lcd.setBacklight(LOW);
    
    enableMotors();

    char command = cmd[commandLaunched];
    short stepSize = getStepSize(cmd, &commandLaunched);

    Serial.print(F("stepSize :"));
    Serial.println(stepSize);
    switch (command) {
      case 'W':
        Serial.println(F("set waiting step delay"));
        stepDelay=stepSize;
        break;
      case 'U':
        stepForward(stepSize);
        break;
      case 'D':
        Serial.println(F("stepBackward"));
        stepBackward(stepSize);
        break;
      case 'L':
        Serial.println(F("turnLeft"));
        turnLeft(stepSize);
        break;
      case 'R':
        Serial.println(F("turnRight"));
        turnRight(stepSize);
        break;
      case 'P':
        Serial.println(F("pause"));
        delay(stepSize);
        break;
      case '!' :
        Serial.println(F("pen down"));
        moveServo(0);
        break;
      case '|' :
        Serial.println(F("pen up"));  
        moveServo(30);
        break;
      case 'c' :
        currentRadius = stepSize;
        break;
      case 'r':
        if (stepSize==1) {
          reverseOrientation=true;
        } else {
          reverseOrientation=false;         
        }
        break;
      case 'a' :
        Serial.println(F("doCircle"));
        doCircle(currentRadius, stepSize, reverseOrientation);
        break;
      case 'B':
        Serial.println(F("begin loop"));
        if (loopIndex>=MAX_LOOPS) {
          Serial.println(F("too many loops included"));
        } else {
          loopCounter[loopIndex] = stepSize;
          loopPointer[loopIndex] = commandLaunched;
          loopIndex++;
          startLoop=commandLaunched;
          remainingLoop=stepSize;
        }
        break;
      case 'E':
        Serial.println(F("end loop"));
        if (loopCounter[loopIndex-1]>1) {
          commandLaunched=loopPointer[loopIndex-1];
          loopCounter[loopIndex-1]--;
        } else {
          if (loopIndex>0) {
            loopIndex--;
          }
        }
        break;
    }
    commandLaunched++;
    return true;
  }
}

boolean isCommandTerminated() {
  /*
  int diff = (millis() - startMovement);
  if (diff>=100) {
    startMovement=millis();
    stepperSpeed+=100;
    if (stepperSpeed>MAX_STEPPER_SPEED) {
      stepperSpeed=MAX_STEPPER_SPEED;
    } else {
      Serial.print(F("speed:"));
      Serial.println(stepperSpeed);
      stepper2.setSpeed(stepperSpeed);
      stepper1.setSpeed(stepperSpeed);
    }
  }
  */
  stepper2.setSpeed(MAX_STEPPER_SPEED);    
  stepper1.setSpeed(MAX_STEPPER_SPEED);

  steps1 = stepper1.distanceToGo();
  steps2 = stepper2.distanceToGo();
  stepper1.runSpeedToPosition();
  stepper2.runSpeedToPosition();

  if (steps1 == 0 && steps2 == 0) {
    return true;
  } else {
    return false;
  }
}



#include "loop.h"


