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

// Addresses for this node. CHANGE THESE FOR EACH NODE!

#define NETWORKID     0   // Must be the same for all nodes (0 to 255)
#define MYNODEID      1   // My node ID (0 to 255)
#define TONODEID      2   // Destination node ID (0 to 254, 255 = broadcast)

// RFM69 frequency, uncomment the frequency of your module:

//#define FREQUENCY   RF69_433MHZ
#define FREQUENCY     RF69_915MHZ

// AES encryption (or not):

#define ENCRYPT       false // Set to "true" to use encryption
#define ENCRYPTKEY    "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes

// Use ACKnowledge when sending messages (or not):

#define USEACK        true // Request ACKs or not

// Packet sent/received indicator LED (optional):

#define LED           26 // LED positive pin
#define GND           8  // LED ground pin

#define CS (2)
#define IRQ (4)
// Create a library object for our RFM69HCW module:

RFM69 radio(CS, IRQ, false, digitalPinToInterrupt(IRQ));

void setup()
{
  // Open a serial port so we can send keystrokes to the module:
  
  Serial.begin(115200);
  Serial.print("Node ");
  Serial.print(MYNODEID,DEC);
  Serial.println(" ready");  

  // Set up the indicator LED (optional):
  
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);

  radio.printISR_pin_number();
  // Initialize the RFM69HCW:
  if (!radio.initialize(FREQUENCY, MYNODEID, NETWORKID))
  {
    Serial.println("Init Failed!");
  }
  radio.setHighPower(); // Always use this for RFM69HCW

  // Turn on encryption if desired:
  
  if (ENCRYPT)
    radio.encrypt(ENCRYPTKEY);
}

const char* msg = "Fuck You!";
int sz = 10;

void loop()
{
  // Set up a "buffer" for characters that we'll send:
  
  static char sendbuffer[62];
  static int sendlength = 0;

  // SENDING
  
      // Send the packet!
      Serial.print("sending to node ");
      Serial.println(TONODEID, DEC);
      
      // There are two ways to send packets. If you want
      // acknowledgements, use sendWithRetry():
      if (USEACK)
      {
        if (radio.sendWithRetry(TONODEID, msg, sz))
          Serial.println("ACK received!");
        else {
          Serial.println("no ACK received :(");
          //radio.readAllRegs();
        }
      }
      else // don't use ACK
      {
        radio.send(TONODEID, sendbuffer, sendlength);
      }
      Blink(LED,1000);
}

void Blink(byte PIN, int DELAY_MS)
// Blink an LED for a given number of ms
{
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
