# Pg Component Library
A collection of generic, reusable objects, built on the mediator/component design pattern, that provide common services to programs.

### AnalogInput.h
Asynchronous GPIO analog input polling.

### AnalogKeypad.h 
Configuration and asynchronous polling of keypads connected to an analog input..

### DigitalInput.h 
Asynchronous GPIO digital input polling.

### EventSequencer.h 
Facilitates asynchronous execution of code as a timed sequence.

### Jack.h
Turns an Arduino into a remotely controllable data acquisition (DAQ) device.

### LCDDisplay.h 
Facilitates configuration and asynchronous operation of an LCD display including formatted printing.

### RemoteControl.h
The RemoteControl class facilitates asynchronous control of peripheral hardware by a remote host using commands sent over a serial port. NOTE: The RemoteControl class is deprecated. Clients should use the Interpreter class instead (see utilities/Interpreter.h).

### SweepServo.h 
Asynchronous servo controller that uses natural units, degrees of rotation and angular velocity, instead of pulse widths.
