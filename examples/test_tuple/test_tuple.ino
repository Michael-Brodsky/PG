// Part of the Pg test suite for <tuple>
#include <pg.h>
#include <tuple>

bool _tuple_get()
{
  bool result = false;
  std::tuple<int, float, String> tuple1;
  std::tuple<int, String, double> t2{ 42, "Test", -3.14 };
  std::get<0>(tuple1) = 5;
  std::get<1>(tuple1) = 8.3;
  std::get<2>(tuple1) = "Foo";

  result = std::get<0>(tuple1) == 5 && std::get<1>(tuple1) == 8.3 && std::get<2>(tuple1) == "Foo" && 
    std::get<0>(t2) == 42 && std::get<1>(t2) == "Test" && std::get<2>(t2) == -3.14;

  return result;
}

bool _tuple_size()
{
  std::tuple<int, float, String> tuple1;

  static_assert(
    std::tuple_size<decltype(tuple1)>::value == 3, 
    "std::tuple_size std::tuple failed."
    );

  return tuple1.size() == 3;
}

void setup() 
{
  Serial.begin(9600);
  bool b1 = _tuple_get(), b2 = _tuple_size();
  Serial.print("_tuple_get() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("_tuple_size() = "); Serial.println(b2 ? "OK" : "FAIL");
}

void loop() 
{

}
