#include <AccelStepper.h>

// On some step motors direction may be inverted
#define INVERT_DIRECTION

// motor pins
#define motorPin1  4     // IN1 on the ULN2003 driver 1
#define motorPin2  5     // IN2 on the ULN2003 driver 1
#define motorPin3  6     // IN3 on the ULN2003 driver 1
#define motorPin4  7     // IN4 on the ULN2003 driver 1

#define motorPin5  8     // IN1 on the ULN2003 driver 2
#define motorPin6  9     // IN2 on the ULN2003 driver 2
#define motorPin7  10    // IN3 on the ULN2003 driver 2
#define motorPin8  11    // IN4 on the ULN2003 driver 2


// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepper1(AccelStepper::HALF4WIRE, motorPin1, motorPin3, motorPin2, motorPin4);
AccelStepper stepper2(AccelStepper::HALF4WIRE, motorPin5, motorPin7, motorPin6, motorPin8);


void enableMotors() {
  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);
  stepper1.enableOutputs();
  stepper2.enableOutputs();
}

void disableMotors() {
  stepper1.stop();
  stepper2.stop();
  stepper1.disableOutputs();
  stepper2.disableOutputs();
}

void doCircle(float radius, float angle, boolean reverseOrientation){
  isMoving = true;
  startMovement=millis();
  //AAW10r1c50a90G
  //AAW10r0c200a45G
  float lenght_big_arc, lenght_small_arc;
  float steps_big_arc, steps_small_arc;
  float speed_big_arc, speed_small_arc;

  lenght_big_arc = 2 * PI * (radius + (WHEEL_SPACING_MM/2)) * (angle / 360);
  steps_big_arc = round(lenght_big_arc * params.stepCm / 10);
  speed_big_arc = MAX_STEPPER_SPEED;

  lenght_small_arc = 2 * PI * (radius - (WHEEL_SPACING_MM/2)) * (angle / 360);
  steps_small_arc = round(lenght_small_arc * params.stepCm / 10);
  speed_small_arc = abs((steps_small_arc*speed_big_arc)/steps_big_arc);

  if (reverseOrientation) {
    stepper2.move(-steps_big_arc);
    stepper2.setSpeed(speed_big_arc);
    stepper1.move(-steps_small_arc);
    stepper1.setSpeed(speed_small_arc);
  } else {
    stepper1.move(steps_big_arc);
    stepper1.setSpeed(speed_big_arc);
    stepper2.move(steps_small_arc);
    stepper2.setSpeed(speed_small_arc);
  }

}

// conver distance to number of steps
int cm2step( short cm) {
  int target = (int) ((float) cm * (float)params.lineSteps / 10.0f);
#ifdef INVERT_DIRECTION
  target = target * -1;
#endif
  return target;
}

void stepForward(short distance) {
  Serial.println(F("stepForward"));

  isMoving = true;
  startMovement=millis();
  stepperSpeed = MIN_STEPPER_SPEED;
  int target = cm2step( distance);
  stepper1.move(-target);
  stepper2.move(target);
}

void stepBackward(short distance) {
  isMoving = true;
  startMovement=millis();
  stepperSpeed = MIN_STEPPER_SPEED;

  int target = cm2step( distance);
  stepper1.moveTo(target);
  stepper2.moveTo(-target);
}

int deg2step( short deg) {
  int angleStep = (int)((float)deg / 90.0 * (float)params.turnSteps);
  return angleStep;
}

void turnLeft(short angle) {
  isMoving = true;
  startMovement=millis();
  stepperSpeed = MIN_STEPPER_SPEED;

  int angleStep = deg2step( angle);
  stepper1.moveTo(angleStep);
  stepper2.moveTo(angleStep);
}

void turnRight(short angle) {
  isMoving = true;
  startMovement=millis();
  stepperSpeed = MIN_STEPPER_SPEED;

  int angleStep = deg2step( angle);
  stepper1.moveTo(-angleStep);
  stepper2.moveTo(-angleStep);
}
