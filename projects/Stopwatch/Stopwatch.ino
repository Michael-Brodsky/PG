// This sketch demonstrates a stopwatch that displays elapsed time as 
// hh:mm:ss.t on an LCD display and can be started, stopped and reset
// using a keypad.
// 
// The program assumes a compatible LCD/Keypad Shield is attached to 
// the board, but this is not a strict requirement. To start/stop the 
// stopwatch press the <Select> key. To reset it press the <Left> key.
//
// This program uses a non-preemptive multitasking scheme and callbacks 
// to handle the tasks of polling the keypad and refreshing the display.
// Tutorials can be found at any 4-year course in Computer Science and 
// with a few years experience in embedded systems development. 

#include <LiquidCrystal.h>				// Arduino LCD api.
#include <pg.h>							// https://github.com/Michael-Brodsky/pg
#include <components/LCDDisplay.h>		// Defines an async display manager class.
#include <components/AnalogKeypad.h>	// Defines an async keypad polling class.
#include <utilities/TaskScheduler.h>	// Asynchronous task scheduler.

using namespace pg;
using namespace std::chrono;

using Keypad = AnalogKeypad<>;			// Keypad uses analogRead() type.
using Display = LCDDisplay<16, 2>;		// Defines a 16x2 character display.
using Scheduler = TaskScheduler<>;		// Scheduler has millisecond resolution.
using Stopwatch = Timer<milliseconds>;	// Stopwatch timer has millisecond resolution.
using tenths = duration<std::time_t, std::deci>; // Type that counts tenths of a second.
												 // duration is std::chrono::duration
												 // Arduino-compatible C++ Standard Library
												 // avalable at https://github.com/Michael-Brodsky/pg
//
// Callback functions.
//
void displayCallback();
void keypadCallback(const Keypad::Button*, Keypad::Event);
//
// Keypad objects.
//
const pin_t KeypadInputPin = 0;			// Buttons must be attached to a valid analog input.
const Keypad::value_type LeftButtonTriggerLevel = 600;
const Keypad::value_type SelectButtonTriggerLevel = 800;
Keypad::Button btn_left(LeftButtonTriggerLevel);
Keypad::Button btn_select(SelectButtonTriggerLevel);
Keypad keypad(KeypadInputPin, &keypadCallback, { &btn_left, &btn_select });
//
// Display objects.
//
Display::Field fld_hours = { 6,1,"","%02u:",true }; // Field params are {col,row,label,format,visible}
Display::Field fld_mins = { 9,1,"","%02u:",true };	// formats use the printf() format specifiers.
Display::Field fld_secs = { 12,1,"","%02u.",true };	// Using the wrong fmt spec will result in garbled output.
Display::Field fld_tenths = { 15,1,"","%1u",true };
Display::Screen screen({ &fld_hours,&fld_mins,&fld_secs,&fld_tenths }, "STOPWATCH");
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);	// Attach the lcd device to the appropriate pins.
Display display(&lcd, displayCallback, &screen); // Init the display mgr.

//
// Task are scheduled asynchronously by the Scheduler. This allows the program to 
// poll the keypad and refresh the display at any interval without using delay().
// delay() is EVIL!! Do not use.
//
ClockCommand clock_keypad(&keypad);
ClockCommand clock_display(&display);
Scheduler::Task task1(milliseconds(100), &clock_keypad, Scheduler::Task::State::Active);
Scheduler::Task task2(milliseconds(100), &clock_display, Scheduler::Task::State::Active);
Scheduler scheduler({ &task1, &task2 });

Stopwatch timer;		// The stopwatch timer.
bool running = false;	// Flag indicating whether the stopwatch is running or stopped.

void setup() 
{
	lcd.begin(Display::cols(), Display::rows());
	display.clear();
	display.refresh(0, 0, 0, 0); // Show 0 time elapsed.
	scheduler.start();
}

void loop() 
{
	scheduler.tick(); // Asynchronous programs only need one line of code in loop().
}

void displayCallback()
{
	// Convert elapsed time in milliseconds to hours, minutes, seconds and tenths.
	milliseconds ms = timer.elapsed();
	auto tenth = std::chrono::duration_cast<tenths>(ms) % 10;
	ms -= std::chrono::duration_cast<milliseconds>(tenth);
	auto secs = std::chrono::duration_cast<seconds>(ms);
	ms -= std::chrono::duration_cast<milliseconds>(secs);
	auto mins = std::chrono::duration_cast<minutes>(secs);
	secs -= std::chrono::duration_cast<seconds>(mins);
	auto hour = std::chrono::duration_cast<hours>(mins);
	mins -= std::chrono::duration_cast<minutes>(hour);
	// Refresh the display with the elapsed time. The display manager 
	// automatically formats the time according to the Display::Field formats.
	// Args must be passed in same order as the fields used to initialize the 
	// Screen object.
	display.refresh(hour, mins, secs, tenth);
}

void keypadCallback(const Keypad::Button* button, Keypad::Event event)
{
	switch (event)
	{
	case Keypad::Event::Press:
		if (button->id() == btn_select.id())
		{
			if ((running = !running)) timer.resume();
			else timer.stop();
		}
		else if (button->id() == btn_left.id())
		{
			if (!running) timer.reset();
		}
	default:
		break;
	}
}
