#include "gnid/objparser.hpp"
#include "gnid/gamebase.hpp"
#include "gnid/matrix/matrix.hpp"
#include "gnid/node.hpp"
#include "gnid/phongshader.hpp"
#include <sstream>
#include <unordered_map>

using namespace std;
using namespace gnid;
using namespace tmat;

static const char *obj = R"END(
# Blender v2.93.3 OBJ File: ''
# www.blender.org
mtllib Cube.mtl
o Cube
v 1.000000 1.000000 -1.000000
v 1.000000 -1.000000 -1.000000
v 1.000000 1.000000 1.000000
v 1.000000 -1.000000 1.000000
v -1.000000 1.000000 -1.000000
v -1.000000 -1.000000 -1.000000
v -1.000000 1.000000 1.000000
v -1.000000 -1.000000 1.000000
vt 0.625000 0.500000
vt 0.875000 0.500000
vt 0.875000 0.750000
vt 0.625000 0.750000
vt 0.375000 0.750000
vt 0.625000 1.000000
vt 0.375000 1.000000
vt 0.375000 0.000000
vt 0.625000 0.000000
vt 0.625000 0.250000
vt 0.375000 0.250000
vt 0.125000 0.500000
vt 0.375000 0.500000
vt 0.125000 0.750000
vn 0.0000 1.0000 0.0000
vn 0.0000 0.0000 1.0000
vn -1.0000 0.0000 0.0000
vn 0.0000 -1.0000 0.0000
vn 1.0000 0.0000 0.0000
vn 0.0000 0.0000 -1.0000
usemtl Material
s off
f 1/1/1 5/2/1 7/3/1 3/4/1
f 4/5/2 3/4/2 7/6/2 8/7/2
f 8/8/3 7/9/3 5/10/3 6/11/3
f 6/12/4 2/13/4 4/5/4 8/14/4
f 2/13/5 1/1/5 3/4/5 4/5/5
f 6/11/6 5/10/6 1/1/6 2/13/6
)END";

static shared_ptr<Node> rendererNode;
static shared_ptr<Node> physicsNode;
static shared_ptr<PhongShader> phongShader;
static shared_ptr<PhongMaterial> phongMaterial;

class TestGame : public GameBase
{
public:

    shared_ptr<Scene> scene;

    TestGame()
        : GameBase("Test ObjParser", 32, 32),
          scene(nullptr)
    {
    }

    const shared_ptr<Scene> &currentScene() const override
    {
        return scene;
    }

    bool init() override
    {
        cout << "init() called" << endl;
        phongShader = make_shared<PhongShader>();
        phongShader->init();
        return true;
    }

    bool loadContent() override
    {
        cout << "loadContent() called" << endl;
        phongMaterial = make_shared<PhongMaterial>(
                phongShader,
                Vector3f { 1, 0, 0 },
                Vector3f { 0, 1, 0 },
                100.0f);
        stringstream stream;
        stream << obj;
        ObjParser parser(stream);
        parser.parse();
        parser.buildRendererNode(phongMaterial);
        parser.buildRendererNode({
            { "Material", phongMaterial }
        });
        return true;
    }

    bool postLoadContent() override
    {
        /* postLoadContent should happen after loadContent. */
        cout << "postLoadContent() called" << endl;
        return true;
    }

    void update(float dt) override
    {
        cout << "update() called" << endl;
        /* Stop immediately. */
        stop();
    }
};

int main(int argc, char *argv[])
{
    TestGame game;
    game.start();
}
