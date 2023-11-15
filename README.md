# Lab4.2-Remote-control-car

# MEAM 5100 Group 1 - Motor and Encoder Feedback Control

This code is developed for MEAM 5100 Group 1 by Yining Guo. It provides motor and encoder feedback control for a car assembly and is designed to be compatible with a 9V battery.

## Table of Contents
- [Dependencies](#dependencies)
- [Pin Configuration](#pin-configuration)
- [Motor and Encoder Setup](#motor-and-encoder-setup)
- [WiFi Configuration](#wifi-configuration)
- [Web Interface](#web-interface)
- [Control Functions](#control-functions)
- [Interrupt Handling](#interrupt-handling)
- [Setup Function](#setup-function)
- [Main Loop](#main-loop)

## Dependencies
- WiFi library
- WiFiUdp library
- Webpage.h
- html510.h

## Pin Configuration
- Motor Channel 1: GPIO pins 6 and 7
- Motor Channel 2: GPIO pins 18 and 19
- Servo Motor: GPIO pin 10
- Photodiode 1: GPIO pin 1
- Photodiode 2: GPIO pin 4

## Motor and Encoder Setup
The code configures motor channels using the `ledcSetup` function and attaches pins to these channels with `ledcAttachPin`. It also sets up interrupt handling for photodiodes.

## WiFi Configuration
WiFi credentials (SSID and password) are set in the `ssid` and `password` variables. The code connects to the specified WiFi network and configures the IP address.

## Web Interface
A web server is created to serve HTML pages. Handlers are attached to process different routes, such as setting motor direction, duty cycle, and servo position.

## Control Functions
A control function is implemented to adjust the motor output based on the desired value and sensor feedback. Proportional control (`KP`) is used.

## Interrupt Handling
Interrupts are used to measure the speed of the car through photodiodes. Two interrupt service routines (`handleInterrupt1` and `handleInterrupt2`) are defined to record interrupt times.

## Setup Function
The `setup` function initializes the serial communication, sets up motors, servos, interrupts, and establishes a WiFi connection. It also initializes the web server and attaches handlers.

## Main Loop
The `loop` function continuously serves the web interface, handles interrupts, and updates motor control based on feedback from photodiodes.

**Note:** This README provides a high-level overview of the code. For detailed information on each function and variable, refer to the code comments.
