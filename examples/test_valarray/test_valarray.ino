// Part of the Pg test suite for std::valarray.

#include <pg.h>
#include <valarray>

bool _valarray();
bool _valarray_slicer();
bool _valarray_nonmembers();
bool _valarray_stuff();

void setup() 
{
  Serial.begin(9600);
  bool b1 = _valarray(), b2 = _valarray_slicer(), b3 = _valarray_nonmembers(),
    b4 = _valarray_stuff();
  Serial.print("_valarray() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("_slicer() = "); Serial.println(b2 ? "OK" : "FAIL");
  Serial.print("_nonmembers() = "); Serial.println(b3 ? "OK" : "FAIL");
  Serial.print("_stuff() = "); Serial.println(b4 ? "OK" : "FAIL");
}

void loop() 
{
  
}

bool _valarray()
{
  class Matrix {
    std::valarray<int, 16> data;
    int dim;
  public:
    Matrix(int r, int c) : data(r* c), dim(c) {}
    int& operator()(int r, int c) { return data[r * dim + c]; }
    int trace() const {
      return data[std::slice(0, dim, dim + 1)].sum();
    }
  };
  bool result = true;
  int ints[] = { 1,2,3,4 };
  std::valarray<int, 16> va, va2(42, 16);
  std::valarray<int, 16> va3(ints, 4);
  result = result && va.size() == 0;
  va = va2;
  result = result && va.size() == 16;
  int* p = std::begin(va);
  result = result && *p == 42;
  va = -va2;
  for (auto& i : va)
    result = result && i == -42;
  std::swap(va, va2);
  for (auto& i : va)
    result = result && i == 42;
  va *= 2;
  for (auto& i : va)
    result = result && i == 84;
  std::valarray<int, 16> va4 = va3.shift(4);
  result = result && va4.size() == 8;
  for (size_t i = 0; i < 4; ++i)
    result = result && va4[i] == 0;
  int n = 1;
  for (size_t i = 4; i < va4.size(); ++i)
    result = result && va4[i] == n++;
  result = result && va3.sum() == 10 && va3.min() == 1 && va3.max() == 4;
  std::valarray<int, 16> va5(5);
  result = result && va4.size() == 8;
  va5 = 99;
  for (auto& i : va5)
    result = result && i == 99;
  va5 = { 1,2,3,4,5,6,7 };
  n = 1;
  for (auto& i : va5)
    result = result && i == n++;
  std::valarray<int, 16> va6(23, 16), va7 = va6;
  std::valarray<int, 16> va8 = va6 + va7;
  for (auto& i : va8)
    result = result && i == 46;
  std::valarray<int, 16> foo(12);
  for (int i = 0; i < 12; ++i) 
    foo[i] = i;
  std::valarray<int, 16> bar = foo[std::slice(2, 3, 4)];
  std::valarray<int, 16> slicer{2, 6, 10};
  std::valarray<bool, 16> res = bar == slicer;
  for (auto& i : res)
    result = result && i == true;
  std::valarray<int, 16> v9(42, 16), v10(7, 16);
  std::valarray<bool, 16> res9 = v9 == 42, res10 = v10 != 42;
  for (auto& i : res9)
    result = result && i == true;
  for (auto& i : res10)
    result = result && i == true;
  std::valarray<int, 16> v11(1, 16), v13(16);
  std::valarray<bool, 16> res12 = v11 && 0;
  for (auto& i : res12)
    result = result && i == false;
  std::valarray<bool, 16> res13 = v11 || v13;
  for (auto& i : res13)
    result = result && i == true;
  Matrix m(3, 3);
  n = 0;
  for (int r = 0; r < 3; ++r)
    for (int c = 0; c < 3; ++c)
      m(r, c) = ++n;
  result = result && m.trace() == 15;

  return result;
}

bool _valarray_slicer()
{
  bool result = true;
  std::array<int, 9> arr{ 99,10,2,99,4,50,99,7,8 }; // result
  std::valarray<int, 16> foo(9);
  for (int i = 0; i < 9; ++i) 
    foo[i] = i;
  std::slice myslice = std::slice(1, 3, 2);
  foo[myslice] *= std::valarray<int, 16>(10, 3);
  foo[std::slice(0, 3, 3)] = 99;
  auto j = arr.begin();
  for (auto& i : foo)
    result = result && i == *j++;

  return result;
}

bool _valarray_nonmembers()
{
  bool result = true;
  std::valarray<int, 16> foo(-42, 9);
  std::valarray<int, 16> bar = std::abs(foo);
  std::valarray<float, 16> a(42.f, 9);
  std::valarray<float, 16> b = std::sqrt(a);
  for (auto& i : b)
    result = result && i <= 6.480741;

  return result;
}

bool _valarray_stuff()
{
  bool result = true;
  std::valarray<int, 8> res = { 3,4,5,6,7,8,1,2 };
  std::valarray<int, 8> v{ 1, 2, 3, 4, 5, 6, 7, 8 }, w = v.cshift(2);
  std::valarray<bool, 8> b = w == res;
  for (auto& i : b)
    result = result && i == true;

  return result;
}
