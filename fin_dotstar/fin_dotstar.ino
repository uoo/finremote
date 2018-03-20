#include "FastLED.h"

// How many leds in your strip?
#define NUM_LEDS 100
#define MAXDELAY  10
#define MAXLONGDELAY 4
#define LONG 25
#define MAXPAUSE 600
//#define PAUSEFRAC  5
#define COLORFRAC 20

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 4
#define CLOCK_PIN 3

// Define the array of leds
static CRGB leds[NUM_LEDS];

static int lastled;
static int led;
static int ledstart;
static int ledend;
static int dtime;
static boolean pause;
static CRGB ledcolor;

static void
pick()
{
  int maxdelay;
#ifdef PAUSEFRAC
  pause    = random(PAUSEFRAC) == 0;
#else
  pause = false;
#endif

  maxdelay = (abs(ledend - ledstart) > LONG) ? MAXLONGDELAY : MAXDELAY;
  ledstart = random(NUM_LEDS);
  ledend   = random(NUM_LEDS);
  dtime    = random(pause ? MAXPAUSE : maxdelay);
  led      = ledstart;
  if (random(COLORFRAC) == 0) {
    ledcolor = CHSV(random(0x100), 255, 255);
  } else {
    ledcolor = CRGB::Red;
  }
}

void
setup()
{ 
  FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  randomSeed(analogRead(A0) + analogRead(A1));
  pick();
}

void
loop()
{
  leds[lastled] = CRGB::Black;

  if (pause) {
    FastLED.show();
    delay(dtime);
    pick();
    return;
  }

  leds[led]     = ledcolor;
  FastLED.show();

  lastled = led;

  if (ledstart < ledend) {
    ++led;
    if (led >= ledend) {
      pick();
    }
  } else {
    --led;
    if (led <= ledend) {
      pick();
    }
  }

  delay(dtime);
}
