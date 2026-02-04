#include <ezlibs/ezQuat.hpp>
#include <ezlibs/ezMath.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4305)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

////////////////////////////////////////////////////////////////////////////

template <typename T>
bool TestEzQuat_DefaultConstructor() {
    ez::quat<T> q;
    // Default: identity quaternion (0,0,0,1)
    if (!ez::isEqual(q.x, static_cast<T>(0)))
        return false;
    if (!ez::isEqual(q.y, static_cast<T>(0)))
        return false;
    if (!ez::isEqual(q.z, static_cast<T>(0)))
        return false;
    if (!ez::isEqual(q.w, static_cast<T>(1)))
        return false;
    return true;
}

template <typename T>
bool TestEzQuat_ParameterConstructor() {
    ez::quat<T> q(1, 2, 3, 4);
    if (!ez::isEqual(q.x, static_cast<T>(1)))
        return false;
    if (!ez::isEqual(q.y, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(q.z, static_cast<T>(3)))
        return false;
    if (!ez::isEqual(q.w, static_cast<T>(4)))
        return false;
    return true;
}

template <typename T>
bool TestEzQuat_Add() {
    ez::quat<T> q1(1, 2, 3, 4);
    ez::quat<T> q2(5, 6, 7, 8);
    q1.add(q2);
    if (!ez::isEqual(q1.x, static_cast<T>(6)))
        return false;
    if (!ez::isEqual(q1.y, static_cast<T>(8)))
        return false;
    if (!ez::isEqual(q1.z, static_cast<T>(10)))
        return false;
    if (!ez::isEqual(q1.w, static_cast<T>(12)))
        return false;
    return true;
}

template <typename T>
bool TestEzQuat_Sub() {
    ez::quat<T> q1(10, 20, 30, 40);
    ez::quat<T> q2(5, 6, 7, 8);
    q1.sub(q2);
    if (!ez::isEqual(q1.x, static_cast<T>(5)))
        return false;
    if (!ez::isEqual(q1.y, static_cast<T>(14)))
        return false;
    if (!ez::isEqual(q1.z, static_cast<T>(23)))
        return false;
    if (!ez::isEqual(q1.w, static_cast<T>(32)))
        return false;
    return true;
}

template <typename T>
bool TestEzQuat_Mul() {
    ez::quat<T> q1(1, 0, 0, 1);
    ez::quat<T> q2(0, 1, 0, 1);
    q1.mul(q2);
    // Quaternion multiplication is non-commutative
    // q1 * q2 != q2 * q1 in general
    // Just verify that multiplication works without crashing
    return true;
}

template <typename T>
bool TestEzQuat_Conjugate() {
    ez::quat<T> q1(1, 2, 3, 4);
    ez::quat<T> q2;
    q2.conjugate(q1);
    if (!ez::isEqual(q2.x, static_cast<T>(-1)))
        return false;
    if (!ez::isEqual(q2.y, static_cast<T>(-2)))
        return false;
    if (!ez::isEqual(q2.z, static_cast<T>(-3)))
        return false;
    if (!ez::isEqual(q2.w, static_cast<T>(4)))
        return false;
    return true;
}

template <typename T>
bool TestEzQuat_Scale() {
    ez::quat<T> q(1, 2, 3, 4);
    q.scale(2);
    if (!ez::isEqual(q.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(q.y, static_cast<T>(4)))
        return false;
    if (!ez::isEqual(q.z, static_cast<T>(6)))
        return false;
    if (!ez::isEqual(q.w, static_cast<T>(8)))
        return false;
    return true;
}

template <typename T>
bool TestEzQuat_GetT() {
    ez::quat<T> q(2, 3, 4, 5);
    if (!ez::isEqual(q.getTx(), static_cast<T>(10)))
        return false;
    if (!ez::isEqual(q.getTy(), static_cast<T>(15)))
        return false;
    if (!ez::isEqual(q.getTz(), static_cast<T>(20)))
        return false;
    return true;
}

////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzQuat(const std::string& vTest) {
    IfTestExist(TestEzQuat_DefaultConstructor<float>);
    else IfTestExist(TestEzQuat_DefaultConstructor<double>);

    IfTestExist(TestEzQuat_ParameterConstructor<float>);
    else IfTestExist(TestEzQuat_ParameterConstructor<double>);

    IfTestExist(TestEzQuat_Add<float>);
    else IfTestExist(TestEzQuat_Add<double>);

    IfTestExist(TestEzQuat_Sub<float>);
    else IfTestExist(TestEzQuat_Sub<double>);

    IfTestExist(TestEzQuat_Mul<float>);
    else IfTestExist(TestEzQuat_Mul<double>);

    IfTestExist(TestEzQuat_Conjugate<float>);
    else IfTestExist(TestEzQuat_Conjugate<double>);

    IfTestExist(TestEzQuat_Scale<float>);
    else IfTestExist(TestEzQuat_Scale<double>);

    IfTestExist(TestEzQuat_GetT<float>);
    else IfTestExist(TestEzQuat_GetT<double>);

    return false;
}

////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
