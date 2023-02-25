#include "main.hpp"
#include "debug_indicator.hpp"
#include "config.hpp"

/** Setup the 3 pins for the indicator led
 */
void setupIndicator() {
  // define the debug indicator LEDs
  gpio_init(INDICATOR_LED_R);
  gpio_init(INDICATOR_LED_G);
  gpio_init(INDICATOR_LED_B);
  gpio_set_dir(INDICATOR_LED_R, GPIO_OUT);
  gpio_set_dir(INDICATOR_LED_G, GPIO_OUT);
  gpio_set_dir(INDICATOR_LED_B, GPIO_OUT);
  gpio_put(INDICATOR_LED_R, 0);
  gpio_put(INDICATOR_LED_G, 1);
  gpio_put(INDICATOR_LED_B, 1);
}


void indicatorRed() {
  gpio_put(INDICATOR_LED_R, 0);
  gpio_put(INDICATOR_LED_G, 1);
  gpio_put(INDICATOR_LED_B, 1);
}

void indicatorBlue() {
  gpio_put(INDICATOR_LED_R, 1);
  gpio_put(INDICATOR_LED_G, 1);
  gpio_put(INDICATOR_LED_B, 0);
}

void indicatorGreen() {
  gpio_put(INDICATOR_LED_R, 1);
  gpio_put(INDICATOR_LED_G, 0);
  gpio_put(INDICATOR_LED_B, 1);
}

void indicatorWhite() {
  gpio_put(INDICATOR_LED_R, 0);
  gpio_put(INDICATOR_LED_G, 0);
  gpio_put(INDICATOR_LED_B, 0);
}
