#include <pg.h>
#include <lib/fmath.h>

void setup() 
{
  Serial.begin(9600);

  Serial.println(pg::sign(2.f));
  Serial.println(pg::sign(-2.f));
  Serial.println(pg::sqr(-2.f));
  Serial.println(pg::cube(-2.f));
  Serial.println(pg::cmp(-2.f, 42.f));
  Serial.println(pg::cmp(2.f, -42.f));
  Serial.println(pg::cmp(0.0, 0.0));
  Serial.println(pg::clamp(-2.f, 0.0f, 42.f));
  Serial.println(pg::clamp(2.f, 0.0f, 42.f));
  Serial.println(pg::clamp(2.f, 0.0f, 1.0f));
  Serial.println(pg::exp(0.0f), 6);
  Serial.println(pg::exp(1.0f), 6);
  Serial.println(pg::exp(-0.5f), 6);
  Serial.println(pg::deg(std::numbers::pi), 6);
  Serial.println(pg::rads(180.0f), 6);
  Serial.println(pg::sin(std::numbers::pi_four), 6);
  Serial.println(pg::cos(std::numbers::pi_four), 6);
  Serial.println(pg::tan(std::numbers::pi_four), 6);
  Serial.println(pg::sec(std::numbers::pi_four), 6);
  Serial.println(pg::csc(std::numbers::pi_four), 6);
  Serial.println(pg::cot(std::numbers::pi_four), 6);
  Serial.println(pg::sinh(std::numbers::pi_four), 6);
  Serial.println(pg::cosh(std::numbers::pi_four), 6);
  Serial.println(pg::tanh(std::numbers::pi_four), 6);
  Serial.println(pg::sech(std::numbers::pi_four), 6);
  Serial.println(pg::coth(std::numbers::pi_four), 6);
  Serial.println(pg::csch(std::numbers::pi_four), 6);
  Serial.println(pg::asin(std::numbers::pi_four), 6);
  Serial.println(pg::acos(std::numbers::pi_four), 6);
  Serial.println(pg::atan(std::numbers::pi_four), 6);
}

void loop() 
{

}
