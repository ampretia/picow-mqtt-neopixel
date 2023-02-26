
# Ensure all properties are exported as shell env-vars
set export

set dotenv-load

# set the current directory, and the location of the test dats
CWDIR := justfile_directory()

_default:
  @just -f {{justfile()}} --list

# To get the neopixel library
# git clone "https://github.com/martinkooij/pi-pico-adafruit-neopixels"
# cp -r ./pi-pico-adafruit-neopixels/pico_neopixels .
# rm -rf ./pi-pico-adafruit-neopixels
# Edit the cmake file as per the readme in this repo.


# Build the code and pull down the idemix tool
setup:
    #!/bin/bash
    set -ex


    git submodule update --init --recursive

    mkdir -p ${CWDIR}/build
    pushd ${CWDIR}/build
    PICO_SDK_PATH=${CWDIR}/pico-sdk cmake -DPICO_BOARD=pico_w -DWIFI_SSID=${WIFI_SSID} -DWIFI_PASSWORD=${WIFI_PASSWORD} ..
    popd

build:
    #!/bin/bash
    set -ex

    pushd ${CWDIR}/build
    make -j 1
    popd

upload:
    #!/bin/bash
    set -ex

    export D=$( dmesg | tail | perl -n -e'/\[(sd.)] Attached SCSI/ && print $1')
    sudo mount /dev/${D}1 /mnt/pico
    
    sudo cp build/PicoLedExample.uf2 /mnt/pico && sudo sync

listen:
    sudo minicom -b 115200 -o -D /dev/ttyACM0