#include <pg.h>
#include <components/EventSequencer.h>
using namespace pg;
using Sequencer = EventSequencer<>; // Alias for default Sequencer.

// Class Foo represents a reusable type, usually part of a user-library 
// that performs some commonly required, but complex task. 
class Foo 
{
  public:
    Foo() = default;
  public: 
    void doSomething() { value_ = 42; Serial.println("42 achieved"); }
    unsigned getSomething() const { return value_; }
    void reset() { value_ = 0; }
  private:
    unsigned value_;
};

// Class Bar represents a reusable type, usually part of a user-library 
// that performs some commonly required, but complex task. 
class Bar 
{
public:
  Bar(Foo* foo) : foo_(foo) {}
public:
  void check_value() { if (foo_->getSomething() == 42) Serial.println("42 retrieved"); }
private:
  Foo* foo_;
};

// The actual program then requires only objects instatiated from appropriate parameters
// and possibly callback functions.

// Handles MySequencer callbacks (not strictly necessary).
void sequencer_cb(const Sequencer::Event& event, Sequencer::Event::State state) 
{
  Serial.print(event.name_); Serial.print(": "); Serial.println(state == Sequencer::Event::State::Begin ? "begin" : "end");
}

// Objects needed by the program.
Foo f;  // Polls a keypad, flashes LEDs, whatever ...
Bar b(&f); // Orders pizza, then calls your mom asking for money ...
// Commands that "talk" to the objects.
Command<void, Foo, void> cmd_foo(&f, &Foo::doSomething);
Command<void, Bar, void> cmd_bar(&b, &Bar::check_value);
// Sequencer event objects 
Sequencer::Event event_foo = { "Foo Event", std::chrono::seconds(2), &cmd_foo };
Sequencer::Event event_bar = { "Bar Event", std::chrono::seconds(4), &cmd_bar };
//Sequencer::Event* events[] = { &event_foo, &event_bar };
// Sequencer object.
Sequencer seq({ &event_foo, &event_bar }, &sequencer_cb, true); // If callbacks not used, then substitute nullptr for the callback function. 

void setup() 
{
  Serial.begin(9600);
  seq.start(); // Start the MySequencer.
}

void loop() 
{
  seq.tick(); // The sequencing of tasks is reduced to a single line of code.
}
