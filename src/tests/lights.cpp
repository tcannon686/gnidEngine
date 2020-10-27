#include "gnid/directionallight.hpp"
#include "gnid/pointlight.hpp"

using namespace std;
using namespace gnid;
using namespace tmat;

int main(int argc, char *argv[])
{
    /* Simply makes sure the constructors are working. */
    auto dl = make_shared<DirectionalLight>();
    auto pl = make_shared<PointLight>();
}
