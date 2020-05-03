#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>
#include "matrix/matrix.hpp"
#include "node.hpp"
#include "renderernode.hpp"
#include "scene.hpp"
#include "phongshader.hpp"

using namespace std;

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

float vertices[]{
    0, 0, -2,
    1, 0, -2,
    0, 1, -2
};

unsigned int indices[]{
    0, 1, 2
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
        cerr << "error: glad error." << endl;
        return -2;
    }

	cout << "info: OpenGL " << GLVersion.major << "." << GLVersion.minor << endl;

    shared_ptr<PhongShader> phongshader = make_shared<PhongShader>();
    phongshader->init();

    // Generate simple mesh
    GLuint vao, vbo, ibo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    assert(vao != 0);
    assert(vbo != 0);
    assert(ibo != 0);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices, GL_STATIC_DRAW);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices, GL_STATIC_DRAW);
    glVertexAttribPointer(
        ShaderProgram::ATTRIB_LOCATION_VERTEX,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        nullptr);
    glEnableVertexAttribArray(1);

    shared_ptr<RendererMesh> renderMesh = make_shared<RendererMesh>(
        GL_TRIANGLES,
        3,
        GL_UNSIGNED_INT,
        vao);
    shared_ptr<PhongMaterial> mat = make_shared<PhongMaterial>(phongshader);

    double time;
    shared_ptr<Scene> scene = make_shared<Scene>();
    scene->init();
    scene->root->add(make_shared<Camera>(
                (float) M_PI / 4.0f, 640.0f / 480.0f, 0.1f, 100.0f));
    scene->root->add(make_shared<RendererNode>(
                renderMesh, mat));

	while(!glfwWindowShouldClose(window))
	{
        float dt;
        GLenum error;

        dt = glfwGetTime() - time;
        time += dt;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene->update(dt);
        scene->updateWorldMatrix();
        scene->render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        while((error = glGetError()) != GL_NO_ERROR)
        {
            cerr << "error: OpenGL error " << hex << error << endl;
        }
	}
    
    cout << "info: Done." << endl;
	
	glfwTerminate();
}
