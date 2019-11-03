// RFM69HCW Example Sketch
// Send serial input characters from one RFM69 node to another
// Based on RFM69 library sample code by Felix Rusu
// http://LowPowerLab.com/contact
// Modified for RFM69HCW by Mike Grusin, 4/16

// This sketch will show you the basics of using an
// RFM69HCW radio module. SparkFun's part numbers are:
// 915MHz: https://www.sparkfun.com/products/12775
// 434MHz: https://www.sparkfun.com/products/12823

// See the hook-up guide for wiring instructions:
// https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide

// Uses the RFM69 library by Felix Rusu, LowPowerLab.com
// Original library: https://www.github.com/lowpowerlab/rfm69
// SparkFun repository: https://github.com/sparkfun/RFM69HCW_Breakout

// Include the RFM69 and SPI libraries:

#include <RFM69.h>
#include <SPI.h>
#include "Remote_Control.h"

#define JOY_STICK (1)

#define ANALOG_PIN_X (34) // Left/Right 
#define ANALOG_PIN_Y (35) // Up/Down
#define BUTTON_PIN   (25) // Button

// Addresses for this node. CHANGE THESE FOR EACH NODE!
#define NETWORKID     0   // Must be the same for all nodes (0 to 255)
#define MYNODEID      1   // My node ID (0 to 255)
#define TONODEID      2   // Destination node ID (0 to 254, 255 = broadcast)

// Frequency for the RFM69 915MHz model
#define FREQUENCY     RF69_915MHZ

// AES encryption (or not):
#define ENCRYPT       false // Set to "true" to use encryption
#define ENCRYPTKEY    "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes

// Use ACKnowledge when sending messages (or not):
#define USEACK        true // Request ACKs or not

// Packet sent/received indicator LED (optional):
#define LED           26 // LED positive pin

// Define the Chip select pin
#define CS (2)
// Define the Interrupt Reqeust pin
#define IRQ (4)

// Create a library object for our RFM69HCW module:
RFM69 radio(CS, IRQ, false, digitalPinToInterrupt(IRQ));
bool is_initialized; // Check is the module is present

void setup()
{
  // Open a serial port so we can send keystrokes to the module:
  Serial.begin(9600);

  // Set up the indicator LED (optional):
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);

  // Setup up Joystick input
  #if JOY_STICK
    pinMode(ANALOG_PIN_X, INPUT);
    pinMode(ANALOG_PIN_Y, INPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
  #endif

  // Initialize the RFM69HCW:
  is_initialized = radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  if (!is_initialized)
  {
    Serial.println("Init Failed!");
  }
  else
  {
    Serial.print("Node ");
    Serial.print(MYNODEID,DEC);
    Serial.println(" ready");  
  }
  
  radio.setHighPower(); // Always use this for RFM69HCW

}

static uint8_t mag;
static int8_t mode;

#define DATA_SIZE (2)
static char data[DATA_SIZE];

void loop()
{
    // Set up a "buffer" for characters that we'll send:
    int X,Y;
    #if JOY_STICK
        X = analogRead(ANALOG_PIN_X);
        Y = analogRead(ANALOG_PIN_Y);
        int Xcal = X - XCALIBRATION;
        int Ycal = Y - YCALIBRATION;
        int Xmap = joy_mapping_x(Xcal);
        int Ymap = joy_mapping_y(Ycal);        
    #else
        int Xmap = 50; 
        int Ymap = 50;
    #endif
    mag = (int) magnitude(Xmap,Ymap);
    if (mag > 100) mag = 100;
    float ang = angle(Xmap,Ymap);
    mode = zone_detector(ang);

    data[0] = mag;
    data[1] = mode;

    #if JOY_STICK
     Serial.println("JoyStick values");
     Serial.print("X = ");
     Serial.print(Xmap);
     Serial.print(", ");
     Serial.print("Y = ");
     Serial.println(Ymap);
    #endif

    Serial.print("Mag = ");
    Serial.println(mag);
    Serial.print("Mode = ");
    Serial.println(mode);
    

    if (is_initialized){
        // SENDING

        // Send the packet!
        Serial.print("sending to node ");
        Serial.println(TONODEID, DEC);
        
        // There are two ways to send packets. If you want
        // acknowledgements, use sendWithRetry():
        if (USEACK)
        {
            if (radio.sendWithRetry(TONODEID, data, DATA_SIZE))
            {
                Serial.println("ACK received!");
            }
            else 
            {
                Serial.println("no ACK received :(");
            }
        }
        Blink(LED,300);
    }
    else 
    {
        Serial.println("RFM Module is not present check wiring!");
        delay(2000);
    }

}

// Blink an LED for a given number of ms
void Blink(byte PIN, int DELAY_MS)
{
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
