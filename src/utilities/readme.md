# Pg Utility Library
This library contains a collection of classes and functions that provide commonly needed services in embedded systems development.

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

### Unique.h 
Unique is a base class that provides a unique identifier (number) to derived types which clients can use to identify multiple instances of the same object type.

### SerialProtocols.h 
The SerialProtocols class manipulates and stores/retrieves communications settings in EEPROM. It allows programs to enable real-time editing, storage and retrieval of communications settings with single lines of code.
