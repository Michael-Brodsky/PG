#include <pg.h>
#include "components/AnalogInput.h"
using namespace pg;

// This program demonstrates how to configure and poll an analog input and 
// issue a callback if the input value falls within a specified set of ranges.

// Range tags, these should be descriptive, e.g. LowRange, HiRange, etc. as 
// they identify any input ranges you define.
enum class AnalogInput::Range::Tag
{
  A = 0,
  B,
  C, 
  D, 
  E
};

// Callback function. 
void input_cb(pin_t pin, analog_t value, AnalogInput::Range* range)
{
  Serial.print("input("); Serial.print(pin); Serial.print(") = "); Serial.println(value);
  if (range)
  {
    Serial.print("range {"); Serial.print(range->range_.first); Serial.print(",");
    Serial.print(range->range_.second); Serial.println("}");
  }
}

// Input ranges, range_type is std::pair<analog_t, analog_t>, first is range low, second is range hi.
// analog_t is a descriptive alias for uint16_t, the type returned by Arduino `analogRead()'.
// Also pin_t is a descriptive alias for uint8_t, a type that can hold any valid GPIO pin number, 
// unlike Arduino's int (everything is int), which is not type-safe because int is a signed integer 
// type that can hold negative values which are not valid pin numbers and poor programming practice.
// Or, if you're lazy, screw it, everything's an int.
AnalogInput::Range rangeA{ AnalogInput::Range::range_type(0,60), AnalogInput::Range::Tag::A };
AnalogInput::Range rangeB{ AnalogInput::Range::range_type(61,200), AnalogInput::Range::Tag::B };
AnalogInput::Range rangeC{ AnalogInput::Range::range_type(201,400), AnalogInput::Range::Tag::C };
AnalogInput::Range rangeD{ AnalogInput::Range::range_type(401,600), AnalogInput::Range::Tag::D };
AnalogInput::Range rangeE{ AnalogInput::Range::range_type(601,800), AnalogInput::Range::Tag::E };
// AnalogInput object attached to AN0, will issue callback if input level is within any of the defined ranges.
AnalogInput in(0, &input_cb, { &rangeA,&rangeB,&rangeC,&rangeD,&rangeE });

void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  in.poll();
  delay(100); // Input switch debounce.
}
