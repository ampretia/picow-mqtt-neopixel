#include "main.hpp"
#include "led_effects.hpp"
#include "led_util.hpp"
#include "debug_indicator.hpp"

// The LED Effects module, including some helper functions
// See the FastLED/AdaFruit libraries for ideas on how to write these type
// of functions in different ways.

// Each function here should only return when it detects there is 
// a waiting message on the request queue

static context_t* pContext;

void ledEffects_setContext(context_t* pCtx) {
  pContext = pCtx;
}

// Included as this is a useful fn use in Lua on the ESP8266 
// for sin aproximation
int pseudoSin(int x) {
  int idx = x % 128;
  int lookUp = 32 - idx % 64;
  int val = 256 - (lookUp * lookUp) / 4;
  if (idx > 64)
    val = -val;

  return (256 + val) / 2;
}

// Included as this is useful if debugging what actual values have been
// set for a pixel
void debugPixel(uint8_t pixel) {
  uint32_t setC = pContext->ledStrip->getPixelColor(pixel);
  DEBUG_PRINT("[set:%d] %d %d %d %d\n", pixel, (setC & 0xFF000000) >> 24, (setC & 0xFF0000) >> 16, (setC & 0xFF00) >> 8, setC & 0xFF);
}

// Create a 'sine' wave that has a fixed 'frequency' within a given range
// Quotes as this is aproximated !
//
// FastLED uses this style of processing extensively and more sophisticated
// refer to FastLED for more of the maths and how to opimise the maths
//
// bpm beats/minute 
uint8_t sineyWave(uint8_t bpm, uint8_t from, uint8_t to, uint32_t millis, uint8_t phaseShift) {
  uint16_t bpm88 = bpm * 256;
  // need to get the input to the actual sine function
  // this removes the need to work out time since last call
  // and resolves if we're called more or less often.
  uint16_t beat = ((millis * bpm88) / 60000);
  
  // allow a 'phaseShift'
  double rs = pContext->ledStrip->sine8(beat + phaseShift);

  // range down the calculated value 
  double range = to - from;
  uint8_t s = (rs * range / 255) + from;

  return s;
}


void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  Adafruit_NeoPixel* pLedStrip = pContext->ledStrip;

  // HSV has hue in the 0.65535 range rather than RGB which is 255
  // hence need to 'race' through the range quicker
  for (j = 0;; j += 500) {
    for (i = 0; i < pLedStrip->numPixels(); i++) {
      // check the AdaFruit NeoPixels port example for an alternative
      // way of doing this with RGB colours.
      uint32_t c = pLedStrip->ColorHSV(((i * 65535 / pLedStrip->numPixels()) + j) & 65535);
      pLedStrip->setPixelColor(i, pLedStrip->gamma32(c));
    }
    pLedStrip->show();

    // we need to quit the loop if there's something else come in
    if (queue_get_level(pContext->pQueue) > 0) {
      return;
    }
    sleep_ms(wait);
  }
}

// an 'wipe' across all the pixels
void intro(uint8_t wait) {
  Adafruit_NeoPixel* pLedStrip = pContext->ledStrip;
  uint16_t numPixels = pLedStrip->numPixels();
  int i = 0;
  while (1) {
    uint16_t pixel = i++ % numPixels;
    if (pixel == 0 ){
      pLedStrip->clear();
    }

    uint32_t c = pLedStrip->Color((i % 4) * 150, ((i + 1) % 4) * 150, ((i + 4) % 3) * 150);
    pLedStrip->setPixelColor(pixel, c);
    pLedStrip->show();
    
    
    // we need to quit the loop if there's something else come in
    if (queue_get_level(pContext->pQueue) > 0) {
      return;
    }
    sleep_ms(wait);
  }
}

// Uses three of the 'siney' waves to get 
// values to vary the RGB values
void fadeInOut(uint8_t wait) {
  Adafruit_NeoPixel* pLedStrip = pContext->ledStrip;
  uint16_t numPixels = pLedStrip->numPixels();
  while (1) {
    uint32_t timeNow = to_ms_since_boot(get_absolute_time());
    uint8_t red = sineyWave(10, 30, 255, timeNow, 0);
    uint8_t green = sineyWave(20, 30, 255, timeNow, 0);
    uint8_t blue = sineyWave(30, 30, 255, timeNow, 0);
    for (uint8_t i = 0; i < numPixels; i++) {
      pLedStrip->setPixelColor(i, pLedStrip->Color(red, green, blue));
    }
    pLedStrip->show();

    // we need to quit the loop if there's something else come in
    if (queue_get_level(pContext->pQueue) > 0) {
      return;
    }
    sleep_ms(wait);
  }
}