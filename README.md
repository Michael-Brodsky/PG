# Pg (Pretty Good)

Pg is a development library for the Arduino (avr) platform that includes a C++ Standard Library which uses only static memory, interfaces for implementing common C++ design patterns, components for common system tasks such as scanning a keypad or writing formatted output to an lcd, and a set of utilities including timers, schedulers, eeprom streaming support, fast math algorithms and engineering functions.

This is a work in progress with almost daily updates. Check back often for more stuff!

Installation:

The entire Pg folder must be installed under your sketchbook/libraries folder (the same location all other user libraries go).

Usage:

Every project using Pg must include the <pg.h> header and this should be done from the Arduino IDE: Sketch->Include Library->Pg. Additional #include files can be placed below that. Including files from subdirectories (those not in Pg/src) must be qualified: 

/* #include <lib/imath.h>, or 
#include <interfaces/icommand.h> */
  
Cheers!
