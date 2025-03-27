#include <pg.h>
#include<utilities/CommandTimer.h>
using namespace pg;
using namespace std::chrono;

// Simple interval timer.
Timer<milliseconds> timer(seconds(2)); // millisecond timer set to 2s, which = 2000ms;

// Command timer.
void foobar42() { Serial.println("foobar42() called by cmd_timer"); }
Command<void> command(&foobar42); // Command object to execute, calls foobar42().
CommandTimer<microseconds> cmd_timer(seconds(3), &command); // microsecond command timer set to 2s, which = 2000000us;

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
      cmd_timer.start();
      Serial.print("cmd_timer started, interval = "); Serial.println(cmd_timer.interval().count());
    }
  }
  else if(cmd_timer.active())
  {
    if (!cmd_timer.expired())
      cmd_timer.tick();
    else
      cmd_timer.stop();
  }
}
