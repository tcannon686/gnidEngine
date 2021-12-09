#ifndef OBJPARSER_HPP
#define OBJPARSER_HPP

#include <iostream>
#include <unordered_map>
#include <memory>
#include <cassert>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>

#include "gnid/shader.hpp"
#include "gnid/renderer.hpp"
#include "gnid/renderernode.hpp"
#include "gnid/spatialnode.hpp"
#include "gnid/collider.hpp"
#include "gnid/hull.hpp"
#include "gnid/node.hpp"
#include "gnid/matrix/matrix.hpp"

namespace gnid
{

/**
 * \brief A template based Wavefront OBJ parser
 *
 */
class ObjParser
{
public:
    /**
     * \brief Create an OBJ parser for the given stream
     */
    ObjParser(std::istream &stream) : stream(stream) {}

    /**
     * \brief Parse mesh data from the stream
     */
    void parse();

    /**
     * \brief Create a render node from the parsed data
     *
     * \details
     *     This method must be called after parse is called.
     *
     * \tparam Shader   The type of shader to use for materials
     * \tparam Material The type of materials to use
     *
     * \param shader    The shader to use for the materials
     */
    template<typename Material>
    std::shared_ptr<Node> buildRendererNode(std::shared_ptr<Material> material);
    std::shared_ptr<Node> buildPhysicsNode();

private:
    std::istream &stream;

    std::vector<tmat::Vector4f> vertices;
    std::vector<tmat::Vector3f> normals;
    std::vector<tmat::Vector2f> texCos;
    std::vector<int> vIndices;
    std::vector<int> nIndices;
    std::vector<int> tIndices;
    bool smooth;
    bool done;

    int line = 1;

    typedef enum
    {
        COMMENT,
        VERTEX,
        NORMAL,
        TEXCO,
        FACE,
        USEMTL,
        MTLLIB,
        OBJECT,
        GROUP,
        SHADE,
        INDEX_SEPARATOR,
        NUMBER,
        END_OF_FILE,
        UNKNOWN,
        STRING,
        END_OF_LINE,
        ON,
        OFF
    } TokenType;

    static const std::string tokenStrings[];

    std::string token;
    std::string next_token;
    TokenType token_type;

    const std::string COMMENT_STR = "#";
    const std::string VERTEX_STR = "v";
    const std::string NORMAL_STR = "vn";
    const std::string TEXCO_STR = "vt";
    const std::string FACE_STR = "f";
    const std::string USEMTL_STR = "usemtl";
    const std::string MTLLIB_STR = "mtllib";
    const std::string OBJECT_STR = "o";
    const std::string GROUP_STR = "g";
    const std::string SHADE_STR = "s";
    const std::string INDEX_SEPARATOR_STR = "/";
    const std::string ON_STR = "on";
    const std::string OFF_STR = "off";

    /* Read the next token from the input. */
    void nextToken();

    /**
     * \brief Parse from the current position to the end of the line
     */
    void parseString();

    bool accept(TokenType type);
    bool expect(TokenType type);

    bool parseVertex();
    bool parseNormal();
    bool parseTexCo();
    bool parseFace();
    bool parseUseMtl();
    bool parseMtlLib();
    bool parseObject();
    bool parseGroup();
    bool parseShade();

    std::ostream &error() const;
    std::ostream &warning() const;
};

template<typename Material>
std::shared_ptr<Node> ObjParser::buildRendererNode(
        std::shared_ptr<Material> material)
{
    assert(done);
    /* Change the separate indices to one set of vertex indices. */

    /* Maps old vIndices, tIndices, nIndices tuple to new index. */
    std::unordered_map<std::string, unsigned int> index_table;

    /* Vertex array buffer */
    std::vector<unsigned int> index_array;

    /* Vertex buffer format: vvvttnnn */
    std::vector<float> data;
    int cur_index = 0;

    for(std::size_t i = 0; i < vIndices.size(); i ++)
    {
        std::string table_index =
            std::to_string(vIndices[i])
            + "/" + (tIndices.size() > 0 ?
                    std::to_string(tIndices[i])
                    : "")
            + "/" + (nIndices.size() > 0 ?
                    std::to_string(nIndices[i])
                    : "");

        auto in_table = index_table.find(table_index);

        /* If the vertex has not been added yet, add it. */
        if(in_table == end(index_table))
        {
            /* Add the data. */
            for(int j = 0; j < 3; j ++)
                data.push_back(vertices[vIndices[i]][j]);
            if(tIndices.size() > 0)
            {
                for(int j = 0; j < 2; j ++)
                    data.push_back(texCos[tIndices[i]][j]);
            }
            if(nIndices.size() > 0)
            {
                for(int j = 0; j < 3; j ++)
                    data.push_back(normals[nIndices[i]][j]);
            }

            index_table[table_index] = cur_index;
            cur_index ++;
        }
        index_array.push_back(index_table[table_index]);
    }

    const std::size_t vertex_size = 3 * sizeof(float);
    const std::size_t normal_size = (
            nIndices.size() > 0? 3 * sizeof(float) : 0);
    const std::size_t texco_size = (
            tIndices.size() > 0? 2 * sizeof(float) : 0);
    const std::size_t stride = vertex_size + normal_size + texco_size;

    GLuint vao, vbo, ibo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    /* TODO throw exception. */
    assert(vao != 0);
    assert(vbo != 0);
    assert(ibo != 0);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glBufferData(
        GL_ARRAY_BUFFER,
        data.size() * sizeof(float),
        data.data(), GL_STATIC_DRAW);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        index_array.size() * sizeof(unsigned int),
        index_array.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(
        ShaderProgram::ATTRIB_LOCATION_VERTEX,
        3,
        GL_FLOAT,
        GL_FALSE,
        stride,
        nullptr);

    glEnableVertexAttribArray(ShaderProgram::ATTRIB_LOCATION_VERTEX);

    if(tIndices.size() > 0)
    {
        glVertexAttribPointer(
            ShaderProgram::ATTRIB_LOCATION_TEXCO,
            2,
            GL_FLOAT,
            GL_FALSE,
            stride,
            (void *)(vertex_size));

        glEnableVertexAttribArray(ShaderProgram::ATTRIB_LOCATION_TEXCO);
    }

    if(nIndices.size() > 0)
    {
        glVertexAttribPointer(
            ShaderProgram::ATTRIB_LOCATION_NORMAL,
            3,
            GL_FLOAT,
            GL_FALSE,
            stride,
            (void *)(vertex_size + texco_size));

        glEnableVertexAttribArray(ShaderProgram::ATTRIB_LOCATION_NORMAL);
    }

    std::shared_ptr<RendererMesh> renderMesh = std::make_shared<RendererMesh>(
        GL_TRIANGLES,
        index_array.size(),
        GL_UNSIGNED_INT,
        vao);
    return std::make_shared<RendererNode>(renderMesh, material);
}

} /* namespace */

#endif /* OBJPARSER_HPP */
