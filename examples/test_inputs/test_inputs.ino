// This program demonstrates the use of the Pg AnalogInput class, how 
// to configure and poll an analog input, and issue a callback if the 
// input value falls within a specified set of ranges. Description and 
// usage info can be found at the top of the <AnalogInput.h> file.
// Note: this program uses named namespaces which are supported by the 
// Arduino IDE v1.8 and later. Pg promotes strongly-typed, 
// const-correct, modern object-oriented design methodologies. There 
// are no macros, "magic numbers" and almost nothing is of type <int>. 
// Join the 21st century!!

#include <pg.h> 
#include <components/AnalogInput.h>
using namespace pg;

// Callback function declaration. 
void inputCallback();

// Range objects (nested type defined in AnalogInput), are constructed 
// from a `range_type' which is of type std::pair<analog_t, analog_t>,  
// where member `first' represents the low value of the range and 
// `second' the high value. A match occurs if the value returned by 
// analogRead() is in [first, second]. `std::pair' is a type defined by 
// the C++ Standard Library in header <utility>. `analog_t' is a 
// descriptive type alias that holds values of the type returned by the 
// analogRead() function.
using Input = AnalogInput<>;
using InputRange = typename Input::Range;

AnalogInput<>::Range rangeA{ InputRange { 0, 60 } };
AnalogInput<>::Range rangeB{ InputRange { 61,200 } };
AnalogInput<>::Range rangeC{ InputRange { 201,400 } };
AnalogInput<>::Range rangeD{ InputRange { 401,600 } };
AnalogInput<>::Range rangeE{ InputRange { 601,800 } };

// Analog input pin. AnalogInput objects must be attached to a valid GPIO 
// analog input pin. `pin_t is a descriptive type alias that holds values 
// of GPIO pin number.
const pin_t AnalogInputPin = 0; // Attach to AN0.

// AnalogInput objects are constructed from the input pin, callback method and 
// optinally a list of ranges. Here a callback will be generated whenever the 
// polled input value falls within one of the specified ranges and the matched 
// range is different from the previously matched range. Thus, this configuration 
// only notifies the client on changes in input conditions that satisfy some 
// criteria - all with one line of code. Brilliant!! This behavior can be 
// changed by the client (see decription in <AnalogInput.h>).
AnalogInput<> analog_input(AnalogInputPin, inputCallback, { &rangeA,&rangeB,&rangeC,&rangeD,&rangeE });

void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  analog_input.poll();
  delay(100); // Input switch debounce.
}

// Method that handles AnalogInput callbacks. Depending on how an AnalogInput 
// object is configured, callbacks can be issued when: the object is polled,   
// the polled input value falls within any specified range, or the polled value 
// matches a range different from the previously matched range. Here we simply 
// print the callback arguments to the serial monitor.
void inputCallback()
{
  Serial.print("input("); Serial.print(analog_input.attach()); Serial.print(") = "); Serial.println(analog_input.value());
  if (analog_input.range())
  {
    Serial.print("range {"); Serial.print(analog_input.range()->low()); Serial.print(",");
    Serial.print(analog_input.range()->high()); Serial.println("}");
  }
}
