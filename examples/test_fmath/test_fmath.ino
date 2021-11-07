#include <pg.h>
#include <array>
#include <lib/fmath.h>

float arr[] = {1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 2.0f, 3.0f, 4.0f, 5.0f};

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
  Serial.println(pg::fact(10U));
  Serial.println(pg::exp(0.0f), 6);
  Serial.println(pg::exp(1.0f), 6);
  Serial.println(pg::exp(-0.5f), 6);
  Serial.println(pg::deg(std::numbers::pi), 6);
  Serial.println(pg::rads(180.0f), 6);
  Serial.println(pg::sin(std::numbers::pi_inv_four), 6);
  Serial.println(pg::cos(std::numbers::pi_inv_four), 6);
  Serial.println(pg::tan(std::numbers::pi_inv_four), 6);
  Serial.println(pg::sec(std::numbers::pi_inv_four), 6);
  Serial.println(pg::csc(std::numbers::pi_inv_four), 6);
  Serial.println(pg::cot(std::numbers::pi_inv_four), 6);
  Serial.println(pg::sinh(std::numbers::pi_inv_four), 6);
  Serial.println(pg::cosh(std::numbers::pi_inv_four), 6);
  Serial.println(pg::tanh(std::numbers::pi_inv_four), 6);
  Serial.println(pg::sech(std::numbers::pi_inv_four), 6);
  Serial.println(pg::coth(std::numbers::pi_inv_four), 6);
  Serial.println(pg::csch(std::numbers::pi_inv_four), 6);
  Serial.println(pg::asin(std::numbers::pi_inv_four), 6);
  Serial.println(pg::acos(std::numbers::pi_inv_four), 6);
  Serial.println(pg::atan(-1.0), 6);
  Serial.println(pg::hypot(-3.0f, 4.0f), 6);
  Serial.println(pg::atan2(-3.0f, 4.0f), 6);
  Serial.println(std::isinf(pg::cot(std::tan(0))));
  Serial.println(pg::mean(std::begin(arr), std::end(arr)), 6);
  Serial.println(pg::median(std::begin(arr), std::end(arr)), 6);
  Serial.println(pg::mode(std::begin(arr), std::end(arr)), 6);
  Serial.println(pg::range(std::begin(arr), std::end(arr)), 6);
  Serial.println(pg::variance(std::begin(arr), std::end(arr)), 6);
  Serial.println(pg::stddev(std::begin(arr), std::end(arr)), 6);
}

void loop() 
{

}
