
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdlib.h>

#include "lodepng.h"
#include "luapng.h"

// decode(encoded)
int _G_png_decode(lua_State *L)
{
    const char *encoded;
    size_t len_encoded;
    unsigned char* image;
    unsigned int width, height;
    unsigned int error;

    image = NULL;

    encoded = lua_tolstring(L, 1, &len_encoded);
    if(encoded == NULL)
        return luaL_error(L, "_G_png_decode expects string encoded.");
    
    error = lodepng_decode32(&image, &width, &height, encoded, len_encoded);
    if(error)
        return luaL_error(L, "_G_png_decode LodePNG error %u: %s\n",
            error,
            lodepng_error_text(error));
    lua_newtable(L);
    lua_pushlstring(L, (char *) image, width * height * 4);
    lua_setfield(L, -2, "data");
    lua_pushinteger(L, (lua_Integer) width);
    lua_setfield(L, -2, "width");
    lua_pushinteger(L, (lua_Integer) height);
    lua_setfield(L, -2, "height");
    free(image);
    return 1;
}

int _G_png_flipY(lua_State *L)
{
    unsigned char *data;
    unsigned char *flipped;
    unsigned int width, height;
    int isnum;
    int i, j, k;

    if(!lua_istable(L, 1))
        return luaL_error(L, "_G_png_flipY expects table image.");
    
    lua_getfield(L, 1, "width");
    width = (unsigned int) lua_tonumberx(L, -1, &isnum);
    lua_pop(L, 1);
    if(!isnum)
        return luaL_error(L, "_G_png_flipY expects table with integer width.");

    lua_getfield(L, 1, "height");
    height = (unsigned int) lua_tonumberx(L, -1, &isnum);
    lua_pop(L, 1);
    if(!isnum)
        return luaL_error(L, "_G_png_flipY expects table with integer height.");

    // Keep the data on the stack.
    lua_getfield(L, 1, "data");
    data = (unsigned char *) lua_tostring(L, -1);
    if(data == NULL)
        return luaL_error(L, "_G_png_flipY expects table with string data.");
    
    flipped = (unsigned char *) malloc(width * height * 4);
    for(i = 0; i < width; i ++)
    {
        for(j = 0; j < height; j ++)
        {
            for(k = 0; k < 4; k ++)
            {
                flipped[(i + j * width) * 4 + k] =
                    data[(i + (height - j - 1) * width)  * 4 + k];
            }
        }
    }
    lua_newtable(L);
    lua_pushlstring(L, (const char *) flipped, width * height * 4);
    lua_setfield(L, -2, "data");
    lua_pushinteger(L, (lua_Integer) width);
    lua_setfield(L, -2, "width");
    lua_pushinteger(L, (lua_Integer) height);
    lua_setfield(L, -2, "height");
    free(flipped);
    return 1;
}


static const luaL_Reg png_funcs[] = {
    { "decode", _G_png_decode },
    { "flipY", _G_png_flipY },
    { NULL, NULL }
};

int luaopen_png(lua_State *L)
{
    luaL_newlib(L, png_funcs);
    return 1;
}
