// Main header

#ifndef _MAIN_LED_H
#define _MAIN_LED_H

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "pico/time.h"
#include "pico/cyw43_arch.h"
#include "pico/util/queue.h"

#include <tusb.h>

// Using the AdaFruit library here
#include "Adafruit_NeoPixel.hpp"

#include "config.hpp"

// Request object to transfer between cores
typedef struct
{
  char cmd[10];
  char cmddata[20];
} queue_entry_t;

// Context to use within the various modules
// contains the inter-core queue and the led strip object
typedef struct
{
  // Reference to the inter-core queue
  queue_t *pQueue;

// Reference to the LedStrip object
  Adafruit_NeoPixel *ledStrip;

} context_t;

#endif