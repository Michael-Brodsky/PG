// Part of the Pg test suite for <algorithm>
#include <pg.h>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <numeric>
#include <random>

bool _algorithm_for_each()
{
  struct Sum
  {
    void operator()(int n) { sum += n; }
    int sum{ 0 };
  };
  auto printme = [](const int& n) { Serial.print(" "); Serial.print(n); };
  bool result = true;
  std::array<int, 6> nums{ 3, 4, 2, 8, 15, 267 };
  std::array<int, 6> after{ 4, 5, 3, 9, 16, 268 };
  std::for_each(nums.begin(), nums.end(), [](int& n) { n++; });
  Sum s = std::for_each(nums.begin(), nums.end(), Sum());
  std::array<int, 6>::const_iterator it = after.begin();
  for (auto& i : nums)
    result = result && i == *it++;
  result = result && s.sum == 305;

  return result;
}

bool _algorithm_count()
{
  bool result = true;
  constexpr std::array<int,10> v = { 1, 2, 3, 4, 4, 3, 7, 8, 9, 10 };
  // determine how many integers match a target value.
  int target1 = 3;
  int target2 = 5;
  int num_items1 = std::count(v.begin(), v.end(), target1);
  int num_items2 = std::count(v.begin(), v.end(), target2);
  // use a lambda expression to count elements divisible by 3.
  int num_items3 = std::count_if(v.begin(), v.end(), [](int i) {return i % 3 == 0; });
  result = result && num_items1 == 2 && num_items2 == 0 && num_items3 == 3;

  return result;
}

bool mypredicate(int i, int j) {
  return (i == j);
}

bool _algorithm_mismatch()
{
  bool result = true;
  std::array<int, 5> myvector{ 10,20,30,40,50 };
  int myints[] = { 10,20,80,320,1024 };
  std::pair<std::array<int, 5>::iterator, int*> mypair;
  // using default comparison:
  mypair = std::mismatch(myvector.begin(), myvector.end(), myints);
  result = result && *mypair.first == 30 && *mypair.second == 80;
  ++mypair.first; ++mypair.second;
  // using predicate comparison:
  mypair = std::mismatch(mypair.first, myvector.end(), mypair.second, mypredicate);
  result = result && *mypair.first == 40 && *mypair.second == 320;

  return result;
}

bool _IsOdd(int i) {
  return ((i % 2) == 1);
}
bool _IsEven(int i) {
  return ((i % 2) == 0);
}
bool _algorithm_find()
{
  bool result = true;
  int myints[] = { 10, 20, 30, 40 };
  int* p;
  p = std::find(myints, myints + 4, 30);
  result = p != myints + 4;
  std::array<int, 4> arr;
  std::copy(std::begin(myints), std::end(myints), std::begin(arr));
  std::array<int, 4>::iterator it = std::find(arr.begin(), arr.end(), 30);
  result = result && it != arr.end();
  std::array<int, 4> arr2{ 10,25,40,55 };
  std::array<int, 4>::iterator it2 = std::find_if(arr2.begin(), arr2.end(), _IsOdd);
  result = result && *it2 == 25;
  std::array<int, 5> foo = { 1,2,3,4,5 };
  std::array<int, 5>::iterator bar =
    std::find_if_not(foo.begin(), foo.end(), [](int i) { return i % 2; });
  result = result && *bar == 2;

  return result;
}

bool _algorithm_any_all()
{
  bool result = true;
  std::array<int, 8> foo = { 3,5,7,11,13,17,19,23 };
  result = std::all_of(foo.begin(), foo.end(), [](int i) { return i % 2; });
  std::array<int, 7> bar = { 0,1,-1,3,-3,5,-5 };
  result = result && std::any_of(bar.begin(), bar.end(), [](int i) {return i < 0; });
  std::array<int, 8> baz = { 1,2,4,8,16,32,64,128 };
  result = result && std::none_of(baz.begin(), baz.end(), [](int i) {return i < 0; });

  return result;
}

bool mypredicate2(int i, int j) {
  return (i == j);
}

bool _algorithm_search()
{
  bool result = true;
  std::array<int, 10> haystack{ 10, 20, 30, 40, 50, 60, 70, 80, 90 };
  int needle1[] = { 40,50,60,70 };
  std::array<int, 10>::iterator it = std::search(haystack.begin(), haystack.end(), needle1, needle1 + 4);
  result = it != haystack.end() && it - haystack.begin() == 3;
  int needle2[] = { 20,30,50 };
  it = std::search(haystack.begin(), haystack.end(), needle2, needle2 + 3, mypredicate2);
  result = result && it == haystack.end();
  int myints[] = { 10,20,30,30,20,10,10,20 };
  std::array<int, 10> arr;
  std::copy(std::begin(myints), std::end(myints), std::begin(arr));
  it = std::search_n(arr.begin(), arr.end(), 2, 30);
  result = result && it != arr.end() && it - arr.begin() == 2;
  it = std::search_n(arr.begin(), arr.end(), 2, 10, mypredicate2);
  result = result && it != arr.end() && it - arr.begin() == 5;

  return result;
}

bool algorithm_find_end()
{
  std::array<int, 10> haystack = { 1,2,3,4,5,1,2,3,4,5 };
  int needle1[] = { 1,2,3 };
  std::array<int, 10>::iterator it = std::find_end(haystack.begin(), haystack.end(), needle1, needle1 + 3);
  bool result = it != haystack.end() && it - haystack.begin() == 5;
  int needle2[] = { 4,5,1 };
  it = std::find_end(haystack.begin(), haystack.end(), needle2, needle2 + 3, mypredicate2);
  result = it != haystack.end() && it - haystack.begin() == 3;

  return result;
}

bool _algorithm_adjacent_find()
{
  std::array<int, 10> arr = { 5,20,5,30,30,20,10,10,20 };
  std::array<int, 10>::iterator it = std::adjacent_find(arr.begin(), arr.end());
  bool result = it != arr.end() && *it == 30;
  it = std::adjacent_find(++it, arr.end(), mypredicate2);
  result = it != arr.end() && *it == 10;

  return result;
}

bool _algorithm_find_first_of()
{
  std::array<char, 10> haystack = { 'a','b','c','A','B','C' };
  int needle[] = { 'A','B','C' };
  std::array<char, 10>::iterator it = std::find_first_of(haystack.begin(), haystack.end(), needle, needle + 3);
  bool result = it != haystack.end() && *it == 'A';

  return result;
}

bool _algotithm_is_partitioned()
{
  std::array<int, 7> foo{ 1,2,3,4,5,6,7 };
  bool result = !std::is_partitioned(foo.begin(), foo.end(), _IsOdd);
  std::partition(foo.begin(), foo.end(), _IsOdd);
  result = result && std::is_partitioned(foo.begin(), foo.end(), _IsOdd);

  return result;
}

bool _algorithm_partition_point()
{
  bool result = true;
  std::array<int, 9> foo{ 1,2,3,4,5,6,7,8,9 };
  std::array<int, 5> odd;
  std::partition(foo.begin(), foo.end(), _IsOdd);
  auto it = std::partition_point(foo.begin(), foo.end(), _IsOdd);
  std::copy(foo.begin(), it, odd.begin());
  int j = -1;
  for (int& x : odd)
    result = result && x == (j += 2);

  return result;
}

bool _algorithm_partition_copy()
{
  bool result = true;
  std::array<int, 9> foo{ 1,2,3,4,5,6,7,8,9 };
  std::array<int, 9> odd{ 0,0,0,0,0,0,0,0,0 }, even{ 0,0,0,0,0,0,0,0,0 };
  unsigned n = std::count_if(foo.begin(), foo.end(), _IsOdd);
  std::partition_copy(foo.begin(), foo.end(), odd.begin(), even.begin(), _IsOdd);
  int j = -1, k = 0;
  for (auto& i : odd)
  {
    if (j < 9)
      result = result && i == (j += 2);
  }
  for (auto& i : even)
  {
    if (k < 8)
      result = result && i == (k += 2);
  }

  return result;
}

bool _algorithm_copy_if()
{
  std::array<int, 5> foo = { 25,15,5,-5,-15 };
  std::array<int, 3> bar;
  int i[] = { 25,15,5 };
  auto it = std::copy_if(foo.begin(), foo.end(), bar.begin(), [](int i) {return !(i < 0); });
  std::array<int, 10>::iterator jt = std::search(bar.begin(), bar.end(), i, i + 3);

  return jt == bar.begin();
}

bool _algorithm_copy_backward()
{
  std::array<int, 8> foo = { 10, 20, 30, 40, 50 };
  std::array<int, 8> bar = { 10, 20, 30, 10, 20, 30, 40, 50 };
  std::copy_backward(foo.begin(), foo.begin() + 5, foo.end());

  return bar == foo;
}
bool _algorithm_copy_n()
{
  int myints[] = { 10,20,30,40,50,60,70 };
  std::array<int, 7> arr;
  std::array<int, 7>::iterator it = std::copy_n(myints, 7, arr.begin());

  return *--it == 70;
}

bool _algorithm_move()
{
  std::array<String, 4> foo = { "air","water","fire","earth" };
  std::array<String, 4> bar;
  std::array<String, 4> baz = { "air","water","fire","earth" };
  std::move(foo.begin(), foo.begin() + 4, bar.begin());

  return bar == baz && foo != bar;
}

bool _algorithm_fill()
{
  bool result = true;
  std::array<float, 4> arr;
  std::fill(arr.begin(), arr.end(), 3.14f);
  for (auto& x : arr)
    result = result && x == 3.14f;
  std::array<int, 4> arr2;
  std::fill_n(arr2.begin(), arr2.size(), 42);
  for (auto& x : arr2)
    result = result && x == 42;

  return result;
}

bool _algorithm_generate()
{
  bool result = true;

  std::array<unsigned long, 5> arr, arr2;
  std::minstd_rand lcg0, lcg1, lcg2, lcg3;
  std::generate_n(arr.begin(), arr.max_size(), lcg0);
  std::generate(arr2.begin(), arr2.end(), lcg2);
  for (auto& x : arr)
    result = result && x == lcg1();
  for (auto& x : arr)
    result = result && x == lcg3();
  result = result && arr == arr2;

  return result;
}

void setup() 
{
  Serial.begin(9600);
  bool b1 = _algorithm_for_each(), b2 = _algorithm_count(), b3 = _algorithm_mismatch(), 
    b4 = _algorithm_find(), b5 = _algorithm_any_all(), b6 = _algorithm_search(), 
    b7 = algorithm_find_end(), b8 = _algorithm_adjacent_find(), b9 = _algorithm_find_first_of(), 
    b10 = _algotithm_is_partitioned(), b11 = _algorithm_partition_point(), b12 = _algorithm_partition_copy(), 
    b13 = _algorithm_copy_if(), b14 = _algorithm_copy_backward(), b15 = _algorithm_copy_n(), 
    b16 = _algorithm_move(), b17 = _algorithm_fill(), b18 = _algorithm_generate();
  Serial.print("_for_each() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("_count() = "); Serial.println(b2 ? "OK" : "FAIL");
  Serial.print("_mismatch() = "); Serial.println(b3 ? "OK" : "FAIL");
  Serial.print("_find() = "); Serial.println(b4 ? "OK" : "FAIL");
  Serial.print("_any_all_none() = "); Serial.println(b5 ? "OK" : "FAIL");
  Serial.print("_search() = "); Serial.println(b6 ? "OK" : "FAIL");
  Serial.print("_find_end() = "); Serial.println(b7 ? "OK" : "FAIL");
  Serial.print("_adjacent_find() = "); Serial.println(b8 ? "OK" : "FAIL");
  Serial.print("_find_first_of() = "); Serial.println(b9 ? "OK" : "FAIL");
  Serial.print("_is_partitioned() = "); Serial.println(b10 ? "OK" : "FAIL");
  Serial.print("_partition_point() = "); Serial.println(b11 ? "OK" : "FAIL");
  Serial.print("_partition_copy() = "); Serial.println(b12 ? "OK" : "FAIL");
  Serial.print("_copy_if() = "); Serial.println(b13 ? "OK" : "FAIL");
  Serial.print("_copy_backward() = "); Serial.println(b14 ? "OK" : "FAIL");
  Serial.print("_copy_n() = "); Serial.println(b15 ? "OK" : "FAIL");
  Serial.print("_move() = "); Serial.println(b16 ? "OK" : "FAIL");
  Serial.print("_fill() = "); Serial.println(b17 ? "OK" : "FAIL");
  Serial.print("_generate() = "); Serial.println(b18 ? "OK" : "FAIL");
}

void loop() 
{

}
