#include <pg.h>
#include <components/AnalogKeypad.h>
#include <components/Sequencer.h>
using namespace pg;
using namespace std::chrono;
using MySequencer = Sequencer<>; // Alias for default Sequencer.

// This program demonstrates how a Keypad Shield can be used to control an event sequence.

enum class pg::ButtonTag  // Enumerates valid tags that identify Keypad::Button objects.
{
  Right = 0,
  Up,
  Down,
  Left,
  Select
};

void keypad_cb(const Keypad::Button&, Keypad::Event); // Keypad callback func fwd decl.

// Commands we wish to sequence.
struct Command1 : public icommand { void execute() override { Serial.println("Order Pizza"); } } command1;
struct Command2 : public icommand { void execute() override { Serial.println("Cancel Order"); } } command2;
// MySequencer Event objects.
MySequencer::Event event1 = { "Event1", seconds(4), &command1 }; // Event "name", duration and command to execute.
MySequencer::Event event2 = { "Event2", seconds(1), &command2 };
MySequencer::Event* events[] = { &event1,&event2 };

// Keypad Shield parameters.
const pin_t KeypadInputPin = 0;

const analog_t RightButtonTriggerLevel = 60;
const analog_t UpButtonTriggerLevel = 200;
const analog_t DownButtonTriggerLevel = 400;
const analog_t LeftButtonTriggerLevel = 600;
const analog_t SelectButtonTriggerLevel = 800;
// Keypad button objects.
const Keypad::Button right_button(ButtonTag::Right, RightButtonTriggerLevel);
const Keypad::Button up_button(ButtonTag::Up, UpButtonTriggerLevel);
const Keypad::Button down_button(ButtonTag::Down, DownButtonTriggerLevel);
const Keypad::Button left_button(ButtonTag::Left, LeftButtonTriggerLevel);
const Keypad::Button select_button(ButtonTag::Select, SelectButtonTriggerLevel);
const Keypad::Button buttons[] = { right_button, up_button, down_button, left_button, select_button };
// Keypad and MySequencer objects instatiated from parameters.
Keypad keypad(KeypadInputPin, &keypad_cb, Keypad::LongPress::None, seconds(), buttons);
MySequencer seq(events, nullptr, true);

void setup() 
{
  Serial.begin(9600);
  Serial.println("Press the <Select> key to start/stop.");
}

void loop() 
{
  keypad.poll();
  seq.tick();
}

void keypad_cb(const Keypad::Button& button, Keypad::Event event)
{
  switch (event)
  {
  case Keypad::Event::Press:
    switch (button.tag_)
    {
    case ButtonTag::Select: // Toggle the sequencer's status on each press of <Select> button.
      seq.status() == MySequencer::Status::Active ? Serial.println("Sequencer Stopped") : Serial.println("Sequencer Started");
      seq.status() == MySequencer::Status::Active ? seq.stop() : seq.resume();
      break;
    default:
      break;
    };
    break;
  default:
    break;
  };
}
