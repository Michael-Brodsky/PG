# Pg Utility Library
The Pg Utility Library contains a collection of classes and functions that provide commonly needed services in embedded systems development.

### Timer.h
Defines a simple interval timer class for timing and scheduling events. Timers can be set, started, stopped, resumed and queried by clients.

### CommandTimer.h
Extends the Timer class in <Timer.h> by adding the ability to execute "Command" objects (see <interfaces/icommand.h>) at specified intervals.

### TaskSheduler.h
The TaskSheduler class is used to execute tasks at scheduled intervals concurrently, and to prioritize tasks or manage CPU loads.

### EEStream.h 
The EEStream class enables simple object serialization/deserialization to and from the onboard EEPROM memory.

### PIDController.h
The PIDController class is a control loop mechanism used in industrial automation applications to control devices according to a proportional-integral-derivative (PID) algorithm.

### PWMOutput.h
The PwmOutput class provides a simple interface for managing pulse-width modulated (PWM) outputs and generating waveforms.

### RemoteControl.h
The RemoteControl class facilitates asynchronous control of peripheral hardware by a remote host using commands sent over a serial port. 

### Serial.h 
Defines several types that extend the capabilities of the Arduino HardwareSerial objects.

### Unique.h 
Unique is a base class that provides a unique identifier (number) to derived types which clients can use to identify multiple instances of the same object type.

### ValueWrappers.h 
Defines several wrapper classes that encapsulate objects and collections, and expose getters and setters with familiar semantics. They are useful, for instance, in mapping machine- to human-readable values for user interfaces and eeprom storage of program settings.
