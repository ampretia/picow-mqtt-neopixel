# NeoPixels driven from PicoW via C++ & MQTT

Featuring:
- latest Raspberry Pico-W
- C++ Implementation using the Pico 1.5.0 SDK
- NeoPixel (ws2812b) driven from the Pico using pio
- MQTT client to get requests for display changes
- Multicore example

Will give a quick introduction to the application, and then explain each major section. Highlighting the gotchas along the way. See the [workflow](#workflow) for notes on how I actually setupt his and built it.

THe code has been commented hopefully to be useful; things to do...

- Actually put the hardware into a case so I can use this
- Add more links to some of the most helpful blogs etc
- As I come for enterprise software - do I really put a `while(1);` loop in the end of the `main()`? Really??
- Proper message payload, dump the string parsing
- Proper C++ and consistent types


## Application Structure

Logically the application breaks down into two parts, one with an MQTT Client subscribed to a topic, and the second with the NeoPixel effects. The NeoPixel effects are driven form the Pico's second core.

The code is using C++ with the 1.5.0 PicoSDK.

Once the wifi connection has been established, the MQTT client is created and connects. A single topic is subscribed to. When something arrives on this topic, a structure is created and put onto the pico-util's queue. This allows for easy transfer between the cores. 

On core1, there is the "led loop". This blocks waiting for a message on the queue. Once it gets one there's some basic decoding, and the led effect takes over. These are infinite loops, but I've set them to regularly (non-blocking) peek to see if a message is available. If there are the loop terminates so the main "led loop" can actually pull the message.

## WiFi Connection
Pulling in the WiFi settings and connecting hasn't proved to be problem. Apart from one massive gotcha.

> _**DO NOT USE the builtin LED and expect the WiFi to work at the same time**_

Despite the PicoW blink example using the builtin LED, soon as I added actual WiFi connections the whole thing failed. 

## MQTT

When looking for examples of using the PicoW and C++ with MQTT, there was very little; a few things and mostly python. This seemed odd, so I started looking at how to recompile the MQTT libraries. A concern was what the networking stack on the PicoW was like. Does it have sockets, how does the threading work, what about subscribe call backs. 

When I started to look at that which led me to the lwip stack. Turns out this has MQTT application support in it, along with some examples. As luck would have it the  1.5.0 PicoSDK was published the same week I started this. That has the MQTT library available. 

Add these lines to the copy of the [`lwipopts.h`](./src/lwipopts.h)

```
 * see https://forums.raspberrypi.com/viewtopic.php?t=341914
 */
#define MEMP_NUM_SYS_TIMEOUT   (LWIP_NUM_SYS_TIMEOUT_INTERNAL + 1)
#define MQTT_REQ_MAX_IN_FLIGHT  (5) /* maximum of subscribe requests */
```
### Message Format

Well at the moment it's a string format with separates, but need to change this...
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
Then remove or comment out these lines from the `CMakeLists.txt`

```
pico_enable_stdio_usb(pico_neopixel 1)
pico_enable_stdio_uart(pico_neopixel 0)
```

Without this change the whole thing will fail to compile. Also it might be possible to use this as a submodule but I couldn't get CMake to play nicely.

## Hardware

Nothing noteable on the hardware side; when prototyping I added a reset button to save having to disconnect the USB cable to trigger a reset. On the next project I'll aim to add in the extra debug pins for serial debugging.

The NeoPixels I had (another strip from an online retailer named after a long river) are 5v, so I used a level shifter between then and Pico. Easily available and useable. 

If you look at the code, you'll see a 'debug indiciator', these are a set of three regular LEDs that can be used to give some feedback as to the code state. Very useful for a degree of confidence; in the final project this will be a tri-colour LED. Not so much to give feedback, but because the case I'm re-using already had a hole!

- [Notes on Pico reset button](https://www.raspberrypi-spy.co.uk/2022/12/how-to-add-a-raspberry-pi-pico-reset-button/). Makes it a lot easier to manage than unplugging USB everytime.

## Workflow

Note I use just typically to help configure and run separate scripts; just makes everything faster. [Install it today :-)](https://github.com/casey/just). The Pico is connected to a Linux machine.

Also please ensure you've the Pico toolchain - I found it best to follow the guide within the Pico's S[DK getting started](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html).



Steps to recreate the setup:

- Clone this repo and cd into it 
```
git clone https://github.com/ampretia/picow-mqtt-neopixel
cd picow-mqtt-neopixel
```

- Need to get the adafruit pico port
```
git clone "https://github.com/martinkooij/pi-pico-adafruit-neopixels"
cp -r ./pi-pico-adafruit-neopixels/pico_neopixels .
rm -rf ./pi-pico-adafruit-neopixels
```
Checks the notes above for editting the make file

- Setup the SDK's submodules, and also configure the makefile via cmake
```
just setup
```

- The we can build the source
```
just build
```

- Put the PicoW into it's mass storage usb mode for upload (this is where the reset button comes in useful). Press and hold reset, press and hold the bootselect, release reset, release bootselect.  The upload the code; I found linux always assigned a new special drive so had to write a special script to work out where the Pico was available

```
just upload
```

Having this scripted finally made the process not quite so painful. Note the `sudo sync` otherwise the Pico doesn't always get the new file immediately.

## License
My own code I've put under Apache-2.0 but if you pull in the AdaFruit Library, then this makes the whole code GPL. No problem with that, but just an FYI.