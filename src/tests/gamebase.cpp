#include "gnid/gamebase.hpp"
#include <cassert>
#include <iostream>

using namespace std;
using namespace gnid;

static int t = 0;

class TestGame : public GameBase
{
public:

    shared_ptr<Scene> scene;

    TestGame()
        : GameBase("Test GameBase", 32, 32),
          scene(nullptr)
    {
    }

    const shared_ptr<Scene> &currentScene() const override
    {
        return scene;
    }

    void init() override
    {
        /* Init should happen before load content. */
        cout << "init() called" << endl;
        assert(t == 0);
        t ++;
    }

    void loadContent() override
    {
        /* Load content should happen before postLoadContent. */
        cout << "loadContent() called" << endl;
        assert(t == 1);
        t ++;
    }

    void postLoadContent() override
    {
        /* postLoadContent should happen after loadContent. */
        cout << "postLoadContent() called" << endl;
        assert(t == 2);
        t ++;
    }

    void update(float dt) override
    {
        cout << "update() called" << endl;
        assert(t == 3);
        t ++;
        /* Stop immediately. */
        stop();
    }
};

int main(int argc, char *argv[])
{
    TestGame game;
    game.start();
    cout << t << endl;
    assert(t == 4);
    assert(game.currentScene() == game.scene);
}

