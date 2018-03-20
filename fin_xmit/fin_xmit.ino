// rf69 demo tx rx.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging client
// with the RH_RF69 class. RH_RF69 class does not provide for addressing or
// reliability, so you should only use RH_RF69  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf69_server.
// Demonstrates the use of AES encryption, setting the frequency and modem 
// configuration

#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>

#define PIN_SWITCH  A0
#define MIN_CHANGE  1000

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 915.0

// Where to send packets to!
#define DEST_ADDRESS   1
// change addresses for each client board, any number :)
#define MY_ADDRESS     2


#if defined (__AVR_ATmega32U4__) // Feather 32u4 w/Radio
  #define RFM69_CS      8
  #define RFM69_INT     7
  #define RFM69_RST     4
  #define LED           13
#endif

#if defined(ARDUINO_SAMD_FEATHER_M0) // Feather M0 w/Radio
  #define RFM69_CS      8
  #define RFM69_INT     3
  #define RFM69_RST     4
  #define LED           13
#endif

#if defined (__AVR_ATmega328P__)  // Feather 328P w/wing
  #define RFM69_INT     3  // 
  #define RFM69_CS      4  //
  #define RFM69_RST     2  // "A"
  #define LED           13
#endif

#if defined(ESP8266)    // ESP8266 feather w/wing
  #define RFM69_CS      2    // "E"
  #define RFM69_IRQ     15   // "B"
  #define RFM69_RST     16   // "D"
  #define LED           0
#endif

#if defined(ESP32)    // ESP32 feather w/wing
  #define RFM69_RST     13   // same as LED
  #define RFM69_CS      33   // "B"
  #define RFM69_INT     27   // "A"
  #define LED           13
#endif

/* Teensy 3.x w/wing
#define RFM69_RST     9   // "A"
#define RFM69_CS      10   // "B"
#define RFM69_IRQ     4    // "C"
#define RFM69_IRQN    digitalPinToInterrupt(RFM69_IRQ )
*/
 
/* WICED Feather w/wing 
#define RFM69_RST     PA4     // "A"
#define RFM69_CS      PB4     // "B"
#define RFM69_IRQ     PA15    // "C"
#define RFM69_IRQN    RFM69_IRQ
*/

static uint8_t key[] = {
    0x12, 0x34, 0xe8, 0x56, 0xe3, 0x7e, 0x30, 0xc2, 
    0xba, 0x40, 0x09, 0x90, 0xb3, 0xde, 0xe6, 0xbd
};

// Singleton instance of the radio driver
static RH_RF69 rf69(RFM69_CS, RFM69_INT);

// Class to manage message delivery and receipt, using the driver declared above
static RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

// Dont put this on the stack:
static uint8_t        buf[RH_RF69_MAX_MESSAGE_LEN];
static uint8_t        data[]        = "  OK";
static char           radiopacket[] = "FIN";
static boolean        last;
static unsigned long  nextchangetime;

void
setup() 
{
  Serial.begin(115200);
  //while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer

  pinMode(PIN_SWITCH, INPUT_PULLUP);
  pinMode(LED, OUTPUT);     
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("Feather Addressed RFM69 TX Test!");
  Serial.println();

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  
  if (!rf69_manager.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(14, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

  rf69.setEncryptionKey(key);
  
  Serial.print("RFM69 radio @");
  Serial.print((int)RF69_FREQ);
  Serial.println(" MHz");

  last           = true;
  nextchangetime = 0;
}

void
loop()
{
  boolean       state;
  unsigned long now;

  state = digitalRead(A0);

  if (state != last) {
    now = millis();
    if (state && (now > nextchangetime)) {
      digitalWrite(LED, HIGH);
      Serial.print("Sending ");
      Serial.println(radiopacket);
  
      // Send a message to the DESTINATION!
      if (rf69_manager.sendtoWait((uint8_t *)radiopacket, strlen(radiopacket), DEST_ADDRESS)) {
        // Now wait for a reply from the server
        uint8_t len = sizeof(buf);
        uint8_t from;
        
        if (rf69_manager.recvfromAckTimeout(buf, &len, 2000, &from)) {
          buf[len] = 0; // zero out remaining string
      
          Serial.print("Got reply from #"); Serial.print(from);
          Serial.print(" [RSSI :");
          Serial.print(rf69.lastRssi());
          Serial.print("] : ");
          Serial.println((char*)buf);     
        } else {
          Serial.println("No reply, is anyone listening?");
        }
      } else {
        Serial.println("Sending failed (no ack)");
      }

      nextchangetime = now + MIN_CHANGE;
      digitalWrite(LED, LOW);
    }

    last = state;
  }
}

