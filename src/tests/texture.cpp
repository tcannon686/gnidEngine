#include "gnid/texture.hpp"

using namespace std;
using namespace gnid;

int main(int argc, char **argv)
{
    vector<GLuint> data;
    data.push_back(0xFF0000FF);
    Texture<2, GLuint> texture(1, 1, GL_RGBA, data);
}
