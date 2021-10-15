#include <pg.h>
#include <iterator>
#include <interfaces/iclockable.h>

// A "clockable" type.
class Clockable : public pg::iclockable
{
public:
  Clockable(const char* str = nullptr) : str_(str) {}
private:
  void clock() override { Serial.println(str_); } // Only accessible through interface.
private:
  const char* str_;
};

// A bunch of "clockable" objects.
Clockable object1("foobar");
Clockable object2("frobnicate");
Clockable object3("blueberry frogurt");

// A collection of commands to "clock" the objects.
pg::ClockCommand command1(&object1);
pg::ClockCommand command2(&object2);
pg::ClockCommand command3(&object3);
pg::icommand* commands[] = { &command1,&command2,&command3 };
auto it = std::begin(commands);

void setup() 
{
  Serial.begin(9600);
  Serial.println("clocking all objects:"); Serial.println();
  for (auto i : commands)
    i->execute();
  Serial.println();Serial.println("clocking objects at intervals:");
}

void loop() 
{
  if(it == std::end(commands))
    it = std::begin(commands);
  (*it++)->execute();
  delay(1000);
}
