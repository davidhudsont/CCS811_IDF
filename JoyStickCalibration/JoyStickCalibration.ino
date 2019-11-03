
#define ANALOG_PIN_0  34 //X 
#define ANALOG_PIN_1  35 //Y
#define BUTTON_PIN    13 //button

//Joystick
const int joyX = ANALOG_PIN_0;
const int joyY = ANALOG_PIN_1;
const int joyButton = BUTTON_PIN;


void setup() {
  // DEBUG monitoring
  Serial.begin(115200);

  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);
  pinMode(joyButton, INPUT_PULLUP);
  Serial.printf("x,y\n");
}

void loop() {
  int xPos = analogRead(joyX);
  int yPos = analogRead(joyY);
  int buttonState = digitalRead(joyButton);
  Serial.printf("%d, %d\n",xPos,yPos);
  delay(1000);
}
