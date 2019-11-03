
#include <ArduinoJson.h>

StaticJsonDocument<100> doc;

#define ANALOG_PIN_0  34 //X 
#define ANALOG_PIN_1  35 //Y
#define BUTTON_PIN    13 //button

#define XCALIBRATION 1775
#define YCALIBRATION 1776

#define Y_POS_MAX 2319  // Forwards
#define Y_NEG_MIN -1776 // Backwards

#define X_POS_MAX 2320  // Left
#define X_NEG_MIN -1775 // Right

#define SPEED_MIN 0
#define SPEED_MAX_FORWARD 255
#define SPEED_MAX_REVERSE -255

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

int SPEED_L = 0;
int SPEED_R = 0;

//Joystick
const int joyX = ANALOG_PIN_0;
const int joyY = ANALOG_PIN_1;
const int joyButton = BUTTON_PIN;

#define Calibration 0

void setup() {
  // DEBUG monitoring
  Serial.begin(115200);

  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);
  pinMode(joyButton, INPUT_PULLUP);
  #if Calibration
    Serial.printf("x,y\n");
  #else
    Serial.println(F("Joy Stick Test"));
  #endif
}

void loop() {
  int xPos = analogRead(joyX);
  int yPos = analogRead(joyY);
  int buttonState = digitalRead(joyButton);
  #if Calibration
    Serial.printf("%d, %d\n",xPos,yPos);
  #else
    int CalX = xPos - XCALIBRATION;
    int CalY = yPos - YCALIBRATION;
    int MapX = joy_mapping_x(CalX);
    int MapY = joy_mapping_y(CalY);
    Serial.printf("x: %d, y: %d, btn: %d\n",MapX,MapY,buttonState);
    DIRECTION STATE = SET_STATE(MapX, MapY);
    PRINT_STATE(STATE);
    doc["state"] = int(STATE);
    doc["motorL"] = MapX;
    doc["motorR"] = MapY;
    Serial.printf("x: %d, y: %d, STATE: %d\n",MapX,MapY,STATE);
    //Serial.println(measureJson(doc));
    String output;
    serializeJson(doc, output);
    Serial.println(output);

    DeserializationError err = deserializeJson(doc, output);
    if (err) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(err.c_str());
    }
    Serial.printf("x: %d, y: %d, STATE: %d\n",doc["motorL"],doc["motorR"],doc["state"]);
  #endif
  delay(100);
}

void PRINT_STATE(DIRECTION state) {
  switch(state) {
    case FORWARDS:
      Serial.println("FORWARDS");
      break;
    case BACKWARDS:
      Serial.println("BACKWARDS");
      break;
    case LEFT:
      Serial.println("LEFT");
      break;
    case RIGHT:
      Serial.println("RIGHT");
      break;
    case LEFT_FORWARDS:
      Serial.println("LEFT_FORWARDS");
      break;
    case RIGHT_FORWARDS:
      Serial.println("RIGHT_FORWARDS");
      break;
    case LEFT_BACKWARDS:
      Serial.println("LEFT_BACKWARDS");
      break;
    case RIGHT_BACKWARDS:
      Serial.println("RIGHT_BACKWARDS");
      break;
    case STOP: 
      Serial.println("Stopped");
      break;
    default:
      Serial.println("Default");
      break;
  }
}
//mapped_value = map(value, fromLow, fromHigh, toLow, toHigh)

int joy_mapping_y(int yPos) {
  if (yPos >= 0) return map(yPos, 0, Y_POS_MAX, 0, 1000);
  else return map(yPos, Y_NEG_MIN, 0, -1000, -1);
}

int joy_mapping_x(int xPos) {
  if (xPos >= 10) return map(xPos, 0, X_POS_MAX, 0, 1000);
  else return map(xPos, X_NEG_MIN, 0, -1000, -1);
}


DIRECTION SET_STATE(int MapX, int MapY) {
  DIRECTION STATE;
  if (MapY > 10) {
    if (MapX < 250 && MapX > -250) 
      STATE = FORWARDS;
    else if (MapX > 250)  
      STATE = LEFT_FORWARDS;
    else if (MapX < -250) 
      STATE = RIGHT_FORWARDS;
    else { //Positive Y, Left and Right Region 
      if (MapX > 10) 
        STATE = LEFT;
      else if (MapX < -10) 
        STATE = RIGHT;
    }
  }
  else if (MapY < -10) {
    if (MapX < 250 && MapX > -250) 
      STATE = BACKWARDS;
    else if (MapX > 250)  
      STATE = LEFT_BACKWARDS;
    else if (MapX < -250) 
      STATE = RIGHT_BACKWARDS;
    else { //Negative Y, Left and Right Region 
      if (MapX > 10) 
        STATE = LEFT;
      else if (MapX < -10) 
        STATE = RIGHT;
    }
  }
  else {
    if (MapX > 10) STATE = LEFT;
    else if (MapX < -10) STATE = RIGHT;
    else STATE = STOP;
  }
  return STATE;
}
