#include <EEPROM.h>

struct Params {
  int stepCm;           // move distance for 1 actions (14cm)
  int turnSteps;        // see deg2step: nb step for 90deg
  int lineSteps;        // see cm2step:  nb step for 10cm
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
    params = default_params;
    return false;
  }

  params = savedParams;
  return true;
}
