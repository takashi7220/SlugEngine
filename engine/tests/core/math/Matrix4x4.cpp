// tests/test_matrix4x4.cpp
#include <gtest/gtest.h>
#include <type_traits>
#include "math/MatrixBase.hpp"
#include "math/Vector4.hpp"
#include "math/Matrix4x4.hpp" // 実際のパスに合わせて

using slug::math::Matrix4x4;
using slug::math::Vector4;
using slug::math::ValueType;

// ValueType が float/double/整数のいずれでも使える比較
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

static void ExpectMatNear(const Matrix4x4& A, const Matrix4x4& B, double eps = 1e-9)
{
    for (int i = 0; i < 16; ++i)
    {
        ExpectNearT<ValueType>(A.m[i], B.m[i], eps);
    }
}

TEST(Matrix4x4, DefaultIsZero)
{
    Matrix4x4 m;
    for (int i = 0; i < 16; ++i)
    {
        ExpectNearT<ValueType>(m.m[i], ValueType(0));
    }
    EXPECT_TRUE(m == Matrix4x4::Zero());
}

TEST(Matrix4x4, FromArrayColumnMajor)
{
    // 行列 [ 1  2  3  4
    //        5  6  7  8
    //        9 10 11 12
    //       13 14 15 16 ]
    // の列優先配列
    float raw[16] = {
        1, 5, 9, 13,  // col0
        2, 6,10, 14,  // col1
        3, 7,11, 15,  // col2
        4, 8,12, 16   // col3
    };
    Matrix4x4 m(raw);
    for (int i = 0; i < 16; ++i)
    {
        ExpectNearT<ValueType>(m.m[i], ValueType(raw[i]));
    }
    // at(row,col)
    ExpectNearT<ValueType>(m.at(0, 0), ValueType(1));
    ExpectNearT<ValueType>(m.at(0, 1), ValueType(2));
    ExpectNearT<ValueType>(m.at(3, 2), ValueType(15));
    ExpectNearT<ValueType>(m.at(2, 3), ValueType(12));
}

TEST(Matrix4x4, RowMajorInitializerPacksToColumnMajor)
{
    Matrix4x4 m({
        {ValueType(1),  ValueType(2),  ValueType(3),  ValueType(4)},
        {ValueType(5),  ValueType(6),  ValueType(7),  ValueType(8)},
        {ValueType(9),  ValueType(10), ValueType(11), ValueType(12)},
        {ValueType(13), ValueType(14), ValueType(15), ValueType(16)}
    });
    // 列優先: {1,5,9,13, 2,6,10,14, 3,7,11,15, 4,8,12,16}
    const ValueType expect[16] = {
        1,5,9,13, 2,6,10,14, 3,7,11,15, 4,8,12,16
    };
    for (int i = 0; i < 16; ++i)
    {
        ExpectNearT<ValueType>(m.m[i], expect[i]);
    }
    // at 確認
    ExpectNearT<ValueType>(m.at(0, 0), ValueType(1));
    ExpectNearT<ValueType>(m.at(1, 2), ValueType(7));
    ExpectNearT<ValueType>(m.at(3, 3), ValueType(16));
}

TEST(Matrix4x4, PartialInitializerFillsZeros)
{
    Matrix4x4 m({
        {ValueType(1), ValueType(2), ValueType(3), ValueType(4)},
        {ValueType(5), ValueType(6), ValueType(7), ValueType(8)}
        // 残り 2 行は 0 埋め
    });
    // 期待列優先: {1,5,0,0, 2,6,0,0, 3,7,0,0, 4,8,0,0}
    ExpectNearT<ValueType>(m.m[0], ValueType(1));
    ExpectNearT<ValueType>(m.m[1], ValueType(5));
    ExpectNearT<ValueType>(m.m[2], ValueType(0));
    ExpectNearT<ValueType>(m.m[3], ValueType(0));
    ExpectNearT<ValueType>(m.m[4], ValueType(2));
    ExpectNearT<ValueType>(m.m[5], ValueType(6));
    ExpectNearT<ValueType>(m.m[6], ValueType(0));
    ExpectNearT<ValueType>(m.m[7], ValueType(0));
    ExpectNearT<ValueType>(m.m[8], ValueType(3));
    ExpectNearT<ValueType>(m.m[9], ValueType(7));
    ExpectNearT<ValueType>(m.m[10], ValueType(0));
    ExpectNearT<ValueType>(m.m[11], ValueType(0));
    ExpectNearT<ValueType>(m.m[12], ValueType(4));
    ExpectNearT<ValueType>(m.m[13], ValueType(8));
    ExpectNearT<ValueType>(m.m[14], ValueType(0));
    ExpectNearT<ValueType>(m.m[15], ValueType(0));
}

TEST(Matrix4x4, IdentityAndZeroAlgebra)
{
    Matrix4x4 I = Matrix4x4::Identity();
    Matrix4x4 Z = Matrix4x4::Zero();

    ExpectMatNear(I * I, I);
    ExpectMatNear(I * Z, Z);
    ExpectMatNear(Z * I, Z);

    Matrix4x4 A({
        {ValueType(1), ValueType(2), ValueType(3), ValueType(4)},
        {ValueType(0), ValueType(1), ValueType(4), ValueType(0)},
        {ValueType(5), ValueType(6), ValueType(0), ValueType(1)},
        {ValueType(2), ValueType(0), ValueType(1), ValueType(3)}
    });
    ExpectMatNear(A * I, A);
    ExpectMatNear(I * A, A);
}

TEST(Matrix4x4, Transpose)
{
    Matrix4x4 A({
        {ValueType(1),  ValueType(2),  ValueType(3),  ValueType(4)},
        {ValueType(5),  ValueType(6),  ValueType(7),  ValueType(8)},
        {ValueType(9),  ValueType(10), ValueType(11), ValueType(12)},
        {ValueType(13), ValueType(14), ValueType(15), ValueType(16)}
    });
    Matrix4x4 AT = A.Transpose();
    ExpectMatNear(AT, Matrix4x4({
        {ValueType(1), ValueType(5),  ValueType(9),  ValueType(13)},
        {ValueType(2), ValueType(6),  ValueType(10), ValueType(14)},
        {ValueType(3), ValueType(7),  ValueType(11), ValueType(15)},
        {ValueType(4), ValueType(8),  ValueType(12), ValueType(16)}
                                }));
}

TEST(Matrix4x4, InverseRegular_GaussJordan)
{
    // ピボットが 0 にならないような上三角寄りの行列（Gauss-Jordan と相性良い）
    // A = diag(4,5,3,2) に少し上三角の成分を加えたもの
    Matrix4x4 A({
        {ValueType(4), ValueType(1), ValueType(2), ValueType(0)},
        {ValueType(0), ValueType(5), ValueType(1), ValueType(1)},
        {ValueType(0), ValueType(0), ValueType(3), ValueType(2)},
        {ValueType(0), ValueType(0), ValueType(0), ValueType(2)}
    });

    Matrix4x4 Ainv = A.Inverse();

    // A * A^{-1} = I, A^{-1} * A = I （誤差許容）
    ExpectMatNear(A * Ainv, Matrix4x4::Identity(), 1e-6);
    ExpectMatNear(Ainv * A, Matrix4x4::Identity(), 1e-6);
}

TEST(Matrix4x4, InverseSingularReturnsZero)
{
    // 行が一次従属（2 行目 = 1 行目 * 2、4 行目 = 1 行目）
    Matrix4x4 S({
        {ValueType(1), ValueType(2), ValueType(3), ValueType(4)},
        {ValueType(2), ValueType(4), ValueType(6), ValueType(8)},
        {ValueType(0), ValueType(1), ValueType(0), ValueType(1)},
        {ValueType(1), ValueType(2), ValueType(3), ValueType(4)}
    });
    Matrix4x4 Sinv = S.Inverse();
    EXPECT_TRUE(Sinv == Matrix4x4::Zero());
}

TEST(Matrix4x4, EqualityAndCopy)
{
    Matrix4x4 A({
        {ValueType(1), ValueType(0), ValueType(2), ValueType(0)},
        {ValueType(0), ValueType(1), ValueType(0), ValueType(3)},
        {ValueType(4), ValueType(0), ValueType(1), ValueType(0)},
        {ValueType(0), ValueType(5), ValueType(0), ValueType(1)}
    });
    Matrix4x4 B(A);
    EXPECT_TRUE(A == B);

    Matrix4x4 C = Matrix4x4::Identity();
    EXPECT_FALSE(A == C);
}

TEST(Matrix4x4, MatrixAdditionSubtraction)
{
    Matrix4x4 A({
        {ValueType(1), ValueType(2), ValueType(3), ValueType(4)},
        {ValueType(5), ValueType(6), ValueType(7), ValueType(8)},
        {ValueType(9), ValueType(10),ValueType(11),ValueType(12)},
        {ValueType(13),ValueType(14),ValueType(15),ValueType(16)}
    });
    Matrix4x4 B({
        {ValueType(16),ValueType(15),ValueType(14),ValueType(13)},
        {ValueType(12),ValueType(11),ValueType(10),ValueType(9)},
        {ValueType(8), ValueType(7), ValueType(6), ValueType(5)},
        {ValueType(4), ValueType(3), ValueType(2), ValueType(1)}
    });

    ExpectMatNear(A + B, Matrix4x4({
        {ValueType(17),ValueType(17),ValueType(17),ValueType(17)},
        {ValueType(17),ValueType(17),ValueType(17),ValueType(17)},
        {ValueType(17),ValueType(17),ValueType(17),ValueType(17)},
        {ValueType(17),ValueType(17),ValueType(17),ValueType(17)}
                                   }));
    ExpectMatNear(A - B, Matrix4x4({
        {ValueType(-15),ValueType(-13),ValueType(-11),ValueType(-9)},
        {ValueType(-7), ValueType(-5), ValueType(-3), ValueType(-1)},
        {ValueType(1),  ValueType(3),  ValueType(5),  ValueType(7)},
        {ValueType(9),  ValueType(11), ValueType(13), ValueType(15)}
                                   }));
}

TEST(Matrix4x4, ScalarOps)
{
    Matrix4x4 A({
        {ValueType(1), ValueType(2), ValueType(3), ValueType(4)},
        {ValueType(5), ValueType(6), ValueType(7), ValueType(8)},
        {ValueType(9), ValueType(10),ValueType(11),ValueType(12)},
        {ValueType(13),ValueType(14),ValueType(15),ValueType(16)}
    });

    ExpectMatNear(A + ValueType(1), Matrix4x4({
        {ValueType(2), ValueType(3), ValueType(4), ValueType(5)},
        {ValueType(6), ValueType(7), ValueType(8), ValueType(9)},
        {ValueType(10),ValueType(11),ValueType(12),ValueType(13)},
        {ValueType(14),ValueType(15),ValueType(16),ValueType(17)}
                                              }));

    ExpectMatNear(A - ValueType(1), Matrix4x4({
        {ValueType(0), ValueType(1), ValueType(2), ValueType(3)},
        {ValueType(4), ValueType(5), ValueType(6), ValueType(7)},
        {ValueType(8), ValueType(9), ValueType(10),ValueType(11)},
        {ValueType(12),ValueType(13),ValueType(14),ValueType(15)}
                                              }));

    ExpectMatNear(A * ValueType(2), Matrix4x4({
        {ValueType(2),  ValueType(4),  ValueType(6),  ValueType(8)},
        {ValueType(10), ValueType(12), ValueType(14), ValueType(16)},
        {ValueType(18), ValueType(20), ValueType(22), ValueType(24)},
        {ValueType(26), ValueType(28), ValueType(30), ValueType(32)}
                                              }));

    ExpectMatNear(A / ValueType(2), Matrix4x4({
        {ValueType(0.5), ValueType(1.0), ValueType(1.5), ValueType(2.0)},
        {ValueType(2.5), ValueType(3.0), ValueType(3.5), ValueType(4.0)},
        {ValueType(4.5), ValueType(5.0), ValueType(5.5), ValueType(6.0)},
        {ValueType(6.5), ValueType(7.0), ValueType(7.5), ValueType(8.0)}
                                              }), 1e-9);
}

TEST(Matrix4x4, CompoundAssignmentOps)
{
    Matrix4x4 A({
        {ValueType(4), ValueType(1), ValueType(2), ValueType(0)},
        {ValueType(0), ValueType(5), ValueType(1), ValueType(1)},
        {ValueType(0), ValueType(0), ValueType(3), ValueType(2)},
        {ValueType(0), ValueType(0), ValueType(0), ValueType(2)}
    });
    Matrix4x4 B({
        {ValueType(1), ValueType(0), ValueType(1), ValueType(0)},
        {ValueType(2), ValueType(1), ValueType(0), ValueType(1)},
        {ValueType(0), ValueType(1), ValueType(1), ValueType(0)},
        {ValueType(1), ValueType(0), ValueType(0), ValueType(1)}
    });

    Matrix4x4 t = A; t += B;
    ExpectMatNear(t, Matrix4x4({
        {ValueType(5), ValueType(1), ValueType(3), ValueType(0)},
        {ValueType(2), ValueType(6), ValueType(1), ValueType(2)},
        {ValueType(0), ValueType(1), ValueType(4), ValueType(2)},
        {ValueType(1), ValueType(0), ValueType(0), ValueType(3)}
                               }));

    t = A; t -= B;
    ExpectMatNear(t, Matrix4x4({
        {ValueType(3), ValueType(1), ValueType(1), ValueType(0)},
        {ValueType(-2),ValueType(4), ValueType(1), ValueType(0)},
        {ValueType(0), ValueType(-1),ValueType(2), ValueType(2)},
        {ValueType(-1),ValueType(0), ValueType(0), ValueType(1)}
                               }));

    // 行列積（列優先実装の検証）
    t = A; t *= B;
    // 期待値は手計算（行×列）
    Matrix4x4 AB({
        {
            // row0
            ValueType(4 * 1 + 1 * 2 + 2 * 0 + 0 * 1),
            ValueType(4 * 0 + 1 * 1 + 2 * 1 + 0 * 0),
            ValueType(4 * 1 + 1 * 0 + 2 * 1 + 0 * 0),
            ValueType(4 * 0 + 1 * 1 + 2 * 0 + 0 * 1)
        },
        {
            // row1
            ValueType(0 * 1 + 5 * 2 + 1 * 0 + 1 * 1),
            ValueType(0 * 0 + 5 * 1 + 1 * 1 + 1 * 0),
            ValueType(0 * 1 + 5 * 0 + 1 * 1 + 1 * 0),
            ValueType(0 * 0 + 5 * 1 + 1 * 0 + 1 * 1)
        },
        {
            // row2
            ValueType(0 * 1 + 0 * 2 + 3 * 0 + 2 * 1),
            ValueType(0 * 0 + 0 * 1 + 3 * 1 + 2 * 0),
            ValueType(0 * 1 + 0 * 0 + 3 * 1 + 2 * 0),
            ValueType(0 * 0 + 0 * 1 + 3 * 0 + 2 * 1)
        },
        {
            // row3
            ValueType(0 * 1 + 0 * 2 + 0 * 0 + 2 * 1),
            ValueType(0 * 0 + 0 * 1 + 0 * 1 + 2 * 0),
            ValueType(0 * 1 + 0 * 0 + 0 * 1 + 2 * 0),
            ValueType(0 * 0 + 0 * 1 + 0 * 0 + 2 * 1)
        }
    });
    ExpectMatNear(t, AB);

    t = A; t += ValueType(10);
    ExpectMatNear(t, Matrix4x4({
        {ValueType(14), ValueType(11), ValueType(12), ValueType(10)},
        {ValueType(10), ValueType(15), ValueType(11), ValueType(11)},
        {ValueType(10), ValueType(10), ValueType(13), ValueType(12)},
        {ValueType(10), ValueType(10), ValueType(10), ValueType(12)}
                               }));

    t = A; t -= ValueType(1);
    ExpectMatNear(t, Matrix4x4({
        {ValueType(3), ValueType(0), ValueType(1), ValueType(-1)},
        {ValueType(-1),ValueType(4), ValueType(0), ValueType(0)},
        {ValueType(-1),ValueType(-1),ValueType(2), ValueType(1)},
        {ValueType(-1),ValueType(-1),ValueType(-1),ValueType(1)}
                               }));

    t = A; t *= ValueType(2);
    ExpectMatNear(t, Matrix4x4({
        {ValueType(8), ValueType(2), ValueType(4), ValueType(0)},
        {ValueType(0), ValueType(10),ValueType(2), ValueType(2)},
        {ValueType(0), ValueType(0), ValueType(6), ValueType(4)},
        {ValueType(0), ValueType(0), ValueType(0), ValueType(4)}
                               }));

    t = A; t /= ValueType(2);
    ExpectMatNear(t, Matrix4x4({
        {ValueType(2), ValueType(0.5), ValueType(1), ValueType(0)},
        {ValueType(0), ValueType(2.5), ValueType(0.5), ValueType(0.5)},
        {ValueType(0), ValueType(0),   ValueType(1.5), ValueType(1)},
        {ValueType(0), ValueType(0),   ValueType(0),   ValueType(1)}
                               }), 1e-9);
}

TEST(Matrix4x4, MatrixMultiplyColumnMajor)
{
    Matrix4x4 A({
        {ValueType(1), ValueType(2), ValueType(3), ValueType(4)},
        {ValueType(0), ValueType(1), ValueType(0), ValueType(1)},
        {ValueType(2), ValueType(0), ValueType(1), ValueType(0)},
        {ValueType(0), ValueType(1), ValueType(2), ValueType(1)}
    });
    Matrix4x4 B({
        {ValueType(2), ValueType(0), ValueType(1), ValueType(0)},
        {ValueType(1), ValueType(3), ValueType(0), ValueType(2)},
        {ValueType(0), ValueType(1), ValueType(2), ValueType(0)},
        {ValueType(1), ValueType(0), ValueType(1), ValueType(2)}
    });

    // 期待値（行×列をその場計算）
    Matrix4x4 C({
        {
            ValueType(1 * 2 + 2 * 1 + 3 * 0 + 4 * 1),
            ValueType(1 * 0 + 2 * 3 + 3 * 1 + 4 * 0),
            ValueType(1 * 1 + 2 * 0 + 3 * 2 + 4 * 1),
            ValueType(1 * 0 + 2 * 2 + 3 * 0 + 4 * 2)
        },
        {
            ValueType(0 * 2 + 1 * 1 + 0 * 0 + 1 * 1),
            ValueType(0 * 0 + 1 * 3 + 0 * 1 + 1 * 0),
            ValueType(0 * 1 + 1 * 0 + 0 * 2 + 1 * 1),
            ValueType(0 * 0 + 1 * 2 + 0 * 0 + 1 * 2)
        },
        {
            ValueType(2 * 2 + 0 * 1 + 1 * 0 + 0 * 1),
            ValueType(2 * 0 + 0 * 3 + 1 * 1 + 0 * 0),
            ValueType(2 * 1 + 0 * 0 + 1 * 2 + 0 * 1),
            ValueType(2 * 0 + 0 * 2 + 1 * 0 + 0 * 2)
        },
        {
            ValueType(0 * 2 + 1 * 1 + 2 * 0 + 1 * 1),
            ValueType(0 * 0 + 1 * 3 + 2 * 1 + 1 * 0),
            ValueType(0 * 1 + 1 * 0 + 2 * 2 + 1 * 1),
            ValueType(0 * 0 + 1 * 2 + 2 * 0 + 1 * 2)
        }
    });

    ExpectMatNear(A * B, C);
}

TEST(Matrix4x4, VectorMultiplyTreatsVectorAsColumn)
{
    Matrix4x4 M({
        {ValueType(1), ValueType(2), ValueType(3), ValueType(4)},
        {ValueType(0), ValueType(1), ValueType(0), ValueType(1)},
        {ValueType(2), ValueType(0), ValueType(1), ValueType(0)},
        {ValueType(0), ValueType(1), ValueType(2), ValueType(1)}
    });
    Vector4 v { ValueType(1), ValueType(2), ValueType(3), ValueType(1) }; // 列ベクトル [1;2;3;1]

    // r = M*v
    // row0: 1*1 + 2*2 + 3*3 + 4*1 = 1+4+9+4 = 18
    // row1: 0*1 + 1*2 + 0*3 + 1*1 = 3
    // row2: 2*1 + 0*2 + 1*3 + 0*1 = 5
    // row3: 0*1 + 1*2 + 2*3 + 1*1 = 9
    Vector4 r = M * v;
    ExpectNearT<ValueType>(r.x, ValueType(18));
    ExpectNearT<ValueType>(r.y, ValueType(3));
    ExpectNearT<ValueType>(r.z, ValueType(5));
    ExpectNearT<ValueType>(r.w, ValueType(9));
}

TEST(Matrix4x4, AtAccessorMutatesCorrectElement)
{
    Matrix4x4 m = Matrix4x4::Zero();
    m.at(3, 2) = ValueType(42); // row=3, col=2 → index = 2*4 + 3 = 11
    ExpectNearT<ValueType>(m.m[11], ValueType(42));
    ExpectNearT<ValueType>(m.at(3, 2), ValueType(42));
}
