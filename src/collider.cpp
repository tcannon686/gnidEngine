#include "matrix/matrix.hpp"
#include "scene.hpp"
#include "collider.hpp"
#include <cassert>

using namespace gnid;
using namespace tmat;

Collider::NearestSimplexFunction
Collider::nearestSimplexFunctions[4] = {
    &Collider::nearestSimplex1,
    &Collider::nearestSimplex2,
    &Collider::nearestSimplex3,
    &Collider::nearestSimplex4
};

void Collider::calcBox()
{
    box_ = shape()->box();
    box_.transform(getWorldMatrix());
}

bool Collider::nearestSimplex1(array<Vector3f, 4> &s, int &n, Vector3f &d) const
{
    d = -s[0];

    return d.dot(d) == 0;
}

bool Collider::nearestSimplex2(array<Vector3f, 4> &s, int &n, Vector3f &d) const
{
    Vector3f ab = s[1] - s[0];

    /* If the origin is past the second point it is the closest. */
    if(ab.dot(s[1]) < 0)
    {
        s[0] = s[1];
        n = 1;
        d = -s[0];
        return false;
    }
    /* Otherwise the line must be the closest. */
    else
    {
        d = s[0].cross(ab).cross(ab);
        if(d.magnitude() == 0)
        {
            d = Vector3f::up;
        }
        return false;
    }
}

bool Collider::nearestSimplex3(array<Vector3f, 4> &s, int &n, Vector3f &d) const
{
    Vector3f ac = s[2] - s[0];
    Vector3f bc = s[2] - s[1];
    Vector3f abc = ac.cross(bc);

    /* These should be facing inwards. */
    Vector3f acn = ac.cross(abc);
    Vector3f bcn = abc.cross(bc);
    
    /* Inside the triangle on ac side. */
    if(acn.dot(s[2]) <= 0)
    {
        /* Inside the triangle on both ac and bc sides. */
        if(bcn.dot(s[2]) <= 0)
        {
            /* The simplex is still a triangle. Determine which side. */

            float dot = abc.dot(s[2]);

            if(dot == 0)
            {
                d = abc;
                return true;
            }
            else if(dot < 0)
            {
                d = abc;
                return false;
            }
            else
            {
                d = -abc;
                return false;
            }
        }
        /* Not inside triangle on bc side, so bc is the closest or c is closest.
         */
        else
        {
            /* If the origin is past bc, then c is the closest. */
            if(bc.dot(s[2]) <= 0)
            {
                s[0] = s[2];
                d = -s[0];
                n = 1;
                return false;
            }
            /* Otherwise bc is the closest. */
            else
            {
                s[0] = s[1];
                s[1] = s[2];
                d = s[0].cross(bc).cross(bc);
                n = 2;
                return false;
            }
        }
    }
    /* Inside the triangle on bc side but not the ac side. */
    else if(bcn.dot(s[2]) <= 0)
    {
        /*
         * At this point, we can't be inside the triangle on the ac side, so ac
         * is the closest, or c is the closest.
         */

         /* If the origin is past ac, then c is closest. */
        if(ac.dot(s[2]) <= 0)
        {
            s[0] = s[2];
            n = 1;
            d = -s[0];
            return false;
        }
        /* Otherwise ac is closest. */
        else
        {
            s[1] = s[2];
            n = 2;
            d = s[0].cross(ac).cross(ac);
            return false;
        }
    }
    /* Not inside the triangle on either side. So c is closest. */
    else
    {
        s[0] = s[2];
        n = 1;
        d = -s[0];
        return false;
    }
}

bool Collider::nearestSimplex4(array<Vector3f, 4> &s, int &n, Vector3f &d) const
{
    Vector3f ad = s[3] - s[0];
    Vector3f bd = s[3] - s[1];
    Vector3f cd = s[3] - s[2];


    /* Should be facing inward. */
    Vector3f abd = ad.cross(bd);
    Vector3f bcd = bd.cross(cd);
    Vector3f cad = cd.cross(ad);

    /* Inside on abd face. */
    if(abd.dot(s[3]) <= 0)
    {
        /* Inside on abd and bcd faces. */
        if(bcd.dot(s[3]) <= 0)
        {
            /* Inside on all three faces. */
            if(cad.dot(s[3]) <= 0)
            {
                return true;
            }
            /* Inside on all three faces except for cad. Nearest simplex must be
             * on cad.*/
            else
            {
                s[1] = s[0];
                s[0] = s[2];
                s[2] = s[3];
                n = 3;

                return nearestSimplex3(s, n, d);
            }
        }
        /* Inside on abd face but not bcd face. */
        else
        {
            /* Inside on abd and cad, but not bcd. Nearest must be on bcd. */
            if(cad.dot(s[3]) <= 0)
            {
                s[0] = s[1];
                s[1] = s[2];
                s[2] = s[3];
                n = 3;

                return nearestSimplex3(s, n, d);
            }
            /* Only inside abd. Nearest simplex must be on cd.  */
            else
            {
                s[0] = s[2];
                s[1] = s[3];
                n = 2;

                return nearestSimplex2(s, n, d);
            }
        }
    }
    /* Inside on bcd face but not abd face. */
    else if(bcd.dot(s[3]) <= 0)
    {
        /* Inside on bcd and cad, but not abd. Nearest must be on abd. */
        if(cad.dot(s[3]) <= 0)
        {
            s[2] = s[3];
            n = 3;

            return nearestSimplex3(s, n, d);
        }
        /* Only inside bcd. Nearest simplex must be on ad.  */
        else
        {
            s[1] = s[3];
            n = 2;

            return nearestSimplex2(s, n, d);
        }
    }
    /* Inside on cad face but not bcd face or abd face. Nearest must be bd. */
    else
    {
        s[0] = s[1];
        s[1] = s[3];
        n = 2;
        
        return nearestSimplex2(s, n, d);
    }
}

bool Collider::nearestSimplex(array<Vector3f, 4> &s, int &n, Vector3f &d) const
{
    NearestSimplexFunction function;

    assert(n - 1 >= 0 && n - 1 < 4);

    function = nearestSimplexFunctions[n - 1];
    return (this->*function)(s, n, d);

    /* useful for debugging but not as efficient */
    /*
    if(n == 1)
        return nearestSimplex1(s, n, d);
    else if(n == 2)
        return nearestSimplex2(s, n, d);
    else if(n == 3)
        return nearestSimplex3(s, n, d);
    else if(n == 4)
        return nearestSimplex4(s, n, d);
    else
        cout << "error" << endl;
    */
}

bool Collider::getOverlap(
        Vector3f &out,
        const shared_ptr<Collider> &other,
        const Vector3f initialAxis,
        const int maxIterations) const
{
    auto thisToWorld = getWorldMatrix();
    auto worldToThis = thisToWorld.inverse();
    auto otherToWorld = other->getWorldMatrix();
    auto worldToOther = otherToWorld.inverse();

    Vector3f a =
        transform(
                thisToWorld,
                shape()->support(
                    transformDirection(worldToThis, initialAxis)))
        - transform(
                otherToWorld,
                other->shape()->support(
                    transformDirection(worldToOther, -initialAxis)));

    int iterations = maxIterations;

    array<Vector3f, 4> s;
    int n = 0;
    s[0] = a;

    Vector3f d = -a;

    while(maxIterations == 0 || iterations > 0)
    {
        a = transform(
                    thisToWorld,
                    shape()->support(
                        transformDirection(worldToThis, d)))
            - transform(
                    otherToWorld,
                    other->shape()->support(
                        transformDirection(worldToOther, -d)));

        if(a.dot(d) <= 0)
            return false;

        /* Add to the simplex. */
        s[n ++] = a;

        if(nearestSimplex(s, n, d))
        {
            a = transform(
                        thisToWorld,
                        shape()->support(transform(worldToThis, d)))
                - transform(
                        otherToWorld,
                        other->shape()->support(transform(worldToOther, -d)));
            out = a;
            return true;
        }

        iterations --;
    }

    return false;
}

void Collider::onSceneChanged(shared_ptr<Scene> newScene)
{
    auto oldScene = getScene().lock();

    if(oldScene)
        oldScene->removeCollider(
                static_pointer_cast<Collider>(shared_from_this()));

    if(newScene)
        newScene->addCollider(
                static_pointer_cast<Collider>(shared_from_this()));
}

