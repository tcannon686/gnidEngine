#ifndef GAMEBASE_HPP
#define GAMEBASE_HPP

#include "gnid/glad/glad.h"
#include "GLFW/glfw3.h"

#include <memory>
#include <string>

/**
 * \brief The gnidEngine namespace
 */
namespace gnid
{

class Scene;

/**
 * \brief The game class that all games should derive from
 *
 * \details
 *     Games Should derive from this class. Includes functions for setting up
 *     the game, as well as callback functions.
 */
class GameBase
{
public:

    /**
     * \brief
     *     Create a new game using a window of the given width and height with
     *     the given title
     */
    GameBase(std::string title, int clientWidth, int clientHeight);

    virtual ~GameBase();

    /**
     * \brief The title of the game
     */
    const std::string &title();

    /**
     * \brief The width of the game window
     */
    const int &clientWidth();

    /**
     * \brief The height of the game window
     */
    const int &clientHeight();

    /**
     * \brief Returns the scene to be displayed
     */
    virtual const std::shared_ptr<Scene> &currentScene() const = 0;

    /**
     * \brief Initialize the game.
     *
     * \details
     *     This function is called after construction and context creation but
     *     before loadContent().
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
     * \brief
     *     Called at the beginning of every frame, before the scene is updated
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

    /**
     * \brief Set whether or not the cursor is visible
     */
    void setCursorEnabled(bool enabled);

    /**
     * \brief Start the game loop
     */
    void start();

    /**
     * \brief Stop the game loop
     */
    void stop();

    /**
     * \brief Perform one iteration of the game loop
     */
    void spinOnce();

    GLFWwindow *window();

private:
    GLFWwindow *window_;
    const int clientWidth_;
    const int clientHeight_;
    const std::string title_;

    bool shouldStop_ = false;

    double time_;

    static void keyCallback(
            GLFWwindow* window,
            int key,
            int scancode,
            int action,
            int mods);

    static void charCallback(
            GLFWwindow* window,
            unsigned int character);

    static void mouseMoveCallback(
            GLFWwindow* window,
            double x,
            double y);

    static void mouseCallback(
            GLFWwindow* window,
            int button,
            int action,
            int mods);

};

} /* namespace */

#endif /* ifndef GAMEBASE_HPP */

