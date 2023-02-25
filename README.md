# NeoPixels driven from PicoW via C++ & MQTT

## AdaFruit NeoPixel Library

Get a cloned copy of the AdaFruit neopixels port to Pico C++.

```
git clone "https://github.com/martinkooij/pi-pico-adafruit-neopixels"          
```

Symbolic link the `pico_neopixels` directory here.


cd adafruit-neopixels/pico_neopixels

remove these lines from the CMakeLists.txt

```
pico_enable_stdio_usb(pico_neopixel 1)
pico_enable_stdio_uart(pico_neopixel 0)
```