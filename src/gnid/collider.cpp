#include "gnid/collider.hpp"

#include "gnid/matrix/matrix.hpp"
#include "gnid/scene.hpp"
#include <cassert>
#include <limits>
#include <map>
#include <queue>

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
    auto thisToWorld = getWorldMatrix();
    auto worldToThis = thisToWorld.inverse();

    /* Calculate the extents in local space. */
    auto forward = transformDirection(worldToThis, Vector3f::forward);
    auto backward = transformDirection(worldToThis, -Vector3f::forward);
    auto right = transformDirection(worldToThis, Vector3f::right);
    auto left = transformDirection(worldToThis, -Vector3f::right);
    auto up = transformDirection(worldToThis, Vector3f::up);
    auto down = transformDirection(worldToThis, -Vector3f::up);

    /* Convert to world space and add to the box. */
    box_.clear();
    box_.add(transform(thisToWorld, shape()->support(forward)));
    box_.add(transform(thisToWorld, shape()->support(backward)));
    box_.add(transform(thisToWorld, shape()->support(left)));
    box_.add(transform(thisToWorld, shape()->support(right)));
    box_.add(transform(thisToWorld, shape()->support(up)));
    box_.add(transform(thisToWorld, shape()->support(down)));
}

bool Collider::nearestSimplex1(
        vector<Vector3f> &s,
        Vector3f &d,
        const float tolerance) const
{
    assert(s.size() == 1);

    d = -s[0];

    return d.dot(d) == 0;
}

bool Collider::nearestSimplex2(
        vector<Vector3f> &s,
        Vector3f &d,
        const float tolerance) const
{
    Vector3f ab = s[1] - s[0];

    assert(s.size() == 2);

    /* The newest point should always have the greatest dot product. */
    assert(d.dot(s[1]) >= d.dot(s[0]));

    /* If the origin is past the second point it is the closest. */
    if(ab.dot(s[1]) <= tolerance)
    {
        s[0] = s[1];
        d = -s[0];
        s.resize(1);
        return false;
    }
    /* Otherwise the line must be the closest. */
    else
    {
        d = s[0].cross(ab).cross(ab);
        return false;
    }
}

bool Collider::nearestSimplex3(
        vector<Vector3f> &s,
        Vector3f &d,
        const float tolerance) const
{
    Vector3f ac = s[2] - s[0];
    Vector3f bc = s[2] - s[1];
    Vector3f abc = ac.cross(bc);

    /* These should be facing inwards. */
    Vector3f acn = ac.cross(abc);
    Vector3f bcn = abc.cross(bc);

    assert(s.size() == 3);

    /* The newest point should always have the greatest dot product. */
    assert(d.dot(s[2]) >= d.dot(s[0])
            && d.dot(s[2]) >= d.dot(s[1]));
    
    /* Inside the triangle on ac side. */
    if(acn.dot(s[2]) <= tolerance)
    {
        /* Inside the triangle on both ac and bc sides. */
        if(bcn.dot(s[2]) <= tolerance)
        {
            /* The simplex is still a triangle. Determine which side. */
            if(abc.dot(s[2]) <= tolerance)
            {
                d = abc;
                return false;
            }
            else
            {
                d = -abc;

                /* Reverse winding. */
                swap(s[0], s[2]);

                return false;
            }
        }
        /*
         * Not inside triangle on bc side, so bc is the closest or c is closest.
         */
        else
        {
            /* If the origin is past bc, then c is the closest. */
            if(bc.dot(s[2]) <= tolerance)
            {
                s[0] = s[2];
                d = -s[0];
                s.resize(1);
                return false;
            }
            /* Otherwise bc is the closest. */
            else
            {
                s[0] = s[1];
                s[1] = s[2];
                d = s[0].cross(bc).cross(bc);
                s.resize(2);
                return false;
            }
        }
    }
    /* Inside the triangle on bc side but not the ac side. */
    else if(bcn.dot(s[2]) <= tolerance)
    {
        /*
         * At this point, we can't be inside the triangle on the ac side, so ac
         * is the closest, or c is the closest.
         */

         /* If the origin is past ac, then c is closest. */
        if(ac.dot(s[2]) <= tolerance)
        {
            s[0] = s[2];
            s.resize(1);
            d = -s[0];
            return false;
        }
        /* Otherwise ac is closest. */
        else
        {
            s[1] = s[2];
            s.resize(2);
            d = s[0].cross(ac).cross(ac);
            return false;
        }
    }
    /* Not inside the triangle on either side. So c is closest. */
    else
    {
        s[0] = s[2];
        s.resize(1);
        d = -s[0];
        return false;
    }
}

bool Collider::nearestSimplex4(
        vector<Vector3f> &s,
        Vector3f &d,
        const float tolerance) const
{
    Vector3f ad = s[3] - s[0];
    Vector3f bd = s[3] - s[1];
    Vector3f cd = s[3] - s[2];


    /* Should be facing inward. */
    Vector3f abd = bd.cross(ad);
    Vector3f bcd = cd.cross(bd);
    Vector3f cad = ad.cross(cd);

    assert(s.size() == 4);
    
    /* The newest point should always have the greatest dot product. */
    assert(d.dot(s[3]) >= d.dot(s[0])
            && d.dot(s[3]) >= d.dot(s[1])
            && d.dot(s[3]) >= d.dot(s[2]));

    /* Inside on abd face. */
    if(abd.dot(s[3]) <= tolerance)
    {
        /* Inside on abd and bcd faces. */
        if(bcd.dot(s[3]) <= tolerance)
        {
            /* Inside on all three faces. */
            if(cad.dot(s[3]) <= tolerance)
            {
                return true;
            }
            /*
             * Inside on all three faces except for cad. Nearest simplex must be
             * on cad.
             */
            else
            {
                s[1] = s[0];
                s[0] = s[2];
                s[2] = s[3];
                s.resize(3);

                return nearestSimplex3(s, d, tolerance);
            }
        }
        /* Inside on abd face but not bcd face. */
        else
        {
            /* Inside on abd and cad, but not bcd. Nearest must be on bcd. */
            if(cad.dot(s[3]) <= tolerance)
            {
                s[0] = s[1];
                s[1] = s[2];
                s[2] = s[3];
                s.resize(3);

                return nearestSimplex3(s, d, tolerance);
            }
            /* Only inside abd. Nearest simplex must be on bcd, or cad. */
            else
            {
                /* Choose the nearest face. */
                vector<Vector3f> s1, s2;
                Vector3f d1, d2;

                /* Create simplices for the triangles. */
                s1.push_back(s[1]);
                s1.push_back(s[2]);
                s1.push_back(s[3]);

                s2.push_back(s[2]);
                s2.push_back(s[0]);
                s2.push_back(s[3]);

                nearestSimplex3(s1, d1, tolerance);
                nearestSimplex3(s2, d2, tolerance);

                /* Calculate the distance to both, choose the closest one. */
                float l1, l2;
                l1 = -d1.normalized().dot(s[3]);
                l2 = -d2.normalized().dot(s[3]);

                if(l1 < l2)
                {
                    d = d1;
                    s = s1;
                }
                else
                {
                    d = d2;
                    s = s2;
                }

                return false;
            }
        }
    }
    /* Inside on bcd face but not abd face. */
    else if(bcd.dot(s[3]) <= tolerance)
    {
        /* Inside on bcd and cad, but not abd. Nearest must be on abd. */
        if(cad.dot(s[3]) <= tolerance)
        {
            s[2] = s[3];
            s.resize(3);

            return nearestSimplex3(s, d, tolerance);
        }
        /* Only inside bcd. Nearest simplex must be on abd or cad.  */
        else
        {
            /* Choose the nearest face. */
            vector<Vector3f> s1, s2;
            Vector3f d1, d2;

            /* Create simplices for the triangles. */
            s1.push_back(s[0]);
            s1.push_back(s[1]);
            s1.push_back(s[3]);

            s2.push_back(s[2]);
            s2.push_back(s[0]);
            s2.push_back(s[3]);

            nearestSimplex3(s1, d1, tolerance);
            nearestSimplex3(s2, d2, tolerance);

            /* Calculate the distance to both, choose the closest one. */
            float l1, l2;
            l1 = -d1.normalized().dot(s[3]);
            l2 = -d2.normalized().dot(s[3]);

            if(l1 < l2)
            {
                d = d1;
                s = s1;
            }
            else
            {
                d = d2;
                s = s2;
            }

            return false;
        }
    }
    /* Only inside cad face. Nearest must be on abd or bcd. */
    else
    {
        /* Choose the nearest face. */
        vector<Vector3f> s1, s2;
        Vector3f d1, d2;

        /* Create simplices for the triangles. */
        s1.push_back(s[0]);
        s1.push_back(s[1]);
        s1.push_back(s[3]);

        s2.push_back(s[1]);
        s2.push_back(s[2]);
        s2.push_back(s[3]);

        nearestSimplex3(s1, d1, tolerance);
        nearestSimplex3(s2, d2, tolerance);

        /* Calculate the distance to both, choose the closest one. */
        float l1, l2;
        l1 = -d1.normalized().dot(s[3]);
        l2 = -d2.normalized().dot(s[3]);

        if(l1 < l2)
        {
            d = d1;
            s = s1;
        }
        else
        {
            d = d2;
            s = s2;
        }

        return false;
    }
}

bool Collider::nearestSimplex(
        vector<Vector3f> &s,
        Vector3f &d,
        const float tolerance) const
{
    NearestSimplexFunction function;

    assert(s.size() > 0 && s.size() <= 4);

    function = nearestSimplexFunctions[s.size() - 1];
    return (this->*function)(s, d, tolerance);
}

Collider::Triangle::Triangle(
        const int ia,
        const int ib,
        const int ic,
        const vector<Vector3f> &vertices)
    : ia(ia), ib(ib), ic(ic),
      normal_((vertices[ic] - vertices[ia])
              .cross(vertices[ic] - vertices[ib]).normalized()),
      distance_(normal_.dot(vertices[ic]))
{
    /* If we have negative distance, flip the triangle. */
    if(distance_ < 0)
    {
        swap(this->ia, this->ic);
        normal_ = -normal_;
        distance_ = -distance_;
    }
}

bool Collider::Triangle::operator<(const Triangle &other) const
{
    return distance_ > other.distance_;
}

bool Collider::getOverlap(
        Vector3f &out,
        const shared_ptr<Collider> &other,
        const Vector3f initialAxis,
        const float tolerance) const
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

    vector<Vector3f> s;
    s.push_back(a);

    Vector3f d = -a;

    if(gjk(d, s, other, tolerance))
    {
        /*
         * If the overlap is on a line, point, or triangle, we know that the
         * penetration depth must be zero.
         */
        if(s.size() < 4)
        {
            out = Vector3f::zero;
        }
        /* Otherwise, perform the Expanding Polytope Algorithm (EPA). */
        else
        {
            /* Create a triangular mesh from the simplex. */
            priority_queue<Triangle> triangles;
            vector<Vector3f> &vertices = s;
            
            /* Create indices for the simplex. */
            triangles.emplace(0, 2, 1, vertices);
            triangles.emplace(0, 1, 3, vertices);
            triangles.emplace(1, 2, 3, vertices);
            triangles.emplace(2, 0, 3, vertices);

            epa(out, triangles, vertices, other, tolerance);
        }

        /* Return that there was an intersection. */
        return true;
    }

    return false;
}

bool Collider::gjk(
        Vector3f &d,
        vector<Vector3f> &s,
        const shared_ptr<Collider> &other,
        const float tolerance) const
{
    auto thisToWorld = getWorldMatrix();
    auto worldToThis = thisToWorld.inverse();
    auto otherToWorld = other->getWorldMatrix();
    auto worldToOther = otherToWorld.inverse();

    Vector3f a;

    while(true)
    {
        a = transform(
                    thisToWorld,
                    shape()->support(
                        transformDirection(worldToThis, d)))
            - transform(
                    otherToWorld,
                    other->shape()->support(
                        transformDirection(worldToOther, -d)));

        if(a.dot(d) <= tolerance)
            return false;

        /* Add to the simplex. */
        s.push_back(a);

        if(nearestSimplex(s, d, 0))
        {
            /*
             * We now know that the shapes are colliding, and we have a simplex
             * in the Minkowski sum.
             */
            return true;
        }
    }
}

void Collider::epa(
        Vector3f &out,
        priority_queue<Triangle> &triangles,
        vector<Vector3f> &vertices,
        const shared_ptr<Collider> &other,
        const float tolerance) const
{
    auto thisToWorld = getWorldMatrix();
    auto worldToThis = thisToWorld.inverse();
    auto otherToWorld = other->getWorldMatrix();
    auto worldToOther = otherToWorld.inverse();

    while(true)
    {
        /*
         * Find the closest triangle to the origin on the simplex.
         */
        const Triangle triangle = triangles.top();

        /*
         * Find the point on the Minkowski difference furthest along
         * the face normal.
         */
        Vector3f a = transform(
                    thisToWorld,
                    shape()->support(
                        transformDirection(worldToThis, triangle.normal())))
            - transform(
                    otherToWorld,
                    other->shape()->support(
                        transformDirection(worldToOther, -triangle.normal())));

        /*
         * If we can't expand the polytope anymore, we're at the closest
         * triangle.
         */
        if(triangle.distance() == 0 || a.dot(triangle.normal())
                - triangle.distance() <= tolerance)
        {
            out = triangle.normal() * triangle.distance();
            break;
        }
        /*
         * Otherwise add the point to the polytope and continue
         * expanding.
         */
        else
        {
            map<pair<int, int>, int> edgeCounts;

            /* List of triangles to be re-added. */
            vector<Triangle> okayTriangles;

            /*
             * Find the triangles "visible" from the added vertex. We only need
             * to check the ones who are closer than the point.
             */
            while(!triangles.empty()
                    && triangles.top().distance() <= a.magnitude())
            {
                const Triangle top = triangles.top();
                triangles.pop();

                /* If the face is "visible", add its edges. */
                if((a - vertices[top.indexA()]).dot(top.normal()) >= 0)
                {
                    /* Add the edges. */
                    if(top.indexA() < top.indexB())
                        edgeCounts[make_pair(top.indexA(), top.indexB())] ++;
                    else
                        edgeCounts[make_pair(top.indexB(), top.indexA())] ++;

                    if(top.indexB() < top.indexC())
                        edgeCounts[make_pair(top.indexB(), top.indexC())] ++;
                    else
                        edgeCounts[make_pair(top.indexC(), top.indexB())] ++;

                    if(top.indexC() < top.indexA())
                        edgeCounts[make_pair(top.indexC(), top.indexA())] ++;
                    else
                        edgeCounts[make_pair(top.indexA(), top.indexC())] ++;
                }
                /* If the face is not "visible", re-add it to the queue. */
                else
                {
                    okayTriangles.push_back(top);
                }
            }

            /* Add the new vertex. */
            vertices.push_back(a);

            /* Re-add the triangles. */
            for(auto &&triangle : okayTriangles)
            {
                triangles.push(triangle);
            }

            /* Create new faces from the non-shared edges. */
            for(auto &[edge, count] : edgeCounts)
            {
                if(count == 1)
                {
                    triangles.emplace(
                            edge.first,
                            edge.second,
                            vertices.size() - 1,
                            vertices);
                }
            }
        }
    }
}

void Collider::onSceneChanged(shared_ptr<Scene> newScene)
{
    auto oldScene = getScene().lock();

    if(oldScene)
        oldScene->unregisterNode(
                static_pointer_cast<Collider>(shared_from_this()));

    if(newScene)
        newScene->registerNode(
                static_pointer_cast<Collider>(shared_from_this()));
}

bool &Collider::isTrigger()
{
    return isTrigger_;
}

