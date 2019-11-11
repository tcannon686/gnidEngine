
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "matrix.h"
#include "luamatrix.h"

#include "luagl.h"
#include "luamath3d.h"
#include "luapng.h"

static GLFWwindow *window;
static int window_width;
static int window_height;
static int lua_reg_callbacks = 0;
static lua_State *L;

static int _G_window_close(lua_State *L)
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return 0;
}

static int LuaWindowIndex(lua_State *L)
{
	if(lua_isstring(L, 2))
	{
		const char *str = lua_tostring(L, 2);
		if(strcmp(str, "width") == 0)
		{
			int width;
			glfwGetWindowSize(window, &width, NULL);
			lua_pushinteger(L, width);
			return 1;
		}
		else if(strcmp(str, "height") == 0)
		{
			int height;
			glfwGetWindowSize(window, NULL, &height);
			lua_pushinteger(L, height);
			return 1;
		}
		else if(strcmp(str, "callbacks") == 0)
		{
			if(lua_reg_callbacks != 0)
				lua_geti(L, LUA_REGISTRYINDEX, lua_reg_callbacks);
			else
				lua_pushnil(L);
			return 1;
		}
        else if(strcmp(str, "close") == 0)
        {
            lua_pushcfunction(L, _G_window_close);
            return 1;
        }
        else if(strcmp(str, "cursorMode") == 0)
        {
            lua_pushinteger(L, glfwGetInputMode(window, GLFW_CURSOR));
            return 1;
        }
        else if(strcmp(str, "time") == 0)
        {
            lua_pushnumber(L, glfwGetTime());
            return 1;
        }
	}

    return 0;
}

static int LuaWindowNewIndex(lua_State *L)
{
	if(lua_isstring(L, 2))
	{
		const char *str = lua_tostring(L, 2);
		if(strcmp(str, "width") == 0)
		{
            window_width = (int) lua_tointeger(L, 3);
			glfwSetWindowSize(
				window,
				window_width,
				window_height);
            glViewport(0, 0, window_width, window_height);
			return 0;
		}
		else if(strcmp(str, "height") == 0)
		{
            window_height = lua_tointeger(L, 3);
			glfwSetWindowSize(
				window,
			    window_width,
				window_height);
            glViewport(0, 0, window_width, window_height);
			return 0;
		}
		else if(strcmp(str, "callbacks") == 0)
		{
			return 0;
		}
        else if(strcmp(str, "cursorMode") == 0)
        {
            glfwSetInputMode(window, GLFW_CURSOR, lua_tointeger(L, 3));
            return 0;
        }
        else
        {
            return luaL_error(L, "Cannot set window.%s", str);
        }
	}
    else
    {
        return luaL_error(
            L,
            "Trying to index window with a key that is not a string.");
    }
}

static void KeyCallback(GLFWwindow* window,
                        int key,
                        int scancode,
                        int action,
                        int mods)
{
	if(lua_reg_callbacks != 0)
	{
		lua_geti(L, LUA_REGISTRYINDEX, lua_reg_callbacks);
        lua_getfield(L, -1, "key");
		if(lua_isnil(L, -1))
		{

			lua_pop(L, 2);
			return;
		}
		lua_pushinteger(L, key);
		lua_pushinteger(L, scancode);
		lua_pushinteger(L, action);
		lua_pushinteger(L, mods);
		lua_call(L, 4, 0);
        lua_pop(L, 1);
	}
}

static void CharCallback(GLFWwindow* window,
                         unsigned int character)
{
	if(lua_reg_callbacks != 0)
	{
		lua_geti(L, LUA_REGISTRYINDEX, lua_reg_callbacks);
        lua_getfield(L, -1, "char");
		if(lua_isnil(L, -1))
		{

			lua_pop(L, 2);
			return;
		}
		lua_pushinteger(L, character);
		lua_call(L, 1, 0);
        lua_pop(L, 1);
	}
}

static void MouseMoveCallback(GLFWwindow* window,
                              double x, double y)
{
    if(lua_reg_callbacks != 0)
    {
        lua_geti(L, LUA_REGISTRYINDEX, lua_reg_callbacks);
        lua_getfield(L, -1, "mouseMove");
        if(lua_isnil(L, -1))
        {
            lua_pop(L, 2);
            return;
        }
        lua_pushnumber(L, (lua_Number) x);
        lua_pushnumber(L, (lua_Number) y);
        lua_call(L, 2, 0);
        lua_pop(L, 1);
    }
}

static void MouseCallback(GLFWwindow* window,
                          int button,
                          int action,
                          int mods)
{
    if(lua_reg_callbacks != 0)
    {
        lua_geti(L, LUA_REGISTRYINDEX, lua_reg_callbacks);
        lua_getfield(L, -1, "mouse");
        if(lua_isnil(L, -1))
        {
            lua_pop(L, 2);
            return;
        }
        lua_pushinteger(L, (lua_Integer) button);
        lua_pushinteger(L, (lua_Integer) action);
        lua_pushinteger(L, (lua_Integer) mods);
        lua_call(L, 3, 0);
        lua_pop(L, 1);
    }
}

static luaL_Reg window_mt_funcs[] = {
	{ "__index", LuaWindowIndex },
	{ "__newindex", LuaWindowNewIndex },
	{ NULL, NULL }
};

int main(int argc, char **argv)
{

	if(!glfwInit())
		return -1;
	
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_RESIZABLE, 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_width = 640;
    window_height = 480;
    window = glfwCreateWindow(window_width, window_height, "Game", NULL, NULL);
    if(!window)
	{
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCharCallback(window, CharCallback);
	glfwSetMouseButtonCallback(window, MouseCallback);
	glfwSetCursorPosCallback(window, MouseMoveCallback);
    
    if(!gladLoadGL())
    {
        fprintf(stderr, "error: glad error.");
        return -2;
    }

	printf("info: OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);

	L = luaL_newstate();
	
	luaL_openlibs(L);
	
	luaopen_matrix(L);
	lua_setglobal(L, "Matrix");
	
	luaopen_vector(L);
	lua_setglobal(L, "Vector");

    luaopen_gl(L);
    lua_setglobal(L, "gl");

    luaopen_math3d(L);
    lua_setglobal(L, "math3d");

    luaopen_png(L);
    lua_setglobal(L, "png");
	
	lua_pushlightuserdata(L, window);
	luaL_newlib(L, window_mt_funcs);
	lua_setmetatable(L, -2);
	lua_setglobal(L, "window");

    // Set the callbacks to an empty table.
    lua_newtable(L);
    lua_reg_callbacks = luaL_ref(L, LUA_REGISTRYINDEX);
	
	luaL_loadfile(L, "game.lua");
	if(lua_pcall(L, 0, 0, 0) != LUA_OK)
	{
		fprintf(stderr, "error: lua: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
        return -3;
	}
	
    glClearColor(0, 0.5f, 1.0f, 1);
	while(!glfwWindowShouldClose(window))
	{
        GLenum error;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        lua_pushlightuserdata(L, window);
        lua_getfield(L, -1, "callbacks");
        lua_getfield(L, -1, "update");
        if(!lua_isnil(L, -1))
            if(lua_pcall(L, 0, 0, 0) != LUA_OK)
            {
                fprintf(stderr, "error: lua: %s\n", lua_tostring(L, -1));
            }
        lua_pop(L, 1);

        glfwSwapBuffers(window);
        glfwPollEvents();

        while((error = glGetError()) != GL_NO_ERROR)
        {
            fprintf(stderr, "error: OpenGL error %x.\n", error);
        }
	}
    
    printf("info: Done.\n");
	
	glfwTerminate();
}

