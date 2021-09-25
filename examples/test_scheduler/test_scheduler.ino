#include <pg.h>
#include <interfaces/iclockable.h>
#include <utilities/TaskScheduler.h>
using namespace pg;
using namespace std::chrono;
using Scheduler = pg::TaskScheduler<>; // Alias for default TaskScheduler.

// Define a "clockable" type ...
class Clockable : public pg::iclockable
{
public:
  Clockable(const char* str = nullptr) : str_(str) {}
private:
  void clock() override { Serial.print(millis()); Serial.print("\t"); Serial.println(str_); }
private:
  const char* str_;
};
// ... and create a few instances.
Clockable object1("standard");
Clockable object2("typical");
Clockable object3("normal");

// Create commands to call each object's clock() method (defined in <iclockable.h>).
ClockCommand command1(&object1); // Calls object1::clock().
ClockCommand command2(&object2);
ClockCommand command3(&object3);

// A custom command object.
struct ACommand : public icommand 
{ void execute() override { Serial.println("ACommand executed"); } } acommand; // The execute() method can be anything, from blinking an LED to an entire program.

// Create a collection of tasks.
Scheduler::Task task1(seconds(1), &command1, Scheduler::Task::State::Active); // runs every second. (seconds is std::chrono::seconds)
Scheduler::Task task2(seconds(2), &command2, Scheduler::Task::State::Active); // runs every two seconds.
Scheduler::Task task3(seconds(3), &command3, Scheduler::Task::State::Active); // runs every three seconds.
Scheduler::Task task4(seconds(4), &acommand, Scheduler::Task::State::Active); // runs every four seconds.
Scheduler::Task* tasks[] =
{ &task1, &task2, &task3, &task4 };

Scheduler ts(tasks);

void setup() 
{
  Serial.begin(9600);
  ts.start();
}

void loop() 
{
  ts.tick();
}
