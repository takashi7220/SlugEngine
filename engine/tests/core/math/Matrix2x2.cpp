// tests/test_matrix2x2.cpp
#include <gtest/gtest.h>
#include <type_traits>
#include "math/Math.hpp"
#include "math/Vector2.hpp"
#include "math/Matrix2x2.hpp" // このヘッダ名は実際の配置に合わせて

using slug::math::Matrix2x2;
using slug::math::Vector2;
using slug::math::ValueType;

// 近似比較ヘルパ
template <class T>
inline void ExpectNearT(const T& a, const T& b, double eps = 1e-9)
{
    if constexpr (std::is_floating_point_v<T>)
    {
        EXPECT_NEAR(static_cast<double>(a), static_cast<double>(b), eps);
    } else
    {
        EXPECT_EQ(a, b);
    }
}

static void ExpectMatrixNear(const Matrix2x2& A, const Matrix2x2& B, double eps = 1e-9)
{
    for (int i = 0; i < 4; ++i)
    {
        ExpectNearT<ValueType>(A.m[i], B.m[i], eps);
    }
}

TEST(Matrix2x2, DefaultIsZero)
{
    Matrix2x2 m;
    for (int i = 0; i < 4; ++i)
    {
        ExpectNearT<ValueType>(m.m[i], ValueType(0));
    }
    EXPECT_TRUE(m == Matrix2x2::Zero());
}

TEST(Matrix2x2, RowMajorInitializerPacksToColumnMajor)
{
    // [ a b ]
    // [ c d ]  （行ごとに渡す）
    const ValueType a = 1, b = 2, c = 3, d = 4;
    Matrix2x2 m({ {a, b}, {c, d} });

    // 列優先：m = { a, c, b, d }
    ExpectNearT<ValueType>(m.m[0], a);
    ExpectNearT<ValueType>(m.m[1], c);
    ExpectNearT<ValueType>(m.m[2], b);
    ExpectNearT<ValueType>(m.m[3], d);

    // at(row,col) の動作
    ExpectNearT<ValueType>(m.at(0, 0), a);
    ExpectNearT<ValueType>(m.at(0, 1), b);
    ExpectNearT<ValueType>(m.at(1, 0), c);
    ExpectNearT<ValueType>(m.at(1, 1), d);
}

TEST(Matrix2x2, PartialInitializerFillsZeros)
{
    Matrix2x2 m({ { ValueType(5), ValueType(6) } }); // 2 行目省略 → 0 埋め
    // 期待: [5 6; 0 0] （列優先だと {5,0,6,0}）
    ExpectNearT<ValueType>(m.m[0], ValueType(5));
    ExpectNearT<ValueType>(m.m[1], ValueType(0));
    ExpectNearT<ValueType>(m.m[2], ValueType(6));
    ExpectNearT<ValueType>(m.m[3], ValueType(0));
}

TEST(Matrix2x2, IdentityAndZero)
{
    auto I = Matrix2x2::Identity();
    auto Z = Matrix2x2::Zero();

    // I * I = I, I * Z = Z, Z * I = Z
    ExpectMatrixNear(I * I, I);
    ExpectMatrixNear(I * Z, Z);
    ExpectMatrixNear(Z * I, Z);
}

TEST(Matrix2x2, Transpose)
{
    Matrix2x2 m({ {1, 2}, {3, 4} }); // [1 2; 3 4]
    Matrix2x2 mt = m.Transpose();    // [1 3; 2 4]

    // 列優先で {1,3,2,4}
    ExpectNearT<ValueType>(mt.m[0], ValueType(1));
    ExpectNearT<ValueType>(mt.m[1], ValueType(2));
    ExpectNearT<ValueType>(mt.m[2], ValueType(3));
    ExpectNearT<ValueType>(mt.m[3], ValueType(4));
}

TEST(Matrix2x2, InverseRegular)
{
    // 逆行列が存在する行列
    // A = [4 7; 2 6], A^{-1} = (1/10) * [6 -7; -2 4]
    Matrix2x2 A({ {4, 7}, {2, 6} });
    Matrix2x2 Ainv = A.Inverse();

    Matrix2x2 expected({
        { ValueType(6) / ValueType(10), ValueType(-7) / ValueType(10) },
        { ValueType(-2) / ValueType(10), ValueType(4) / ValueType(10) }
    });

    ExpectMatrixNear(Ainv, expected, 1e-9);

    // A * A^{-1} ≒ I
    ExpectMatrixNear(A * Ainv, Matrix2x2::Identity(), 1e-6);
    ExpectMatrixNear(Ainv * A, Matrix2x2::Identity(), 1e-6);
}

TEST(Matrix2x2, InverseSingularReturnsZero)
{
    // det = 0 の場合は Zero()
    Matrix2x2 S({ {1, 2}, {2, 4} }); // 2 行目が 1 行目の 2 倍
    auto Sinv = S.Inverse();
    EXPECT_TRUE(Sinv == Matrix2x2::Zero());
}

TEST(Matrix2x2, EqualityAndCopy)
{
    Matrix2x2 a({ {1,2}, {3,4} });
    Matrix2x2 b(a);
    EXPECT_TRUE(a == b);

    Matrix2x2 c({ {1,2}, {3,5} });
    EXPECT_FALSE(a == c);
}

TEST(Matrix2x2, MatrixAdditionSubtraction)
{
    Matrix2x2 a({ {1,2}, {3,4} });
    Matrix2x2 b({ {5,6}, {7,8} });

    Matrix2x2 add = a + b; // [6 8; 10 12]
    Matrix2x2 sub = b - a; // [4 4; 4 4]

    ExpectMatrixNear(add, Matrix2x2({ {6,8}, {10,12} }));
    ExpectMatrixNear(sub, Matrix2x2({ {4,4}, {4,4} }));
}

TEST(Matrix2x2, ScalarOps)
{
    Matrix2x2 a({ {1,2}, {3,4} });

    ExpectMatrixNear(a + ValueType(2), Matrix2x2({ {3,4}, {5,6} }));
    ExpectMatrixNear(a - ValueType(1), Matrix2x2({ {0,1}, {2,3} }));
    ExpectMatrixNear(a * ValueType(3), Matrix2x2({ {3,6}, {9,12} }));
    ExpectMatrixNear(a / ValueType(2), Matrix2x2({ {ValueType(0.5), ValueType(1)}, {ValueType(1.5), ValueType(2)} }), 1e-9);
}

TEST(Matrix2x2, CompoundAssignmentOps)
{
    Matrix2x2 a({ {1,2}, {3,4} });
    Matrix2x2 b({ {5,6}, {7,8} });

    Matrix2x2 t = a; t += b;
    ExpectMatrixNear(t, Matrix2x2({ {6,8}, {10,12} }));

    t = a; t -= b;
    ExpectMatrixNear(t, Matrix2x2({ {-4,-4}, {-4,-4} }));

    t = a; t *= b; // 行列積
    // a*b = [1 2; 3 4] * [5 6; 7 8] = [19 22; 43 50]
    ExpectMatrixNear(t, Matrix2x2({ {19,22}, {43,50} }));

    t = a; t += ValueType(10);
    ExpectMatrixNear(t, Matrix2x2({ {11,12}, {13,14} }));

    t = a; t -= ValueType(1);
    ExpectMatrixNear(t, Matrix2x2({ {0,1}, {2,3} }));

    t = a; t *= ValueType(2);
    ExpectMatrixNear(t, Matrix2x2({ {2,4}, {6,8} }));

    t = a; t /= ValueType(2);
    ExpectMatrixNear(t, Matrix2x2({ {ValueType(0.5), ValueType(1)}, {ValueType(1.5), ValueType(2)} }), 1e-9);
}

TEST(Matrix2x2, MatrixMultiplyColumnMajor)
{
    // 列優先の実装を検証（上の積テストと同値だが、列ループの網羅性も見る）
    Matrix2x2 A({ {2, 1}, {0, 3} });  // [2 1; 0 3]
    Matrix2x2 B({ {4, 5}, {6, 7} });  // [4 5; 6 7]
    // A*B = [2*4+1*6  2*5+1*7; 0*4+3*6  0*5+3*7] = [14 17; 18 21]
    Matrix2x2 C = A * B;
    ExpectMatrixNear(C, Matrix2x2({ {14,17}, {18,21} }));
}

TEST(Matrix2x2, VectorMultiplyTreatsVectorAsColumn)
{
    Matrix2x2 M({ {1,2}, {3,4} }); // [1 2; 3 4]
    Vector2 v { ValueType(5), ValueType(6) }; // 列ベクトル [5;6]
    // M*v = [1*5 + 2*6; 3*5 + 4*6] = [17; 39]
    Vector2 r = M * v;
    ExpectNearT<ValueType>(r.x, ValueType(17));
    ExpectNearT<ValueType>(r.y, ValueType(39));
}

TEST(Matrix2x2, AtAccessorMutatesCorrectElement)
{
    Matrix2x2 m = Matrix2x2::Zero();
    m.at(0, 1) = ValueType(42); // (row=0,col=1) → m[2]
    ExpectNearT<ValueType>(m.m[2], ValueType(42));
    ExpectNearT<ValueType>(m.at(0, 1), ValueType(42));
}
