cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME Home_Climate_Control_v1)
project(${PROJECT_NAME})

set(${CMAKE_PROJECT_NAME}_SKETCH Home_Climate_Control_v1/Home_Climate_Control_v1.ino)

#### Uncomment below additional settings as needed.
set(${CMAKE_PROJECT_NAME}_BOARD mega)
set(${CMAKE_PROJECT_NAME}_PORT /dev/tty.usbserial-14120)
set(mega.build.mcu atmega2560)
set(mega.upload.protocol wiring)
set(mega.upload.speed 115200)

generate_arduino_firmware(${CMAKE_PROJECT_NAME})

include_directories(/Applications/Arduino.app/Contents/Java/libraries/)
include_directories(/Users/roman/Documents/Arduino/libraries/)