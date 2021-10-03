#include <pg.h>
#include "components/AnalogInput.h"
using namespace pg;

enum class AnalogInput::Range::Tag
{
  A = 0,
  B,
  C, 
  D, 
  E
};

void input_cb(pin_t pin, analog_t value, AnalogInput::Range* range)
{
  Serial.print("input("); Serial.print(pin); Serial.print(") = "); Serial.println(value);
  if (range)
  {
    Serial.print("range {"); Serial.print(range->range_.first); Serial.print(",");
    Serial.print(range->range_.second); Serial.println("}");
  }
}

AnalogInput::Range rangeA{ AnalogInput::Range::range_type(0,60), AnalogInput::Range::Tag::A };
AnalogInput::Range rangeB{ AnalogInput::Range::range_type(61,200), AnalogInput::Range::Tag::B };
AnalogInput::Range rangeC{ AnalogInput::Range::range_type(201,400), AnalogInput::Range::Tag::C };
AnalogInput::Range rangeD{ AnalogInput::Range::range_type(401,600), AnalogInput::Range::Tag::D };
AnalogInput::Range rangeE{ AnalogInput::Range::range_type(601,800), AnalogInput::Range::Tag::E };
AnalogInput in(0, &input_cb, { &rangeA,&rangeB,&rangeC,&rangeD,&rangeE });

void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  static_cast<iclockable*>(&in)->clock();
  delay(100); // Input switch debounce.
}
