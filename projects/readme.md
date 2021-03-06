# Pg Projects Library
A collection of Arduino projects built with the Pg Libraries that demonstrate modern, object-oriented design methodologies. 

### EZThermometer
A simple sketch to read ambient temperature from a thermistor and display it on an LCD display.

### EZThermostat
A simple sketch demonstrating a thermostat that reads ambient temperature from a thermistor and passes through a proportional-integral-derivative (PID) algorithm to generate a pulse-width-modulated (PWM) output that can be used to control machinery, such as a heater. Shows how the Pg Libraries can perform complex tasks in only a few lines of code. 

### Jack 
Demonstrates how an Arduino can be used as a remote control and data acquisition (DAQ) platform. Jack communicates over a network using a simple query language that hosts use to configure and control GPIO pins on the device (see pg/docs/Jack/Description.txt and View below). 

### PIDThermostat
A programmable PID-controlled thermostat with a fully functioning user interface, process control, alarm, editable settings and EEPROM storage/recall.

### PgServo 
A simple program that demonstrates how a servo can be controlled remotely with commands over a serial connection.

### Stopwatch
A simple stopwatch program that displays the elapsed time and can be controlled from a keypad. Demostrates how to implement multitasking schemes using callbacks.

### Thermometer
A programmable digital thermometer with a fully functioning user interface, alarm, editable settings and EEPROM storage/recall.

### View
Windows client app that remotely controls and displays the state of a Jack device. This is a demonstration version, updates for a production version coming soon.
