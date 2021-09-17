// This is part of the Pg test suite for <type_traits>.
#include <pg.h>
#include <type_traits>

bool _is_const()
{
    static_assert(
        !std::is_const<int>::value
        && std::is_const<const int>::value
        && !std::is_const<const int*>::value
        && std::is_const<int* const>::value
        && !std::is_const<const int&>::value
        && std::is_const<typename std::remove_reference<const int&>::type>::value,
        "std::is_const failed."
        );
    return true;
}

bool _is_reference()
{
    class A {};

    static_assert(
        !std::is_reference<A>::value
        && std::is_reference<A&>::value
        && std::is_reference<A&&>::value
        && !std::is_reference<int>::value
        && std::is_reference<int&>::value
        && std::is_reference<int&&>::value,
        "std::is_reference failed."
        );

    return true;
}

template<class T, size_t N> // std::array
struct type_traits_arr {
    T data_[N];
};

bool _is_array()
{
    class A {};

    static_assert(
        !std::is_array<A>::value
        && std::is_array<A[]>::value
        && std::is_array<A[3]>::value
        && !std::is_array<float>::value
        && !std::is_array<int>::value
        && std::is_array<int[]>::value
        && std::is_array<int[3]>::value
        && !std::is_array<type_traits_arr<int, 3>>::value,
        ""
        );

    return true;
}

template<typename>
struct PM_traits {};

template<class T, class U>
struct PM_traits<U T::*> {
    using member_type = U;
};

int f2();

bool _is_function()
{
    struct A {
        int fun() const&;
    };
    using Tp = PM_traits<decltype(&A::fun)>::member_type;

    static_assert(
        !std::is_function<A>::value
        && std::is_function<int(int)>::value
        && std::is_function<decltype(f2)>::value
        && !std::is_function<int>::value
        && std::is_function<Tp>::value,
        "std::_is_function failed."
        );

    return true;
}

bool _is_type()
{
    struct A {};
    typedef union {
        int a;
        float b;
    } B;
    struct C {
        B d;
    };
    enum fruit { orange, apple };

    static_assert(
        !std::is_union<A>::value && 
        std::is_union<B>::value && 
        !std::is_union<C>::value && 
        std::is_enum<fruit>::value && 
        std::is_class<A>::value,
        "std::is_union failed."
        );

    return true;
}

bool _is_base_of()
{
    class A {};
    class B : A {};
    class C {};

    static_assert(
        std::is_base_of<A, B>::value && 
        !std::is_base_of<B, A>::value && 
        !std::is_base_of<C, B>::value && 
        std::is_base_of<C, C>::value,
        "std::is_base_of failed."
        );
    return true;
}

bool _alignment_of()
{
    struct Foo {
        int   i;
        float f;
        char  c;
    };
    // Note: `alignas(alignof(long double))` below can be simplified to simply 
    // `alignas(long double)` if desired.
    struct alignas(alignof(long double)) Foo2 {
        double d;
    };
    struct Empty {};
    struct alignas(64) Empty64 {};

    // std::alignment_of returns 1 for all types except Empty64 = 64, 
    // also the values not printed in the order the test is executed?? 
    Serial.println("Alignment of");
    Serial.print("char: "); Serial.println(std::alignment_of<char>());
    Serial.print("pointer: "); Serial.println(std::alignment_of<int*>());
    Serial.print("Class Foo: "); Serial.println(std::alignment_of<Foo>());
    Serial.print("Class Foo2: "); Serial.println(std::alignment_of<Foo2>());
    Serial.print("Class Empty: "); Serial.println(std::alignment_of<Empty>());
    Serial.print("Class Empty64: "); Serial.println(std::alignment_of<Empty64>());

    return true;
}

bool _rank()
{
    return std::rank<int>{} == 0 && std::rank<int[5]>{} == 1 &&
        std::rank<int[5][5]>{} == 2 && std::rank<int[][5][5]>{} == 3;
}

bool _extent()
{
    const auto ext = std::extent<int[9]>{};
    const int ints[] = { 1,2,3,4 };
    const auto ext0 = std::extent<int[3][4], 0>::value;
    const auto ext1 = std::extent<int[3][4], 1>::value;
    const auto ext2 = std::extent<int[3][4], 2>::value;

    return std::extent<int[3]>::value == 3 && ext0 == 3 && ext1 == 4 && ext2 == 0 && 
        std::extent<int[]>::value == 0 && ext == 9 && std::extent<decltype(ints)>::value == 4;
}

class E { public: template<class T> E(T&&) { } };

bool _is_convertible()
{
    class A {};
    class B : public A {};
    class C {};
    class D { public: operator C() { return c; }  C c; };

    bool b2a = std::is_convertible<B*, A*>::value;
    bool a2b = std::is_convertible<A*, B*>::value;
    bool b2c = std::is_convertible<B*, C*>::value;
    bool d2c = std::is_convertible<D, C>::value;
    bool everything2e = std::is_convertible<A, E>::value; //< B, C, D, etc

    return b2a && !a2b && !b2c && d2c && everything2e;
}

bool _is_fundamental()
{
    class A {};

    static_assert(
        !std::is_fundamental<A>::value
        && std::is_fundamental<int>::value
        && !std::is_fundamental<int&>::value
        && !std::is_fundamental<int*>::value
        && std::is_fundamental<float>::value
        && !std::is_fundamental<float&>::value
        && !std::is_fundamental<float*>::value,
        "std::is_fundamental failed."
        );

    return true;
}

template <class T>
T f1(T i)
{
    static_assert(std::is_integral<T>::value, "Integral required.");
    return i;
}

bool _is_integral()
{
    class A {};
    enum E : int {};

    static_assert(
        !std::is_integral<A>::value
        && !std::is_integral<E>::value
        && !std::is_integral<float>::value
        && std::is_integral<int>::value
        && std::is_integral<const int>::value
        && std::is_integral<bool>::value,
        "std::is_integral failed."
        );

    return f1(123) == 123;
}

bool _is_unsigned()
{
    class A {};
    enum B : int {};
    enum class C : int {};

    static_assert(
        ! std::is_unsigned<A>::value
        && ! std::is_unsigned<float>::value
        && ! std::is_unsigned<signed int>::value
        && std::is_unsigned<unsigned int>::value
        && ! std::is_unsigned<B>::value
        && ! std::is_unsigned<C>::value,
        "std::is_unsigned failed."
        );

    return true;
}

bool _is_signed()
{
    class A {};
    enum B : int {};
    enum class C : int {};

    static_assert(
        ! std::is_signed<A>::value
        && std::is_signed<float>::value
        && std::is_signed<signed int>::value
        && ! std::is_signed<unsigned int>::value
        && ! std::is_signed<B>::value
        && ! std::is_signed<C>::value
        && ! std::is_signed<bool>::value
        && std::is_signed<signed int>()
        && ! std::is_signed<unsigned int>{},
        "std::is_signed failed."
        );

    return true;
}

bool _make_signed()
{
    typedef std::make_signed<char>::type char_type;
    typedef std::make_signed<const volatile char>::type cv_char_type;
    typedef std::make_signed<char16_t>::type char16_type;
    typedef std::make_signed<const char32_t>::type const_char32_type;
    typedef std::make_signed<wchar_t>::type wchar_type;
    typedef std::make_signed<int>::type int_type;
    typedef std::make_signed<volatile long>::type volatile_long_type;
    typedef std::make_signed<const long>::type const_long_type;
    typedef std::make_signed<const volatile signed long long>::type cv_long_long_type;

    static_assert(
        std::is_same<char_type, signed char>::value 
        && std::is_same<cv_char_type, const volatile signed char>::value
        && std::is_same<wchar_type, wchar_t>::value
        && std::is_same<char16_type, signed short>::value
        && std::is_same<const_char32_type, const long>::value
        && std::is_same<int_type, int>::value
        && std::is_same<volatile_long_type, volatile long>::value
        && std::is_same<const_long_type, const long>::value
        // Fails to compile if std::make_signed changes the type off any long long.
        && std::is_same<cv_long_long_type, const volatile signed long long>::value, 
        "std::make_signed failed."
        );

    return true;
}

bool _make_unsigned()
{
    typedef std::make_unsigned<char>::type char_type;
    typedef std::make_unsigned<const volatile char>::type cv_char_type;
    typedef std::make_unsigned<char16_t>::type char16_type;
    typedef std::make_unsigned<const char32_t>::type const_char32_type;
    typedef std::make_unsigned<wchar_t>::type wchar_type;
    typedef std::make_unsigned<int>::type int_type;
    typedef std::make_unsigned<volatile long>::type volatile_long_type;
    typedef std::make_unsigned<const long>::type const_long_type;
    typedef std::make_unsigned<const volatile unsigned long long>::type cv_long_long_type;
    
    static_assert(
        std::is_same<char_type, unsigned char>::value 
        && std::is_same<cv_char_type, const volatile unsigned char>::value
        && std::is_same<wchar_type, unsigned short>::value
        && std::is_same<char16_type, char16_t>::value
        && std::is_same<const_char32_type, const char32_t>::value
        && std::is_same<int_type, unsigned int>::value
        && std::is_same<volatile_long_type, volatile unsigned long>::value
        && std::is_same<const_long_type, const unsigned long>::value
        // Fails to compile if std::make_unsigned changes the type off any long long.
        && std::is_same<cv_long_long_type, const volatile unsigned long long>::value, 
        "std::make_unsigned failed."
        );

    return true;
}

template<typename F, typename Class>
void ptr_to_member_func_cvref_test(F Class::*)
{
    // F is an "abominable function type"
    using FF = typename std::add_pointer<F>::type;
    static_assert(std::is_same<F, FF>::value, "FF should be precisely F");
}

bool _add_pointer () 
{
    struct S
    {
        void f_ref()& {}
        void f_const() const {}
    };

    bool result = false;
    int i = 123;
    int& ri = i;
    typedef std::add_pointer<decltype(i)>::type IntPtr;
    typedef std::add_pointer<decltype(ri)>::type IntPtr2;
    IntPtr pi = &i;

    static_assert(std::is_pointer<IntPtr>::value, "IntPtr should be a pointer");
    static_assert(std::is_same<IntPtr, int*>::value, "IntPtr should be a pointer to int");
    static_assert(std::is_same<IntPtr2, IntPtr>::value, "IntPtr2 should be equal to IntPtr");

    typedef std::remove_pointer<IntPtr>::type IntAgain;
    IntAgain j = i;
    
    result = j == 123 && *pi == 123 && i == 123;

    static_assert(!std::is_pointer<IntAgain>::value, "IntAgain should not be a pointer");
    static_assert(std::is_same<IntAgain, int>::value, "IntAgain should be equal to int");

    ptr_to_member_func_cvref_test(&S::f_ref);
    ptr_to_member_func_cvref_test(&S::f_const);

    return result;
}

bool _add_lrvalue_reference()
{
    using nonref = int;
    using lref = typename std::add_lvalue_reference<nonref>::type;
    using rref = typename std::add_rvalue_reference<nonref>::type;
    using voidref = typename std::add_lvalue_reference<void>::type;

    static_assert(
        ! std::is_lvalue_reference<nonref>::value
        && std::is_lvalue_reference<lref>::value
        && std::is_rvalue_reference<rref>::value
        && std::is_rvalue_reference<rref>::value
        && ! std::is_reference<voidref>::value,
        "std::add_lvalue_reference failed."
        );

    return true;
}


template<typename T, typename U>
struct decay_equiv :
    std::is_same<typename std::decay<T>::type, U>::type
{};
template<typename T, typename U>
struct decay_equiv_t :
    std::is_same<typename std::decay<T>::type, U>::type
{};

bool _decay()
{
    static_assert(
        decay_equiv<int, int>::value
        && decay_equiv<int&, int>::value
        && decay_equiv<int&&, int>::value
        && decay_equiv<const int&, int>::value
        && decay_equiv<int[2], int*>::value
        && decay_equiv<int(int), int(*)(int)>::value,
        "std::decay failed."
        );

    return true;
}

bool _decay_t()
{
    static_assert(
        decay_equiv_t<int, int>::value
        && decay_equiv_t<int&, int>::value
        && decay_equiv_t<int&&, int>::value
        && decay_equiv_t<const int&, int>::value
        && decay_equiv_t<int[2], int*>::value
        && decay_equiv_t<int(int), int(*)(int)>::value,
        "std::decay_t failed."
        );

    return true;
}

bool _negation()
{
    static_assert(
        std::is_same<
        std::bool_constant<false>,
        typename std::negation<std::bool_constant<true>>::type>::value,
        "std::negation failed.");
    static_assert(
        std::is_same<
        std::bool_constant<true>,
        typename std::negation<std::bool_constant<false>>::type>::value,
        "std::negation failed.");

    return true;
}

bool _declval()
{
    struct Default { int foo() const { return 1; } };
    struct NonDefault
    {
        NonDefault() = delete;
        int foo() const { return 1; }
    };

    bool result = false;
    decltype(Default().foo()) n1 = 1;
    decltype(std::declval<NonDefault>().foo()) n2 = n1;

    static_assert(std::is_same<int, decltype(std::declval<NonDefault>().foo())>(), "std::declval failed.");
    result = n2 == n1 == 1;

    return result;
}

bool _is_pointer()
{
    class A {};

    static_assert(
        ! std::is_pointer<A>::value 
        && ! std::is_pointer<A>()   // same as above, using inherited operator bool
        && ! std::is_pointer<A>{}   // ditto
//        && ! std::is_pointer<A>()() // These versions don't compile
//        && ! std::is_pointer<A>{}() // ditto
        && std::is_pointer<A*>::value
        && ! std::is_pointer<A&>::value
        && ! std::is_pointer<int>::value
        && std::is_pointer<int*>::value
        && std::is_pointer<int**>::value
        && ! std::is_pointer<int[10]>::value
        && ! std::is_pointer<nullptr_t>::value, 
            "std::is_pointer failed."
        );

    return true;
}

bool _remove_pointer()
{
    static_assert(
        std::is_same<int, int>()
        && !std::is_same<int, int*>()
        && !std::is_same<int, int**>()
        && std::is_same<int, std::remove_pointer<int>::type>()
        && std::is_same<int, std::remove_pointer<int*>::type>()
        && !std::is_same<int, std::remove_pointer<int**>::type>()
        && std::is_same<int, std::remove_pointer<int* const>::type>()
        && std::is_same<int, std::remove_pointer<int* volatile>::type>()
        && std::is_same<int, std::remove_pointer<int* const volatile>::type>(),
        "std::remove_pointer failed."
        );

    return true;
}

void setup() 
{
  Serial.begin(9600);
  bool b1 = _is_const(), b2 = _is_reference(), b3 = _is_array(), 
    b4 = _is_function(), b5 = _is_type(), b6 = _is_base_of() , 
    b7 = _extent(), b8 = _alignment_of(), b9 = _rank(), 
    b10 = _is_convertible(), b11 = _is_fundamental(), b12 = _add_lrvalue_reference(), 
    b13 = _is_integral(), b14 = _is_unsigned(), b15 = _is_signed(), 
    b16 = _make_signed(), b17 = _make_unsigned(), b18 = _add_pointer(), 
    b19 = _decay(), b20 = _decay_t(), b21 = _negation(), 
    b22 = _declval(), b23 = _is_pointer(), b24 = _remove_pointer();
  Serial.print("_is_const() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("_is_reference() = "); Serial.println(b2 ? "OK" : "FAIL");
  Serial.print("_is_array() = "); Serial.println(b3 ? "OK" : "FAIL");
  Serial.print("_is_function() = "); Serial.println(b4 ? "OK" : "FAIL");
  Serial.print("_is_type() = "); Serial.println(b5 ? "OK" : "FAIL");
  Serial.print("_is_base_of() = "); Serial.println(b6 ? "OK" : "FAIL");
  Serial.print("_extent() = "); Serial.println(b7 ? "OK" : "FAIL");
  Serial.print("_alignment_of() = "); Serial.println(b8 ? "OK" : "FAIL");
  Serial.print("_rank() = "); Serial.println(b9 ? "OK" : "FAIL");
  Serial.print("_is_convertible() = "); Serial.println(b10 ? "OK" : "FAIL");
  Serial.print("_is_fundamental() = "); Serial.println(b11 ? "OK" : "FAIL");
  Serial.print("_is_integral() = "); Serial.println(b12 ? "OK" : "FAIL");
  Serial.print("_add_lrvalue_reference() = "); Serial.println(b13 ? "OK" : "FAIL");
  Serial.print("_is_unsigned() = "); Serial.println(b14 ? "OK" : "FAIL");
  Serial.print("_is_signed() = "); Serial.println(b15 ? "OK" : "FAIL");
  Serial.print("_make_signed() = "); Serial.println(b16 ? "OK" : "FAIL");
  Serial.print("_make_unsigned() = "); Serial.println(b17 ? "OK" : "FAIL");
  Serial.print("_add_pointer() = "); Serial.println(b18 ? "OK" : "FAIL");
  Serial.print("_decay() = "); Serial.println(b19 ? "OK" : "FAIL");
  Serial.print("_decay_t() = "); Serial.println(b20 ? "OK" : "FAIL");
  Serial.print("_negation() = "); Serial.println(b21 ? "OK" : "FAIL");
  Serial.print("_declval() = "); Serial.println(b22 ? "OK" : "FAIL");
  Serial.print("_is_pointer() = "); Serial.println(b23 ? "OK" : "FAIL");
  Serial.print("_remove_pointer() = "); Serial.println(b24 ? "OK" : "FAIL");
}

void loop() 
{

}
