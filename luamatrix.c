
#ifndef LUA_PATH
#define LUA_PATH
#endif

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <string.h>
#include <stdlib.h>
#include "matrix.h"

static int matrix_mt_index, vector_mt_index;
static int matrix_index, vector_index;

matrix_t *lua_newmatrix(lua_State *L)
{
    matrix_t *ret = (matrix_t *)lua_newuserdata(L, sizeof(matrix_t));
    // Get the metatable from the registry.
    lua_geti(L, LUA_REGISTRYINDEX, matrix_mt_index);
    
    // Make it the metatable for the userdata.
    lua_setmetatable(L, -2);
    return ret;
}

vector_t *lua_newvector(lua_State *L)
{
    vector_t *ret = (vector_t *)lua_newuserdata(L, sizeof(vector_t));
    // Get the metatable from the registry.
    lua_geti(L, LUA_REGISTRYINDEX, vector_mt_index);
    
    // Make it the metatable for the userdata.
    lua_setmetatable(L, -2);
    return ret;
}

vector_t *lua_tovectorptrx(lua_State *L, int index, int *is_vector)
{
    lua_getmetatable(L, index);
    lua_geti(L, LUA_REGISTRYINDEX, vector_mt_index);
    if(lua_compare(L, -1, -2, LUA_OPEQ))
    {
        lua_pop(L, 2);
        if(is_vector != NULL)
            *is_vector = 1;
        return (vector_t *)lua_topointer(L, index);
    }
    else
    {
        if(is_vector != NULL)
            *is_vector = 0;
        lua_pop(L, 2);
        return NULL;
    }
}

vector_t *lua_tovectorptr(lua_State *L, int index)
{
    return lua_tovectorptrx(L, index, NULL);
}

vector_t lua_tovectorx(lua_State *L, int index, int *is_vector)
{
    vector_t *ret = lua_tovectorptrx(L, index, is_vector);
    if(ret != NULL)
        return *ret;
    else
        return EmptyVector();
}

vector_t lua_tovector(lua_State *L, int index)
{
    return lua_tovectorx(L, index, NULL);
}

matrix_t *lua_tomatrixptrx(lua_State *L, int index, int *is_matrix)
{
    lua_getmetatable(L, index);
    lua_geti(L, LUA_REGISTRYINDEX, matrix_mt_index);
    if(lua_compare(L, -1, -2, LUA_OPEQ))
    {
        lua_pop(L, 2);
        if(is_matrix != NULL)
            *is_matrix = 1;
        return (matrix_t *)lua_topointer(L, index);
    }
    else
    {
        lua_pop(L, 2);
        if(is_matrix != NULL)
            *is_matrix = 0;
        return NULL;
    }
}

matrix_t *lua_tomatrixptr(lua_State *L, int index)
{
    return lua_tomatrixptrx(L, index, NULL);
}

matrix_t lua_tomatrixx(lua_State *L, int index, int *is_matrix)
{
    matrix_t *ret = lua_tomatrixptrx(L, index, is_matrix);
    if(ret != NULL)
        return *ret;
    else
        return EmptyMatrix();
}

matrix_t lua_tomatrix(lua_State *L, int index)
{
    return lua_tomatrixx(L, index, NULL);
}

int lua_ismatrix(lua_State *L, int index)
{
    lua_getmetatable(L, index);
    lua_geti(L, LUA_REGISTRYINDEX, matrix_mt_index);
    if(lua_compare(L, -1, -2, LUA_OPEQ))
    {
        lua_pop(L, 2);
        return 1;
    }
    return 0;
}

int lua_isvector(lua_State *L, int index)
{
    lua_getmetatable(L, index);
    lua_geti(L, LUA_REGISTRYINDEX, vector_mt_index);
    if(lua_compare(L, -1, -2, LUA_OPEQ))
    {
        lua_pop(L, 2);
        return 1;
    }
    return 0;
}

int _G_matrix_newTranslate(lua_State *L)
{
    
    if(lua_isvector(L, 1))
    {
        NewTranslateMatrixP(lua_newmatrix(L), *(vector_t *)lua_topointer(L, 1));
        return 1;
    }
    else
        return luaL_error(L, "invalid arguments.");
}

int _G_matrix_newScale(lua_State *L)
{
    if(lua_isvector(L, 1))
    {
        NewScaleMatrixP(lua_newmatrix(L), *(vector_t *)lua_topointer(L, 1));
        return 1;
    }
    else
        return luaL_error(L, "invalid arguments.");
}

int _G_matrix_newRotate(lua_State *L)
{
    if(lua_isnumber(L, 1) && lua_isvector(L, 2))
    {
        NewRotateMatrixP(
            lua_newmatrix(L),
            lua_tonumber(L, 1),
            *(vector_t *)lua_topointer(L, 2));
        return 1;
    }
    else
        return luaL_error(L, "invalid arguments.");
}

int _G_matrix_newOrthographic(lua_State *L)
{
    if(lua_isnumber(L, 1)
        && lua_isnumber(L, 2)
        && lua_isnumber(L, 3)
        && lua_isnumber(L, 4)
        && lua_isnumber(L, 5)
        && lua_isnumber(L, 6))
    {
        NewOrthographicMatrixP(
            lua_newmatrix(L),
            lua_tonumber(L, 1),
            lua_tonumber(L, 2),
            lua_tonumber(L, 3),
            lua_tonumber(L, 4),
            lua_tonumber(L, 5),
            lua_tonumber(L, 6));
        return 1;
    }
    else
        return luaL_error(L, "invalid arguments.");
}

int _G_matrix_newFrustrum(lua_State *L)
{
    if(lua_isnumber(L, 1)
        && lua_isnumber(L, 2)
        && lua_isnumber(L, 3)
        && lua_isnumber(L, 4)
        && lua_isnumber(L, 5)
        && lua_isnumber(L, 6))
    {
        NewFrustrumMatrixP(
            lua_newmatrix(L),
            lua_tonumber(L, 1),
            lua_tonumber(L, 2),
            lua_tonumber(L, 3),
            lua_tonumber(L, 4),
            lua_tonumber(L, 5),
            lua_tonumber(L, 6));
        return 1;
    }
    else
        return luaL_error(L, "invalid arguments.");
}

int _G_matrix_newPerspective(lua_State *L)
{
    if(lua_isnumber(L, 1)
        && lua_isnumber(L, 2)
        && lua_isnumber(L, 3)
        && lua_isnumber(L, 4))
    {
        NewPerspectiveMatrixP(
            lua_newmatrix(L),
            lua_tonumber(L, 1),
            lua_tonumber(L, 2),
            lua_tonumber(L, 3),
            lua_tonumber(L, 4));
        return 1;
    }
    else
        return luaL_error(L, "invalid arguments.");
}

int _G_matrix_newLookAt(lua_State *L)
{
    if(lua_isvector(L, 1)
        && lua_isvector(L, 2)
        && lua_isvector(L, 3))
    {
        NewLookAtMatrixP(
            lua_newmatrix(L),
            *(vector_t *)lua_topointer(L, 1),
            *(vector_t *)lua_topointer(L, 2),
            *(vector_t *)lua_topointer(L, 3));
        return 1;
    }
    else
        return luaL_error(L, "invalid arguments.");
}

int _G_matrix_toMatrix(lua_State *L)
{
    if(lua_ismatrix(L, 1))
    {
        return 1;
    }
    else if(lua_istable(L, 1))
    {
        int len;
        lua_len(L, 1);
        len = lua_tointeger(L, -1);
        lua_pop(L, 1);
        
        matrix_t *ret = lua_newmatrix(L);
        
        for(int i = 0; i < 16; i ++)
        {
            if(i < len)
            {
                lua_geti(L, 1, i + 1);
                if(lua_isnumber(L, -1))
                {
                    ret->m[i] = lua_tonumber(L, -1);
                    lua_pop(L, 1);
                }
                else
                    return luaL_error(L, "invalid argument type.\n");
            }
            else
                ret->m[i] = 0;
        }
        
        return 1;
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
}

int _G_matrix_new(lua_State *L)
{
    matrix_t *ret = lua_newmatrix(L);
    for(int i = 1; i <= 16; i++)
    {
        if(i < lua_gettop(L))
        {
            if(lua_isnumber(L, i))
                ret->m[i - 1] = lua_tonumber(L, i);
            else
                return luaL_error(L, "invalid argument type.\n");
        }
        else
            ret->m[i - 1] = 0;
    }
    return 1;
}

int _G_matrix_negate(lua_State *L)
{
    if(lua_ismatrix(L, 1))
        MatrixNegateP(
            (matrix_t *) lua_newuserdata(L, sizeof(matrix_t)),
            (matrix_t *) lua_topointer(L, 1));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_matrix_index(lua_State *L)
{
    if(lua_ismatrix(L, 1) && lua_isinteger(L, 2))
        lua_pushnumber(L, ((matrix_t *)lua_topointer(L, 1))->m[lua_tointeger(L, 2) - 1]);
    else if(lua_ismatrix(L, 1) && lua_isstring(L, 2))
    {
        lua_geti(L, LUA_REGISTRYINDEX, matrix_index);
        lua_getfield(L, -1, lua_tostring(L, 2));
        lua_remove(L, -2);
    }
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_matrix_new_index(lua_State *L)
{
    if(lua_ismatrix(L, 1) && lua_isinteger(L, 2))
        ((matrix_t *)lua_topointer(L, 1))->m[lua_tointeger(L, 2) - 1] = lua_tonumber(L, 3);
    else
        return luaL_error(L, "invalid argument type.\n");
    return 0;
}
int _G_matrix_plus(lua_State *L)
{
    if(lua_ismatrix(L, 1) && lua_ismatrix(L, 2))
        MatrixPlusMatrixP(
            lua_newmatrix(L),
            (matrix_t *) lua_topointer(L, 1),
            (matrix_t *) lua_topointer(L, 2));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_matrix_minus(lua_State *L)
{
    if(lua_ismatrix(L, 1) && lua_ismatrix(L, 2))
        MatrixMinusMatrixP(
            lua_newmatrix(L),
            (matrix_t *) lua_topointer(L, 1),
            (matrix_t *) lua_topointer(L, 2));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_matrix_times(lua_State *L)
{
    if(lua_isnumber(L, 1) && lua_ismatrix(L, 2))
    {
        MatrixTimesScalarP(
            lua_newmatrix(L),
            (matrix_t *) lua_topointer(L, 2),
            lua_tonumber(L, 1));
    }
    else if(lua_isnumber(L, 2) && lua_ismatrix(L, 1))
    {
        MatrixTimesScalarP(
            lua_newmatrix(L),
            (matrix_t *) lua_topointer(L, 1),
            lua_tonumber(L, 2));
    }
    else if(lua_ismatrix(L, 1) && lua_isvector(L, 2))
    {
        MatrixTimesVectorP(
            lua_newvector(L),
            (matrix_t *) lua_topointer(L, 1),
            (vector_t *) lua_topointer(L, 2));
    }
    else if(lua_ismatrix(L, 1) && lua_ismatrix(L, 2))
    {
        MatrixTimesMatrixP(
            lua_newmatrix(L),
            (matrix_t *) lua_topointer(L, 1),
            (matrix_t *) lua_topointer(L, 2));
    }
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_matrix_det(lua_State *L)
{
    if(lua_ismatrix(L, 1))
        lua_pushnumber(L, MatrixDeterminantP((matrix_t *) lua_topointer(L, 1)));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_matrix_trace(lua_State *L)
{
    if(lua_ismatrix(L, 1))
        lua_pushnumber(L, MatrixTraceP((matrix_t *) lua_topointer(L, 1)));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_matrix_transpose(lua_State *L)
{
    if(lua_ismatrix(L, 1))
        MatrixTransposeP(
            lua_newmatrix(L),
            (matrix_t *) lua_topointer(L, 1));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_matrix_to3x3(lua_State *L)
{
    if(lua_ismatrix(L, 1))
        MatrixTo3x3P(lua_newmatrix(L), (matrix_t *) lua_topointer(L, 1));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_matrix_to2x2(lua_State *L)
{
    if(lua_ismatrix(L, 1))
        MatrixTo2x2P(lua_newmatrix(L), (matrix_t *) lua_topointer(L, 1));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_matrix_inverse(lua_State *L)
{
    if(lua_ismatrix(L, 1))
        MatrixInverseP(
            lua_newmatrix(L),
            (matrix_t *) lua_topointer(L, 1));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_matrix_equals(lua_State *L)
{
    if(lua_ismatrix(L, 1) && lua_ismatrix(L, 2))
        lua_pushboolean(L, MatrixEqualsMatrix(
            *(matrix_t *)lua_topointer(L, 1),
            *(matrix_t *)lua_topointer(L, 2)));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_matrix_isMatrix(lua_State *L)
{
    if(lua_ismatrix(L, 1))
    {
        lua_pushboolean(L, 1);
        return 1;
    }
    else
    {
        lua_pushboolean(L, 0);
        return 1;
    }
}
int _G_matrix_toString(lua_State *L)
{
    if(lua_ismatrix(L, 1))
    {
        const char *string = MatrixToString(*(matrix_t *)lua_topointer(L, 1));
        lua_pushstring(L, string);
        free((void *)string);
    }
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}

int _G_vector_toVector(lua_State *L)
{
    if(lua_isvector(L, 1))
    {
        return 1;
    }
    else if(lua_istable(L, 1))
    {
        int len;
        lua_len(L, 1);
        len = lua_tointeger(L, -1);
        lua_pop(L, 1);
        
        vector_t *ret = lua_newvector(L);
        
        for(int i = 0; i < 4; i ++)
        {
            if(i < len)
            {
                lua_geti(L, 1, i + 1);
                if(lua_isnumber(L, -1))
                {
                    ret->m[i] = lua_tonumber(L, -1);
                    lua_pop(L, 1);
                }
                else
                    return luaL_error(L, "invalid argument type.\n");
            }
            else
                ret->m[i] = 0;
        }
        
        return 1;
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
}

int _G_vector_index(lua_State *L)
{
    if(lua_isnumber(L, 2))
    {
        lua_pushnumber(L,
            ((vector_t *)lua_topointer(L, 1))->m[lua_tointeger(L, 2) - 1]);
        return 1;
    }
    else
    {
        size_t len;
        vector_t *vec = (vector_t *)lua_topointer(L, 1);
        const char *str = lua_tolstring(L, 2, &len);
        if(len == 1)
        {
            switch(str[0])
            {
            case 'x':
                lua_pushnumber(L, vec->m[0]);
                break;
            case 'y':
                lua_pushnumber(L, vec->m[1]);
                break;
            case 'z':
                lua_pushnumber(L, vec->m[2]);
                break;
            case 'w':
                lua_pushnumber(L, vec->m[3]);
                break;
            default:
                return luaL_error(L, "invalid component specifier.\n");
            }
            return 1;
        }
        else if(str[0] >= 'w' && str[0] <= 'z')
        {
            vector_t *ret = lua_newvector(L);
            for(int i = 0; i < 4; i++)
            {
                if(i < len)
                {
                    switch(str[i])
                    {
                    case 'x':
                        ret->m[i] = vec->m[0];
                        break;
                    case 'y':
                        ret->m[i] = vec->m[1];
                        break;
                    case 'z':
                        ret->m[i] = vec->m[2];
                        break;
                    case 'w':
                        ret->m[i] = vec->m[3];
                        break;
                    default:
                        return luaL_error(L, "invalid component specifier.\n");
                    }
                }
                else
                    ret->m[i] = 0;
            }
        }
        else
        {
            lua_geti(L, LUA_REGISTRYINDEX, vector_index);
            lua_getfield(L, -1, str);
            lua_remove(L, -2);
        }
        
        return 1;
    }
}

int _G_vector_new_index(lua_State *L)
{
    if(lua_isnumber(L, 2))
    {
        ((vector_t *)lua_topointer(L, 1))->m[lua_tointeger(L, 2) - 1] = lua_tonumber(L, 3);
        return 0;
    }
    else
    {
        size_t len;
        const char *str = lua_tolstring(L, 2, &len);
        vector_t *vec = (vector_t *)lua_topointer(L, 1);
        for(int i = 0; i < len; i++)
        {
            switch(str[i])
            {
            case 'x':
                vec->m[0] = lua_tonumber(L, 3);
                break;
            case 'y':
                vec->m[1] = lua_tonumber(L, 3);
                break;
            case 'z':
                vec->m[2] = lua_tonumber(L, 3);
                break;
            case 'w':
                vec->m[3] = lua_tonumber(L, 3);
                break;
            default:
                return luaL_error(L, "invalid component specifier.\n");
            }
        }
        
        return 0;
    }
}

int _G_vector_new(lua_State *L)
{
    vector_t *ret = lua_newvector(L);
    for(int i = 1; i <= 4; i++)
    {
        if(i < lua_gettop(L))
        {
            if(lua_isnumber(L, i))
                ret->m[i - 1] = lua_tonumber(L, i);
            else
                return luaL_error(L, "invalid argument type.\n");
        }
        else
            ret->m[i - 1] = 0;
    }
    return 1;
}

int _G_vector_negate(lua_State *L)
{
    if(lua_isvector(L, 1))
        VectorNegateP(
            lua_newvector(L),
            (vector_t *) lua_topointer(L, 1));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_vector_magnitude(lua_State *L)
{
    if(lua_isvector(L, 1))
        lua_pushnumber(L, VectorMagnitudeP((vector_t *)lua_topointer(L, 1)));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_vector_homo(lua_State *L)
{
    if(lua_isvector(L, 1))
        VectorHomoP(
            lua_newvector(L),
            (vector_t *) lua_topointer(L, 1));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_vector_three(lua_State *L)
{
    if(lua_isvector(L, 1))
        VectorThreeP(
            lua_newvector(L),
            (vector_t *) lua_topointer(L, 1));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_vector_mean(lua_State *L)
{
    if(lua_isvector(L, 1) && lua_isvector(L, 2))
        VectorMeanP(
            lua_newvector(L),
            (vector_t *) lua_topointer(L, 1),
            (vector_t *) lua_topointer(L, 2));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_vector_distance(lua_State *L)
{
    if(lua_isvector(L, 1) && lua_isvector(L, 2))
        lua_pushnumber(L, VectorDistanceP(
            (vector_t *) lua_topointer(L, 1),
            (vector_t *) lua_topointer(L, 2)));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_vector_plus(lua_State *L)
{
    if(lua_isvector(L, 1) && lua_isvector(L, 2))
        VectorPlusVectorP(
            lua_newvector(L),
            (vector_t *) lua_topointer(L, 1),
            (vector_t *) lua_topointer(L, 2));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_vector_minus(lua_State *L)
{
    if(lua_isvector(L, 1) && lua_isvector(L, 2))
        VectorMinusVectorP(
            lua_newvector(L),
            (vector_t *) lua_topointer(L, 1),
            (vector_t *) lua_topointer(L, 2));
    else
        luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_vector_times(lua_State *L)
{
    if(lua_isnumber(L, 2) && lua_isvector(L, 1))
    {
        VectorTimesScalarP(
            lua_newvector(L),
            (vector_t *) lua_topointer(L, 1),
            lua_tonumber(L, 2));
    }
    else if(lua_isnumber(L, 1) && lua_isvector(L, 2))
    {
        VectorTimesScalarP(
            lua_newvector(L),
            (vector_t *) lua_topointer(L, 2),
            lua_tonumber(L, 1));
    }
    else if(lua_isvector(L, 1) && lua_isvector(L, 2))
    {
        VectorTimesVectorP(
            lua_newvector(L),
            (vector_t *) lua_topointer(L, 1),
            (vector_t *) lua_topointer(L, 2));
    }
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_vector_dot(lua_State *L)
{
    if(lua_isvector(L, 1) && lua_isvector(L, 2))
        lua_pushnumber(L, VectorDotVectorP(
            (vector_t *) lua_topointer(L, 1),
            (vector_t *) lua_topointer(L, 2)));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_vector_cross(lua_State *L)
{
    if(lua_isvector(L, 1) && lua_isvector(L, 2))
        VectorCrossVectorP(
            lua_newvector(L),
            (vector_t *) lua_topointer(L, 1),
            (vector_t *) lua_topointer(L, 2));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_vector_normalize(lua_State *L)
{
    if(lua_isvector(L, 1))
        VectorNormalizeP(
            lua_newvector(L),
            (vector_t *) lua_topointer(L, 1));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_vector_equals(lua_State *L)
{
    if(lua_isvector(L, 1) && lua_isvector(L, 2))
        lua_pushboolean(L, VectorEqualsVector(
            *(vector_t *)lua_topointer(L, 1),
            *(vector_t *)lua_topointer(L, 2)));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_vector_toString(lua_State *L)
{
    if(lua_isvector(L, 1))
    {
        const char *string = VectorToString(*(vector_t *)lua_topointer(L, 1));
        lua_pushstring(L, string);
        free((void *)string);
    }
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_vector_clamp(lua_State *L)
{
    if(lua_isvector(L, 1))
        VectorClampP(
            lua_newvector(L),
            (vector_t *) lua_topointer(L, 1));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}
int _G_vector_reflect(lua_State *L)
{
    if(lua_isvector(L, 1) && lua_isvector(L, 2))
        VectorReflectP(
            lua_newvector(L),
            (vector_t *) lua_topointer(L, 1),
            (vector_t *) lua_topointer(L, 2));
    else
        return luaL_error(L, "invalid argument type.\n");
    return 1;
}

int _G_vector_pack4(lua_State *L)
{
    int is_vector;
    vector_t *vector;
    vector = lua_tovectorptrx(L, 1, &is_vector);
    if(is_vector)
    {
        lua_pushlstring(L, (char *) vector, sizeof(vector_t));
        return 1;
    }
    else
        return luaL_error(L, "invalid arguments.");
}

int _G_vector_pack4array(lua_State *L)
{
    if(lua_istable(L, 1))
    {
        size_t capacity = 1;
        size_t index = 0;
        int is_vector;
        vector_t *vector;
        vector_t *out = (vector_t *) malloc(sizeof(vector_t));
        
        /* Iterate through the array. */
        lua_pushnil(L);
        while (lua_next(L, 1) != 0)
        {
            vector = lua_tovectorptrx(L, -1, &is_vector);
            if(is_vector)
            {
                out[index] = *vector;
                index += 1;
                if(index >= capacity)
                {
                    capacity *= 2;
                    out = (vector_t *) realloc(
                        out,
                        sizeof(vector_t) * capacity);
                }
            }
            else
                return luaL_error(L,
                    "_G_vector_pack4array array must only contain vectors.");
            lua_pop(L, 1);
        }
        lua_pushlstring(L, (char *) out, index * sizeof(vector_t));
        free(out);
        return 1;
    }
    else
        return luaL_error(L, "invalid arguments.");
}

int _G_vector_isVector(lua_State *L)
{
    if(lua_isvector(L, 1))
    {
        lua_pushboolean(L, 1);
        return 1;
    }
    else
    {
        lua_pushboolean(L, 0);
        return 1;
    }
}

static int MatrixCall(lua_State *L)
{
    if(lua_isnumber(L, -1) || lua_gettop(L) == 1)
        lua_pushcfunction(L, _G_matrix_new);
    else
        lua_pushcfunction(L, _G_matrix_toMatrix);
    lua_replace(L, 1);
    lua_call(L, lua_gettop(L) - 1, 1);
    return 1;
}

static int VectorCall(lua_State *L)
{
    if(lua_isnumber(L, -1) || lua_gettop(L) == 1)
        lua_pushcfunction(L, _G_vector_new);
    else
        lua_pushcfunction(L, _G_vector_toVector);
    lua_replace(L, 1);
    lua_call(L, lua_gettop(L) - 1, 1);
    return 1;
}

static const luaL_Reg matrix_mt_funcs[] = {
    {"__add", _G_matrix_plus},
    {"__sub", _G_matrix_minus},
    {"__mul", _G_matrix_times},
    {"__unm", _G_matrix_negate},
    {"__eq", _G_matrix_equals},
    {"__index", _G_matrix_index},
    {"__newindex", _G_matrix_new_index},
    {"__tostring", _G_matrix_toString},
    {NULL, NULL}
};

static const luaL_Reg matrix_funcs[] = {
    {"toMatrix", _G_matrix_toMatrix},
    {"new", _G_matrix_new},
    {"newTranslate", _G_matrix_newTranslate},
    {"newScale", _G_matrix_newScale},
    {"newRotate", _G_matrix_newRotate},
    {"newOrthographic", _G_matrix_newOrthographic},
    {"newFrustrum", _G_matrix_newFrustrum},
    {"newPerspective", _G_matrix_newPerspective},
    {"newLookAt", _G_matrix_newLookAt},
    {"negate", _G_matrix_negate},
    {"index", _G_matrix_index},
    {"plus", _G_matrix_plus},
    {"minus", _G_matrix_minus},
    {"times", _G_matrix_times},
    {"det", _G_matrix_det},
    {"trace", _G_matrix_trace},
    {"transpose", _G_matrix_transpose},
    {"inverse", _G_matrix_inverse},
    {"to2x2", _G_matrix_to2x2},
    {"to3x3", _G_matrix_to3x3},
    {"isMatrix", _G_matrix_isMatrix},
    {"equals", _G_matrix_equals},
    {"toString", _G_matrix_toString},
    {NULL, NULL}
};

static const luaL_Reg matrix_table_mt_funcs[] = {
    {"__call", MatrixCall},
    {NULL, NULL}
};

static const luaL_Reg vector_mt_funcs[] = {
    {"__add", _G_vector_plus},
    {"__sub", _G_vector_minus},
    {"__mul", _G_vector_times},
    {"__unm", _G_vector_negate},
    {"__eq", _G_vector_equals},
    {"__index", _G_vector_index},
    {"__newindex", _G_vector_new_index},
    {"__tostring", _G_vector_toString},
    {"__len", _G_vector_magnitude},
    {NULL, NULL}
};

static const luaL_Reg vector_funcs[] = {
    {"toVector", _G_vector_toVector},
    {"new", _G_vector_new},
    {"negate", _G_vector_negate},
    {"magnitude", _G_vector_magnitude},
    {"homo", _G_vector_homo},
    {"three", _G_vector_three},
    {"mean", _G_vector_mean},
    {"distance", _G_vector_distance},
    {"plus", _G_vector_plus},
    {"minus", _G_vector_minus},
    {"times", _G_vector_times},
    {"dot", _G_vector_dot},
    {"cross", _G_vector_cross},
    {"normalize", _G_vector_normalize},
    {"equals", _G_vector_equals},
    {"toString", _G_vector_toString},
    {"clamp", _G_vector_clamp},
    {"reflect", _G_vector_reflect},
    {"pack4", _G_vector_pack4},
    {"pack4array", _G_vector_pack4array},
    {"isVector", _G_vector_isVector},
    {NULL, NULL}
};

static const luaL_Reg vector_table_mt_funcs[] = {
    {"__call", VectorCall},
    {NULL, NULL}
};

int luaopen_matrix(lua_State *L)
{
    luaL_newlib(
        L,
        matrix_mt_funcs);
    matrix_mt_index = luaL_ref(L, LUA_REGISTRYINDEX);
    luaL_newlib(
        L,
        matrix_funcs);
    lua_pushvalue(L, -1);

    lua_pushcfunction(L, _G_matrix_new);
    lua_pushnumber(L, 1); 
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0); 
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0); 
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0); 
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 1);
    lua_call(L, 16, 1);
    lua_setfield(L, -2, "identity");
    
    lua_pushcfunction(L, _G_matrix_new);
    lua_pushnumber(L, 1); 
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0); 
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0); 
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0); 
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_call(L, 16, 1);
    lua_setfield(L, -2, "identity3");

    lua_pushcfunction(L, _G_matrix_new);
    lua_pushnumber(L, 1); 
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0); 
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0); 
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0); 
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_call(L, 16, 1);
    lua_setfield(L, -2, "identity2");

    matrix_index = luaL_ref(L, LUA_REGISTRYINDEX);
    luaL_newlib(L, matrix_table_mt_funcs);
    lua_setmetatable(L, -2);
    return 1;
}

int luaopen_vector(lua_State *L)
{
    luaL_newlib(
        L,
        vector_mt_funcs);
    vector_mt_index = luaL_ref(L, LUA_REGISTRYINDEX);
    luaL_newlib(
        L,
        vector_funcs);
    lua_pushvalue(L, -1);
    vector_index = luaL_ref(L, LUA_REGISTRYINDEX);
    luaL_newlib(L, vector_table_mt_funcs);
    lua_setmetatable(L, -2);

    lua_pushcfunction(L, _G_vector_new);
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 1);
    lua_call(L, 4, 1);
    lua_setfield(L, -2, "one");

    lua_pushcfunction(L, _G_vector_new);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_call(L, 4, 1);
    lua_setfield(L, -2, "zero");

    lua_pushcfunction(L, _G_vector_new);
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 1);
    lua_call(L, 4, 1);
    lua_setfield(L, -2, "right");

    lua_pushcfunction(L, _G_vector_new);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 1);
    lua_call(L, 4, 1);
    lua_setfield(L, -2, "up");

    lua_pushcfunction(L, _G_vector_new);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 1);
    lua_call(L, 4, 1);
    lua_setfield(L, -2, "forward");

    lua_pushcfunction(L, _G_vector_new);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 1);
    lua_call(L, 4, 1);
    lua_setfield(L, -2, "fourth");

    lua_pushcfunction(L, _G_vector_new);
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_call(L, 4, 1);
    lua_setfield(L, -2, "nright");

    lua_pushcfunction(L, _G_vector_new);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_call(L, 4, 1);
    lua_setfield(L, -2, "nup");

    lua_pushcfunction(L, _G_vector_new);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 0);
    lua_call(L, 4, 1);
    lua_setfield(L, -2, "nforward");

    lua_pushcfunction(L, _G_vector_new);
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 1);
    lua_pushnumber(L, 0);
    lua_call(L, 4, 1);
    lua_setfield(L, -2, "nfourth");
    return 1;
}


