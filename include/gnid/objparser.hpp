#ifndef OBJPARSER_HPP
#define OBJPARSER_HPP

#include <iostream>
#include <memory>
#include <cassert>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>

#include "gnid/material.hpp"
#include "gnid/shader.hpp"
#include "gnid/renderer.hpp"
#include "gnid/renderernode.hpp"
#include "gnid/emptynode.hpp"
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
     * \param material  The material to use for the model
     */
    std::shared_ptr<Node> buildRendererNode(std::shared_ptr<Material> material);

    /**
     * \brief Create a render node from the parsed data
     *
     * \details
     *     This method must be called after parse is called.
     *
     * \param materials A map from material name to material
     */
    std::shared_ptr<Node> buildRendererNode(const MaterialMapping &materials);

    /**
     * \brief Create a physics node from the triangles in the parsed data
     *
     * \details
     *     This method must be called after parse is called.
     */
    std::shared_ptr<Node> buildPhysicsNode();

private:
    std::istream &stream;

    class Mesh
    {
    public:
        std::string material;
        std::vector<int> vIndices;
        std::vector<int> nIndices;
        std::vector<int> tIndices;
    };

    std::vector<tmat::Vector4f> vertices;
    std::vector<tmat::Vector3f> normals;
    std::vector<tmat::Vector2f> texCos;
    std::vector<Mesh> meshes;

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

    Mesh &mesh();

    std::shared_ptr<Node> buildRendererNode(
            const Mesh &mesh,
            std::shared_ptr<Material> material);
};


} /* namespace */

#endif /* OBJPARSER_HPP */
