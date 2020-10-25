#include <cassert>
#include <iostream>
#include "gnid/matrix/matrix.hpp"
#include "gnid/box.hpp"

using namespace gnid;
using namespace tmat;

int main(int argc, char *argv[])
{
    Box box;
    box.add({ -1, -1, -1 });
    box.add({ 1, 1, 1 });

    /* Make sure the support function works. */
    assert(box.support(Vector3f::right)[0] > 0);
    cout << box.support(-Vector3f::right) << endl;
    assert(box.support(-Vector3f::right)[0] < 0);
    cout << box.support(Vector3f::up) << endl;
    assert(box.support(Vector3f::up)[1] > 0);
    cout << box.support(-Vector3f::up) << endl;
    assert(box.support(-Vector3f::up)[1] < 0);
    cout << box.support(Vector3f::forward) << endl;
    assert(box.support(Vector3f::forward)[2] > 0);
    cout << box.support(-Vector3f::forward) << endl;
    assert(box.support(-Vector3f::forward)[2] < 0);
}

