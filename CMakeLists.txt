# Essential CMake
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PROJECT_NAME "PicoLedExample")

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# Pull in Pico SDK (must be before project)
include(pico_sdk_import.cmake)
project(${PROJECT_NAME} C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)
set(PICO_CXX_ENABLE_RTTI 1)

set(WIFI_SSID $ENV{WIFI_SSID})
set(WIFI_PASSWORD $ENV{WIFI_PASSWORD})

# Initialise the Pico SDK
pico_sdk_init()

# Include the PicoLed Library; this is a clone of the PicoLed repo, with some changes. See README.md
# include("picoled/PicoLed.cmake")

add_subdirectory(./pico_neopixels build)

# Add executable.
add_executable(${PROJECT_NAME}
                src/debug_indicator.cpp
                src/led_effects.cpp
                src/main.cpp
                src/mqtt.cpp
                  )

pico_set_program_name(${PROJECT_NAME} "PicoLedExample")
pico_set_program_version(${PROJECT_NAME} "0.1")

pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

# Include Header files from src
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src 
${CMAKE_CURRENT_LIST_DIR}/pico_neopixels/include)

target_compile_definitions(${PROJECT_NAME}  PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )

# Add the standard library to the build
# note that addition of the pico_lwip_mqtt available in the 1.5.0 C++ PicoSDK
target_link_libraries(${PROJECT_NAME} 
                        pico_stdlib 
                        # PicoLed 
                        pico_multicore
                        pico_neopixel
                        pico_cyw43_arch_lwip_threadsafe_background 
                        pico_lwip_mqtt )

pico_add_extra_outputs(${PROJECT_NAME})

