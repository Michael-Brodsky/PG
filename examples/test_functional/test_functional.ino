// Part of the Pg test suite for <functional>.
#include <pg.h>
#include <functional>

bool _functional_cmp()
{
  bool result = true;
  unsigned i = 42, j = i;
  result == result && std::equal_to<unsigned*>()(&i, &j) && std::not_equal_to<unsigned>()(42U, 23U) &&
    std::greater<float>()(3.14f, 0.f) && std::less<char>()('a', 'z') && !std::greater_equal<float>()(-1.f, 0.f) && 
    std::less_equal<float>()(0.f, -0.f);

  return result;
}

void setup() 
{
  Serial.begin(9600);
  bool b1 = _functional_cmp();
  Serial.print("_functional_cmp() = "); Serial.println(b1 ? "OK" : "FAIL");
}

void loop() 
{

}
