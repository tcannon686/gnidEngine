
#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

#include <unordered_map>
#include <memory>
#include <cassert>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>

#include "shader.hpp"
#include "renderer.hpp"
#include "renderernode.hpp"
#include "node.hpp"
#include "matrix/matrix.hpp"

namespace gnid
{

/**
 * \brief A template based Wavefront OBJ parser
 *
 * \tparam Stream A stream class with the bitshift operator.
 */
template<typename Stream>
class ObjParser
{
public:
    /**
     * \brief Create an OBJ parser for the given stream
     */
    ObjParser(Stream &stream) : stream(stream) {}

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
    template<typename Shader, typename Material>
    std::shared_ptr<Node> buildRendererNode(std::shared_ptr<Shader> shader);
    
private:
    Stream &stream;

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
    void nextToken()
    {
        int c;
        std::stringstream s;

        token = next_token;

        /* Skip whitespace. */
        while(isspace(c = stream.get()) && c != '\n' && c != '\r')
            /* pass */;

        if(c == EOF)
        {
            token_type = TokenType::END_OF_FILE;
        }
        else if(c == '\n' || c == '\r')
        {
            line ++;
            token_type = TokenType::END_OF_LINE;
            if(c == '\n' && stream.peek() == '\r')
                stream.get();
            else if(c == '\r' && stream.peek() == '\n')
                stream.get();
        }
        /* If reading a command. */
        else if(isalpha(c))
        {
            /* Read until non alphanumeric. */
            s << (char) c;
            while(isalnum((c = stream.get())))
            {
                s << (char) c;
            }
            stream.unget();

            next_token = s.str();
            if(next_token == VERTEX_STR)
                token_type = TokenType::VERTEX;
            else if(next_token == NORMAL_STR)
                token_type = TokenType::NORMAL;
            else if(next_token == TEXCO_STR)
                token_type = TokenType::TEXCO;
            else if(next_token == FACE_STR)
                token_type = TokenType::FACE;
            else if(next_token == USEMTL_STR)
                token_type = TokenType::USEMTL;
            else if(next_token == MTLLIB_STR)
                token_type = TokenType::MTLLIB;
            else if(next_token == OBJECT_STR)
                token_type = TokenType::OBJECT;
            else if(next_token == GROUP_STR)
                token_type = TokenType::GROUP;
            else if(next_token == SHADE_STR)
                token_type = TokenType::SHADE;
            else if(next_token == ON_STR)
                token_type = TokenType::ON;
            else if(next_token == OFF_STR)
                token_type = TokenType::OFF;
            else
                token_type = TokenType::UNKNOWN;
        }
        else if(isdigit(c) || c == '-')
        {
            s << (char) c;
            while(isdigit((c = stream.get()))
                    || c == '.'
                    || c == 'e'
                    || c == 'E')
            {
                s << (char) c;
            }
            stream.unget();
            
            next_token = s.str();
            token_type = TokenType::NUMBER;
        }
        else if(c == '#')
        {
            s << (char) c;
            while((c = stream.get()) != '\n' && c != '\r')
            {
                s << (char) c;
            }
            stream.unget();

            next_token = s.str();
            token_type = TokenType::COMMENT;
            line ++;
        }
        else if(c == '/')
        {
            s << (char) c;
            next_token = s.str();
            token_type = TokenType::INDEX_SEPARATOR;
        }
    }

    /**
     * \brief Parse from the current position to the end of the line
     */
    void parseString()
    {
        int c;
        std::stringstream s;
        while((c = stream.get()) != '\n' && c != '\r')
        {
            s << (char) c;
        }
        stream.unget();

        token = s.str();
        token_type = TokenType::STRING;
    }

    bool accept(TokenType type)
    {
        if(token_type == type)
        {
            nextToken();
            return true;
        }
        else
            return false;
    }

    bool expect(TokenType type)
    {
        if(accept(type))
            return true;
        else
        {
            error() << "expected '" <<
                tokenStrings[type] << "' got '" << tokenStrings[token_type] <<
                "'." << std::endl;
        }
        assert(false);
        return false;
    }

    bool parseVertex()
    {
        if(accept(TokenType::VERTEX))
        {
            tmat::Vector4f vertex;
            int index = 0;

            while(expect(TokenType::NUMBER))
            {
                vertex[index] = stof(token);
                index ++;
                if(index >= 3)
                    break;
            }

            if(accept(TokenType::NUMBER))
            {
                vertex[index] = stof(token);
                index ++;
            }

            /* Add the vertex to the list of vertices. */
            vertices.push_back(vertex);
        }
        return false;
    }

    bool parseNormal()
    {
        if(accept(TokenType::NORMAL))
        {
            tmat::Vector3f vertex;
            int index = 0;

            while(expect(TokenType::NUMBER))
            {
                vertex[index] = stof(token);
                index ++;
                if(index >= 3)
                    break;
            }
            
            /* Add the normal to the list of normals. */
            normals.push_back(vertex);
        }
        return false;
    }

    bool parseTexCo()
    {
        if(accept(TokenType::TEXCO))
        {
            tmat::Vector2f vertex;
            int index = 0;

            while(expect(TokenType::NUMBER))
            {
                vertex[index] = stof(token);
                index ++;
                if(index >= 1)
                    break;
            }

            if(accept(TokenType::NUMBER))
            {
                vertex[index] = stof(token);
                index ++;
            }

            /* Add the texco to the list of texcos. */
            texCos.push_back(vertex);
        }
        return false;
    }

    bool parseFace()
    {
        if(accept(TokenType::FACE))
        {
            std::vector<int> vIndices;
            std::vector<int> tIndices;
            std::vector<int> nIndices;

            while(accept(TokenType::NUMBER))
            {
                int v_index = stoi(token) - 1;
                vIndices.push_back(v_index);
                
                if(accept(TokenType::INDEX_SEPARATOR))
                {
                    if(accept(TokenType::NUMBER))
                    {
                        int t_index = stoi(token) - 1;
                        tIndices.push_back(t_index);
                    }
                }

                if(accept(TokenType::INDEX_SEPARATOR))
                {
                    if(accept(TokenType::NUMBER))
                    {
                        int n_index = stoi(token) - 1;
                        nIndices.push_back(n_index);
                    }
                }
            }

            if(tIndices.size() > 0)
                assert(tIndices.size() == vIndices.size());
            if(nIndices.size() > 0)
                assert(nIndices.size() == vIndices.size());

            /* Convert to triangles. */
            for(size_t i = 1; i < vIndices.size() - 1; i ++)
            {
                this->vIndices.push_back(vIndices[0]);
                if(nIndices.size() > 0)
                    this->nIndices.push_back(nIndices[0]);
                if(tIndices.size() > 0)
                    this->tIndices.push_back(tIndices[0]);
                for(size_t j = 0; j < 2; j ++)
                {
                    this->vIndices.push_back(vIndices[i + j]);
                    if(nIndices.size() > 0)
                        this->nIndices.push_back(nIndices[i + j]);
                    if(tIndices.size() > 0)
                        this->tIndices.push_back(tIndices[i + j]);
                }
            }

            if(this->nIndices.size() > 0)
                assert(this->nIndices.size() == this->vIndices.size());
            
            if(this->tIndices.size() > 0)
                assert(this->tIndices.size() == this->vIndices.size());

            return true;
        }
        return false;
    }

    bool parseUseMtl()
    {
        if(accept(TokenType::USEMTL))
        {
            parseString();
            expect(TokenType::STRING);
            std::string name = token;

            return true;
        }
        return false;
    }

    bool parseMtlLib()
    {
        if(accept(TokenType::MTLLIB))
        {
            parseString();
            expect(TokenType::STRING);
            std::string path = token;
            return true;
        }
        return false;
    }

    bool parseObject()
    {
        if(accept(TokenType::OBJECT))
        {
            parseString();
            expect(TokenType::STRING);
            std::string name = token;

            return true;
        }
        return false;
    }

    bool parseGroup()
    {
        if(accept(TokenType::OBJECT))
        {
            parseString();
            expect(TokenType::STRING);
            std::string name = token;

            return true;
        }
        return false;
    }

    bool parseShade()
    {
        if(accept(TokenType::SHADE))
        {
            if(accept(TokenType::NUMBER))
            {
                if(token == "1")
                    smooth = true;
                else
                {
                    error() << "unknown shade type '" << token << "'.";
                }
            }
            else
            {
                expect(TokenType::OFF);
                smooth = false;
            }
            return true;
        }
        return false;
    }

    std::ostream &error() const
    {
        return std::cerr << "error parsing obj[" << line << "]: ";
    }

    std::ostream &warning() const
    {
        return std::cerr << "warning parsing obj[" << line << "]: ";
    }
};

template<typename Stream>
const std::string ObjParser<Stream>::tokenStrings[] =
{
    "COMMENT",
    "VERTEX",
    "NORMAL",
    "TEXCO",
    "FACE",
    "USEMTL",
    "MTLLIB",
    "OBJECT",
    "GROUP",
    "SHADE",
    "INDEX_SEPARATOR",
    "NUMBER",
    "END_OF_FILE",
    "UNKNOWN",
    "STRING",
    "END_OF_LINE",
    "ON",
    "OFF"
};

template<typename Stream>
void ObjParser<Stream>::parse()
{
    nextToken();
    while(true)
    {
        if(accept(TokenType::COMMENT));
        else if(parseVertex());
        else if(parseNormal());
        else if(parseTexCo());
        else if(parseFace());
        else if(parseUseMtl());
        else if(parseMtlLib());
        else if(parseObject());
        else if(parseGroup());
        else if(parseShade());
        if(!accept(TokenType::END_OF_LINE))
        {
            expect(TokenType::END_OF_FILE);
            break;
        }
    }
    done = true;
}

template<typename Stream>
template<typename Shader, typename Material>
std::shared_ptr<Node> ObjParser<Stream>::buildRendererNode(
        std::shared_ptr<Shader> shader)
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

    for(size_t i = 0; i < vIndices.size(); i ++)
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

    const size_t vertex_size = 3 * sizeof(float);
    const size_t normal_size = (
            nIndices.size() > 0? 3 * sizeof(float) : 0);
    const size_t texco_size = (
            tIndices.size() > 0? 2 * sizeof(float) : 0);
    const size_t stride = vertex_size + normal_size + texco_size;

    GLuint vao, vbo, ibo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

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
    std::shared_ptr<Material> mat = std::make_shared<Material>(shader);
    return std::make_shared<RendererNode>(renderMesh, mat);
}

} /* namespace */

#endif /* OBJ_PARSER_H */
