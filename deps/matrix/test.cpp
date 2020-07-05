#include <iostream>
#include <cassert>
#include "matrix.hpp"

using namespace tmat;

bool testMultiply()
{
    cout << "test multiply" << endl;

    Matrix<3, 3, float> a{
        { 10, 0, -3 },
        { -2, -4, 1 },
        { 3, 0, 2 }
    };
    Matrix<3, 3, float> b{
        { 1, 2, 3 },
        { 4, 5, 6 },
        { 7, 8, 9}
    };
    Matrix<3, 3, float> expected {
        { -11, -4, 3 },
        { -11, -16, -21 },
        { 17, 22, 27 }
    };

    cout << a << " * " << b << endl;
    cout << "Expected: " << expected << endl;
    cout << "Actual: " << a * b << endl;
    
    return expected == a * b;
}

bool testMultiplyVector()
{
    cout << "test multiply vector" << endl;

    Matrix<4, 4, float> a{
        { 10, 0, -3, 18 },
        { -2, -4, 1, 9 },
        { 3, 0, 2, 3 },
        { 3, 5, 7, 6 }
    };
    Vector<4, float> b{ 4, 9, 1, 3 };
    Vector<4, float> expected{ 91, -16, 23, 82 };

    cout << a << " * " << b << endl;
    cout << "Expected: " << expected << endl;
    cout << "Actual: " << a * b << endl;
    
    return expected == a * b;
}

bool testInvert()
{
    cout << "test invert matrix" << endl;

    Matrix<4, 4, float> a{
        { 1, 2, 3, 4},
        { 3, 2, 1, 4 },
        { 1, 1, 2, 2 },
        { 1, 3, 7, 9 }
    };
    Matrix<4, 4, float> expected{
        { -3.0f/4, 1.0f/4, 1, 0 },
        { 25.0f/12, -1.0f/4, -2.0f/3, -2.0f/3 },
        { -1.0f/4, -1.0f/4, 1, 0 },
        { -5.0f/12, 1.0f/4, -2.0f/3, 1.0f/3}
    };
    auto inverse = a.inverse();

    cout << a << endl;
    cout << "Expected: " << expected << endl;
    cout << "Actual: " << inverse << endl;

    for(int i = 0; i < 4; i ++)
    {
        for(int j = 0; j < 4; j ++)
        {
            if(expected[i][j] - inverse[i][j] > 0.0001)
            {
                return false;
            }
        }
    }
    return true;
}

bool testTranspose()
{
    cout << "test transpose matrix" << endl;

    Matrix<4, 4, float> a {
        { 1, 2, 3, 4 },
        { 5, 6, 7, 8 },
        { 9, 10, 11, 12 },
        { 13, 14, 15, 16 }
    };

    Matrix<4, 4, float> e {
        { 1, 5, 9, 13 },
        { 2, 6, 10, 14 },
        { 3, 7, 11, 15 },
        { 4, 8, 12, 16 }
    };

    cout << a << endl;
    cout << "Expected: " << e << endl;
    a.transpose();
    cout << "Actual: " << a << endl;
    return a == e;
}

int main(int argc, char *argv[])
{
    assert(testMultiply());
    assert(testMultiplyVector());
    assert(testInvert());
    assert(testTranspose());
}

