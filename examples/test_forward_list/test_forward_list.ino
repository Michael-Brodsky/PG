// Part of the Pg test suite for <forward_list>
#include <pg.h>
#include <forward_list>

bool _forward_list()
{
  std::forward_list<int, 4> list1; // (1)
  std::forward_list<int, 8>::allocator_type alloc;
  std::forward_list<int, 8> list2(alloc); // (2)
  std::forward_list<String, 4> list3(4, "Hi"); // (3)
  std::forward_list<int, 16> list4(8); // (4)
  float arr[] = { 3.14f,2.71f,-1.f,1000000.1f };
  std::forward_list<float, 10> list5(std::begin(arr), std::end(arr)); // (5)
  std::forward_list<String, 4> list6(list3); // (6)
  std::forward_list<int, 4> list8(std::forward_list<int, 4>(4)); // (8)
  std::forward_list<int, 4> list9a{ 5,6,7,8 }, list9b(std::move(list9a)); 
  std::forward_list<int, 4> list10{ 1,2,3,4 }; // (10)
  // Check sizes & contents.
  bool result = list1.empty() && list2.empty() && list6 == list3 && list9a.empty() &&
    std::distance(list3.begin(), list3.end()) == 4 && std::distance(list4.begin(), list4.end()) == 8 &&
    std::distance(list5.begin(), list5.end()) == 4 && std::distance(list8.begin(), list8.end()) == 4 &&
    std::distance(list9b.begin(), list9b.end()) == 4 && std::distance(list10.begin(), list10.end()) == 4;
  for (auto& i : list3)
    result = result && i == "Hi";
  for (auto& i : list4)
    result = result && i == int();
  std::size_t n = 0;
  for (auto& i : list5)
    result = result && i == arr[n++];
  for (auto& i : list8)
    result = result && i == int();
  n = 5;
  for (auto& i : list9b)
    result = result && i == n++;
  n = 1;
  for (auto& i : list10)
    result = result && i == n++;

  return result;
}

bool _forward_list_caps()
{
  std::forward_list<int, 4> list1;
  bool result = list1.empty() && list1.max_size() == 4;
  std::forward_list<int, 8> list2(8);
  result = result && !list2.empty() && list2.max_size() == 8;
  list1.push_front(42);
  result = result && !list1.empty() && list1.max_size() == 4;

  return result;
}

bool _forward_list_iters()
{
  std::forward_list<int, 13> list1;
  bool result = list1.begin() == list1.end() && list1.cbegin() == list1.cend();
  std::forward_list<int, 8> list2{ 1,2,3,4 };
  std::forward_list<int, 8>::iterator it = list2.begin();
  int ii = 1;
  while (it != list2.end())
    result = result && *it++ == ii++;
  std::forward_list<int, 10> list3; 
  it = list3.before_begin();
  it = list3.insert_after(it, 42);
  result = result && *it == 42;
  std::forward_list<int, 13>::iterator jt = list1.begin();
  std::forward_list<int, 13>::const_iterator cjt = list1.cbegin();
  const std::forward_list<int, 13>::const_iterator cjt2 = list1.begin();
  result = result && cjt == cjt2 && std::distance<std::forward_list<int, 13>::const_iterator>(cjt, jt) == 0;

  return result;
}

bool _forward_list_access()
{
  std::forward_list<int, 4> list1{ 42,43,44,45 };

  return list1.front() == 42;
}

bool _forward_list_members()
{
  std::forward_list<int, 8> nums1{ 3, 1, 4, 6, 5, 9 };
  std::forward_list<int, 8> nums2;
  std::forward_list<int, 8> nums3;

  // std::distance(list.begin(), list.end()) gives number of allocated elements.
  // Subtracting this from max_size() gives remaining free elements.
  bool result = std::distance(nums1.begin(), nums1.end()) == 6 && 
    std::distance(nums2.begin(), nums2.end()) == 0 && 
    std::distance(nums3.begin(), nums3.end()) == 0;
  nums2 = nums1;
  result = result && std::distance(nums1.begin(), nums1.end()) == 6 &&
    std::distance(nums2.begin(), nums2.end()) == 6 &&
    std::distance(nums3.begin(), nums3.end()) == 0;
  nums3 = std::move(nums1);
  result = result && std::distance(nums1.begin(), nums1.end()) == 0 &&
    std::distance(nums2.begin(), nums2.end()) == 6 &&
    std::distance(nums3.begin(), nums3.end()) == 6;
  nums3 = { 1,2,3,4,5,6,7,8 };
  result = result && std::distance(nums3.begin(), nums3.end()) == 8;
  std::forward_list<char,8> characters;
  characters.assign(5, 'a');
  for (auto& c : characters)
    result = result && c == 'a';
  const char extra[] = { 'b','b','b','b','b','b' };
  characters.assign(std::begin(extra), std::end(extra));
  for (auto& c : characters)
    result = result && c == 'b';
  characters.assign({ 'C', '+', '+', '1', '1' });
  for (auto& c : characters)
    Serial.print(c);
  Serial.println();

  return result;
}

bool _forward_list_mods()
{
  std::forward_list<String, 10>foo;
  bool result = foo.empty();
  foo.push_front("Foo");
  result = result && *foo.begin() == "Foo";
  foo.pop_front();
  result = result && foo.empty();
  std::forward_list<String, 16> words{ "the","frogurt","is","also","cursed" };
  auto beginIt = words.begin();
  words.insert_after(beginIt, "strawberry");
  auto anotherIt = beginIt;
  ++anotherIt;
  anotherIt = words.insert_after(anotherIt, 2, "blueberry");
  std::array<String, 3> arr3 = { "apple","banana","cherry" };
  anotherIt = words.insert_after(anotherIt, arr3.begin(), arr3.end());
  // Final order of list `words'.
  std::array<String, 11> str = {
    "the","strawberry","blueberry","blueberry","apple","banana","cherry","frogurt","is","also","cursed"
  };
  std::forward_list<String, 16>::const_iterator j = words.begin();
  for (auto& i : str)
    result = result && i == *j++;
  std::forward_list<int, 10> l = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  l.erase_after(l.before_begin()); // Removes first element.
  auto fi = std::next(l.begin());
  auto la = std::next(fi, 3);
  l.erase_after(fi, la);
  // `l' list after erase.
  std::array<int, 6> il = { 2,3,6,7,8,9 };
  std::forward_list<int, 10>::iterator it = l.begin();
  for (auto& i : il)
    result = result && i == *it++;
  std::array<int, 3> a1 = { 1,2,3 };
  std::array<int, 3> a2 = { 4,5,6 };
  std::forward_list<int, 3> l1(a1.begin(), a1.end());
  std::forward_list<int, 3> l2(a2.begin(), a2.end());
  auto it1 = std::next(l1.begin());
  auto it2 = std::next(l2.begin());
  int& ref1 = l1.front();
  int& ref2 = l2.front();
  std::size_t n = 0;
  for (auto& i : l1)
    result = result && i == a1[n++];
  n = 0;
  for (auto& i : l2)
    result = result && i == a2[n++];
  result = result && *it1 == a1[1] && *it2 == a2[1] && ref1 == a1[0] && ref2 == a2[0];
  l1.swap(l2); // swap lists.
  n = 0;
  for (auto& i : l1)
    result = result && i == a2[n++];
  n = 0;
  for (auto& i : l2)
    result = result && i == a1[n++];
  result = result && *it1 == a1[1] && *it2 == a2[1] && ref1 == a1[0] && ref2 == a2[0];
  // Note that after swap the iterators and references stay associated with their
  // original elements, e.g. it1 that pointed to an element in 'a1' with value 2
  // still points to the same element, though this element was moved into 'a2'.
  l.clear();
  result = result && l.empty();

  return result;
}

bool _forward_list_ops()
{
  bool result = false;
  {
    std::forward_list<int, 8> x = { 1, 2, 3, 4, 5 }; // before
    std::forward_list<int, 8> y = { 10, 11, 12 };  // before
    std::forward_list<int, 8> zx = { 1, 10, 11, 12, 2, 3, 4, 5 }; // after
    std::forward_list<int, 8> zy; // after
    x.splice_after(x.cbegin(), y);  // splice
    result = x == zx && y == zy;  // check after
  }

  {
    std::forward_list<int, 8> x = { 1, 2, 3, 4, 5 };
    std::forward_list<int, 8> y = { 10, 11, 12 };
    std::forward_list<int, 8> zx = { 1, 11, 2, 3, 4, 5 };
    std::forward_list<int, 8> zy = { 10, 12 };
    x.splice_after(x.cbegin(), y, y.cbegin());
    result = result && x == zx && y == zy;
  }

  {
    std::forward_list<int, 8> x = { 1, 2, 3, 4, 5 };
    std::forward_list<int, 8> y = { 10, 11, 12 };
    std::forward_list<int, 8> zx = { 1, 11, 12, 2, 3, 4, 5 };
    std::forward_list<int, 8> zy = { 10 };
    x.splice_after(x.cbegin(), y, y.cbegin(), y.cend());
    result = result && x == zx && y == zy;
  }

  std::forward_list<int, 10> list = { 8,7,5,9,0,1,3,2,6,4 };
  std::forward_list<int, 10> sorted = { 0,1,2,3,4,5,6,7,8,9 };
  std::forward_list<int, 10> reversed = { 9,8,7,6,5,4,3,2,1,0 };
  list.sort();
  result = result && list == sorted;
  list.reverse();
  result = result && list == reversed;
  std::forward_list<int, 16> fl = { 1, 2, 2, 3, 3, 2, 1, 1, 2 };
  std::forward_list <int, 16> uniq1 = { 1, 2, 3, 2, 1, 2 };
  fl.unique();
  result = result && fl == uniq1;
  fl = { 1, 2, 12, 23, 3, 2, 51, 1, 2 };
  std::forward_list < int, 16> uniq2 = { 1, 2, 23, 2, 51, 2 };
  // Requires C++14
  //fl.unique([mod = 10](int x, int y) { return (x % mod) == (y % mod); });
  //result = result && fl == uniq2;
  std::forward_list<int, 10> l = { 1,100,2,3,10,1,11,-1,12 };
  std::forward_list<int, 10> removed = { 100,2,3,10,11,-1,12 };
  std::forward_list<int, 10> removed_if = { 2,3,10,-1 };
  l.remove(1);
  result = result && l == removed;
  l.remove_if([](int n) { return n > 10; });
  result = result && l == removed_if;
  std::forward_list<int, 10> list1 = { 5,9,0,1,3 };
  std::forward_list<int, 10> list2 = { 8,7,2,6,4 };
  list1.sort();
  list2.sort();
  list1.merge(list2);
  result = result && list1 == sorted; // merged list same as `sorted'.

  return result;
}

bool _forward_list_nonmem()
{
  std::forward_list<int, 3> alice{ 1, 2, 3 };
  std::forward_list<int, 4> bob{ 7, 8, 9, 10 };
  std::forward_list<int, 3> eve{ 1, 2, 3 };
  std::forward_list<int, 3> charlie{ 4, 5, 6 };
  bool result = alice != bob && alice == eve && alice <= eve && charlie > alice && 
    charlie >= alice && !(charlie <= alice);
  int i1 = 1, i2 = 4;
  for (auto& i : alice)
    result = result && i == i1++;
  for (auto& i : charlie)
    result = result && i == i2++;
  std::swap(alice, charlie);
  result = result && alice != eve && eve == charlie;
  result = result && std::size(alice) == 3 && std::size(bob) == 4;

  return result;
}

void setup() 
{
  Serial.begin(9600);
  bool b1 = _forward_list(), b2 = _forward_list_caps(), b3 = _forward_list_iters(),
    b4 = _forward_list_access(), b5 = _forward_list_members(), b6 = _forward_list_mods(), 
    b7 = _forward_list_ops(), b8 = _forward_list_nonmem();
  Serial.print("_list() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("_list_caps() = "); Serial.println(b2 ? "OK" : "FAIL");
  Serial.print("_list_iters() = "); Serial.println(b3 ? "OK" : "FAIL");
  Serial.print("_list_access() = "); Serial.println(b4 ? "OK" : "FAIL");
  Serial.print("_list_members() = "); Serial.println(b5 ? "OK" : "FAIL");
  Serial.print("_list_mods() = "); Serial.println(b6? "OK" : "FAIL");
  Serial.print("_list_ops() = "); Serial.println(b7 ? "OK" : "FAIL");
  Serial.print("_list_nonmem() = "); Serial.println(b8 ? "OK" : "FAIL");
}

void loop() 
{

}
