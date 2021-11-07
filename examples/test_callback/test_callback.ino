#include <pg.h>
#include <interfaces/icommand.h>

// Type with member function of type int taking int arg.
struct Object { int doSomething(int i) { return i * 2; } };
// Free-standing function of type void.
void doSomething() { Serial.println("free-standing doSomething() called"); }
// Object instance.
Object obj;

// Callback type for Object member taking one int arg and returning int.
pg::callback<int, Object, int>::type cb1 = &Object::doSomething;
// Callback type for free-standing function of type void.
pg::callback<void>::type cb2 = &doSomething;

// Command type for Object member taking one int arg and returning int.
pg::Command<int, Object, int> ocmd(&obj, cb1, 42);
// Command type for free-standing function of type void.
pg::Command<void> fcmd(cb2);

void setup() 
{
  Serial.begin(9600);
  Serial.println("Using callback objects");
  Serial.print("obj.doSomething(42) returned: "); 
  Serial.println((obj.*cb1)(42)); // Calls obj.doSomething(42);
  cb2();  // Calls doSomething();
  Serial.println("\nUsing command objects");
  Serial.println("obj.doSomething(42) called"); 
  ocmd.execute(); // Executes cb1.
  fcmd.execute(); // Executes cb2.
}

void loop() 
{

}
