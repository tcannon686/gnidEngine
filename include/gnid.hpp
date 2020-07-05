#ifndef GNID_HPP
#define GNID_HPP

#include <unordered_map>

class GLFWwindow;

namespace gnid
{
class GameBase;

class Gnid
{
public:
    static int start(GameBase &game);
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
