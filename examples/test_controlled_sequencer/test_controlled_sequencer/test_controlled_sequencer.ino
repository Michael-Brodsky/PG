 // This program demonstrates how a keypad can be used to control an event sequence.
 
#include <pg.h>
#include <components/AnalogKeypad.h>
#include <components/EventSequencer.h>
using namespace pg;
using namespace std::chrono;
using Sequencer = EventSequencer<>; // Alias for default EventSequencer.
using Keypad = AnalogKeypad<>; // Alias for default AnalogKeypad.
// Keypad callback func decl.
void keypadCallback(const Keypad::Button*, Keypad::Event); 

// Commands we wish to sequence. Commmand objects can contain any valid code, from blinking an 
// LED to a whole program. These commands just print to the serial monitor.
struct Command1 : public icommand { void execute() override { Serial.println("Order Pizza"); } } command1;
struct Command2 : public icommand { void execute() override { Serial.println("Cancel Order"); } } command2;

// Sequencer Event objects.
Sequencer::Event event1 = { "Event1", seconds(4), &command1 }; // Event "name", duration and command to execute.
Sequencer::Event event2 = { "Event2", seconds(1), &command2 };

// Keypad & Button parameters.
const pin_t KeypadInputPin = 0;
const analog_t ButtonTriggerLevel = 800;

// Keypad button objects. We're going to stop/resume the sequencer 
// using any button, so we only need to define one "button" 
// which is actually the highest trigger level of all the buttons
// wired to the board.
Keypad::Button any_button(ButtonTriggerLevel);

// Keypad and Sequencer objects instatiated from parameters.
Keypad keypad(KeypadInputPin, keypadCallback, { &any_button });
Sequencer seq({ &event1,&event2 }, nullptr, true);

void setup() 
{
  Serial.begin(9600);
  Serial.println("Press the <Any> key to start/stop.");
}

void loop() 
{
  keypad.poll();
  seq.tick();
  delay(100); // Button input debounce.
}

void keypadCallback(const Keypad::Button* button, Keypad::Event event)
{
  // We only need to respond to button press events.
  if(event == Keypad::Event::Press)
  {
    seq.status() == Sequencer::Status::Active ? Serial.println("Sequencer Stopped") : Serial.println("Sequencer Started");
    seq.status() == Sequencer::Status::Active ? seq.stop() : seq.resume();
  };
}
