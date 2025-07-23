#include <ezlibs/ezStackString.hpp>
#include <ezlibs/ezCTest.hpp>
#include <string>

// Desactivation des warnings de conversion
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)  // Conversion from 'double' to 'float', possible loss of data
#pragma warning(disable : 4305)  // Truncation from 'double' to 'float'
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzStackString_Constructor_CopyMove() {
    ez::StackString<char, 8> a("Hello");
    CTEST_ASSERT(strcmp(a.c_str(), "Hello") == 0);
    ez::StackString<char, 8> b(a);  // copy
    CTEST_ASSERT(strcmp(b.c_str(), "Hello") == 0);
    ez::StackString<char, 8> c(std::move(a));  // move
    CTEST_ASSERT(strcmp(c.c_str(), "Hello") == 0);
    return true;
}

bool TestEzStackString_Append_CStr() {
    ez::StackString<char, 8> s("Hi");
    s.append("!");
    CTEST_ASSERT(strcmp(s.c_str(), "Hi!") == 0);
    return true;
}

bool TestEzStackString_Append_Char() {
    ez::StackString<char, 8> s("Hi");
    s.append('!');
    CTEST_ASSERT(strcmp(s.c_str(), "Hi!") == 0);
    return true;
}

bool TestEzStackString_OperatorPlusEqual() {
    ez::StackString<char, 8> s("Hi");
    s += '!';
    s += "!!!";
    CTEST_ASSERT(strcmp(s.c_str(), "Hi!!!!") == 0);
    return true;
}

bool TestEzStackString_HeapSwitch() {
    ez::StackString<char, 4> s("abc");
    CTEST_ASSERT(s.usingHeap() == false);
    s += "def";  // triggers heap
    CTEST_ASSERT(s.usingHeap() == true);
    CTEST_ASSERT(strcmp(s.c_str(), "abcdef") == 0);
    return true;
}

bool TestEzStackString_Clear() {
    ez::StackString<char, 4> s("abc");
    s += "def";
    s.clear();
    CTEST_ASSERT(strcmp(s.c_str(), "") == 0);
    CTEST_ASSERT(s.usingHeap() == false);  // back to stack
    return true;
}

bool TestEzStackString_Find() {
    ez::StackString<char, 16> s("hello world");
    CTEST_ASSERT(s.find("lo") == 3);
    CTEST_ASSERT(s.find("w") == 6);
    CTEST_ASSERT(s.find("x") == s.npos);
    return true;
}

bool TestEzStackString_Substr() {
    ez::StackString<char, 16> s("hello world");
    auto sub = s.substr(6, 5);
    CTEST_ASSERT(strcmp(sub.c_str(), "world") == 0);
    return true;
}

bool TestEzStackString_OperatorAt() {
    ez::StackString<char, 6, 2> s("hello");
    CTEST_ASSERT(s[1] == 'e');
    CTEST_ASSERT(s.at(4) == 'o');
    CTEST_TRY_CATCH(s.at(10));  // index out àof range but in stack
    s += " World"; // one growing
    CTEST_ASSERT(s.usingHeap() == true);
    CTEST_ASSERT(strcmp(s.c_str(), "hello World") == 0);
    CTEST_ASSERT(s.at(9) == 'l');  // index in heap 
    s += " ! Folks !";  // another growing but in already allocated heap
    CTEST_ASSERT(strcmp(s.c_str(), "hello World ! Folks !") == 0);
    CTEST_TRY_CATCH(s.at(99));  // index out àof range but in heap
    return true;
}

bool TestEzStackString_HeapDisabled() {
    ez::StackString<char, 8, 0> s("abc");
    CTEST_ASSERT(s.usingHeap() == false);
    s += "def";  // total = 6 → still OK
    CTEST_ASSERT(strcmp(s.c_str(), "abcdef") == 0);
    CTEST_TRY_CATCH(s += "ghi");  // exceed stack
    return true;
}



////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzStackString(const std::string& vTest) {
    IfTestExist(TestEzStackString_Constructor_CopyMove);
    else IfTestExist(TestEzStackString_Append_CStr);
    else IfTestExist(TestEzStackString_Append_Char);
    else IfTestExist(TestEzStackString_OperatorPlusEqual);
    else IfTestExist(TestEzStackString_HeapSwitch);
    else IfTestExist(TestEzStackString_Clear);
    else IfTestExist(TestEzStackString_Find);
    else IfTestExist(TestEzStackString_Substr);
    else IfTestExist(TestEzStackString_OperatorAt);
    else IfTestExist(TestEzStackString_HeapDisabled);
    return false;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
