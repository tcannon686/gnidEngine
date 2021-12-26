#include "gnid/phongshader.hpp"
#include "gnid/gamebase.hpp"
#include "gnid/matrix/matrix.hpp"

#include <cassert>
#include <iostream>

using namespace tmat;
using namespace std;
using namespace gnid;

static int t = 0;
static shared_ptr<PhongShader> phongShader;
static shared_ptr<PhongMaterial> phongMaterial;

class TestGame : public GameBase
{
public:

    shared_ptr<Scene> scene;

    TestGame()
        : GameBase("Test Phong", 32, 32),
          scene(nullptr)
    {
    }

    const shared_ptr<Scene> &currentScene() const override
    {
        return scene;
    }

    void init() override
    {
        cout << "init() called" << endl;
        phongShader = make_shared<PhongShader>();
        phongShader->init();
    }

    void loadContent() override
    {
        cout << "loadContent() called" << endl;
        phongMaterial = PhongMaterial::Builder()
                .shader(phongShader)
                .diffuse(1, 0, 0)
                .specular(0, 1, 0)
                .shininess(100.0f)
                .build();
    }

    void postLoadContent() override
    {
        /* postLoadContent should happen after loadContent. */
        cout << "postLoadContent() called" << endl;
    }

    void update(float dt) override
    {
        cout << "update() called" << endl;
        
        phongShader->use();
        phongMaterial->bind();

        /* Stop immediately. */
        stop();
    }
};

int main(int argc, char *argv[])
{
    TestGame game;
    game.start();
}

