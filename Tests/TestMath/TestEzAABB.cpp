#include <ezlibs/ezMath.hpp>
#include <ezlibs/ezAABB.hpp>

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
bool TestEzAABB_DefaultConstructor() {
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
bool TestEzAABB_Vec2Constructor() {
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
bool TestEzAABB_Vec4Constructor() {
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
bool TestEzAABB_AddVec() {
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
bool TestEzAABB_SubVec() {
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
bool TestEzAABB_MulScalar() {
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
bool TestEzAABB_DivScalar() {
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
    IfTestExist(TestEzAABB_DefaultConstructor<float>);
    else IfTestExist(TestEzAABB_DefaultConstructor<double>);

    IfTestExist(TestEzAABB_Vec2Constructor<float>);
    else IfTestExist(TestEzAABB_Vec2Constructor<double>);

    IfTestExist(TestEzAABB_Vec4Constructor<float>);
    else IfTestExist(TestEzAABB_Vec4Constructor<double>);

    IfTestExist(TestEzAABB_AddVec<float>);
    else IfTestExist(TestEzAABB_AddVec<double>);

    IfTestExist(TestEzAABB_SubVec<float>);
    else IfTestExist(TestEzAABB_SubVec<double>);

    IfTestExist(TestEzAABB_MulScalar<float>);
    else IfTestExist(TestEzAABB_MulScalar<double>);

    IfTestExist(TestEzAABB_DivScalar<float>);
    else IfTestExist(TestEzAABB_DivScalar<double>);

    return false;
}

////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
