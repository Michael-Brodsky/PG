#include <pg.h>
#include <components/AnalogKeypad.h>
#include <components/EventSequencer.h>
#include <utilities/TaskScheduler.h>
using namespace pg;
using namespace std::chrono;
using Sequencer = EventSequencer<>;
using Scheduler = TaskScheduler<>;
using Keypad = AnalogKeypad<>;

// This program demonstrates how a TaskScheduler can be used to trigger tasks asynchronously,  
// at predefined intervals and start/stop tasks based on criteria. This can greatly improve 
// CPU resource use by executing tasks only when they're needed and at a rate which doesn't 
// overload the CPU or prevent other tasks from timely execution.

// Func fwd decls.
void keypad_cb(const Keypad::Button*, Keypad::Event); 
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

Sequencer seq({ &event1,&event2 }, nullptr, true);

// Keypad Shield parameters & objects.
const pin_t KeypadInputPin = 0;
const analog_t ButtonTriggerLevel = 800;

Keypad::Button any_button(ButtonTriggerLevel);

Keypad keypad(KeypadInputPin, keypad_cb, { &any_button });

// Task scheduling parameters & objects.
const milliseconds KeypadPollingInterval = milliseconds(100); // Keypad is polled every 100ms.
const milliseconds SequencerClockingInterval = microseconds(500000); // us automatically converted to ms.

ClockCommand keypad_clock(&keypad);
ClockCommand sequencer_clock(&seq);

Scheduler::Task keypad_task(KeypadPollingInterval, &keypad_clock, Scheduler::Task::State::Active);
Scheduler::Task sequencer_task(SequencerClockingInterval, &sequencer_clock, Scheduler::Task::State::Idle);

Scheduler task_scheduler({ &keypad_task,&sequencer_task });

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

void keypad_cb(const Keypad::Button* button, Keypad::Event event)
{
  if(event == Keypad::Event::Press)
      toggle(seq.status());
}

void toggle(Sequencer::Status status) 
{
  if (status == Sequencer::Status::Active) 
  {
    Serial.println("Sequencer Stopped");
    seq.stop();
    sequencer_task.state(Scheduler::Task::State::Idle); // No need to clock sequencer when it's idle.
  }
  else 
  {
    Serial.println("Sequencer Started");
    sequencer_task.state(Scheduler::Task::State::Active);
    seq.resume();
  }
}
