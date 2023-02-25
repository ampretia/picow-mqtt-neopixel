# NeoPixels driven from PicoW via C++ & MQTT

Featuring:
- latest Raspberry Pico-W
- C++ Implementation using the Pico 1.5.0 SDK
- NeoPixel (ws2812b) driven from the Pico using pio
- MQTT client to get requests for display changes
- Multicore example

Will give a quick introduction to the application, and then explain each major section. Highlighting the gotchas along the way.

## Application Structure

Logically the application breaks down into two parts, one with an MQTT Client subscribed to a topic, and the second with the NeoPixel effects. The NeoPixel effects are driven form the Pico's second core.

The code is using C++ with the 1.5.0 PicoSDK.

## WiFi Connection
Pulling in the WiFi settings and connecting hasn't proved to be problem. Apart from one massive gotcha.

DO NOT USE the builtin LED and expect the WiFi to work at the same time. 
Despite the PicoW blink example using the builtin LED, soon as I added actual WiFi connections the whole thing failed. 

## MQTT

When looking for examples of using the PicoW and C++ with MQTT, there was very little; a few things and mostly python. This seemed odd, so I started looking at how to recompile the MQTT libraries. A concern was what the networking stack on the PicoW was like. Does it have sockets, how does the threading work, what about subscribe call backs. 

When I started to look at that which led me to the lwip stack. Turns out this has MQTT application support in it, along with some examples. As luck would have it the  1.5.0 PicoSDK was published the same week I started this. That has the MQTT library available. 

> There are updates needed to the `lwipopts.h`

Add these lines to the copy of the [`lwipopts.h`](./src/lwipopts.h)

```
 * see https://forums.raspberrypi.com/viewtopic.php?t=341914
 */
#define MEMP_NUM_SYS_TIMEOUT   (LWIP_NUM_SYS_TIMEOUT_INTERNAL + 1)
#define MQTT_REQ_MAX_IN_FLIGHT  (5) /* maximum of subscribe requests */
```
## AdaFruit NeoPixel Library

There are a few C++ based NeoPixel libraries

- [Port of the AdaFruit library](https://github.com/martinkooij/pi-pico-adafruit-neopixels) 
  The AdaFruit port works as well (with some changes see below) and I found that API slightly easier to use. 

- [PicoLED](https://github.com/ForsakenNGS/PicoLED)
  PicoLED was the first library I tried using, and it cetainly does it's job well. 


- [FastLED](https://github.com/FastLED/FastLED) does now support using PIO and the RP2040, but I've not yet attempted to compile this.
    I've used FastLED on the Arduino, and would have been the first choice here. Though support has only recently been merged, and I've not attempted to build this yet. 


### Steps to use the AdaFruit port

Get a cloned copy of the AdaFruit neopixels port to Pico C++ to a directory.

```
git clone "https://github.com/martinkooij/pi-pico-adafruit-neopixels"          
```
In the repo there is a `pico_neopixels` directory. Either symlink or copy that directory into this repo.  
Then remove these lines from the `CMakeLists.txt`

```
pico_enable_stdio_usb(pico_neopixel 1)
pico_enable_stdio_uart(pico_neopixel 0)
```

Without this change the whole thing will fail to compile. Also it might be possible to use this as a submodule but I couldn't get CMake to play nicely.