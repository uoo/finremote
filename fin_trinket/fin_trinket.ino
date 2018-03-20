#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>

#define NCOL       9
#define INTERVAL  15
#define BRIGHT    50

// If you're using the full breakout...
static Adafruit_IS31FL3731 matrix = Adafruit_IS31FL3731();
// If you're using the FeatherWing version
//Adafruit_IS31FL3731_Wing matrix = Adafruit_IS31FL3731_Wing();

static byte heights[NCOL] = { 16, 8, 4, 8, 7, 12, 11, 10, 4 };

static byte           count;
static unsigned long  next;

void
setup()
{
  Serial.begin(9600);
  Serial.println("ISSI manual animation test");
  if (! matrix.begin()) {
    Serial.println("IS31 not found");
    while (1);
  }
  Serial.println("IS31 Found!");

  next  = 0;
  count = 0;

  matrix.setRotation(0);
}

void
loop()
{
  unsigned      col;
  unsigned      row;
  unsigned long now;

  now = millis();

  if (now < next) {
    return;
  }

  col = count / matrix.width();
  row = count % matrix.width();

  if (row < heights[col]) {
    matrix.drawPixel(row, col, BRIGHT);
  }

  ++count;

  if (count >= matrix.height() * matrix.width()) {
    count = 0;
    matrix.clear();
  }

  next = now + INTERVAL; 
}
