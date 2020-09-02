#include "gnid/gamebase.hpp"
#include <iostream>

#include "gnid/glad/glad.h"
#include "GLFW/glfw3.h"

#include "gnid/scene.hpp"

using namespace std;
using namespace gnid;

GameBase::GameBase(string title, int clientWidth, int clientHeight)
    : clientWidth_(clientWidth),
      clientHeight_(clientHeight),
      title_(title)
{
    /* Set up GLFW. */
	if(!glfwInit())
    {
        /* TODO throw exception. */
        return;
    }

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_RESIZABLE, 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(
            clientWidth_,
            clientHeight_,
            title_.c_str(),
            NULL,
            NULL);

    if(!window_)
    {
        /* TODO throw exception. */
        cerr << "Failed to create window." << endl;
        return;
    }

    glfwSetWindowUserPointer(window_, static_cast<void *>(this));

	glfwMakeContextCurrent(window_);

	glfwSetKeyCallback(window_,         &GameBase::keyCallback);
	glfwSetCharCallback(window_,        &GameBase::charCallback);
	glfwSetMouseButtonCallback(window_, &GameBase::mouseCallback);
	glfwSetCursorPosCallback(window_,   &GameBase::mouseMoveCallback);

    if(!gladLoadGL())
    {
        /* TODO throw exception. */
        cerr << "error: glad error." << endl;
        return;
    }
}

void GameBase::start()
{
    double time;

    init();
    loadContent();
    postLoadContent();

    glEnable(GL_DEPTH_TEST);
	while(!glfwWindowShouldClose(window_))
	{
        float dt;
        GLenum error;

        dt = glfwGetTime() - time;
        time += dt;

        glClearColor(.25f, .5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        update(dt);
        currentScene()->update(dt);
        currentScene()->render();

        glfwSwapBuffers(window_);
        glfwPollEvents();

        while((error = glGetError()) != GL_NO_ERROR)
        {
            cerr << "error: OpenGL error " << hex << error << endl;
        }
	}

    glfwDestroyWindow(window_);
}

const string &GameBase::title()
{
    return title_;
}

const int &GameBase::clientWidth()
{
    return clientWidth_;
}

const int &GameBase::clientHeight()
{
    return clientHeight_;
}

void GameBase::setCursorEnabled(bool enabled)
{
    if(enabled)
    {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else
    {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void GameBase::keyCallback(
        GLFWwindow* window,
        int key,
        int scancode,
        int action,
        int mods)
{
    GameBase *game = static_cast<GameBase *>(glfwGetWindowUserPointer(window));
    game->keyCallback(key, scancode, action, mods);
}

void GameBase::charCallback(
        GLFWwindow* window,
        unsigned int character)
{
    GameBase *game = static_cast<GameBase *>(glfwGetWindowUserPointer(window));
    game->charCallback(character);
}

void GameBase::mouseMoveCallback(
        GLFWwindow* window,
        double x,
        double y)
{
    GameBase *game = static_cast<GameBase *>(glfwGetWindowUserPointer(window));
    game->mouseMoveCallback(x, y);
}

void GameBase::mouseCallback(
        GLFWwindow* window,
        int button,
        int action,
        int mods)
{
    GameBase *game = static_cast<GameBase *>(glfwGetWindowUserPointer(window));
    game->mouseCallback(window, button, action, mods);
}

GLFWwindow *GameBase::window()
{
    return window_;
}
