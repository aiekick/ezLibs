#include <ezlibs/ezMat2.hpp>
#include <ezlibs/ezMath.hpp>
#include <cmath>

// Disable noisy conversion warnings
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
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

template <typename T>
bool TestEzMat2_DefaultConstructor() {
    ez::mat2<T> m;
    // Default constructor creates zero matrix
    if (!ez::isEqual(m(0, 0), static_cast<T>(0)))
        return false;
    if (!ez::isEqual(m(0, 1), static_cast<T>(0)))
        return false;
    if (!ez::isEqual(m(1, 0), static_cast<T>(0)))
        return false;
    if (!ez::isEqual(m(1, 1), static_cast<T>(0)))
        return false;
    return true;
}

template <typename T>
bool TestEzMat2_DiagonalConstructor() {
    ez::mat2<T> m(static_cast<T>(2));
    // Diagonal constructor
    if (!ez::isEqual(m(0, 0), static_cast<T>(2)))
        return false;
    if (!ez::isEqual(m(0, 1), static_cast<T>(0)))
        return false;
    if (!ez::isEqual(m(1, 0), static_cast<T>(0)))
        return false;
    if (!ez::isEqual(m(1, 1), static_cast<T>(2)))
        return false;
    return true;
}

template <typename T>
bool TestEzMat2_ElementConstructor() {
    // Column-major: c0r0, c0r1, c1r0, c1r1
    ez::mat2<T> m(static_cast<T>(1), static_cast<T>(2), static_cast<T>(3), static_cast<T>(4));
    if (!ez::isEqual(m(0, 0), static_cast<T>(1)))
        return false;
    if (!ez::isEqual(m(1, 0), static_cast<T>(2)))
        return false;
    if (!ez::isEqual(m(0, 1), static_cast<T>(3)))
        return false;
    if (!ez::isEqual(m(1, 1), static_cast<T>(4)))
        return false;
    return true;
}

template <typename T>
bool TestEzMat2_Identity() {
    ez::mat2<T> m = ez::mat2<T>::Identity();
    if (!ez::isEqual(m(0, 0), static_cast<T>(1)))
        return false;
    if (!ez::isEqual(m(0, 1), static_cast<T>(0)))
        return false;
    if (!ez::isEqual(m(1, 0), static_cast<T>(0)))
        return false;
    if (!ez::isEqual(m(1, 1), static_cast<T>(1)))
        return false;
    return true;
}

template <typename T>
bool TestEzMat2_Zero() {
    ez::mat2<T> m = ez::mat2<T>::Zero();
    if (!ez::isEqual(m(0, 0), static_cast<T>(0)))
        return false;
    if (!ez::isEqual(m(0, 1), static_cast<T>(0)))
        return false;
    if (!ez::isEqual(m(1, 0), static_cast<T>(0)))
        return false;
    if (!ez::isEqual(m(1, 1), static_cast<T>(0)))
        return false;
    return true;
}

template <typename T>
bool TestEzMat2_Multiplication() {
    // Test matrix multiplication
    ez::mat2<T> m1(static_cast<T>(1), static_cast<T>(2), static_cast<T>(3), static_cast<T>(4));
    ez::mat2<T> m2(static_cast<T>(2), static_cast<T>(0), static_cast<T>(1), static_cast<T>(2));

    ez::mat2<T> result = m1 * m2;

    // Expected: [[1,3],[2,4]] * [[2,1],[0,2]] = [[2,7],[4,10]]
    if (!ez::isEqual(result(0, 0), static_cast<T>(2)))
        return false;
    if (!ez::isEqual(result(0, 1), static_cast<T>(7)))
        return false;
    if (!ez::isEqual(result(1, 0), static_cast<T>(4)))
        return false;
    if (!ez::isEqual(result(1, 1), static_cast<T>(10)))
        return false;

    return true;
}

template <typename T>
bool TestEzMat2_MulVec() {
    // Test vector multiplication
    ez::mat2<T> m(static_cast<T>(1), static_cast<T>(2), static_cast<T>(3), static_cast<T>(4));
    std::array<T, 2> v = {static_cast<T>(2), static_cast<T>(3)};

    auto result = m.mulVec(v);

    // [[1,3],[2,4]] * [2,3] = [1*2 + 3*3, 2*2 + 4*3] = [11, 16]
    if (!ez::isEqual(result[0], static_cast<T>(11)))
        return false;
    if (!ez::isEqual(result[1], static_cast<T>(16)))
        return false;

    return true;
}

template <typename T>
bool TestEzMat2_Transpose() {
    ez::mat2<T> m(static_cast<T>(1), static_cast<T>(2), static_cast<T>(3), static_cast<T>(4));
    ez::mat2<T> t = m.transpose();

    // [[1,3],[2,4]] transposed = [[1,2],[3,4]]
    if (!ez::isEqual(t(0, 0), static_cast<T>(1)))
        return false;
    if (!ez::isEqual(t(0, 1), static_cast<T>(2)))
        return false;
    if (!ez::isEqual(t(1, 0), static_cast<T>(3)))
        return false;
    if (!ez::isEqual(t(1, 1), static_cast<T>(4)))
        return false;

    return true;
}

template <typename T>
bool TestEzMat2_Determinant() {
    ez::mat2<T> m(static_cast<T>(1), static_cast<T>(2), static_cast<T>(3), static_cast<T>(4));

    // det([[1,3],[2,4]]) = 1*4 - 3*2 = 4 - 6 = -2
    T det = m.determinant();
    if (!ez::isEqual(det, static_cast<T>(-2)))
        return false;

    return true;
}

template <typename T>
bool TestEzMat2_Inverse() {
    ez::mat2<T> m(static_cast<T>(4), static_cast<T>(7), static_cast<T>(2), static_cast<T>(6));

    // det([[4,2],[7,6]]) = 4*6 - 2*7 = 24 - 14 = 10
    // inv = (1/10) * [[6,-2],[-7,4]]
    ez::mat2<T> inv = m.inverse();

    if (ez::isDifferent(inv(0, 0), static_cast<T>(0.6)))
        return false;
    if (ez::isDifferent(inv(0, 1), static_cast<T>(-0.2)))
        return false;
    if (ez::isDifferent(inv(1, 0), static_cast<T>(-0.7)))
        return false;
    if (ez::isDifferent(inv(1, 1), static_cast<T>(0.4)))
        return false;

    // Verify M * M^-1 = I
    ez::mat2<T> identity = m * inv;
    // Use a more lenient epsilon for accumulated floating-point errors in matrix multiplication
    T tolerance = static_cast<T>(1e-5);
    if (ez::isDifferent(identity(0, 0), static_cast<T>(1), tolerance))
        return false;
    if (ez::isDifferent(identity(1, 1), static_cast<T>(1), tolerance))
        return false;
    if (ez::isDifferent(identity(0, 1), static_cast<T>(0), tolerance))
        return false;
    if (ez::isDifferent(identity(1, 0), static_cast<T>(0), tolerance))
        return false;

    return true;
}

template <typename T>
bool TestEzMat2_InverseSingular() {
    // Singular matrix (determinant = 0)
    ez::mat2<T> m(static_cast<T>(1), static_cast<T>(2), static_cast<T>(2), static_cast<T>(4));

    // det([[1,2],[2,4]]) = 1*4 - 2*2 = 0
    T det = m.determinant();
    if (!ez::isEqual(det, static_cast<T>(0)))
        return false;

    // Inverse should return zero matrix
    ez::mat2<T> inv = m.inverse();
    if (!ez::isEqual(inv(0, 0), static_cast<T>(0)))
        return false;
    if (!ez::isEqual(inv(0, 1), static_cast<T>(0)))
        return false;
    if (!ez::isEqual(inv(1, 0), static_cast<T>(0)))
        return false;
    if (!ez::isEqual(inv(1, 1), static_cast<T>(0)))
        return false;

    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzMat2(const std::string& vTest) {
    IfTestExist(TestEzMat2_DefaultConstructor<float>);
    else IfTestExist(TestEzMat2_DefaultConstructor<double>);

    IfTestExist(TestEzMat2_DiagonalConstructor<float>);
    else IfTestExist(TestEzMat2_DiagonalConstructor<double>);

    IfTestExist(TestEzMat2_ElementConstructor<float>);
    else IfTestExist(TestEzMat2_ElementConstructor<double>);

    IfTestExist(TestEzMat2_Identity<float>);
    else IfTestExist(TestEzMat2_Identity<double>);

    IfTestExist(TestEzMat2_Zero<float>);
    else IfTestExist(TestEzMat2_Zero<double>);

    IfTestExist(TestEzMat2_Multiplication<float>);
    else IfTestExist(TestEzMat2_Multiplication<double>);

    IfTestExist(TestEzMat2_MulVec<float>);
    else IfTestExist(TestEzMat2_MulVec<double>);

    IfTestExist(TestEzMat2_Transpose<float>);
    else IfTestExist(TestEzMat2_Transpose<double>);

    IfTestExist(TestEzMat2_Determinant<float>);
    else IfTestExist(TestEzMat2_Determinant<double>);

    IfTestExist(TestEzMat2_Inverse<float>);
    else IfTestExist(TestEzMat2_Inverse<double>);

    IfTestExist(TestEzMat2_InverseSingular<float>);
    else IfTestExist(TestEzMat2_InverseSingular<double>);

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
