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
#include <SparkFun_TB6612.h>
#include "Car_Control.h"

// Motor
#define AIN2 32 // Digital
#define AIN1 33 // Digital 
#define BIN1 12 // Digital
#define BIN2 13 // Digital
#define PWMA 25 // PWM
#define PWMB 26 // PWM
#define STBY 27 // Digital

#define PWM_CHANNEL_0 0
#define PWM_CHANNEL_1 1

const int offsetA = 1;
const int offsetB = 1;

#define MOTORS (1)
// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motor1 = Motor(AIN1, AIN2, PWMA, PWM_CHANNEL_0, offsetA, STBY); // Left
Motor motor2 = Motor(BIN1, BIN2, PWMB, PWM_CHANNEL_1, offsetB, STBY); // Right

// Addresses for this node. CHANGE THESE FOR EACH NODE!
#define NETWORKID     0   // Must be the same for all nodes (0 to 255)
#define MYNODEID      2   // My node ID (0 to 255)
#define TONODEID      1   // Destination node ID (0 to 254, 255 = broadcast)

// Frequency for the RFM69 915MHz model
#define FREQUENCY     RF69_915MHZ

// Use ACKnowledge when sending messages (or not):
#define USEACK        true // Request ACKs or not

// Packet sent/received indicator LED (optional):
#define LED           14 // LED positive pin

// Define the Chip select pin
#define CS (2)
// Define the Interrupt Reqeust pin
#define IRQ (4)

// Create a library object for our RFM69HCW module:
RFM69 radio(CS, IRQ, false, digitalPinToInterrupt(IRQ));

int8_t state = 1;

void setup()
{
  // Open a serial port
  Serial.begin(9600);

  // Set up the indicator LED (optional):
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  
  if (!radio.initialize(FREQUENCY, MYNODEID, NETWORKID))
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

void loop()
{
  // RECEIVING

  // In this section, we'll check with the RFM69HCW to see
  // if it has received any packets:
  if (radio.receiveDone()) // Got one!
  {
    // Print out the information:
    Serial.print("received from node ");
    Serial.print(radio.SENDERID, DEC);
    Serial.print(": [");

    // The actual message is contained in the DATA array,
    // and is DATALEN bytes in size:
    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print(radio.DATA[i]);

    // RSSI is the "Receive Signal Strength Indicator",
    // smaller numbers mean higher power.
    Serial.print("], RSSI ");
    Serial.println(radio.RSSI);

    uint8_t speed = radio.DATA[0];
    int8_t state = radio.DATA[1];
    Serial.print("Before map Speed = ");
    Serial.println(speed);
    speed = speed_map(speed);
    Serial.print("Speed = ");
    Serial.println(speed);
    drive_car(speed, state, motor1, motor2);
    print_driving_state(state);


    // Send an ACK if requested.
    // (You don't need this code if you're not using ACKs.)
    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.println("ACK sent");
    }
    Blink(LED,300);
  }
  else 
  {
    //Serial.println("Did not receive a msg");
    
    digitalWrite(LED,LOW);
  } // Else we did not we recieve a message yet

}

// Blink an LED for a given number of ms
void Blink(byte PIN, int DELAY_MS)
{
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
