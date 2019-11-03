#include <SparkFun_TB6612.h>

// Motor
#define AIN2 17
#define AIN1 16
#define BIN1 32
#define BIN2 33
#define PWMA 25
#define PWMB 26
#define STBY 27

#define PWM_CHANNEL_0 0
#define PWM_CHANNEL_1 1

#define POS_MAX 1000
#define NEG_MAX -1000
#define CENTER 0
#define POS_MAX_SPEED 255
#define NEG_MAX_SPEED -255
#define ZERO_SPEED 0

const int offsetA = 1;
const int offsetB = 1;

typedef enum {
    FORWARDS,
    BACKWARDS,
    LEFT,
    RIGHT,
    LEFT_FORWARDS,
    RIGHT_FORWARDS,
    LEFT_BACKWARDS,
    RIGHT_BACKWARDS,
    STOP
} DIRECTION;

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motor1 = Motor(AIN1, AIN2, PWMA, PWM_CHANNEL_0, offsetA, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, PWM_CHANNEL_1, offsetB, STBY);




void setup() {
  // DEBUG monitoring
  //ledcAttachPin(PWMA, 0);
  //ledcSetup(0, 10000.0, 8);
  Serial.begin(115200);
  Serial.println(F("### DW1000-arduino-receiver-test ###"));
}



void loop() {
  DIRECTION state;
  int MapX,MapY;
  
  //Drive(
  delay(1000);
}
/*
#define POS_MAX 1000
#define NEG_MAX -1000
#define POS_MAX_SPEED 255
#define NEG_MAX_SPEED -255
#define ZERO_SPEED 0
*/

void Drive(DIRECTION state, int MapX, int MapY) {
  int mspeed;
  int mspeedL;
  int mspeedR;
  switch(state) {
    case FORWARDS:
      Serial.println("FORWARDS");
      mspeed = map(MapY, CENTER, POS_MAX, ZERO_SPEED, POS_MAX_SPEED);
      forward(motor1, motor2, mspeed);
      break;
    case BACKWARDS:
      Serial.println("BACKWARDS");
      mspeed = map(MapY, CENTER, NEG_MAX, ZERO_SPEED, NEG_MAX_SPEED);
      back(motor1, motor2, mspeed);
      break;
    case LEFT:
      mspeed = map(MapX, CENTER, POS_MAX, ZERO_SPEED, POS_MAX_SPEED);
      left(motor1, motor2, mspeed);
      Serial.println("LEFT");
      break;
    case RIGHT:
      Serial.println("RIGHT");
      mspeed = map(MapX, CENTER, NEG_MAX, ZERO_SPEED, POS_MAX_SPEED);
      right(motor1, motor2, mspeed);
      break;
    case LEFT_FORWARDS:
      Serial.println("LEFT_FORWARDS");
      mspeedL = map(MapX, CENTER, POS_MAX, ZERO_SPEED, POS_MAX_SPEED-40);
      mspeedR = map(MapY, CENTER, POS_MAX, ZERO_SPEED, POS_MAX_SPEED);
      motor1.drive(mspeedL);
      motor2.drive(mspeedR);
      break;
    case RIGHT_FORWARDS:
      Serial.println("RIGHT_FORWARDS");
      mspeedL = map(MapX, CENTER, NEG_MAX, ZERO_SPEED, POS_MAX_SPEED);
      mspeedR = map(MapY, CENTER, POS_MAX, ZERO_SPEED, POS_MAX_SPEED-40);
      motor1.drive(mspeedL);
      motor2.drive(mspeedR);
      break;
    case LEFT_BACKWARDS:
      mspeedL = map(MapX, CENTER, POS_MAX, ZERO_SPEED, NEG_MAX_SPEED-40);
      mspeedR = map(MapY, CENTER, NEG_MAX, ZERO_SPEED, NEG_MAX_SPEED);
      motor1.drive(mspeedL);
      motor2.drive(mspeedR);
      Serial.println("LEFT_BACKWARDS");
      break;
    case RIGHT_BACKWARDS:
      Serial.println("RIGHT_BACKWARDS");
      mspeedL = map(MapX, CENTER, NEG_MAX, ZERO_SPEED, NEG_MAX_SPEED);
      mspeedR = map(MapY, CENTER, NEG_MAX, ZERO_SPEED, NEG_MAX_SPEED-40);
      motor1.drive(mspeedL);
      motor2.drive(mspeedR);
      break;
    case STOP: 
      brake(motor1, motor2);
      Serial.println("Stopped");
      break;
    default:
      brake(motor1, motor2);
      Serial.println("Default");
      break;
  }
}
