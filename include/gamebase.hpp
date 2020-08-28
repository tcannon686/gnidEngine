#ifndef GAMEBASE_HPP
#define GAMEBASE_HPP

#include <memory>

namespace gnid
{

using namespace std;
class Scene;

/**
 * \brief The game class that all games should derive from
 *
 * \details
 *     Games Should derive from this class. Includes functions for setting up
 *     the game, as well as callback functions.
 */
class GameBase {
public:
    /**
     * \brief The width of the game window
     */
    const int clientWidth = 640;

    /**
     * \brief The height of the game window
     */
    const int clientHeight = 480;

    /**
     * \brief Returns the scene to be displayed
     */
    virtual const shared_ptr<Scene> &currentScene() const = 0;

    /**
     * \brief Initialize the game.
     *
     * \details
     *     This function is called after construction and context creation
     */
    virtual bool init()
    {
        return false;
    }

    /**
     * \brief Load the content for the game
     *
     * \details
     *     This function is called after init().
     */
    virtual bool loadContent()
    {
        return false;
    }

    /**
     * \brief Load the content for the game.
     *
     * \details
     *     This function is called after loadContent()
     */
    virtual bool postLoadContent()
    {
        return false;
    }

    /**
     * \brief Called every frame
     */
    virtual void update(float dt) {}

    /**
     * \brief This function is called whenever a key event is performed
     *
     * \details
     *     Keycodes come from GLFW. See glfw3.h for details.
     */
    virtual void keyCallback(
            int key,
            int scancode,
            int action,
            int mods) {}

    /**
     * \brief Called when a character is typed
     *
     * \details
     *     This callback should be used when taking textual input instead of
     *     keyCallback.
     */
    virtual void charCallback(
            unsigned int character) {}

    /**
     * \brief Called when the mouse is moved
     */
    virtual void mouseMoveCallback(
            double x,
            double y) {}

    /**
     * \brief Called when a mouse button is pressed
     *
     * \details
     *     See glfw3.h for details.
     */
    virtual void mouseCallback(
            int button,
            int action,
            int mods) {}

    /*
     * \brief Returns the GLFW window.
     */
    GLFWwindow *&window() { return window_; }

private:
    GLFWwindow *window_;
};

} /* namespace */

#endif /* ifndef GAMEBASE_HPP */

