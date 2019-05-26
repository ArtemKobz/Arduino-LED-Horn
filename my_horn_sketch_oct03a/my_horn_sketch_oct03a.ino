#include <Adafruit_NeoPixel.h>

#define SENSOR_PIN 2
#define STRIP_PIN 4
#define LED_PIN 13
#define LDR_PIN A0

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(10, STRIP_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: Avoid connecting on a live circuit...if you must, connect GND first.

volatile byte touched = LOW;
volatile byte strip_on = LOW;

void touch_ISR() {
  touched = !touched;
  digitalWrite(LED_PIN, touched);

  // toggle STRIP only on touch (not on release)
  if (digitalRead(SENSOR_PIN) == 1) {
     strip_on = !strip_on;
  }
  if (!strip_on) {
    colorWipe(strip.Color(0, 0, 0), 50);
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), touch_ISR, CHANGE);
  
  strip.begin();
  strip.setBrightness(100); //adjust brightness here
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  if (strip_on) {
    rainbowCycle(20);
  } else {
    colorWipe(strip.Color(0, 0, 0), 50);
  }
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      if (!strip_on) return;
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
