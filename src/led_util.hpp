
#ifndef _EFFECT_UTIL_LED_H
#define _EFFECT_UTIL_LED_H



int pseudoSin(int x) ;

uint8_t beats88(uint8_t bpm, uint8_t from, uint8_t to, uint32_t millis,uint8_t phaseShift);

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(uint8_t WheelPos) ;

#endif