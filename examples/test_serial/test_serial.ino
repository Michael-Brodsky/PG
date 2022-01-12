#include <pg.h>
#include <utilities/Serial.h>

using pg::usart::serial;
using pg::usart::hardware_serial;

serial<0> sp0;

void setup() 
{
  sp0.begin();
  if(sp0.isOpen())
  {
    char buf[64];
    
    sp0.printFmt(buf, "%s: %s=%lu, %s=%3s, %s=%lu", "sp0", "baud", sp0.baud(), "frame", 
      std::find(sp0.SupportedFrames.begin(), sp0.SupportedFrames.end(), sp0.frame())->string(), 
      "timeout", sp0.getTimeout());
  }
  sp0.println("Send strings to the serial port.");
}

void loop() 
{
  sp0.loopBack();
}
