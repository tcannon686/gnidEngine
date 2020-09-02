
#include <stdlib.h>
#include "math.h"
#include "matrix.h"
#include "math3d.h"

#define min(a, b)       ((a < b) ? a : b)
#define max(a, b)       ((a > b) ? a : b)
#define clamp(v, a, b)  max(a, min(v, b))
#define clamp01(v)      clamp(v, 0, 1)

void CreateHit(hit_t *hit_ptr)
{
    hit_ptr->t = INFINITY;
}

void CreateRay(ray_t *ray, vector_t o, vector_t d)
{
    VectorThreeP(&ray->o, &o);
    VectorThreeP(&ray->d, &d);
    ray->d_inverse.x = ((vecc_t) 1.0) / d.x;
    ray->d_inverse.y = ((vecc_t) 1.0) / d.y;
    ray->d_inverse.z = ((vecc_t) 1.0) / d.z;
    ray->d_inverse.w = (vecc_t) 0.0;
}

void CreateSphere(sphere_t *sphere, vector_t origin, vecc_t radius)
{
    VectorThreeP(&sphere->origin, &origin);
    sphere->radius = radius;
}

void CreateTriangle(
    triangle_t *triangle,
    vector_t a,
    vector_t b,
    vector_t c)
{
    VectorThreeP(&triangle->a, &a);
    VectorThreeP(&triangle->b, &b);
    VectorThreeP(&triangle->c, &c);
    
    /* Calculate edge directions. */
    VectorMinusVectorP(&triangle->ba, &b, &a);
    VectorMinusVectorP(&triangle->cb, &c, &b);
    VectorMinusVectorP(&triangle->ac, &a, &c);

    /* Calculate edge lengths. */
    triangle->len_ba = VectorMagnitudeP(&triangle->ba);
    triangle->len_cb = VectorMagnitudeP(&triangle->cb);
    triangle->len_ac = VectorMagnitudeP(&triangle->ac);

    /* Calculate triangle origin. */
    VectorPlusVectorP(&triangle->origin, &a, &b);
    VectorPlusVectorP(&triangle->origin, &triangle->origin, &c);
    VectorTimesScalarP(
        &triangle->origin,
        &triangle->origin,
        (vecc_t) (1.0 / 3.0));

    /* Calculate triangle normal. */
    VectorCrossVectorP(&triangle->normal, &triangle->cb, &triangle->ba);
    
    /* Calculate triangle area. */
    triangle->area = VectorMagnitudeP(&triangle->normal);
    VectorTimesScalarP(&triangle->normal, &triangle->normal,
        ((vecc_t) 1.0) / triangle->area);

    /* Normalize the edge directions. */
    VectorNormalizeP(&triangle->ac, &triangle->ac);
    VectorNormalizeP(&triangle->cb, &triangle->cb);
    VectorNormalizeP(&triangle->ba, &triangle->ba);
}

void CreateBoundingBox(
    bounding_box_t *box,
    vector_t min,
    vector_t max)
{
    box->a = min;
    box->b = max;
    VectorPlusVectorP(&box->origin, &min, &max);
    VectorTimesScalarP(&box->origin, &box->origin, (vecc_t) (1.0 / 3.0));
}

int RayBox(bounding_box_t *box, hit_t *hit_ptr, ray_t *ray)
{
    vecc_t tmin, tmax, tymin, tymax, tzmin, tzmax;
    if(ray->d_inverse.x >= 0)
    {
        tmin = (box->a.x - ray->o.x) * ray->d_inverse.x;
        tmax = (box->b.x - ray->o.x) * ray->d_inverse.x;
    }
    else
    {
        tmin = (box->b.x - ray->o.x) * ray->d_inverse.x;
        tmax = (box->a.x - ray->o.x) * ray->d_inverse.x;
    }
    
    if(ray->d_inverse.y >= 0)
    {
        tymin = (box->a.y - ray->o.y) * ray->d_inverse.y;
        tymax = (box->b.y - ray->o.y) * ray->d_inverse.y;
    }
    else
    {
        tymin = (box->b.y - ray->o.y) * ray->d_inverse.y;
        tymax = (box->a.y - ray->o.y) * ray->d_inverse.y;
    }
    
    if(tmin > tymax || tymin > tmax)
        return 0;
    
    if(tymin > tmin)
        tmin = tymin;
    if(tymax < tmax)
        tmax = tymax;
    
    
    if(ray->d_inverse.z >= 0)
    {
        tzmin = (box->a.z - ray->o.z) * ray->d_inverse.z;
        tzmax = (box->b.z - ray->o.z) * ray->d_inverse.z;
    }
    else
    {
        tzmin = (box->b.z - ray->o.z) * ray->d_inverse.z;
        tzmax = (box->a.z - ray->o.z) * ray->d_inverse.z;
    }
    if(tmin > tzmax || tzmin > tmax)
        return 0;
    
    
    if(tzmin > tmin)
        tmin = tzmin;
    if(tzmax < tmax)
        tmax = tzmax;

    if(tmin > 0)
    {
        if(tmin > hit_ptr->t)
            return 0;
        hit_ptr->t = tmin;
        VectorTimesScalarP(&hit_ptr->position, &ray->d, hit_ptr->t);
        VectorPlusVectorP(&hit_ptr->position, &hit_ptr->position, &ray->o);

        // TODO normal.
        return 1;
    }
    else if(tmax > 0)
    {
        if(tmax > hit_ptr->t)
            return 0;
        hit_ptr->t = tmax;
        VectorTimesScalarP(&hit_ptr->position, &ray->d, hit_ptr->t);
        VectorPlusVectorP(&hit_ptr->position, &hit_ptr->position, &ray->o);

        // TODO normal.
        return 1;
    }
    return 0;
}

/*
 * Computes the barycentric coordinate of a point on the given triangle.
 */
int Barycentric(vector_t *dest, vector_t *v, triangle_t *tri)
{
    vecc_t area_inv = 1.0 / tri->area;
    vector_t cross;
    
    /* Calculate the signed areas for each edge. */
    VectorMinusVectorP(&cross, v, &tri->a);
    VectorCrossVectorP(&cross, &cross, &tri->ba);
    if(VectorDotVectorP(&tri->normal, &cross) < 0)
        dest->z = -VectorMagnitude(cross) * area_inv;
    else
        dest->z = VectorMagnitude(cross) * area_inv;
    
    VectorMinusVectorP(&cross, v, &tri->b);
    VectorCrossVectorP(&cross, &cross, &tri->cb);
    if(VectorDotVector(tri->normal, cross) < 0)
        dest->x = -VectorMagnitude(cross) * area_inv;
    else
        dest->x = VectorMagnitude(cross) * area_inv;
    
    VectorMinusVectorP(&cross, v, &tri->c);
    VectorCrossVectorP(&cross, &cross, &tri->ac);
    if(VectorDotVector(tri->normal, cross) < 0)
        dest->y = -VectorMagnitude(cross) * area_inv;
    else
        dest->y = VectorMagnitude(cross) * area_inv;
    
    dest->w = 0;

    /* Return whether or not the point is inside the triangle. */
    return dest->x >= 0 && dest->y >= 0 && dest->z >= 0;
}

int RaySphere(sphere_t *sphere, hit_t *hit_ptr, ray_t *ray)
{
    vector_t oc;
    vector_t l;
    vecc_t l_dot_oc;
    vecc_t norm_oc_squared;
    vecc_t discriminant;
    vecc_t root;
    vecc_t tmin, tmax, t;
    
    VectorNormalizeP(&l, &ray->d);
    VectorMinusVectorP(&oc, &ray->o, &sphere->origin);
    norm_oc_squared = VectorDotVectorP(&oc, &oc);
    l_dot_oc = VectorDotVectorP(&l, &oc);

    discriminant = l_dot_oc * l_dot_oc
        + sphere->radius * sphere->radius - norm_oc_squared;

    if(discriminant < 0)
        return 0;
    
    root = (vecc_t) sqrt(discriminant);

    tmin = -l_dot_oc - (vecc_t) root;
    tmax = -l_dot_oc + (vecc_t) root;
    
    if(tmin < 0)
        t = tmax;
    else
        t = tmin;
    
    if(t < 0)
        return 0;

    if(t < hit_ptr->t)
    {
        hit_ptr->t = t;
        VectorTimesScalarP(&hit_ptr->position, &l, hit_ptr->t);
        VectorPlusVectorP(&hit_ptr->position, &hit_ptr->position, &ray->o);
        return 1;
    }
    return 0;
}

int RayPlane(triangle_t *tri, hit_t *hit_ptr, ray_t *ray)
{
    vecc_t ldotn;
    if(tri->area == 0)
        return 0;
    ldotn = VectorDotVector(ray->d, tri->normal);
    if(ldotn == 0)
        return 0;
    vecc_t d = VectorDotVector(VectorMinusVector(tri->v0, ray->o), tri->normal);
    
    if(d * ldotn < 0)
        return 0;
    d /= ldotn;
    if(d > hit_ptr->t)
        return 0;

    vector_t pos = VectorPlusVector(ray->o, VectorTimesScalar(ray->d, d));

    hit_ptr->t = d;
    hit_ptr->position = pos;
    
    return 1;
}

int RayTriangle(triangle_t *tri, hit_t *hit_ptr, ray_t *ray)
{
    vecc_t ldotn;
    if(tri->area == 0)
        return 0;
    ldotn = VectorDotVector(ray->d, tri->normal);
    if(ldotn == 0)
        return 0;
    vecc_t d = VectorDotVector(VectorMinusVector(tri->v0, ray->o), tri->normal);
    
    if(d * ldotn < 0)
        return 0;
    d /= ldotn;
    if(d > hit_ptr->t)
        return 0;

    vector_t pos = VectorPlusVector(ray->o, VectorTimesScalar(ray->d, d));
    vector_t bary;

    /* There was no intersection if the point is not on the triangle. */
    if(!Barycentric(&bary, &pos, tri))
        return 0;
    
    hit_ptr->t = d;
    hit_ptr->position = pos;
    hit_ptr->barycentric = bary;
    hit_ptr->normal = tri->normal;
    
    return 1;
}

vecc_t TrianglePointDistance(
    triangle_t *self,
    vector_t v,
    vector_t *nearest_pt)
{
    vecc_t dot;
    vector_t v_proj;
    vector_t bary;
    vector_t va;

    VectorMinusVectorP(&va, &v, &self->a);

    /* Project the point onto the triangle. */
    dot = VectorDotVectorP(&self->normal, &va);
    VectorTimesScalarP(&v_proj, &self->normal, -dot);
    VectorPlusVectorP(&v_proj, &v_proj, &v);
    
    /* If the point is in the triangle. */
    if(Barycentric(&bary, &v_proj, self))
    {
        *nearest_pt = v_proj;
    }
    /* If the point is outside of an edge, project it onto that edge, but limit
     * it to the length of the edge. */
    else if(bary.x < 0)
    {
        VectorMinusVectorP(&va, &v, &self->b);
        dot = VectorDotVectorP(&self->cb, &va);
        dot = clamp(dot, 0, self->len_cb);
        VectorTimesScalarP(nearest_pt, &self->cb, dot);
        VectorPlusVectorP(nearest_pt, nearest_pt, &self->b);
    }
    else if(bary.y < 0)
    {
        VectorMinusVectorP(&va, &v, &self->c);
        dot = VectorDotVectorP(&self->ac, &va);
        dot = clamp(dot, 0, self->len_ac);
        VectorTimesScalarP(nearest_pt, &self->ac, dot);
        VectorPlusVectorP(nearest_pt, nearest_pt, &self->c);
    }
    else
    {
        VectorMinusVectorP(&va, &v, &self->a);
        dot = VectorDotVectorP(&self->ba, &va);
        dot = clamp(dot, 0, self->len_ba);
        VectorTimesScalarP(nearest_pt, &self->ba, dot);
        VectorPlusVectorP(nearest_pt, nearest_pt, &self->a);
    }

    /* Return the distance to the nearest point. */
    return VectorDistanceP(nearest_pt, &v);
}

vecc_t TriangleSegmentDistance(
    triangle_t *self,
    vector_t a,
    vector_t b,
    vector_t *nearest_pt,
    vector_t *pt_on_line)
{
    ray_t ray;
    vector_t ba;
    hit_t hit;

    VectorMinusVectorP(&ba, &a, &b);
    CreateHit(&hit);
    CreateRay(&ray, a, ba);

    // If there was an intersection, use it.
    if(RayPlane(self, &hit, &ray))
    {
        *pt_on_line = hit.position;
    }
    // If there was no intersection, calculate the midpoint.
    else
    {
        VectorPlusVectorP(pt_on_line, &a, &b);
        VectorTimesScalarP(pt_on_line, pt_on_line, (vecc_t) 0.5);
    }
    return TrianglePointDistance(self, *pt_on_line, nearest_pt);
}

