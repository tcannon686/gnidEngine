#include <iostream>
#include "matrix/matrix.hpp"

using namespace gnid;
using namespace tmat;

int main(int argc, char *argv[])
{
    Vector3f vector{1.0f, 2.0f, 3.0f};
    Vector3f vector2{2.0f, 1.0f, 4.0f};
    Matrix<3, 3, float> matrix{
        { 10, 0, -3 },
        { -2, -4, 1 },
        { 3, 0, 2 }
    };
    cout << matrix << endl;
    cout << matrix.inverse() << endl;
    cout << matrix.inverse() * matrix << endl;

    float array[3 * 3];
    matrix.toArray(array);
    for(int i = 0; i < 9; i ++)
    {
        cout << array[i] << " ";
    }
    cout << endl;
    matrix.toArray(array, true);
    for(int i = 0; i < 9; i ++)
    {
        cout << array[i] << " ";
    }
    cout << endl;
}

