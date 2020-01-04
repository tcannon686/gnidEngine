
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "matrix/matrix.h"
#include "hashtable.h"
#include "linkedlist.h"
#include "node.h"
#include "root_node.h"
#include "camera_node.h"

static GLFWwindow *window;
static int window_width;
static int window_height;

static void KeyCallback(
        GLFWwindow* window,
        int key,
        int scancode,
        int action,
        int mods)
{
    // pass
}

static void CharCallback(
        GLFWwindow* window,
        unsigned int character)
{
    // pass
}

static void MouseMoveCallback(
        GLFWwindow* window,
        double x,
        double y)
{
    // pass
}

static void MouseCallback(
        GLFWwindow* window,
        int button,
        int action,
        int mods)
{
    // pass
}

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


    // Init scene.
    node_t *root_node = RootNodeNew();
    root_node->references += 1;
    node_t *camera_node = CameraNodeNew();
    NodeAdd(root_node, camera_node);
	
	while(!glfwWindowShouldClose(window))
	{
        GLenum error;

        NodeFixedUpdateAll(root_node);
        NodeUpdateAll(root_node);

        RootNodeRender((root_node_t *) root_node);

        glfwSwapBuffers(window);
        glfwPollEvents();

        while((error = glGetError()) != GL_NO_ERROR)
        {
            fprintf(stderr, "error: OpenGL error %x.\n", error);
        }
	}

    printf("info: Destroying root node.\n");
    root_node->references -= 1;
    NodeDestroy(root_node);
    
    printf("info: Done.\n");
	
	glfwTerminate();
}

