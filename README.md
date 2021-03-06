# Pg (Pretty Good)

Pg is a development library (SDK) for the Arduino (avr/megaavr/sam/samd) and other low-cost architectures that includes a subset of the C++ Standard Library that uses only static memory, interfaces for implementing common C++ design patterns, components for common system tasks such as scanning keypads, writing formatted output to displays, driving servos, reading sensors, network communications, and a set of utilities including timers, schedulers, language interpreters, data streaming, fast math algorithms and engineering functions. There's also an application library of Remote Data Acquisition, HMI, SCADA and IoT projects.

The libraries support rapid development of industrial automation and similar applications on inexpensive hardware. They provide solutions to common application tasks such as data acquisition and display, process control, user interfaces, alarms, timers and network communications. See the /projects folder for examples.

This is a work in progress with almost daily updates. Check back often for more stuff!

## Requirements

Pg requires named namespace support, which is available on the Arduino IDE v1.8 and later.

## Installation:

The entire Pg folder must be installed under your sketchbook/libraries folder (the same location all other user libraries go). A zip or tarball file can be downloaded by clicking the latest release link in the Releases section of this page, and then extracted to the appropriate location on your machine, or by using the Arduino IDE's: Sketch -> Include Library -> Add .ZIP Library menu option and pointing it to the latest release. 

NOTE: If you download a .zip file using the green "Code" button, Github will rename the file to "Pg-main.zip" and the root folder to "Pg-main" - some sort of feature, I guess. It is advised that these be renamed back to just "Pg".

## Usage:

Every project using Pg must include the <pg.h> header and this should be done from the Arduino IDE: Sketch -> Include Library -> Pg. Additional #include files can be placed below that. Including files from subfolders (those not in Pg/src) must be qualified: 

<p> #include &ltlib/imath.h&gt, or <br>
#include &ltinterfaces/icommand.h&gt </p>

The libraries are used by simply invoking the objects defined within them. Documentation exists as inline comments in the source files and usage examples, in the form of test and validation code, can be found in the /examples folder.
  
Cheers!
