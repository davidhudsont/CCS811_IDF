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
#include <RFM69registers.h>
#include <SPI.h>
#include <SparkFun_TB6612.h>

#define INITIALIZE_RFM69 (0)

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

void setup()
{
  // Open a serial port
    Serial.begin(9600);

    // Set up the indicator LED (optional):
    pinMode(LED,OUTPUT);
    digitalWrite(LED,LOW);
  
#if INITIALIZE_RFM69

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

#else
    digitalWrite(CS, HIGH);
    pinMode(CS, OUTPUT);
    SPI.begin();

    radio.writeReg(REG_DIOMAPPING1, 0xC0);

#endif
}


void loop()
{
    uint8_t reg;
    reg = radio.readReg(REG_VERSION);

    Serial.print("Register Version #: 0x");
    Serial.println(reg, HEX);

    reg = radio.readReg(REG_DIOMAPPING1);

    Serial.print("DIO #: 0x");
    Serial.println(reg, HEX);

    delay(2000);
}

// Blink an LED for a given number of ms
void Blink(byte PIN, int DELAY_MS)
{
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
