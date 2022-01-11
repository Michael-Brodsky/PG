#include <pg.h>
#include <components/LCDDisplay.h>
// This program demonstrates how to configure a display with two screens, 
// one to show a running stopwatch and another the temperature and toggle 
// back and forth between them. 
using namespace pg;
using namespace std::chrono;
using Display = LCDDisplay<16, 2>; // Using a 16x2 display device.
using DelayTimer = Timer<milliseconds>;
using tenths = duration<std::time_t, std::deci>;
using time_point_ms = time_point<steady_clock, milliseconds>;

void doit(); // Function that does something.

// Configure the display:
// Each field is initialized with: {col, row, "label", "fmt"}, we're not using labels, and "fmt" is a printf() print format specifier.
// The following will configure the screens thus:
//
// ********************
// *Time  hh::mm::ss.x*
// *                  *
// ********************

// ********************
// *Temp          nn.m*
// *                  *
// ********************
Display::Field field1 = { 6,0,"","%02u:",true }; // Hours field
Display::Field field2 = { 9,0,"","%02u:",true }; // Minutes field
Display::Field field3 = { 12,0,"","%02u.",true };  // Seconds field.
Display::Field field4 = { 15,0,"","%1u",true };  // Tenths field.
Display::Field field5 = { 10,0,"","%5.1f",true };  // Temperature field.
Display::Field field6 = { 15,0,"","%c",true };  // Degree symbol field.
Display::Screen screen1({ &field1,&field2,&field3,&field4 }, "Time"); // Time screen uses fields 1-4.
Display::Screen screen2({ &field5,&field6 }, "Temp"); // Temp screen uses fields 5-6.

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  // Your pinout may vary.
Display display(&lcd, &doit, &screen1); // Create the Display, doit() is the callback, screen1 is the active screen.

DelayTimer timer(seconds(5));   // Timer to switch back and forth between screens.

// Temperature reading. Normally the program would get this value from a sensor, or 
// maybe call your mom, have her look at the thermometer and press some buttons 
// connected to the Arduino that input the temp.
float temp = 42.0;

void setup() 
{
  lcd.begin(display.cols(), display.rows());
  timer.start(); // Gentlemen, start your timers.
}

void loop() 
{
  while (1)
  {
    if (timer.expired()) // When the timer expires, it switches the screen from time to temp.
    {
      display.clear();
      if (display.screen() == &screen1)
      {
        display.screen(&screen2);
        temp += 1.0f; // It's getting hotter!
      }
      else display.screen(&screen1);
      timer.reset();
    }
    static_cast<iclockable*>(&display)->clock(); // EVIL hack to call the private clock() method. 
    delay(100); // 
  }
}

void doit()
{
  if (display.screen() == &screen1)
  {
    // A stopwatch sorta thingy.
    time_point_ms now(steady_clock::now());
    auto ms = milliseconds(now - time_point_ms());

    auto tenth = duration_cast<tenths>(ms) % 10;
    ms -= duration_cast<milliseconds>(tenth);
    auto secs = duration_cast<seconds>(ms);
    ms -= duration_cast<milliseconds>(secs);
    auto mins = std::chrono::duration_cast<minutes>(secs);
    secs -= duration_cast<seconds>(mins);
    auto hour = duration_cast<hours>(mins);
    mins -= duration_cast<minutes>(hour);

    display.refresh(hour, mins, secs, tenth);
  }
  else
  {
    // Show current temp.
    display.refresh(temp, (char)0xDF);
  }
}
