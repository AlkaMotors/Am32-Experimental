# AM32-MultiRotor-ESC-firmware
Firmware for ARMbased speed controllers for use with many vehicles.
This is an experimental version of the firmware. It can only be used with config tool version 2.0 .. see download link below. The firmware is not compatible with older versions of the config tool and the tool is not compatible with older version of the firmware.
<p align="left">
  <a href="/LICENSE"><img src="https://img.shields.io/badge/license-GPL--3.0-brightgreen" alt="GitHub license" /></a>
</p>

The AM32 firmware is designed for STM32 ARM processors to control a brushless motor (BLDC).
The firmware is intended to be safe and fast with smooth fast startups and linear throttle. It is meant for use with multirotor aircraft and a flight controller. The firmware can also be built with support for crawlers.

## Features

AM32 has the following features:

- Firmware upgradable via betaflight passthrough - only support G4, F4 and F7 FCs, and make sure your betaflight firmware is up to date when you trying to use passthrough
- Servo PWM, Dshot(300, 600) motor protocol support
- Bi-directional Dshot
- KISS standard ESC telemetry
- Variable PWM frequency
- Sinusoidal startup mode, which is designed to get larger motors up to speed

## Firmware Release & Configuration Tool

To configure AM32 and upload firmware you can use the Multi_ESC config tool using Betaflight passthrough. They can be downloaded from here:

[WINDOWS](https://drive.google.com/file/d/1fX8EEmHOVC6wkJ1K-cf0KUevaaFpsrv4/view?usp=sharing) 
## Installation & Documentation

To use AM32 firmware on a blank ESC, a bootloader must first be installed using a ST-LINK. Choose the bootloader that matches the signal input pin of the ESC.
The compatibility chart has the bootloader pinouts listed.
Current bootloaders can be found [here](https://github.com/AlkaMotors/AM32_Bootloader_F051/releases/tag/V6).


## Support and Developers Channel

There's a dedicated Slack chat channel here:

https://am32-invite.herokuapp.com/


