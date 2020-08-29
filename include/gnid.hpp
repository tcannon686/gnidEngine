#ifndef GNID_HPP
#define GNID_HPP

class GLFWwindow;

/**
 * \brief The gnidEngine namespace
 *
 * \details
 *     gnidEngine is a simple game engine I am developing. The goals for the
 *     engine are fast runtime on cheaper hardware, and clean well documented
 *     code.
 */
namespace gnid
{
class GameBase;

/**
 * \brief The starting point for the engine
 */
class Gnid
{
public:
    /**
     * \brief Start the game loop for the given game
     */
    static int start(GameBase &game);

    /**
     * \brief Enable or disable the mouse cursor
     */
    static void setCursorEnabled(bool enabled);
private:
    static GameBase *game;
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

    Gnid();
    ~Gnid();
};
} /* namespace */

#endif /* ifndef GNID_HPP */