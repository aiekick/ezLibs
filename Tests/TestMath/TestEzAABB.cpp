#include <ezlibs/ezAABB.hpp>
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
bool TestEzAABBDefaultConstructor() {
    ez::AABB<T> aabb;
    if (!ez::isEqual(aabb.lowerBound.x, static_cast<T>(0)))
        return false;
    if (!ez::isEqual(aabb.lowerBound.y, static_cast<T>(0)))
        return false;
    if (!ez::isEqual(aabb.upperBound.x, static_cast<T>(0)))
        return false;
    if (!ez::isEqual(aabb.upperBound.y, static_cast<T>(0)))
        return false;
    return true;
}

template <typename T>
bool TestEzAABBVec2Constructor() {
    ez::vec2<T> lower(1, 2);
    ez::vec2<T> upper(5, 6);
    ez::AABB<T> aabb(lower, upper);
    if (!ez::isEqual(aabb.lowerBound.x, static_cast<T>(1)))
        return false;
    if (!ez::isEqual(aabb.lowerBound.y, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(aabb.upperBound.x, static_cast<T>(5)))
        return false;
    if (!ez::isEqual(aabb.upperBound.y, static_cast<T>(6)))
        return false;
    return true;
}

template <typename T>
bool TestEzAABBVec4Constructor() {
    ez::vec4<T> v(1, 2, 5, 6);
    ez::AABB<T> aabb(v);
    if (!ez::isEqual(aabb.lowerBound.x, static_cast<T>(1)))
        return false;
    if (!ez::isEqual(aabb.lowerBound.y, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(aabb.upperBound.x, static_cast<T>(5)))
        return false;
    if (!ez::isEqual(aabb.upperBound.y, static_cast<T>(6)))
        return false;
    return true;
}

template <typename T>
bool TestEzAABBAddVec() {
    ez::vec2<T> lower(1, 2);
    ez::vec2<T> upper(5, 6);
    ez::AABB<T> aabb(lower, upper);
    ez::vec2<T> offset(10, 20);
    aabb += offset;
    if (!ez::isEqual(aabb.lowerBound.x, static_cast<T>(11)))
        return false;
    if (!ez::isEqual(aabb.lowerBound.y, static_cast<T>(22)))
        return false;
    if (!ez::isEqual(aabb.upperBound.x, static_cast<T>(15)))
        return false;
    if (!ez::isEqual(aabb.upperBound.y, static_cast<T>(26)))
        return false;
    return true;
}

template <typename T>
bool TestEzAABBSubVec() {
    ez::vec2<T> lower(10, 20);
    ez::vec2<T> upper(50, 60);
    ez::AABB<T> aabb(lower, upper);
    ez::vec2<T> offset(5, 10);
    aabb -= offset;
    if (!ez::isEqual(aabb.lowerBound.x, static_cast<T>(5)))
        return false;
    if (!ez::isEqual(aabb.lowerBound.y, static_cast<T>(10)))
        return false;
    if (!ez::isEqual(aabb.upperBound.x, static_cast<T>(45)))
        return false;
    if (!ez::isEqual(aabb.upperBound.y, static_cast<T>(50)))
        return false;
    return true;
}

template <typename T>
bool TestEzAABBMulScalar() {
    ez::vec2<T> lower(1, 2);
    ez::vec2<T> upper(5, 6);
    ez::AABB<T> aabb(lower, upper);
    aabb *= 2;
    if (!ez::isEqual(aabb.lowerBound.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(aabb.lowerBound.y, static_cast<T>(4)))
        return false;
    if (!ez::isEqual(aabb.upperBound.x, static_cast<T>(10)))
        return false;
    if (!ez::isEqual(aabb.upperBound.y, static_cast<T>(12)))
        return false;
    return true;
}

template <typename T>
bool TestEzAABBDivScalar() {
    ez::vec2<T> lower(10, 20);
    ez::vec2<T> upper(50, 60);
    ez::AABB<T> aabb(lower, upper);
    aabb /= 2;
    if (!ez::isEqual(aabb.lowerBound.x, static_cast<T>(5)))
        return false;
    if (!ez::isEqual(aabb.lowerBound.y, static_cast<T>(10)))
        return false;
    if (!ez::isEqual(aabb.upperBound.x, static_cast<T>(25)))
        return false;
    if (!ez::isEqual(aabb.upperBound.y, static_cast<T>(30)))
        return false;
    return true;
}

////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzAABB(const std::string& vTest) {
    IfTestExist(TestEzAABBDefaultConstructor<float>);
    else IfTestExist(TestEzAABBDefaultConstructor<double>);

    IfTestExist(TestEzAABBVec2Constructor<float>);
    else IfTestExist(TestEzAABBVec2Constructor<double>);

    IfTestExist(TestEzAABBVec4Constructor<float>);
    else IfTestExist(TestEzAABBVec4Constructor<double>);

    IfTestExist(TestEzAABBAddVec<float>);
    else IfTestExist(TestEzAABBAddVec<double>);

    IfTestExist(TestEzAABBSubVec<float>);
    else IfTestExist(TestEzAABBSubVec<double>);

    IfTestExist(TestEzAABBMulScalar<float>);
    else IfTestExist(TestEzAABBMulScalar<double>);

    IfTestExist(TestEzAABBDivScalar<float>);
    else IfTestExist(TestEzAABBDivScalar<double>);

    return false;
}

////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
