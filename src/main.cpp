/*
 Copyright 2022 Matthew B White

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#include "main.hpp"
#include "debug_indicator.hpp"
#include "mqtt.hpp"
#include "led_effects.hpp"
#include "pico/multicore.h"

// Reference to the context
static context_t context;

void core1_main();

/** Main
 */
int main() {

  setupIndicator();

  stdio_init_all();
  while (!tud_cdc_connected()) {
    sleep_ms(100);
  }


  DEBUG_PRINT("a: Starting up.. %s %s\n", WIFI_SSID, WIFI_PASSWORD);
  if (cyw43_arch_init()) {
    DEBUG_PRINT("failed to initialise\n");
    return 1;
  }

  DEBUG_PRINT("b: arch_init\n");
  cyw43_arch_enable_sta_mode();

  DEBUG_PRINT("c: Connecting to Wi-Fi...\n");
  if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
    DEBUG_PRINT("failed to connect.\n");
    return 1;
  } else {
    DEBUG_PRINT("d: Connected\n");
    indicatorBlue();
  }

  mqtt_client_t* client = mqtt_client_new();
  mqtt_connect(client);

  queue_t queue;

  // only need to have a small queue really
  queue_init(&queue, sizeof(queue_entry_t), 5);


  Adafruit_NeoPixel ls = Adafruit_NeoPixel(LED_LENGTH, LED_PIN, NEO_GRB + NEO_KHZ800);
  ls.begin();
  ls.clear();
  for (int i = 0; i < LED_LENGTH; i++) {
    ls.setPixelColor(i, ls.Color((i % 3) * 150, ((i + 1) % 3) * 150, ((i + 2) % 3) * 150));
    ls.show();
    sleep_ms(50);
  }
  context.ledStrip = &ls;
  context.pQueue = &queue;

  mqtt_setContext(&context);
  ledEffects_setContext(&context);

  // need to run this on core 1
  DEBUG_PRINT("f: Starting main in core1\n");
  multicore_launch_core1(core1_main);
  DEBUG_PRINT("f: Started main in core1\n");

  while (1) {
    indicatorBlue();
    sleep_ms(5000);
    indicatorGreen();
    sleep_ms(5000);
  };
}

// main led loop
void core1_main() {

  context.ledStrip->setBrightness(40);

  DEBUG_PRINT("c1: Hello from Core 1");
  // main loop here
  while (true) {

    queue_entry_t request;

    // let's get the next item on the queue
    DEBUG_PRINT("c1: waiting on queue\n");
    queue_remove_blocking(context.pQueue, &request);
    DEBUG_PRINT("c1: %s %s", request.cmd, request.cmddata);

    if (strcmp(request.cmd, "fixed") == 0) {
      int bright, r, g, b;
      char model[] = "   ";
      char _bright[3], _r[3], _g[3], _b[3];
      sscanf(request.cmddata, "%[^','],%[^','],%[^','],%[^','],%s", _bright, model, _r, _g, _b);
      bright = atoi(_bright);
      r = atoi(_r);
      g = atoi(_g);
      b = atoi(_b);

      printf("Fixed %d %s (%d,%d,%d)\n", bright, model, r, g, b);
      if (strcmp(model, "hsv") == 0) {
        context.ledStrip->fill(context.ledStrip->ColorHSV(r, g, b));
      } else {
        context.ledStrip->fill(context.ledStrip->Color(r, g, b));
      }

      context.ledStrip->setBrightness(bright);
      context.ledStrip->show();

    } else if (strcmp(request.cmd, "effect") == 0) {

      char model[] = "   ";
      char _bright[3], _effectIndex[3];
      sscanf(request.cmddata, "%[^','],%[^','],%s", _bright, model, _effectIndex);
      int effectIndex = atoi(_effectIndex);
      int bright = atoi(_bright);
      context.ledStrip->setBrightness(bright);
      DEBUG_PRINT("Effect %d (%d)\n", bright, effectIndex);

      switch (effectIndex) {
        case 0:
        default:
          rainbowCycle(20);
          break;
        case 1:
          fadeInOut(20);
          break;
        case 2:
          intro(20);
          break;
      }

    }
  }
}


