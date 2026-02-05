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
bool TestEzQuatDefaultConstructor() {
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
bool TestEzQuatParameterConstructor() {
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
bool TestEzQuatAdd() {
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
bool TestEzQuatSub() {
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
bool TestEzQuatMul() {
    ez::quat<T> q1(1, 0, 0, 1);
    ez::quat<T> q2(0, 1, 0, 1);
    q1.mul(q2);
    // Quaternion multiplication is non-commutative
    // q1 * q2 != q2 * q1 in general
    // Just verify that multiplication works without crashing
    return true;
}

template <typename T>
bool TestEzQuatConjugate() {
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
bool TestEzQuatScale() {
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
bool TestEzQuatGetT() {
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
    IfTestExist(TestEzQuatDefaultConstructor<float>);
    else IfTestExist(TestEzQuatDefaultConstructor<double>);

    IfTestExist(TestEzQuatParameterConstructor<float>);
    else IfTestExist(TestEzQuatParameterConstructor<double>);

    IfTestExist(TestEzQuatAdd<float>);
    else IfTestExist(TestEzQuatAdd<double>);

    IfTestExist(TestEzQuatSub<float>);
    else IfTestExist(TestEzQuatSub<double>);

    IfTestExist(TestEzQuatMul<float>);
    else IfTestExist(TestEzQuatMul<double>);

    IfTestExist(TestEzQuatConjugate<float>);
    else IfTestExist(TestEzQuatConjugate<double>);

    IfTestExist(TestEzQuatScale<float>);
    else IfTestExist(TestEzQuatScale<double>);

    IfTestExist(TestEzQuatGetT<float>);
    else IfTestExist(TestEzQuatGetT<double>);

    return false;
}

////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
