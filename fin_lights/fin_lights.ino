#include <SPI.h>
#include <RH_RF95.h>
#include <Adafruit_NeoPixel.h>

#define NEO_PIN 6
 
/* for feather m0 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
 
#define RF95_FREQ 433.0

#define NNEO  32

#define PIXEL_COLOR 0x00600000
 
// Singleton instance of the radio driver
static RH_RF95 rf95(RFM95_CS, RFM95_INT);
static Adafruit_NeoPixel strip = Adafruit_NeoPixel(NNEO, NEO_PIN, NEO_GRB + NEO_KHZ800);
 
// Blinky on receipt
#define LED 13
#define SPEED 30

static void
dolights()
{
      strip.setPixelColor(0, PIXEL_COLOR);
      strip.show();
      delay(5*SPEED);
      strip.setPixelColor(0, 0);
      strip.setPixelColor(1, PIXEL_COLOR);
      strip.show();
      delay(5*SPEED);
      strip.setPixelColor(1, 0);
      strip.setPixelColor(3, PIXEL_COLOR);
      strip.show();
      delay(7*SPEED);
      strip.setPixelColor(3, 0);
      strip.setPixelColor(5, PIXEL_COLOR);
      strip.setPixelColor(6, PIXEL_COLOR);
      strip.show();
      delay(4*SPEED);
      strip.setPixelColor(5, 0);
      strip.setPixelColor(7, PIXEL_COLOR);
      strip.show();
      delay(5*SPEED);
      strip.setPixelColor(6, 0);
      strip.setPixelColor(0, PIXEL_COLOR);
      strip.show();
      delay(6*SPEED);
      strip.setPixelColor(7, 0);
      strip.setPixelColor(0, 0);
      strip.show();
      delay(3*SPEED);
      strip.setPixelColor(7, PIXEL_COLOR);
      strip.setPixelColor(0, PIXEL_COLOR);
      strip.show();
      delay(4*SPEED);
      strip.setPixelColor(7, 0);
      strip.setPixelColor(0, 0);
      strip.show();
      delay(10*SPEED);
}
 
void
setup() 
{
  pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  //while (!Serial) {
  //}

  delay(100);
 
  //Serial.begin(9600);
  //delay(100);

  //Serial.println("startup");
   
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

  strip.begin();
  strip.show();

  dolights();
 
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

static void
setall(
  uint32_t  rgb)
{
  int pos;

  for (pos = 0; pos < NNEO; ++pos) {
    strip.setPixelColor(pos, rgb);
  }

  strip.show();
}
 
void
loop()
{
  if (rf95.available()) {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    int pos;
    uint32_t rgb;
    
    if (rf95.recv(buf, &len)) {
      digitalWrite(LED, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len);
      //Serial.print("Got: ");
      //Serial.println((char *) buf);
      //Serial.print("RSSI: ");
      //Serial.println(rf95.lastRssi(), DEC);

      dolights();
    } else {
      //Serial.println("Receive failed");
    }
  }
}
