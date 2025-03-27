// Part of the Pg test suite for <utility>.
#include <pg.h>
#include <utility>

bool _swap()
{
  int a = 5, b = 3;
  int arr1[] = { 1,2,3 }, arr2[] = { -1,-2,-3 };
  bool result = false;
  std::swap(a, b);
  std::swap(arr1, arr2);
  result = a == 3 && b == 5;
  for(size_t i = 0; i < 3; ++i)
  {
    result = result && arr1[i] < arr2[i];
  }
  
  return result;
}

bool _pair()
{
  bool result = false;
  std::pair<int, float> p(1, 3.14f), q(2, 42.0f);
  static_assert(
    std::tuple_size<decltype(p)>::value == 2,
    "std::tuple_size std::pair failed."
    );
  p = q;
  result = p == q;
  std::pair<int, float> r{ 3, 8.f };
  p = r;
  result = result && p == r;
  p = std::pair<int, float>{ 4, 69.9f };
  p = std::move(q);
  result = result && p == q;
  p = std::pair<int, float>{ 5, 6.6f };
  p = std::move(r);
  result = result && p == r && p != q;
  std::swap(p, r);
  result = result && p == r;

  return result;
}

bool _pair_get()
{
  const std::pair<int, char> foo{ 42, 'c' };
  std::pair<int, char> bar{ 10, 'x' };
  auto a = std::get<0>(foo);
  auto b = std::get<1>(foo);
  bool result = foo.first == 42 && foo.second == 'c' && a == foo.first && b == foo.second &&
    bar.first == 10 && bar.second == 'x';
  std::get<0>(bar) = 50;
  result = result && bar.first == 50;
  
  return result; 
}

template<class T>
bool test_size(T t)
{
  int a[std::tuple_size<T>::value]; // can be used at compile time

  return std::tuple_size<T>::value == 2;
}

bool _pair_size()
{
  test_size(std::make_pair(1, 3.14));

  return true;
}

void setup() 
{
  Serial.begin(9600);
  bool b1 = _swap(), b2 = _pair(), b3 = _pair_get(), b4 = _pair_size();
  Serial.print("_swap() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("_pair() = "); Serial.println(b2 ? "OK" : "FAIL");
  Serial.print("_pair_get() = "); Serial.println(b3 ? "OK" : "FAIL");
  Serial.print("_pair_size() = "); Serial.println(b3 ? "OK" : "FAIL");
}

void loop() 
{

}
