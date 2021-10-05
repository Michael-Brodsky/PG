#include <pg.h>
#include <utilities/PWMOutput.h>
using namespace pg;

// Program to demonstrate controlling LED brightness with PWM.

// Change this to the appropriate output pin for your board, if necessary.
const pin_t pin = 13; 
// Create the PWMOutput object attached to the given pin, with a duty cycle of 100% 
// and enable PWM output (duty cycle range  = [0.0, 1.0] for 0% to 100%).
Pwm pwm0(pin, 1.0, true); 

void setup() 
{
  Serial.begin(9600);
  // Turn off the LED.
  digitalWrite(13, false); 
  // Print the pwm params to serial monitor.
  Serial.println("");
  Serial.print("pin="); Serial.println(pwm0.attach());
  Serial.print("freq="); Serial.print(pwm0.frequency()); Serial.println(" Hz");
  Serial.print("dc="); Serial.print(pwm0.duty_cycle() * 100.0); Serial.println("%");
  Serial.print("en="); Serial.println(pwm0.enabled());
}

void loop() 
{
  // Change the brightness every so often with the duty cycle parameter.
  delay(2000);
  pwm0.duty_cycle(0.5);
  delay(2000);
  pwm0.duty_cycle(0.0);
  delay(2000);
  pwm0.duty_cycle(1.0);
}
