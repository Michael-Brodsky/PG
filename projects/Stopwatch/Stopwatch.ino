#include <LiquidCrystal.h>
#include <pg.h>
#include <components/LCDDisplay.h>
#include <components/AnalogKeypad.h>
#include <utilities/TaskScheduler.h>

// Demonstration of a simple stopwatch program.
// Detailed descriptions can be found in the comments of the #include files.

using namespace pg;
using namespace std::chrono;
using Display = LCDDisplay<16, 2>; // Type alias for a 16x2 lcd display.
using Scheduler = pg::TaskScheduler<>; // Type alias for the default TaskScheduler (has millisecond resolution).
using Stopwatch = Timer<milliseconds>;  // Type alias for millisecond timer.
using tenths = duration<std::time_t, std::deci>; // Type alias for tenths of a second duration.

// Button tag enumerator, use descriptive names for each button you have wired up.
enum class Keypad::Button::Tag 
{
	Right = 0,
	Up,
	Down,
	Left,
	Select
};

// Forward decls.
void refresh_display();
void keypad_callback(const Keypad::Button* button, Keypad::Event event);

// Keypad Button objects. Select: start/stop, Left: reset.
const pin_t KeypadInputPin = 0; // Keypad input is AN0.
const analog_t LeftButtonTriggerLevel = 600; // Button trigger levels depend on how buttons are wired to board.
const analog_t SelectButtonTriggerLevel = 800;  // If using an LCD/Keypad Shield (recommended), use the published values.
Keypad::Button btn_left(Keypad::Button::Tag::Left, LeftButtonTriggerLevel); // Resets the stopwatch.
Keypad::Button btn_select(Keypad::Button::Tag::Select, SelectButtonTriggerLevel); // Starts/stops the stopwatch.
Keypad keypad({ &btn_left, &btn_select }, KeypadInputPin, &keypad_callback); // Keypad params are { &buttons_list }, pin, &callback_func.

// Display Screen and Field objects. 16x2 Display is formatted thus (t is tenths): 
//
// *****************
// *     hh:mm:ss.t* 
// *               *
// *****************

Display::Field fld_hours = { 6,0,"","%02u:" }; // Field params are {col,row,"label","format"} (see printf() format specifiers).
Display::Field fld_mins = { 9,0,"","%02u:" };
Display::Field fld_secs = { 12,0,"","%02u." };
Display::Field fld_tenths = { 15,0,"","%1u" };
Display::Screen screen({ &fld_hours,&fld_mins,&fld_secs,&fld_tenths }, ""); // Screen params are { fields_list }, "label".
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Display display(&lcd, &refresh_display, &screen); // Display params are &LiquidCrystal, &callback_func, &active_screen.

// TaskScheduler & Task objects.
ClockCommand clock_keypad(&keypad); // ClockCommand params are &clockable_object.
ClockCommand clock_display(&display);
Scheduler::Task task1(milliseconds(100), &clock_keypad, Scheduler::Task::State::Active); // Task params are std::chrono::duration, &ClockCommand, Task::State
Scheduler::Task task2(milliseconds(100), &clock_display, Scheduler::Task::State::Active);
Scheduler scheduler({ &task1, &task2 });

Stopwatch timer; // The stopwatch timer that tracks elapsed time.
bool running = false; // Flag indicating whether stopwatch is running.

void setup() 
{
	lcd.begin(Display::cols(), Display::rows()); // Params are Display template parameters. 
	display.refresh(0, 0, 0, 0); // Displays 00:00:00.0 on the lcd.
	scheduler.start(); // Don't forget to start the task scheduler or nothing will happen.
}


void loop() 
{
	scheduler.tick();
}

void refresh_display()
{
  // extract hh, mm, ss & tenths from milliseconds.
	milliseconds ms = timer.elapsed();
	auto tenth = std::chrono::duration_cast<tenths>(ms) % 10;
	ms -= std::chrono::duration_cast<milliseconds>(tenth);
	auto secs = std::chrono::duration_cast<seconds>(ms);
	ms -= std::chrono::duration_cast<milliseconds>(secs);
	auto mins = std::chrono::duration_cast<minutes>(secs);
	secs -= std::chrono::duration_cast<seconds>(mins);
	auto hour = std::chrono::duration_cast<hours>(mins);
	mins -= std::chrono::duration_cast<minutes>(hour);

	display.refresh(hour, mins, secs, tenth); // Print it!
}

// Handles button press events. Usually ginormous, nested switch() statements, but here pretty simple.
void keypad_callback(const Keypad::Button* button, Keypad::Event event)
{
	switch (event)
	{
	case Keypad::Event::Press:
		switch (button->tag_)
		{
		case Keypad::Button::Tag::Select: // Select button pressed.
			if ((running = !running)) timer.resume();
			else timer.stop();
			break;
		case Keypad::Button::Tag::Left:   // Left button pressed.
			if (!running) timer.reset();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}
