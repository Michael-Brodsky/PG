#include <pg.h>
#include <components/DigitalInput.h>
using namespace pg;

DigitalInput input1(2), input2(3);

void digital_cb1()
{
  Serial.print("pin("); Serial.print(input1.attach()); Serial.print(") = "); Serial.println(input1.value());
}
void digital_cb2()
{
  Serial.print("pin("); Serial.print(input2.attach()); Serial.print(") = "); Serial.println(input2.value());
}
void setup() 
{
  Serial.begin(9600);
  input1.callback(digital_cb1); 
  input2.callback(digital_cb2);
}

void loop() 
{
  input1.poll();
  input2.poll();
}
