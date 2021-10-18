#pragma once
#include <components/AnalogKeypad.h>

using namespace pg;
using namespace std::chrono;

using Keypad = AnalogKeypad<>;
using button_type = std::make_signed<Unique::unique_type>::type;

const microseconds ServoAbsoluteMin = microseconds(544);
const microseconds ServoAbsoluteMax = microseconds(2400);
const microseconds ServoDefaultStepSize = microseconds(3);
const milliseconds ServoDefaultStepInterval = milliseconds(2);
const milliseconds ServoDefaultSweepDelay = milliseconds(50);

const pin_t KeypadInput = 0;	// Must be a valid analog input pin.
const pin_t ServoOutput = 13;	// Must be a valid PWM output pin.

const Keypad::value_type RightButtonTriggerLevel = 60;
const Keypad::value_type UpButtonTriggerLevel = 200;
const Keypad::value_type DownButtonTriggerLevel = 400;
const Keypad::value_type LeftButtonTriggerLevel = 600;
const Keypad::value_type SelectButtonTriggerLevel = 800;

const uint32_t	SerialBaudRate = 115200;

enum class TestSuite
{
	Init, 
	FindMinPw, 
	FindMaxPw,
	FindMaxStep, 
	FindMinInterval, 
	Done
};

const char* const TestInitPrompt = "Press <Select> key to begin.";
const char* const FindMinPwTestPrompt = "Find Min P/W ?";
const char* const FindMaxPwTestPrompt = "Find Max P/W ?";
const char* const FindMaxStepTestPrompt = "Find Max P/W Step ?";
const char* const FindMinIntervalTestPrompt = "Find Min Step Interval ?";
const char* const TestDonePrompt = "Test Results ?";

