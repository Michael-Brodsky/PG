#include <pg.h>
#include<utilities/timer.h>
using namespace pg;
using namespace std::chrono;

Timer<milliseconds> timer(seconds(2)); // millisecond timer set to 2s, which = 2000ms;

void setup() 
{
  Serial.begin(9600);
  Serial.print("timer interval = "); Serial.print(timer.interval().count()); Serial.println("ms");
  timer.start();
}

void loop() 
{
  if(timer.active()) 
  {
    if (!timer.expired())
    { 
      Serial.println(timer.elapsed().count());
      delay(500);
    }
    else 
    {
      timer.stop();
      Serial.println("done");
    }
  }
}
