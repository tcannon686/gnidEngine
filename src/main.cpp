#include <cassert>

#include <dlfcn.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>
#include <fstream>
#include "scene.hpp"

#include "gamebase.hpp"

#include "gnid.hpp"

using namespace gnid;
using namespace std;

GameBase *Gnid::game = nullptr;

static GLFWwindow *window;
static int window_width;
static int window_height;

void Gnid::setCursorEnabled(bool enabled)
{
    if(enabled)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void Gnid::keyCallback(
        GLFWwindow* window,
        int key,
        int scancode,
        int action,
        int mods)
{
    game->keyCallback(key, scancode, action, mods);
}

void Gnid::charCallback(
        GLFWwindow* window,
        unsigned int character)
{
    game->charCallback(character);
}

void Gnid::mouseMoveCallback(
        GLFWwindow* window,
        double x,
        double y)
{
    game->mouseMoveCallback(x, y);
}

void Gnid::mouseCallback(
        GLFWwindow* window,
        int button,
        int action,
        int mods)
{
    // pass
}

int Gnid::start(GameBase &game)
{
	if(!glfwInit())
		return -1;

    Gnid::game = &game;
	
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_RESIZABLE, 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_width = game.clientWidth;
    window_height = game.clientHeight;
    window = glfwCreateWindow(window_width, window_height, "Game", NULL, NULL);
    if(!window)
	{
		glfwTerminate();
		return -1;
	}

    game.window() = window;
	
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, &Gnid::keyCallback);
	glfwSetCharCallback(window, &Gnid::charCallback);
	glfwSetMouseButtonCallback(window, &Gnid::mouseCallback);
	glfwSetCursorPosCallback(window, &Gnid::mouseMoveCallback);
    
    if(!gladLoadGL())
    {
        cerr << "error: glad error." << endl;
        return -2;
    }

	cout << "info: OpenGL " << GLVersion.major << "." << GLVersion.minor << endl;

    assert(game.init());
    assert(game.loadContent());
    assert(game.postLoadContent());

    double time;

    glEnable(GL_DEPTH_TEST);
	while(!glfwWindowShouldClose(window))
	{
        float dt;
        GLenum error;

        dt = glfwGetTime() - time;
        time += dt;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        game.update(dt);
        game.currentScene()->update(dt);
        game.currentScene()->updateWorldMatrix();
        game.currentScene()->render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        while((error = glGetError()) != GL_NO_ERROR)
        {
            cerr << "error: OpenGL error " << hex << error << endl;
        }
	}

    cout << "info: Done." << endl;
	
	glfwTerminate();
    return 0;
}

