#include <EEPROM.h>

struct Params {
  int stepCm;           // for doCircle
  int turnSteps;        // see deg2step: nb step for 90deg
  int lineSteps;        // see cm2step
  char btName[16];      // bluetooth name
};

// https://www.makerguides.com/28byj-48-stepper-motor-arduino-tutorial/
// noir
#if 0
const int turnSteps_90 = 1365;          // was 1430;
const int step_10cm    = 150;           // was 100
#endif

// violet
#if 1
const int turnSteps_90 = 1300;          // was 1430;
const int step_10cm    = 155;           // was 100
#endif

const Params default_params = {140, turnSteps_90, step_10cm, "oorobot"};
Params params = default_params;


void saveParams() {
  EEPROM.put(0, params);
}

void loadParams() {
  Params savedParams;
  EEPROM.get(0, savedParams);

  if (savedParams.btName[0] >= 48) {
    strcpy(params.btName, savedParams.btName);
  } else {
    strcpy(params.btName, default_params.btName);  
  }
  
  if (savedParams.stepCm > 0 && savedParams.stepCm < 500) {
    params.stepCm = savedParams.stepCm;
  } else {
    params.stepCm = default_params.stepCm;
  }
  if (savedParams.turnSteps > 0 && savedParams.turnSteps < 5000) {
    params.turnSteps = savedParams.turnSteps;
  } else {
    params.turnSteps=default_params.turnSteps;
  }
  if (savedParams.lineSteps > 0 && savedParams.lineSteps < 500) {
    params.lineSteps = savedParams.lineSteps;
  } else {
    params.lineSteps = default_params.lineSteps;
  }
}
