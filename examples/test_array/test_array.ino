// Part of the Pg test suite for <array>.
#include <pg.h>
#include <array>

bool _array()
{
  std::array<int, 3> a1{ 1, 2, 3 };
  std::array<int, 3> a2 = { 1, 2, 3 };
  std::array<String, 2> a3 = { String("a"), "b" };
  std::array<int, 3> a4;
  bool result = a1 == a2;

  static_assert(
    std::tuple_size<decltype(a1)>::value == 3,
    "std::tuple_size<std::array<int, 3>> failed."
    );
  std::sort(a1.begin(), a1.end());
  std::reverse_copy(a2.begin(), a2.end(), a4.begin());
  result = result && a1 == a2 && a2 != a4;
  std::array<int, 3>::const_reverse_iterator it = a4.crbegin();
  for (const auto& s : a2)
  {
    result = result && s == *it;
    ++it;
  }

  return result;
}

bool _array_size()
{
  std::array<int, 3> arr = { 42,23,9 };
  std::array<int, 0> arr0;
  bool result = false;
  
  result = arr.size() == 3 && !arr.empty() && arr0.empty() && arr.max_size() == arr.size() && 
    arr0.size() == 0 && arr0.max_size() == 0;

  return result;
}

bool _array_get()
{
  std::array<int, 3> arr;
  const std::array<int, 3> carr = { 4,5,6 };

  // std::array can be accessed like a std::tuple.
  // set values:
  std::get<0>(arr) = 1;
  std::get<1>(arr) = 2;
  std::get<2>(arr) = 3;

  // get values.
  return std::get<0>(arr) == 1 && std::get<1>(arr) == 2 && std::get<2>(arr) == 3 && 
    std::get<0>(carr) == 4 && std::get<1>(carr) == 5 && std::get<2>(carr) == 6;
}

bool _array_swap()
{
  std::array<int, 3> alice{ 1, 2, 3 }, chk_alice = alice;
  std::array<int, 3> bob{ 7, 8, 9 }, chk_bob = bob;
  bool result = alice == chk_alice && bob == chk_bob;
  std::swap(alice, bob);
  result = result && alice == chk_bob && bob == chk_alice;

  return result;
}

bool _array_fill()
{
  std::array<int, 8> arr = { 8,8,8,8,8,8,8,8 };
  std::array<int, 8> eight;
  bool result = eight != arr;

  eight.fill(8);
  result = result && eight == arr;

  return result;
}

bool _array_iterators()
{
  std::array<int, 0> empty;
  std::array<int, 4> numbers{ 5, 2, 3, 4 };
  bool result = empty.begin() == empty.end() && empty.cbegin() == empty.cend() && 
    numbers.begin() != numbers.end() && numbers.cbegin() != numbers.cend() && 
    *(numbers.begin()) == 5 && *(numbers.cbegin()) == 5;

  *numbers.begin() = 1;
  result = result && *numbers.begin() == 1;
  std::array<int, 4>::const_reverse_iterator::value_type i = 4;
  std::array<int, 4>::const_reverse_iterator crit = numbers.crbegin();
  for (; crit != numbers.crend(); ++crit)
  {
    result = result && *crit == i--;
  }

  return result;
}

void setup() 
{
  Serial.begin(9600);
  bool b1 = _array(), b2 = _array_size(), b3 = _array_get(), 
    b4 = _array_swap(), b5 = _array_fill(), b6 = _array_iterators();
  Serial.print("_array() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("_array_size() = "); Serial.println(b2 ? "OK" : "FAIL");
  Serial.print("_array_get() = "); Serial.println(b3 ? "OK" : "FAIL");
  Serial.print("_array_swap() = "); Serial.println(b4 ? "OK" : "FAIL");
  Serial.print("_array_fill() = "); Serial.println(b5 ? "OK" : "FAIL");
  Serial.print("_array_iterators() = "); Serial.println(b6 ? "OK" : "FAIL");
}

void loop() 
{

}
