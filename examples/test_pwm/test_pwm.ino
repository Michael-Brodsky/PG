#include <pg.h>
#include <utilities/PWMOutput.h>
using namespace pg;
using Pwm = PWMOutput<>;

// Program to demonstrate controlling LED brightness with PWM.
// Note, not all Arduino boards support PWM output on pin 13.

// Change this to an appropriate output pin for your board, if necessary.
// Attaching an invalid pin will cause no output to be generated and the 
// attach() method to return InvalidPin (0xFF).
const pin_t pin = 13; 

// Create the PWMOutput object attached to the given pin, with a duty cycle of 100% 
// and enable PWM output (duty cycle range  = [0.0, 1.0] for 0% to 100%).
Pwm pwm0(pin, 1.0f, true); 

void setup() 
{
  Serial.begin(9600);
  // Force output low.
  digitalWrite(pin, false); 
  // Print the pwm params to serial monitor.
  Serial.println("");
  Serial.print("pin="); Serial.println(pwm0.attach());
  Serial.print("freq="); Serial.print(pwm0.frequency()); Serial.println(" Hz");
  Serial.print("dc="); Serial.print(pwm0.duty_cycle() * 100.0f); Serial.println("%");
  Serial.print("en="); Serial.println(pwm0.enabled());
}

void loop() 
{
  // Change the duty cycle every so often.
  delay(2000);
  pwm0.duty_cycle(0.5f);
  delay(2000);
  pwm0.duty_cycle(0.0f);
  delay(2000);
  pwm0.duty_cycle(1.0f);
}
