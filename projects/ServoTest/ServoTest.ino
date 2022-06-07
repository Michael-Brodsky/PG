// This program can be used to calculate the performance limits of 
// a servo motor. It can find the following parameters:
//
//		Minimum pulse width,
//		Maximum pulse width,
//		Maximum step (change in) pulse width, 
//		Minimum step interval (time between steps).
//
// The program is built assuming a compatible LCD/Keypad Shield is 
// attached to the board, but this is not a strict requirement. 
// The servo motor must be attached to a valid PWM output pin and 
// should be supplied adequate power, separate from the board. The 
// <ServoTest.h> file defines configuration parameters, including 
// hardware connections, which must be set to proper values. All 
// timing is given in microseconds (us).
//
// The program display's prompts in the Serial Monitor, instructing 
// the user what steps to take. These usually involve pressing a key 
// on the keypad, observing the servo's behavior and answering a 
// question using the keypad.
//
// Some tests can have upto 10 steps. It is important that the user 
// observe the servo carefully and complete all the steps as 
// accurately as possible.
//
#include <pg.h>
#include <Servo.h>
#include "ServoTest.h"

button_type getKey();
void selectTest();
void promptTest();
void nextTest();
void prevTest();
void runTest(TestSuite);
void reportResults();
void initialize();
void findMinPw();
void findMaxPw();
void findMaxStep();
void findMinInterval();

microseconds sweep(microseconds, microseconds, microseconds = ServoAbsoluteMin, microseconds = ServoAbsoluteMax);
void rotate(microseconds, microseconds, microseconds = ServoAbsoluteMin, microseconds = ServoAbsoluteMax);

Keypad::Button right_button(RightButtonTriggerLevel);
Keypad::Button up_button(UpButtonTriggerLevel);
Keypad::Button down_button(DownButtonTriggerLevel);
Keypad::Button left_button(LeftButtonTriggerLevel);
Keypad::Button select_button(SelectButtonTriggerLevel);
Keypad keypad(KeypadInput, nullptr,
	{ &right_button,&up_button,&down_button,&left_button,&select_button });

Servo servo;
TestSuite test_suite = TestSuite::Init;
microseconds step_size = ServoDefaultStepSize;
microseconds step_interval = ServoDefaultStepInterval;
milliseconds sweep_delay = ServoDefaultSweepDelay;
microseconds pwm_min = ServoAbsoluteMin;
microseconds pwm_max = ServoAbsoluteMax;

void setup() 
{
#if !defined _DEBUG
	Serial.begin(SerialBaudRate);
#endif
	servo.attach(ServoOutput);
	promptTest();
}

void loop() 
{
	selectTest();
	runTest();
	nextTest();
}

void selectTest()
{
	button_type button;

	while ((button = getKey()) != select_button.id())
	{
		if (button == right_button.id())
			nextTest();
		else if (button == left_button.id())
			prevTest();
		delay(100);
	}
}

void promptTest()
{
	const char* prompt = nullptr;

	switch (test_suite)
	{
	case TestSuite::Init:
		prompt = TestInitPrompt;
		break;
	case TestSuite::FindMinPw:
		prompt = FindMinPwTestPrompt;
		break;
	case TestSuite::FindMaxPw:
		prompt = FindMaxPwTestPrompt;
		break;
	case TestSuite::FindMaxStep:
		prompt = FindMaxStepTestPrompt;
		break;
	case TestSuite::FindMinInterval:
		prompt = FindMinIntervalTestPrompt;
		break;
	case TestSuite::Done:
		prompt = TestDonePrompt;
	default:
		break;
	}
	if (prompt);
		Serial.println(prompt);
}

void nextTest()
{
	switch (test_suite)
	{
	case TestSuite::Init:
		test_suite = TestSuite::FindMinPw;
		break;
	case TestSuite::FindMinPw:
		test_suite = TestSuite::FindMaxPw;
		break;
	case TestSuite::FindMaxPw:
		test_suite = TestSuite::FindMaxStep;
		break;
	case TestSuite::FindMaxStep:
		test_suite = TestSuite::FindMinInterval;
		break;
	case TestSuite::FindMinInterval:
		test_suite = TestSuite::Done;
		break;
	case TestSuite::Done:
		test_suite = TestSuite::Init;
	default:
		break;
	}
	promptTest();
}

void prevTest()
{
	switch (test_suite)
	{
	case TestSuite::FindMinPw:
		test_suite = TestSuite::Init;
		break;
	case TestSuite::FindMaxPw:
		test_suite = TestSuite::FindMinPw;
		break;
	case TestSuite::FindMaxStep:
		test_suite = TestSuite::FindMaxPw;
		break;
	case TestSuite::FindMinInterval:
		test_suite = TestSuite::FindMaxStep;
		break;
	case TestSuite::Done:
		test_suite = TestSuite::FindMinInterval;
	default:
		break;
	}
	promptTest();
}

void runTest()
{
	switch (test_suite)
	{
	case TestSuite::Init:
		initialize();
		break;
	case TestSuite::FindMinPw:
		findMinPw();
		break;
	case TestSuite::FindMaxPw:
		findMaxPw();
		break;
	case TestSuite::FindMaxStep:
		findMaxStep();
		break;
	case TestSuite::FindMinInterval:
		findMinInterval();
		break;
	case TestSuite::Done:
		reportResults();
		break;
	default:
		break;
	}
	Serial.println();
}

void reportResults()
{
	Serial.println();
	Serial.print("Absolute Minimum P/W =  "); Serial.print(pwm_min.count()); Serial.println("us");
	Serial.print("Absolute Maximum P/W =  "); Serial.print(pwm_max.count()); Serial.println("us");
	Serial.print("Maximum P/W Step Size = "); Serial.print(step_size.count()); Serial.println("us");
	Serial.print("Minimum Step Interval = "); Serial.print(step_interval.count()); Serial.println("us");
}

button_type getKey()
{
	static Keypad::Button* previous = nullptr;
	Keypad::Button* current = keypad();
	Keypad::Button* button = current && current != previous ? current : nullptr;
	previous = current;

	return button ? button->id() : -1;
}

void initialize()
{
	// Reset params on initialization.
	step_size = ServoDefaultStepSize;
	step_interval = ServoDefaultStepInterval;
	sweep_delay = ServoDefaultSweepDelay;
	pwm_min = ServoAbsoluteMin;
	pwm_max = ServoAbsoluteMax;
	Serial.print("Initializing... ");
	Serial.print(pwm_min.count()); Serial.print(", ");
	Serial.print(pwm_max.count()); Serial.print(", ");
	Serial.print(step_size.count()); Serial.print(", ");
	Serial.print(step_interval.count()); Serial.print(" -> ");
	// Command servo to sweep min -> max -> min, ensuring it "catches" and winds up at min.
	rotate(step_interval, step_size, pwm_min, pwm_max);
	delay(sweep_delay.count());
	Serial.println("<-");
	rotate(step_interval, step_size, pwm_max, pwm_min);
	Serial.println(); Serial.println("Press <Select> to begin a test.");
	Serial.println("Press <Right>/<Left> to skip forward/backward."); 
}

void findMinPw()
{
	microseconds speed = microseconds(16383); // Use a longer interval so user has time to react to movement,
	microseconds est_min = pwm_max;
	button_type button = -1;

	Serial.println("Press <Select> to begin, then press <Select> again as soon as the servo starts moving.");
	while (getKey() != select_button.id()) delay(100);
	Serial.print(pwm_min.count()); Serial.print(" -> "); Serial.println(est_min.count());
	// Rotate the servo from the default min to max until <Select> pressed.
	// The first estimate is the p/w when <Select> pressed.
	est_min = sweep(speed, step_size, pwm_min, est_min);
	// Cut the difference between min and estimate in half, this is the next estimate. 
	est_min -= (est_min - pwm_min) / 2;
	Serial.println("Press <Select> to go to the next step.");
	while (getKey() != select_button.id()) delay(100);
	// Continue cutting the difference in half until it equals 1.
	while (est_min - pwm_min > microseconds(1))
	{
		Serial.print(pwm_min.count()); Serial.print(" -> "); Serial.println(est_min.count());
		Serial.println("Is the servo moving? Press <Down> = Yes, <Up> = No.");
		while ((button = getKey()) == -1)
		{
			// Jitter the servo back and forth between min and estimate while polling the keypad.
			// This should make movement easier to see.
			rotate(step_interval, step_size, pwm_min, est_min);
			delayMicroseconds(speed.count());
			rotate(step_interval, step_size, est_min, pwm_min);
			button = button == up_button.id() || button == down_button.id() ? button : -1;
		}
		// Each iteration cuts the difference in half and adjusts the estimate up/down 
		// based on whether the servo moved.
		if (button == up_button.id())
		{
			microseconds tmp = est_min;
			est_min += (est_min - pwm_min) / 2;
			pwm_min = tmp;
		}
		else
			est_min -= (est_min - pwm_min) / 2;
	}
	Serial.print("Estimated min p/w = "); Serial.print((pwm_min = est_min).count()); Serial.println("us");
}

void findMaxPw()
{
	microseconds speed = microseconds(16383); // Use a longer interval so user has time to react to movement.
	microseconds est_max = pwm_min;
	button_type button = -1;

	Serial.println("Press <Select> to begin, then press <Select> again as soon as the servo starts moving.");
	while (getKey() != select_button.id()) delay(100);
	Serial.print(pwm_max.count()); Serial.print(" -> "); Serial.println(est_max.count());
	// Rotate the servo from the default max to min until <Select> pressed.
	// The first estimate is the p/w when <Select> pressed.
	est_max = sweep(speed, step_size, pwm_max, est_max);
	// Cut the difference between max and estimate in half, this is the next estimate. 
	est_max += (pwm_max - est_max) / 2;
	Serial.println("Press <Select> to go to the next step.");
	while (getKey() != select_button.id()) delay(100);
	// Continue cutting the difference in half until it equals 1.
	while (pwm_max - est_max > microseconds(1))
	{
		Serial.print(pwm_max.count()); Serial.print(" -> "); Serial.println(est_max.count());
		Serial.println("Is the servo moving? Press <Down> = Yes, <Up> = No.");
		while ((button = getKey()) == -1)
		{
			// Jitter the servo back and forth between min and estimate while polling the keypad.
			// This should make movement easier to see.
			rotate(step_interval, step_size, pwm_max, est_max);
			delayMicroseconds(speed.count());
			rotate(step_interval, step_size, est_max, pwm_max);
			button = button == up_button.id() || button == down_button.id() ? button : -1;
		}
		// Each iteration cuts the difference in half and adjusts the estimate up/down 
		// based on whether the servo moved.
		if (button == up_button.id())
		{
			microseconds tmp = est_max;
			est_max -= (pwm_max - est_max) / 2;
			pwm_max = tmp;
		}
		else
			est_max += (pwm_max - est_max) / 2;
	}
	Serial.print("Estimated max p/w = "); Serial.print((pwm_max = est_max).count()); Serial.println("us");
}

void findMaxStep()
{
	step_size = pwm_max - pwm_min;
	microseconds step_diff = step_size;
	button_type button = -1;

	// Send servo to home pos.
	rotate(ServoDefaultStepInterval, ServoDefaultStepSize, pwm_max, pwm_min);
	Serial.println("Press <Select> to begin.");
	while (getKey() != select_button.id()) delay(100);
	bool result = false;
	// Tolerance is 10us.
	while (step_diff > microseconds(10) || result == false)
	{
		// Command servo to move using one instruction.
		servo.writeMicroseconds(step_size.count());
		Serial.println("Did the servo rotate smoothly? Press <Down> = Yes, <Up> = No.");
		while ((button = getKey()) == -1)
		{
			button = button == up_button.id() || button == down_button.id() ? button : -1;
			delay(100);
		}
		// Cut the difference in half and bump the step according to whether it moved.
		step_diff /= 2;
		if (button == up_button.id())
		{
			step_size -= step_diff;
			result = false;
		}
		else if (step_size < (pwm_max - pwm_min))
		{
			step_size += step_diff;
			result = true;
		}
		else
			break;	// If we're already at max, then we're done.

		// Otherwise, repeat until difference is reduced to tolerance value.
		Serial.println("Resetting...");
		servo.writeMicroseconds(pwm_min.count());
		Serial.println("Press <Select> to go to the next step.");
		while (getKey() != select_button.id()) delay(100);
	}
	Serial.print("Estimated max p/w step = "); Serial.print(step_size.count()); Serial.println("us");
}

void findMinInterval()
{
	if (step_size < pwm_max)
	{
		button_type button = -1;
		step_interval = milliseconds(1);

		rotate(ServoDefaultStepInterval, ServoDefaultStepSize, step_size, pwm_min);
		while (1)
		{
			Serial.println("Resetting...");
			rotate(ServoDefaultStepInterval, ServoDefaultStepSize, step_size, pwm_min);
			Serial.println("Press <Select> to begin.");
			while (getKey() != select_button.id()) delay(100);
			// Command the servo to rotate with one instruction and a delay in between.
			servo.writeMicroseconds(step_size.count());
			delayMicroseconds(step_interval.count());
			servo.writeMicroseconds(pwm_min.count());
			Serial.println("Did the servo rotate back and forth? Press <Down> = Yes, <Up> = No.");
			while ((button = getKey()) == -1)
			{
				button = button == up_button.id() || button == down_button.id() ? button : -1;
				delay(100);
			}
			// Double the delay and repeat until servo moves back and forth. 
			if (button == down_button.id())
				break;
			else
				step_interval *= 2;
		}
		step_interval /= 2;
		microseconds  diff = step_interval;
		if (step_interval >= microseconds(1000))
		{
			diff /= 2;
			step_interval += diff;
		}
		// Now we have our [min,max] interval.
		// Approximate to a sane value using the same divide and conquer techniques as before.
		bool result = false;
		while (diff > microseconds(50) || result == false)
		{

			Serial.println("Resetting...");
			rotate(ServoDefaultStepInterval, ServoDefaultStepSize, step_size, pwm_min);
			Serial.println("Press <Select> to begin.");
			while (getKey() != select_button.id()) delay(100);
			servo.writeMicroseconds(step_size.count());
			delayMicroseconds(step_interval.count());
			servo.writeMicroseconds(pwm_min.count());
			Serial.println("Did the servo rotate smoothly? Press <Down> = Yes, <Up> = No.");
			while ((button = getKey()) == -1)
			{
				button = button == up_button.id() || button == down_button.id() ? button : -1;
				delay(100);
			}
			diff /= 2;
			if (button == down_button.id())
			{
				step_interval -= diff;
				result = true;
			}
			else
			{
				step_interval += diff;
				result = false;
			}
		}
	}
	else
		step_interval = microseconds(0);
	Serial.print("Estimated min step interval= "); Serial.print(step_interval.count()); Serial.println("us");
}

microseconds sweep(microseconds intvl, microseconds step, microseconds min, microseconds max)
{
	if (min > max)
	{
		for (; min > max; min -= step)
		{
			servo.writeMicroseconds(min.count());
			delayMicroseconds(intvl.count());
			if (getKey() == select_button.id())
				break;
		}
	}
	else
	{
		for (; min < max; min += step)
		{
			servo.writeMicroseconds(min.count());
			delayMicroseconds(intvl.count());
			if (getKey() == select_button.id())
				break;
		}
	}

	return min;
}

void rotate(microseconds intvl, microseconds step, microseconds min, microseconds max)
{
	if (min > max)
	{
		for (; min > max; min -= step)
		{
			servo.writeMicroseconds(min.count());
			delayMicroseconds(intvl.count());
		}
	}
	else
	{
		for (; min < max; min += step)
		{
			servo.writeMicroseconds(min.count());
			delayMicroseconds(intvl.count());
		}
	}
}


