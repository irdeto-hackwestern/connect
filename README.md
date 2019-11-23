# Irdeto Connected Vehicles Challenge
# Objective
Design the coolest connected vehicles application or platform.


# Board Details

As part of this challenge, contestants are offered a ESP32 development board. Your final demo does not need to incorporate the board. It is meant to give you more options.

Node MCU-ESP32 Board:
https://esphome.io/devices/nodemcu_esp32.html


# Using the Arduino IDE

If you wish to develop for this board using the Arduino IDE, you can download it from:

https://www.arduino.cc/en/main/software


Adding the board to the Arduino IDE:

https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/

Flash the board with either 'ESP32 Dev Module' board or the 'Node MCU-32S' board.

# Sample Client Code

This repository contains client side code samples that can be used to communicate with our BWM i8 Remote control vehicle.

Client -> i8Controller -> Vehicle

This code is written for for the ESP32 Development board and contains APIs to manipulate the vehicle over Bluetooth Low Energy (BLE) and over WIFI.

# Interfacing with the i8Controller over BLE

Using BLE client code, you can scan the nearby BLE devices and find the one advertising our serviceUUID.
You can then write to  the defined characteristic UUID in order to issues commands to the BMW i8.  

| UUID | Value |
| ------ | ------ |
| serviceUUID | 4fafc201-1fb5-459e-8fcc-c5c9c331914b |
| characteristic UUID| beb5483e-36e1-4688-b7f5-ea07361b26a8 |

BLE Commands

| Message (1 byte) | Action |
| ------ | ------ |
| 0x46 | Forward |
| 0x42 | Back |
| 0x52 | Right |
| 0x4C | Left |
| 0x53 | Stop |
| 0x46 | Driver Door |
| 0x44 | Passenger Door |
| 0x50 | Forward |

Sample code found in:
connect/hw/esp32/arduino/bleClient

#Interfacing with the i8Controller over WIFI/Rest

Over wifi, you can send REST commands to the 

| URL | Method| Action |
| ------ | ------ | ------ |
| /api/action/toggledriverdoor | GET | Open/Close Driver Door|
| /api/action/togglepassengerdoor | GET | Open/Close Passenger Door|
| /api/action/forward | GET | Drive forward|
| /api/action/back | GET | Drive backwards|
| /api/action/stop | GET | Stop|
| /api/action/right | GET | Drive Right|
| /api/action/left | GET | Drive Left|

Sample code found in:
connect/hw/esp32/arduino/webClient

# Having Issues

| Problem | Details | Resotion |
| ------ | ------ | ------ |
| Failed to connect to board when uploading | A fatal error occurred: Failed to connect to ESP32: Invalid head of packet (0x0A)|Press and hold the GPIO-0 on board switch when connecting. |
