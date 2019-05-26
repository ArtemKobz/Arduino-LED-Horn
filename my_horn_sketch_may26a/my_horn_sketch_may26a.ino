#include <Adafruit_NeoPixel.h>

#define SENSOR_PIN 2
#define STRIP_PIN 4
#define LED_PIN 13
#define LDR_PIN A0
#define LONG_TOUCH_DURATION 1200

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(11, STRIP_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: Avoid connecting on a live circuit...if you must, connect GND first.

volatile byte touched = LOW;
volatile byte strip_on = LOW;

long last_touch_time = 0;

int showType = 1;
int processInt = 0;
int showSwitched = 0;

void touch_ISR() {
  touched = !touched;
  digitalWrite(LED_PIN, touched);

  if (touched) {
    last_touch_time = millis();
  } else {
    if (millis() - last_touch_time > LONG_TOUCH_DURATION ) {
       // switch to next showType
       showType++;
       //if (showType > 10) showType=1;
       processInt = 1;
    } else { 
        if (!showSwitched) {
          strip_on = !strip_on;
          processInt = 1;
        }
        showSwitched = 0;
    }
  }
}

void setup() {

  analogReference(INTERNAL);

  pinMode(LED_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), touch_ISR, CHANGE);
  
  strip.begin();
  strip.setBrightness(100); //adjust brightness here
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  if (strip_on) {
    startShow(showType);
  } else {
    colorWipe(strip.Color(0, 0, 0), 50);
  }

  processInt = 0;
  if (digitalRead(SENSOR_PIN) != touched) {
    touched = digitalRead(SENSOR_PIN);
    digitalWrite(LED_PIN, touched);
  }

  if (touched && millis() - last_touch_time > LONG_TOUCH_DURATION) {
    last_touch_time = millis();
    showType++;
    processInt = 1;
    showSwitched = 1;
  }
  
}


void startShow(int i) {
  switch(i){
    case 0: colorWipe(strip.Color(0, 0, 0), 10);    // Black/off
            break;
    case 1: colorWipe(strip.Color(255, 0, 0), 50);  // Red
            break;
    case 2: colorWipe(strip.Color(0, 255, 0), 50);  // Green
            break;
    case 3: colorWipe(strip.Color(0, 0, 255), 50);  // Blue
            break;
    case 4: colorWipe(strip.Color(255, 255, 0), 50);  // R + G
            break;
    case 5: colorWipe(strip.Color(0, 255, 255), 50);  // G + B
            break;
    case 6: colorWipe(strip.Color(255, 0, 255), 50);  // R + B
            break;
    case 7: customPattern(0); // UA FLAG
            break;
    case 8: customPattern(1); // UA FLAG 2
            break;
    case 9: colorWipe(strip.Color(255, 255, 255), 50);  // White
            break;
    case 10: theaterChase(strip.Color(255,   0,   0), 50); // Red
            break;
    case 11: theaterChase(strip.Color(  0, 255,   0), 50); // Green
            break;
    case 12: theaterChase(strip.Color(  0,   0, 255), 50); // Blue
            break;
    case 13: theaterChase(strip.Color(255, 255, 255), 50); // White
            break;
    case 14: rainbow(20);
            break;
    case 15: rainbowCycle(20);
            break;
    case 16: theaterChaseRainbow(50);
            break;
    case 17: showType = 1;
            break;
  }
}



// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if (processInt) return;
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    if (processInt) return;
    for(i=0; i<strip.numPixels(); i++) {
      if (processInt) return;
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    if (processInt) return;
    for(i=0; i< strip.numPixels(); i++) {
      if (processInt) return;
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    if (processInt) return;
    for (int q=0; q < 3; q++) {
      if (processInt) return;
      for (int i=0; i < strip.numPixels(); i=i+3) {
        if (processInt) return;
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        if (processInt) return;
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    if (processInt) return;
    for (int q=0; q < 3; q++) {
      if (processInt) return;
      for (int i=0; i < strip.numPixels(); i=i+3) {
        if (processInt) return;
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        if (processInt) return;
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

void customPattern(uint8_t pattern_num) {
    switch(pattern_num){
      //Flag
      case 0: strip.setPixelColor( 0, strip.Color(255, 255, 0));
              strip.setPixelColor( 1, strip.Color(255, 255, 0));
              strip.setPixelColor( 2, strip.Color(255, 255, 0));
              strip.setPixelColor( 3, strip.Color(0, 0, 255));
              strip.setPixelColor( 4, strip.Color(0, 0, 255));
              strip.setPixelColor( 5, strip.Color(0, 0, 255));
              strip.setPixelColor( 6, strip.Color(0, 0, 255));
              strip.setPixelColor( 7, strip.Color(0, 0, 255));
              strip.setPixelColor( 8, strip.Color(255, 255, 0));
              strip.setPixelColor( 9, strip.Color(255, 255, 0));
              strip.setPixelColor(10, strip.Color(255, 255, 0));
              break;
      case 1: strip.setPixelColor( 0, strip.Color(255, 255, 0));
              strip.setPixelColor( 1, strip.Color(255, 255, 0));
              strip.setPixelColor( 2, strip.Color(255, 255, 0));
              strip.setPixelColor( 3, strip.Color(255, 255, 0));
              strip.setPixelColor( 4, strip.Color(255, 255, 0));
              strip.setPixelColor( 5, strip.Color(255, 255, 255));
              strip.setPixelColor( 6, strip.Color(0, 0, 255));
              strip.setPixelColor( 7, strip.Color(0, 0, 255));
              strip.setPixelColor( 8, strip.Color(0, 0, 255));
              strip.setPixelColor( 9, strip.Color(0, 0, 255));
              strip.setPixelColor(10, strip.Color(0, 0, 255));
              break;
    }
    strip.show();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
