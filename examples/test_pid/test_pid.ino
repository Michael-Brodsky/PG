#include <pg.h>
#include <numeric> // std::abs()
#include <utilities/PIDController.h>

using namespace pg;
using PID = PIDController<>; // Alias for PIDController<float>
using value_type = PID::value_type; // Alias for float
using input_type = PID::input_type; // Alias for (float *)(void)
using output_type = PID::output_type; // Alias for (void *)(float)

// Process variable would normally be stored externally by the machinery being 
// controlled and accessed by a function that reads/writes to an i/o port.
value_type pv = 0.0; 

// Function that returns the current process variable.
value_type get_pv()
{
  return pv; // Usually this would read a sensor connected to one of the GPIO inputs.
}

// Function that sets the current control variable.
void set_cv(const value_type value)
{
  pv += value; // Usually this would drive a GPIO output that controls the machinery. 
}

value_type set_point = 1.0; // The desired process set point, e.g. a thermostat setting.
value_type Kp = 1.0;        // The proportional coefficient.
value_type Ki = 0.0;        // The integral coefficient.
value_type Kd = 0.0;        // The derivative coefficient.
value_type A = 0.25;        // The controller gain setting, effectively multiplies its output value.
input_type in = &get_pv;    // Method to read the current process variable.
output_type out = &set_cv;  // Method to write the controller's output value.

PID pid(set_point, Kp, Ki, Kd, A);     // The PID controller.

void setup() 
{
  Serial.begin(9600);
  Serial.print("set point = "); Serial.println(pid.set_point());
  Serial.println("PID controller will adjust the process variable to converge to the set point value.");
  PID::time_point now = PID::time_point(PID::clock_type::now());
  pid.start(now);
}

void loop() 
{
  Serial.print("pv = ");Serial.println(pv);
  delay(1000);    // Loop must be run at constant intervals of some sane value. 1 sec is usually a good choice.
                  // delay() is Evil! Use the TaskScheduler class for asynchronous execution if you have other 
                  // tasks that need to run more often, which of course you probably will.
  (*out)(pid.loop((*in)()));
  if(std::abs(pv - set_point) < 0.002)
  {
    Serial.println("Changes to the set point will cause process variable to converge to the new set point.");
    delay(3000);
    if(set_point < 1.0) set_point = 1.2;
    else set_point = 0.8;
    pid.set_point(set_point);
    Serial.print("set point = "); Serial.println(pid.set_point());
  }
}
