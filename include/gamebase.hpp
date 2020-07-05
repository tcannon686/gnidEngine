#ifndef GAMEBASE_HPP
#define GAMEBASE_HPP

#include <memory>

namespace gnid
{

using namespace std;
class Scene;

/*
 * Games Should derive from this class. Includes functions for setting up the
 * game.
 */
class GameBase {
public:
    const int clientWidth = 640;
    const int clientHeight = 480;

    /* Returns the scene to be displayed. */
    virtual const shared_ptr<Scene> &currentScene() const = 0;

    /* Initialize the game. Called after construction and context creation. */
    virtual bool init()
    {
        return false;
    }

    /* Load the content for the game. Called after init(). */
    virtual bool loadContent()
    {
        return false;
    }

    /* Load the content for the game. Called after loadContent(). */
    virtual bool postLoadContent()
    {
        return false;
    }

    /* Called every frame. */
    virtual void update(float dt) {}

    virtual void keyCallback(
            int key,
            int scancode,
            int action,
            int mods) {}

    virtual void charCallback(
            unsigned int character) {}

    virtual void mouseMoveCallback(
            double x,
            double y) {}

    virtual void mouseCallback(
            int button,
            int action,
            int mods) {}

    /* Returns the GLFW window. */
    GLFWwindow *&window() { return window_; }

private:
    GLFWwindow *window_;
};

} /* namespace */

#endif /* ifndef GAMEBASE_HPP */

