Sensor Breakout
===============

http://fabiobaltieri.com/2014/06/15/sensor-breakout/

This project is brekout board for prototyping wireless sensors.

Main features:
- ATMega88P microcontroller (can mount up to an ATMetga328)
- nRF24L01+ low power 2.4GHz radio
- On board Lithium battery charger and 3.3V regulator
- Dual mounting option for the USB port (micro B and A)
- Prototype area

License
-------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Contents
--------

    COPYING      text version of the GPL
    README       this file
    bootloader/  source directory for bootloader
    firmware/    source directory for main firmware
    hardware/    source directory for the hardware design

Firmware and Bootloader
-----------------------

Building requires an avr-gcc toolchain, in the firmware/ or bootloader/
directory, to build run:

    make

Flashing the firmware on the device requires avrdude and a compatible hardware
programmer. Default configuration is stored at the beginning of the Makefile.
To program with the default configuration, run:

    make flash

Fuses can be configured running:

    make fuses

To program the main firmware from the bootloader, use the command:

    make boot

Hardware
--------

All hardware files (schematic, layout and libraries) are in CadSoft Eagle
format.
