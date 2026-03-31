// tests/test_matrix3x3.cpp
#include <gtest/gtest.h>
#include <type_traits>
#include "math/MatrixBase.hpp"
#include "math/Vector3.hpp"
#include "math/Matrix3x3.hpp"  // 実際のパスに合わせて調整

using slug::math::Matrix3x3;
using slug::math::Vector3;
using slug::math::ValueType;

// ValueType の整数/浮動小数を吸収して比較
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

static void ExpectMatNear(const Matrix3x3& A, const Matrix3x3& B, double eps = 1e-9)
{
    for (int i = 0; i < 9; ++i)
    {
        ExpectNearT<ValueType>(A.m[i], B.m[i], eps);
    }
}

TEST(Matrix3x3, DefaultIsZero)
{
    Matrix3x3 m;
    for (int i = 0; i < 9; ++i)
    {
        ExpectNearT<ValueType>(m.m[i], ValueType(0));
    }
    EXPECT_TRUE(m == Matrix3x3::Zero());
}

TEST(Matrix3x3, RowMajorInitializerPacksToColumnMajor)
{
    // 行で渡す：
    // [ 1 2 3
    //   4 5 6
    //   7 8 9 ]
    Matrix3x3 m({
        {ValueType(1), ValueType(2), ValueType(3)},
        {ValueType(4), ValueType(5), ValueType(6)},
        {ValueType(7), ValueType(8), ValueType(9)}
    });

    // 列優先: m = {1,4,7, 2,5,8, 3,6,9}
    ExpectNearT<ValueType>(m.m[0], ValueType(1));
    ExpectNearT<ValueType>(m.m[1], ValueType(4));
    ExpectNearT<ValueType>(m.m[2], ValueType(7));
    ExpectNearT<ValueType>(m.m[3], ValueType(2));
    ExpectNearT<ValueType>(m.m[4], ValueType(5));
    ExpectNearT<ValueType>(m.m[5], ValueType(8));
    ExpectNearT<ValueType>(m.m[6], ValueType(3));
    ExpectNearT<ValueType>(m.m[7], ValueType(6));
    ExpectNearT<ValueType>(m.m[8], ValueType(9));

    // at(i,j) = 要素(i行, j列)
    ExpectNearT<ValueType>(m.at(0, 0), ValueType(1));
    ExpectNearT<ValueType>(m.at(0, 1), ValueType(2));
    ExpectNearT<ValueType>(m.at(0, 2), ValueType(3));
    ExpectNearT<ValueType>(m.at(1, 0), ValueType(4));
    ExpectNearT<ValueType>(m.at(1, 1), ValueType(5));
    ExpectNearT<ValueType>(m.at(1, 2), ValueType(6));
    ExpectNearT<ValueType>(m.at(2, 0), ValueType(7));
    ExpectNearT<ValueType>(m.at(2, 1), ValueType(8));
    ExpectNearT<ValueType>(m.at(2, 2), ValueType(9));
}

TEST(Matrix3x3, PartialInitializerFillsZeros)
{
    // 2 行のみ渡す → 残りは 0 埋め
    Matrix3x3 m({
        {ValueType(1), ValueType(2), ValueType(3)},
        {ValueType(4), ValueType(5), ValueType(6)}
    });
    // 行列：
    // [1 2 3
    //  4 5 6
    //  0 0 0] → 列優先 {1,4,0, 2,5,0, 3,6,0}
    ExpectNearT<ValueType>(m.m[0], ValueType(1));
    ExpectNearT<ValueType>(m.m[1], ValueType(4));
    ExpectNearT<ValueType>(m.m[2], ValueType(0));
    ExpectNearT<ValueType>(m.m[3], ValueType(2));
    ExpectNearT<ValueType>(m.m[4], ValueType(5));
    ExpectNearT<ValueType>(m.m[5], ValueType(0));
    ExpectNearT<ValueType>(m.m[6], ValueType(3));
    ExpectNearT<ValueType>(m.m[7], ValueType(6));
    ExpectNearT<ValueType>(m.m[8], ValueType(0));
}

TEST(Matrix3x3, IdentityAndZeroBasicAlgebra)
{
    Matrix3x3 I = Matrix3x3::Identity();
    Matrix3x3 Z = Matrix3x3::Zero();

    ExpectMatNear(I * I, I);
    ExpectMatNear(I * Z, Z);
    ExpectMatNear(Z * I, Z);

    // A*I = I*A = A
    Matrix3x3 A({
        {ValueType(1), ValueType(2), ValueType(3)},
        {ValueType(0), ValueType(1), ValueType(4)},
        {ValueType(5), ValueType(6), ValueType(0)}
    });
    ExpectMatNear(A * I, A);
    ExpectMatNear(I * A, A);
}

TEST(Matrix3x3, Transpose)
{
    Matrix3x3 A({
        {ValueType(1), ValueType(2), ValueType(3)},
        {ValueType(4), ValueType(5), ValueType(6)},
        {ValueType(7), ValueType(8), ValueType(9)}
    });
    Matrix3x3 AT = A.Transpose();

    // A^T は [1 4 7; 2 5 8; 3 6 9]
    ExpectMatNear(AT, Matrix3x3({
        {ValueType(1), ValueType(4), ValueType(7)},
        {ValueType(2), ValueType(5), ValueType(8)},
        {ValueType(3), ValueType(6), ValueType(9)}
                                }));
}

TEST(Matrix3x3, InverseRegular)
{
    // よく使われる例: 
    // A = [1 2 3; 0 1 4; 5 6 0]
    // A^{-1} = [-24  18   5; 20 -15 -4; -5   4   1]
    Matrix3x3 A({
        {ValueType(1), ValueType(2), ValueType(3)},
        {ValueType(0), ValueType(1), ValueType(4)},
        {ValueType(5), ValueType(6), ValueType(0)}
    });
    Matrix3x3 Ainv = A.Inverse();

    Matrix3x3 expected({
        {ValueType(-24), ValueType(18),  ValueType(5)},
        {ValueType(20), ValueType(-15), ValueType(-4)},
        {ValueType(-5), ValueType(4),   ValueType(1)}
    });

    ExpectMatNear(Ainv, expected, 1e-9);
    // A * A^{-1} = I（数値誤差は Near で許容）
    ExpectMatNear(A * Ainv, Matrix3x3::Identity(), 1e-9);
    ExpectMatNear(Ainv * A, Matrix3x3::Identity(), 1e-9);
}

TEST(Matrix3x3, InverseSingularReturnsZero)
{
    // 行が一次従属：3 行目 = 1 行目 + 2 行目
    Matrix3x3 S({
        {ValueType(1), ValueType(2), ValueType(3)},
        {ValueType(4), ValueType(5), ValueType(6)},
        {ValueType(5), ValueType(7), ValueType(9)}
    });
    auto Sinv = S.Inverse();
    EXPECT_TRUE(Sinv == Matrix3x3::Zero());
}

TEST(Matrix3x3, EqualityAndCopy)
{
    Matrix3x3 A({
        {ValueType(1), ValueType(2), ValueType(3)},
        {ValueType(4), ValueType(5), ValueType(6)},
        {ValueType(7), ValueType(8), ValueType(9)}
    });
    Matrix3x3 B(A);
    EXPECT_TRUE(A == B);

    Matrix3x3 C({
        {ValueType(1), ValueType(2), ValueType(3)},
        {ValueType(4), ValueType(5), ValueType(6)},
        {ValueType(7), ValueType(8), ValueType(10)}
    });
    EXPECT_FALSE(A == C);
}

TEST(Matrix3x3, MatrixAdditionSubtraction)
{
    Matrix3x3 A({
        {ValueType(1), ValueType(2), ValueType(3)},
        {ValueType(4), ValueType(5), ValueType(6)},
        {ValueType(7), ValueType(8), ValueType(9)}
    });
    Matrix3x3 B({
        {ValueType(9), ValueType(8), ValueType(7)},
        {ValueType(6), ValueType(5), ValueType(4)},
        {ValueType(3), ValueType(2), ValueType(1)}
    });

    Matrix3x3 add = A + B;
    Matrix3x3 sub = A - B;

    ExpectMatNear(add, Matrix3x3({
        {ValueType(10), ValueType(10), ValueType(10)},
        {ValueType(10), ValueType(10), ValueType(10)},
        {ValueType(10), ValueType(10), ValueType(10)}
                                 }));
    ExpectMatNear(sub, Matrix3x3({
        {ValueType(-8), ValueType(-6), ValueType(-4)},
        {ValueType(-2), ValueType(0), ValueType(2)},
        {ValueType(4), ValueType(6), ValueType(8)}
                                 }));
}

TEST(Matrix3x3, ScalarOps)
{
    Matrix3x3 A({
        {ValueType(1), ValueType(2), ValueType(3)},
        {ValueType(4), ValueType(5), ValueType(6)},
        {ValueType(7), ValueType(8), ValueType(9)}
    });

    ExpectMatNear(A + ValueType(1), Matrix3x3({
        {ValueType(2), ValueType(3), ValueType(4)},
        {ValueType(5), ValueType(6), ValueType(7)},
        {ValueType(8), ValueType(9), ValueType(10)}
                                              }));

    ExpectMatNear(A - ValueType(1), Matrix3x3({
        {ValueType(0), ValueType(1), ValueType(2)},
        {ValueType(3), ValueType(4), ValueType(5)},
        {ValueType(6), ValueType(7), ValueType(8)}
                                              }));

    ExpectMatNear(A * ValueType(2), Matrix3x3({
        {ValueType(2), ValueType(4),  ValueType(6)},
        {ValueType(8), ValueType(10), ValueType(12)},
        {ValueType(14),ValueType(16), ValueType(18)}
                                              }));

    ExpectMatNear(A / ValueType(2), Matrix3x3({
        {ValueType(0.5), ValueType(1.0), ValueType(1.5)},
        {ValueType(2.0), ValueType(2.5), ValueType(3.0)},
        {ValueType(3.5), ValueType(4.0), ValueType(4.5)}
                                              }), 1e-9);
}

TEST(Matrix3x3, CompoundAssignmentOps)
{
    Matrix3x3 A({
        {ValueType(1), ValueType(2), ValueType(3)},
        {ValueType(0), ValueType(1), ValueType(4)},
        {ValueType(5), ValueType(6), ValueType(0)}
    });
    Matrix3x3 B({
        {ValueType(2), ValueType(0), ValueType(1)},
        {ValueType(3), ValueType(0), ValueType(0)},
        {ValueType(4), ValueType(1), ValueType(0)}
    });

    Matrix3x3 t = A; t += B;
    ExpectMatNear(t, Matrix3x3({
        {ValueType(3), ValueType(2), ValueType(4)},
        {ValueType(3), ValueType(1), ValueType(4)},
        {ValueType(9), ValueType(7), ValueType(0)}
                               }));

    t = A; t -= B;
    ExpectMatNear(t, Matrix3x3({
        {ValueType(-1), ValueType(2), ValueType(2)},
        {ValueType(-3), ValueType(1), ValueType(4)},
        {ValueType(1),  ValueType(5), ValueType(0)}
                               }));

    // 行列積（列優先実装の検証も兼ねる）
    // 手計算しておくと：
    // A*B =
    // [ 1*2+2*3+3*4  , 1*0+2*0+3*1 , 1*1+2*0+3*0 ] = [20, 3, 1]
    // [ 0*2+1*3+4*4  , 0*0+1*0+4*1 , 0*1+1*0+4*0 ] = [19, 4, 0]
    // [ 5*2+6*3+0*4  , 5*0+6*0+0*1 , 5*1+6*0+0*0 ] = [28, 0, 5]
    t = A; t *= B;
    ExpectMatNear(t, Matrix3x3({
        {ValueType(20), ValueType(3), ValueType(1)},
        {ValueType(19), ValueType(4), ValueType(0)},
        {ValueType(28), ValueType(0), ValueType(5)}
                               }));

    t = A; t += ValueType(10);
    ExpectMatNear(t, Matrix3x3({
        {ValueType(11), ValueType(12), ValueType(13)},
        {ValueType(10), ValueType(11), ValueType(14)},
        {ValueType(15), ValueType(16), ValueType(10)}
                               }));

    t = A; t -= ValueType(1);
    ExpectMatNear(t, Matrix3x3({
        {ValueType(0), ValueType(1), ValueType(2)},
        {ValueType(-1),ValueType(0), ValueType(3)},
        {ValueType(4), ValueType(5), ValueType(-1)}
                               }));

    t = A; t *= ValueType(2);
    ExpectMatNear(t, Matrix3x3({
        {ValueType(2), ValueType(4), ValueType(6)},
        {ValueType(0), ValueType(2), ValueType(8)},
        {ValueType(10),ValueType(12),ValueType(0)}
                               }));

    t = A; t /= ValueType(2);
    ExpectMatNear(t, Matrix3x3({
        {ValueType(0.5), ValueType(1.0), ValueType(1.5)},
        {ValueType(0.0), ValueType(0.5), ValueType(2.0)},
        {ValueType(2.5), ValueType(3.0), ValueType(0.0)}
                               }), 1e-9);
}

TEST(Matrix3x3, MatrixMultiplyColumnMajor)
{
    Matrix3x3 A({
        {ValueType(2), ValueType(1), ValueType(0)},
        {ValueType(0), ValueType(3), ValueType(1)},
        {ValueType(4), ValueType(0), ValueType(5)}
    });
    Matrix3x3 B({
        {ValueType(1), ValueType(2), ValueType(3)},
        {ValueType(4), ValueType(5), ValueType(6)},
        {ValueType(7), ValueType(8), ValueType(9)}
    });
    // 手計算で A*B
    Matrix3x3 C({
        {ValueType(2 * 1 + 1 * 4 + 0 * 7), ValueType(2 * 2 + 1 * 5 + 0 * 8), ValueType(2 * 3 + 1 * 6 + 0 * 9)},
        {ValueType(0 * 1 + 3 * 4 + 1 * 7), ValueType(0 * 2 + 3 * 5 + 1 * 8), ValueType(0 * 3 + 3 * 6 + 1 * 9)},
        {ValueType(4 * 1 + 0 * 4 + 5 * 7), ValueType(4 * 2 + 0 * 5 + 5 * 8), ValueType(4 * 3 + 0 * 6 + 5 * 9)}
    });
    ExpectMatNear(A * B, C);
}

TEST(Matrix3x3, VectorMultiplyTreatsVectorAsColumn)
{
    Matrix3x3 M({
        {ValueType(1), ValueType(2), ValueType(3)},
        {ValueType(4), ValueType(5), ValueType(6)},
        {ValueType(7), ValueType(8), ValueType(9)}
    });
    Vector3 v { ValueType(1), ValueType(2), ValueType(3) }; // 列ベクトル [1;2;3]

    // M*v = [1*1+2*2+3*3 ; 4*1+5*2+6*3 ; 7*1+8*2+9*3] = [14; 32; 50]
    Vector3 r = M * v;
    ExpectNearT<ValueType>(r.x, ValueType(14));
    ExpectNearT<ValueType>(r.y, ValueType(32));
    ExpectNearT<ValueType>(r.z, ValueType(50));
}

TEST(Matrix3x3, AtAccessorMutatesCorrectElement)
{
    Matrix3x3 m = Matrix3x3::Zero();
    m.at(2, 1) = ValueType(42); // (row=2, col=1) → 列優先 index = 1*3+2 = 5
    ExpectNearT<ValueType>(m.m[5], ValueType(42));
    ExpectNearT<ValueType>(m.at(2, 1), ValueType(42));
}
