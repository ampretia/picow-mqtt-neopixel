#ifndef _DEBUG_INDICATOR_LED_H
#define _DEBUG_INDICATOR_LED_H

#define DEBUG 1

#ifdef DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

void setupIndicator();
void indicatorWhite();
void indicatorRed();
void indicatorGreen();
void indicatorBlue();

#endif