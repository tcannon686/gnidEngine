#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "matrix.h"
#include "luamatrix.h"
#include "luamath3d.h"
#include "math3d.h"

// math3d.rayBox(hit, a, b, o, d)
int _G_math3d_rayBox(lua_State *L)
{
    vector_t *a, *b, *o, *d;
    bounding_box_t box;
    ray_t ray;
    hit_t hit;

    if(!lua_istable(L, 1))
        return luaL_error(L, "_G_math3d_rayBox expects table hit.");
    
    a = lua_tovectorptr(L, 2);
    if(a == NULL)
        return luaL_error(L, "_G_math3d_rayBox expects vector a.");

    b = lua_tovectorptr(L, 3);
    if(b == NULL)
        return luaL_error(L, "_G_math3d_rayBox expects vector b.");

    o = lua_tovectorptr(L, 4);
    if(o == NULL)
        return luaL_error(L, "_G_math3d_rayBox expects vector o.");
    
    d = lua_tovectorptr(L, 5);
    if(d == NULL)
        return luaL_error(L, "_G_math3d_rayBox expects vector d.");

    CreateHit(&hit);
    CreateBoundingBox(&box, *a, *b);
    CreateRay(&ray, *o, *d);
    if (RayBox(&box, &hit, &ray))
    {
        // TODO include normal.
        lua_pushnumber(L, (lua_Number) hit.t);
        lua_setfield(L, 1, "t");
        *lua_newvector(L) = hit.position;
        lua_setfield(L, 1, "position");
        lua_pushboolean(L, 1);
        return 1;
    }
    lua_pushboolean(L, 0);
    return 1;
}

// math3d.rayTriangle(hit, a, b, c, o, d)
int _G_math3d_rayTriangle(lua_State *L)
{
    vector_t *a, *b, *c, *o, *d;
    lua_Number t;
    int has_t;
    triangle_t triangle;
    ray_t ray;
    hit_t hit;

    if(!lua_istable(L, 1))
        return luaL_error(L, "_G_math3d_rayTriangle expects table hit.");
    
    a = lua_tovectorptr(L, 2);
    if(a == NULL)
        return luaL_error(L, "_G_math3d_rayTriangle expects vector a.");

    b = lua_tovectorptr(L, 3);
    if(b == NULL)
        return luaL_error(L, "_G_math3d_rayTriangle expects vector b.");

    c = lua_tovectorptr(L, 4);
    if(c == NULL)
        return luaL_error(L, "_G_math3d_rayTriangle expects vector c.");

    o = lua_tovectorptr(L, 5);
    if(o == NULL)
        return luaL_error(L, "_G_math3d_rayTriangle expects vector o.");
    
    d = lua_tovectorptr(L, 6);
    if(d == NULL)
        return luaL_error(L, "_G_math3d_rayTriangle expects vector d.");

    lua_getfield(L, 1, "t");
    t = lua_tonumberx(L, -1, &has_t);
    if(has_t)
        hit.t = (vecc_t) t;
    else
        CreateHit(&hit);

    CreateTriangle(&triangle, *a, *b, *c);
    CreateRay(&ray, *o, *d);
    if(RayTriangle(&triangle, &hit, &ray))
    {
        lua_pushnumber(L, (lua_Number) hit.t);
        lua_setfield(L, 1, "t");
        *lua_newvector(L) = hit.position;
        lua_setfield(L, 1, "position");
        *lua_newvector(L) = hit.barycentric;
        lua_setfield(L, 1, "barycentric");
        *lua_newvector(L) = hit.normal;
        lua_setfield(L, 1, "normal");
        lua_pushboolean(L, 1);
        return 1;
    }
    lua_pushboolean(L, 0);
    return 1;
}

// math3d.trianglePointDistance(a, b, c, p)
int _G_math3d_trianglePointDistance(lua_State *L)
{
    vector_t *a, *b, *c, *p;
    vector_t nearest_pt;
    triangle_t triangle;

    a = lua_tovectorptr(L, 1);
    if(a == NULL)
        return luaL_error(L,
            "_G_math3d_trianglePointDistance expects vector a.");

    b = lua_tovectorptr(L, 2);
    if(b == NULL)
        return luaL_error(L,
            "_G_math3d_trianglePointDistance expects vector b.");

    c = lua_tovectorptr(L, 3);
    if(c == NULL)
        return luaL_error(L,
            "_G_math3d_trianglePointDistance expects vector c.");

    p = lua_tovectorptr(L, 4);
    if(p == NULL)
        return luaL_error(L,
            "_G_math3d_trianglePointDistance expects vector p.");
    
    CreateTriangle(&triangle, *a, *b, *c);
    
    lua_pushnumber(L, (lua_Number)TrianglePointDistance(
        &triangle,
        *p,
        &nearest_pt));
    *lua_newvector(L) = nearest_pt;
    return 2;
}

// math3d.triangleSegmentDistance(a, b, c, s1, s2)
int _G_math3d_triangleSegmentDistance(lua_State *L)
{
    vector_t *a, *b, *c, *s1, *s2;
    vector_t nearest_pt, segment_pt;
    triangle_t triangle;

    a = lua_tovectorptr(L, 1);
    if(a == NULL)
        return luaL_error(L,
            "_G_math3d_triangleSegmentDistance expects vector a.");

    b = lua_tovectorptr(L, 2);
    if(b == NULL)
        return luaL_error(L,
            "_G_math3d_triangleSegmentDistance expects vector b.");

    c = lua_tovectorptr(L, 3);
    if(c == NULL)
        return luaL_error(L,
            "_G_math3d_triangleSegmentDistance expects vector c.");

    s1 = lua_tovectorptr(L, 4);
    if(s1 == NULL)
        return luaL_error(L,
            "_G_math3d_triangleSegmentDistance expects vector s1.");

    s2 = lua_tovectorptr(L, 4);
    if(s2 == NULL)
        return luaL_error(L,
            "_G_math3d_triangleSegmentDistance expects vector s2.");
    
    CreateTriangle(&triangle, *a, *b, *c);
    
    lua_pushnumber(L, (lua_Number)TriangleSegmentDistance(
        &triangle,
        *s1,
        *s2,
        &nearest_pt,
        &segment_pt));
    *lua_newvector(L) = nearest_pt;
    *lua_newvector(L) = segment_pt;
    return 2;
}

// math3d.lerp(a, b, t)
int _G_math3d_lerp(lua_State *L)
{
    if(lua_isnumber(L, 1) && lua_isnumber(L, 2))
    {
        lua_Number a, b, t;
        a = lua_tonumber(L, 1);
        b = lua_tonumber(L, 2);
        t = lua_tonumber(L, 3);
        lua_pushnumber(L, a + (b - a) * t);
        return 1;
    }
    else if(lua_isvector(L, 1) && lua_isvector(L, 2))
    {
        vector_t *a, *b, *ret;
        lua_Number t;
        a = lua_tovectorptr(L, 1);
        b = lua_tovectorptr(L, 2);
        t = lua_tonumber(L, 3);
        ret = lua_newvector(L);
        
        VectorMinusVectorP(ret, b, a);
        VectorTimesScalarP(ret, ret, t);
        VectorPlusVectorP(ret, ret, a);
        return 1;
    }
    else if(lua_ismatrix(L, 1) && lua_ismatrix(L, 2))
    {
        matrix_t *a, *b, *ret;
        lua_Number t;
        a = lua_tomatrixptr(L, 1);
        b = lua_tomatrixptr(L, 2);
        t = lua_tonumber(L, 3);
        ret = lua_newmatrix(L);
        
        MatrixMinusMatrixP(ret, b, a);
        MatrixTimesScalarP(ret, ret, t);
        MatrixPlusMatrixP(ret, ret, a);
        return 1;
    }
    else
    {
        return luaL_error(L, "unknown types for math3d.lerp.");
    }
}

static const luaL_Reg math3d_funcs[] = {
    { "rayBox", _G_math3d_rayBox },
    { "rayTriangle", _G_math3d_rayTriangle },
    { "trianglePointDistance", _G_math3d_trianglePointDistance },
    { "triangleSegmentDistance", _G_math3d_triangleSegmentDistance },
    { "lerp", _G_math3d_lerp },
    { NULL, NULL }
};

int luaopen_math3d(lua_State *L)
{
    luaL_newlib(L, math3d_funcs);
    return 1;
}


