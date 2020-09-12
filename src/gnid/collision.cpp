#include "gnid/collision.hpp"

#include "gnid/matrix/matrix.hpp"

using namespace gnid;
using namespace tmat;
using namespace std;

Collision::Collision(
        const shared_ptr<Collider> a,
        const shared_ptr<Collider> b,
        Vector3f overlap)
    : colliders_ { a, b },
      overlap_(overlap),
      visited_(true)
{
}

bool Collision::operator==(const Collision &other) const
{
    return (colliders()[0] == other.colliders()[0]
                    && colliders()[1] == other.colliders()[1])
            || (colliders()[0] == other.colliders()[1]
                    && colliders()[1] == other.colliders()[0]);
}

void Collision::swapColliders()
{
    swap(colliders_[0], colliders_[1]);
    overlap_ = -overlap_;
}

Collision Collision::swapped() const
{
    Collision ret = *this;
    ret.swapColliders();
    return ret;
}
