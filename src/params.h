#include <EEPROM.h>

struct Params {
  int stepCm;           // for doCircle
  int turnSteps;        // see deg2step: nb step for 90deg
  int lineSteps;        // see cm2step
  char btName[16];      // bluetooth name

  int crc;              // crc for validating params
};

// very simple crc function
int computeCRC( const Params& p) {
  int sum = 0;
  sum += p.stepCm;
  sum += p.turnSteps;
  sum += p.lineSteps;

  for( int* i = (int*) p.btName; i < (int*)( p.btName + 16); ++i) {
    sum += *i;
  }

  return sum;
}

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

const Params default_params = {140, turnSteps_90, step_10cm, "oorobot", 0};
Params params = default_params;


void saveParams() {
  params.crc = computeCRC( params);
  EEPROM.put(0, params);
}

// load saved params
// indicate if checksum was ok
bool loadParams() {
  Params savedParams;
  EEPROM.get(0, savedParams);

  int check = computeCRC( savedParams);

  if( check != savedParams.crc) {
    savedParams = default_params;
    return false;
  }

  return true;
/*
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
  }*/
}
