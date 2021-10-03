#include <LiquidCrystal.h>
#include <pg.h>
#include <components/LCDDisplay.h>
#include <components/AnalogKeypad.h>
#include <utilities/TaskScheduler.h>

using namespace pg;
using namespace std::chrono;
using Display = LCDDisplay<16, 2>;
using Scheduler = pg::TaskScheduler<>;
using Stopwatch = Timer<milliseconds>;
using tenths = duration<std::time_t, std::deci>;

enum class pg::ButtonTag 
{
	Right = 0,
	Up,
	Down,
	Left,
	Select
};

void refresh_display();
void keypad_callback(const Keypad::Button* button, Keypad::Event event);

const pin_t KeypadInputPin = 0;
const analog_t LeftButtonTriggerLevel = 600;
const analog_t SelectButtonTriggerLevel = 800;
Keypad::Button btn_left(ButtonTag::Left, LeftButtonTriggerLevel);
Keypad::Button btn_select(ButtonTag::Select, SelectButtonTriggerLevel);
Keypad keypad({ &btn_left, &btn_select }, KeypadInputPin, &keypad_callback);

Display::Field fld_hours = { 6,0,"","%02u:" };
Display::Field fld_mins = { 9,0,"","%02u:" };
Display::Field fld_secs = { 12,0,"","%02u." };
Display::Field fld_tenths = { 15,0,"","%1u" };
Display::Screen screen({ &fld_hours,&fld_mins,&fld_secs,&fld_tenths }, "");
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Display display(&lcd, &refresh_display, &screen);

ClockCommand clock_keypad(&keypad);
ClockCommand clock_display(&display);
Scheduler::Task task1(milliseconds(100), &clock_keypad, Scheduler::Task::State::Active);
Scheduler::Task task2(milliseconds(100), &clock_display, Scheduler::Task::State::Active);
Scheduler scheduler({ &task1, &task2 });

Stopwatch timer;
bool running = false;

void setup() 
{
	lcd.begin(Display::cols(), Display::rows());
	display.refresh(0, 0, 0, 0);
	scheduler.start();
}


void loop() 
{
	scheduler.tick();
}

void refresh_display()
{
	milliseconds ms = timer.elapsed();
	auto tenth = std::chrono::duration_cast<tenths>(ms) % 10;
	ms -= std::chrono::duration_cast<milliseconds>(tenth);
	auto secs = std::chrono::duration_cast<seconds>(ms);
	ms -= std::chrono::duration_cast<milliseconds>(secs);
	auto mins = std::chrono::duration_cast<minutes>(secs);
	secs -= std::chrono::duration_cast<seconds>(mins);
	auto hour = std::chrono::duration_cast<hours>(mins);
	mins -= std::chrono::duration_cast<minutes>(hour);

	display.refresh(hour, mins, secs, tenth);
}

void keypad_callback(const Keypad::Button* button, Keypad::Event event)
{
	switch (event)
	{
	case Keypad::Event::Press:
		switch (button->tag_)
		{
		case ButtonTag::Select:
			if ((running = !running)) timer.resume();
			else timer.stop();
			break;
		case ButtonTag::Left:
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
