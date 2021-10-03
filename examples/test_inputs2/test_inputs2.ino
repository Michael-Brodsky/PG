#include <pg.h>
#include "components/DigitalInput.h"
using namespace pg;

void digital_cb(pin_t pin, bool level)
{
  Serial.print("pin("); Serial.print(pin); Serial.print(") = "); Serial.println(level);
}

DigitalInput input1(2), input2(3);
input1.callback(&digital_cb); 
input2.callback(&digital_cb);
  
void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  static_cast<iclockable*>(&input1)->clock();
  static_cast<iclockable*>(&input2)->clock();
}
