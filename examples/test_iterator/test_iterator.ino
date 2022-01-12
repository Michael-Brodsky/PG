// Part of the Pg test suite for <iterator>.
#include <pg.h>
#include <iterator>

template<class InputIt, class OutputIt>
OutputIt cpy(InputIt first, InputIt last, OutputIt dest)
{
    while (first != last)
        *dest++ = *first++;

    return dest;
}

template<class T, size_t SIZE>
class Stack {
public:
    using difference_type = ptrdiff_t;
    using value_type = T;
    using reference = T&;
    using pointer = T*;
private:
    T arr[SIZE];
    size_t pos = 0;
public:
    T pop() {
        return arr[--pos];
    }
    Stack& push(const T& t) {
        arr[pos++] = t;
        return *this;
    }
    Stack& push_front(const T& t) { return push(t); }
    // we wish that looping on Stack would be in LIFO order
    // thus we use std::reverse_iterator as an adaptor to existing iterators
    // (which are in this case just simple pointers: [arr, arr+pos)
    std::reverse_iterator<T*> begin() {
        return std::reverse_iterator<T*>(arr + pos);
    }
    std::reverse_iterator<T*> end() {
        return std::reverse_iterator<T*>(arr);
    }
};

// A simple sequence container so we didn't have to #include <array>.
template<class T, size_t SIZE> 
struct Array
{
    using difference_type = ptrdiff_t;
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    T* begin() { return data_; }
    T* end() { return data_ + SIZE; }
    T& operator[](size_t n) { return data_[n]; }
    constexpr std::size_t size() const { return SIZE; }
    void push_front(const T& t) { for (size_t i = 0; i < SIZE - 1; ++i) data_[i + 1] = data_[i]; data_[0] = t; }
    T data_[SIZE];
};


bool _iterator_circular()
{
  bool result = true;
  Array<int, 4> arr{ 1,2,3,4 };
  Array<int, 5> arr5{ 1,2,3,4,5 };
  const int ci = 100;
  std::experimental::circular_iterator<decltype(arr)> it(arr);
  std::experimental::circular_iterator<decltype(arr)> it2(arr), it3(arr);
  std::experimental::circular_iterator<decltype(arr5)> it4(arr5);
  result = result && it == it3++;
  result = result && it < it3;
  int i = 1;
  for (std::size_t j = 0; j < arr.size()*2; ++j)
  { 
    i = j % arr.size() + 1; // use this to cmp iterator output.
    result = result && i == *it++ && *it2++ == i;
  }

  return result;
}

bool _iterator_reverse()
{
  bool result = true;
    Stack<int, 8> s;
    s.push(5).push(15).push(25).push(35);
    int res[] = { 35,25,15,5 }; // stack order reversed.
    int i = 0;
    for (int val : s) {
        result = result && val == res[i++];
    }

  return result;
}

bool _iterator_front_inserter()
{
    bool result = true;
    int v[] = { 1,2,3,4 };
    Stack<int, 4> d;
    cpy(std::begin(v), std::end(v), 
        std::front_insert_iterator<Stack<int, 4>>(d)); // or std::front_inserter(d)
    int i = 4;
    for (int n : d)
    {
        result = result && n == i--;
    }

    return result;
}

void setup() 
{
  Serial.begin(9600);
  bool b1 = _iterator_circular(), b2 = _iterator_reverse(), b3 = _iterator_front_inserter();
  Serial.print("_circular() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("_reverse() = "); Serial.println(b2 ? "OK" : "FAIL");
  Serial.print("_front_inserter() = "); Serial.println(b3 ? "OK" : "FAIL");
}

void loop() 
{

}
