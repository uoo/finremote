#include <SPI.h>
#include <RH_RF95.h>
 
/* for feather m0 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 433.0

#define BUFSIZE 32

#define PIN_SWITCH  A0
#define PIN_LED     9
 
// Singleton instance of the radio driver
static RH_RF95 rf95(RFM95_CS, RFM95_INT);
 
void
setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  //while (!Serial) {
  //}
 
  //Serial.begin(9600);
  delay(100);
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    //Serial.println("LoRa radio init failed");
    while (1);
  }
  //Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    //Serial.println("setFrequency failed");
    while (1);
  }
  //Serial.print("Set Freq to: ");
  //Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_SWITCH, INPUT_PULLUP);
}

static void
process(
  char *  cmd)
{
  //Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
  
  //Serial.print("Sending "); Serial.println(cmd);
  
  //Serial.println("Sending..."); delay(10);
  rf95.send((uint8_t *) cmd, strlen(cmd) + 1);
 
  //Serial.println("Waiting for packet to complete...");
  //delay(10);
  rf95.waitPacketSent();
}

void
loop()
{
  char    ch;
  char    cmd[BUFSIZE];
  char *  cptr = cmd;
  char *  eptr = cptr + BUFSIZE;
  uint8_t cmdbyte;

#if 0
  while (Serial.available()) {
    if (cptr == eptr) {
      return;
    }
    ch = Serial.read();
    //Serial.print("got ");
    //Serial.println(ch, HEX);

    if (ch == '\r') {
      *cptr++ = '\0';
      process(cmd);
    } else {
      *cptr++ = ch;
    }
  }
#endif

  if (digitalRead(PIN_SWITCH) == LOW) {
    digitalWrite(PIN_LED, HIGH);
    rf95.send(&cmdbyte, sizeof(cmdbyte));
    //Serial.println("Waiting for packet to complete...");
    delay(10);
    rf95.waitPacketSent();
    //Serial.println("packet sent");
    delay(100);
    digitalWrite(PIN_LED, LOW);
    delay(1000);
  }
}
