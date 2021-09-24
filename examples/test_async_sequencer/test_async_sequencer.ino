#include <pg.h>
#include <components/AnalogKeypad.h>
#include <components/Sequencer.h>
#include <utilities/TaskScheduler.h>
using namespace pg;
using namespace std::chrono;

// This program demonstrates how a TaskScheduler can be used to trigger tasks asynchronously,  
// at predefined intervals and start/stop tasks based on criteria. This can greatly improve 
// CPU resource use by executing tasks only when they're needed and at a rate which doesn't 
// overload the CPU or prevent other tasks from timely execution.

// Func fwd decls.

enum class pg::ButtonTag // Must be fully scoped as pg::ButtonTag
{
  Right = 0,
  Up,
  Down,
  Left,
  Select
};
void keypad_cb(const Keypad::Button&, Keypad::Event); 
void toggle(Sequencer::Status);

// Commands we wish to sequence.
struct Command1 : public icommand { 
  void execute() override { Serial.println("Order Pizza"); } 
} command1;

struct Command2 : public icommand { 
  void execute() override { Serial.println("Cancel Order"); } 
} command2;

// Sequencer & Event objects.
Sequencer::Event event1 = { "Event1", seconds(4), &command1 }; // Event "name", duration and command to execute.
Sequencer::Event event2 = { "Event2", seconds(1), &command2 };
Sequencer::Event* events[] = { &event1,&event2 };

Sequencer seq(events, nullptr, true);

// Keypad Shield parameters & objects.
const pin_t KeypadInputPin = 0;

const analog_t RightButtonTriggerLevel = 60;
const analog_t UpButtonTriggerLevel = 200;
const analog_t DownButtonTriggerLevel = 400;
const analog_t LeftButtonTriggerLevel = 600;
const analog_t SelectButtonTriggerLevel = 800;

const Keypad::Button right_button(ButtonTag::Right, RightButtonTriggerLevel);
const Keypad::Button up_button(ButtonTag::Up, UpButtonTriggerLevel);
const Keypad::Button down_button(ButtonTag::Down, DownButtonTriggerLevel);
const Keypad::Button left_button(ButtonTag::Left, LeftButtonTriggerLevel);
const Keypad::Button select_button(ButtonTag::Select, SelectButtonTriggerLevel);
const Keypad::Button buttons[] = { right_button, up_button, down_button, left_button, select_button };

Keypad keypad(KeypadInputPin, &keypad_cb, Keypad::LongPress::None, seconds(), buttons);

// Task scheduling parameters & objects.
const milliseconds KeypadPollingInterval = milliseconds(100); // Keypad is polled every 100ms.
const milliseconds SequencerClockingInterval = microseconds(500000); // us automatically converted ms.

ClockCommand keypad_clock(&keypad);
ClockCommand sequencer_clock(&seq);

TaskScheduler::Task keypad_task(KeypadPollingInterval, &keypad_clock, TaskScheduler::Task::State::Active);
TaskScheduler::Task sequencer_task(SequencerClockingInterval, &sequencer_clock, TaskScheduler::Task::State::Idle);
TaskScheduler::Task* tasks[] = { &keypad_task, &sequencer_task };

TaskScheduler task_scheduler(tasks);

void setup() 
{
  Serial.begin(9600);
  Serial.println("Press the <Select> key to start.");
  task_scheduler.start();
}

void loop() 
{
  task_scheduler.tick(); // Now only the task scheduler needs to be ticked.
}

void keypad_cb(const Keypad::Button& button, Keypad::Event event)
{
  switch (event)
  {
  case Keypad::Event::Press:
    switch (button.tag_)
    {
    case ButtonTag::Select: // Toggle the sequencer's status on each press of <Select> button.
      toggle(seq.status());
      break;
    default:
      break;
    };
    break;
  default:
    break;
  };
}

void toggle(Sequencer::Status status) 
{
  if (status == Sequencer::Status::Active) 
  {
    Serial.println("Sequencer Stopped");
    seq.stop();
    sequencer_task.state(TaskScheduler::Task::State::Idle); // No need to clock sequencer when it's idle.
  }
  else 
  {
    Serial.println("Sequencer Started");
    sequencer_task.state(TaskScheduler::Task::State::Active);
    seq.resume();
  }
}
