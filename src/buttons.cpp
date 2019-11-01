#include <Arduino.h>
#include "buttons.h"

int LONG_CLICK_DELAY = 500;
int DEBOUNCING_DELAY = 300;
int lastButtonId = -1;
unsigned long lastClick = 0;
unsigned long lastRawPressed = 0;


void setupButtons()
{
  pinMode(PIN_KEY_0,INPUT_PULLUP);
  pinMode(PIN_KEY_1,INPUT_PULLUP);
  pinMode(PIN_KEY_2,INPUT_PULLUP);
  pinMode(PIN_KEY_3,INPUT);
  pinMode(PIN_KEY_4,INPUT);
  pinMode(PIN_KEY_5,INPUT);
}


int getPressedButton() {
 
//  int raw = analogRead(KEYS_PIN);
  int b = -1;

  pinMode(PIN_KEY_3,INPUT);
  pinMode(PIN_KEY_4,OUTPUT);
  digitalWrite(PIN_KEY_4,LOW);
  pinMode(PIN_KEY_5,INPUT);

  if (!digitalRead(PIN_KEY_0)) {b=7;}
  if (!digitalRead(PIN_KEY_1)) {b=6;}
  if (!digitalRead(PIN_KEY_2)) {b=9;}
 
  pinMode(PIN_KEY_3,OUTPUT);
  digitalWrite(PIN_KEY_3,LOW);
  pinMode(PIN_KEY_4,INPUT);
  pinMode(PIN_KEY_5,INPUT);
  
  if (!digitalRead(PIN_KEY_0)) {b=10;}
  if (!digitalRead(PIN_KEY_1)) {b=0;}
  if (!digitalRead(PIN_KEY_2)) {b=11;}
  
  
  pinMode(PIN_KEY_3,INPUT);
  pinMode(PIN_KEY_4,INPUT);
  pinMode(PIN_KEY_5,OUTPUT);
  digitalWrite(PIN_KEY_5,LOW);

  if (!digitalRead(PIN_KEY_0)) {b=1;}
  if (!digitalRead(PIN_KEY_1)) {b=5;}
  if (!digitalRead(PIN_KEY_2)) {b=3;}
  


  
  if (b!=-1) {
    lastRawPressed = millis();
    
    unsigned long currentClick = millis();
    if (lastButtonId != b) {
      if (currentClick > lastClick + DEBOUNCING_DELAY) {
        lastClick = currentClick;
        lastButtonId = b;
      } else {
        b = -1;
      }
    } else {
      if (currentClick > lastClick + LONG_CLICK_DELAY) {
        lastButtonId = b;
        b = b + 12;
        lastClick = currentClick;
      } else {
        b = -1;
      }
    }
  } else {
    if (millis() - lastRawPressed > 100) {
      lastButtonId = -1;
    }
  }
  return b;
}
