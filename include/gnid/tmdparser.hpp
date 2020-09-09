#ifndef TMDPARSER_HPP
#define TMDPARSER_HPP

#include <cstdint>
#include <sstream>
#include <unordered_map>
#include <cassert>
#include "gnid/emptynode.hpp"
#include "gnid/matrix/matrix.hpp"

namespace gnid
{

/**
 * \brief
 *     A parser for Tom's Model format (.tmd), a simple binary format for
 *     storing meshes and skeletons
 *
 * \details
 *     This class can be used to parse the data exported from
 *     operator_export_tmd.py.
 *
 * \todo
 *      - Add feature allowing user to map material names to materials
 *      - Add support for loading armatures
 *      - Add support for loading animations
 */
template<typename Stream>
class TmdParser
{
public:
    TmdParser(Stream &stream) : stream_(stream)
    {
    }

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
    /* Numbers */
    static const uint32_t TMD_MAGIC_NUMBER         = 0xCA770770;
    static const uint8_t  TMD_RECORD_DONE          = 0x00;
    static const uint8_t  TMD_RECORD_VERSION       = 0x01;
    static const uint8_t  TMD_RECORD_VERTEXATTRIB  = 0x10;
    static const uint8_t  TMD_RECORD_VERTEX        = 0x11;
    static const uint8_t  TMD_RECORD_BINDING       = 0x20;
    static const uint8_t  TMD_RECORD_TRIANGLE      = 0x21;
    static const uint8_t  TMD_RECORD_BONE          = 0x30;
    static const uint8_t  TMD_RECORD_BONEPARENT    = 0x31;
    static const uint8_t  TMD_RECORD_BONENAME      = 0x32;
    static const uint8_t  TMD_RECORD_ACTION        = 0x40;
    static const uint8_t  TMD_RECORD_CURVE         = 0x41;
    static const uint8_t  TMD_RECORD_CURVESAMPLE   = 0x42;
    static const uint8_t  TMD_RECORD_PLAYBACKDATA  = 0x43;

    static const uint8_t  TMD_PARSER_VERSION_MAJOR = 1;
    static const uint8_t  TMD_PARSER_VERSION_MINOR = 2;

    typedef void (TmdParser<Stream>::*ParseRecordFunction)() const;

    static std::unordered_map<uint8_t, ParseRecordFunction> recordParsers;

    Stream &stream_;
    bool isDone = false;


    typedef enum
    {
        INVALID,
        VERTICES,
        NORMALS,
        TEXCOS,
        BONEWEIGHTS
    } VertexArrayName;

    std::array<std::vector<float>, 5> vertexArrays;
    std::array<uint32_t, 5> vertexArrayComponentCounts;
    VertexArrayName currentVertexArray = INVALID;

    std::vector<std::vector<uint32_t>> bindings;
    int currentBinding = 0;

    /**
     * \brief Read the next integral value from the stream
     */
    template<typename T>
    T next()
    {
        static_assert(std::is_integral<T>::value
                || std::is_floating_point<T>::value);

        char data[sizeof(T)];

        stream_.read(data, sizeof(T));
        
        /* TODO throw exception. */
        assert(!stream_.fail());

        return *reinterpret_cast<T *>(data);
    }

    /**
     * \brief Read the next null terminated string from the stream
     */
    std::string nextString()
    {
        std::stringstream sstr;
        int data;
        while((data = stream_.get()) != 0)
        {
            /* TODO throw exception. */
            assert(data != EOF);
            sstr << (char) data;
        }
        return sstr.str();
    }

    void parseVersion()
    {
        auto magicNumber = next<uint32_t>();
        assert(magicNumber == TMD_MAGIC_NUMBER);

        auto versionMajor = next<uint8_t>();
        auto versionMinor = next<uint8_t>();

        assert(versionMajor == TMD_PARSER_VERSION_MAJOR
                && versionMinor == TMD_PARSER_VERSION_MINOR);
    }

    void parseDone()
    {
        isDone = true;
    }

    void parseVertexAttrib()
    {
        std::string name = nextString();
        auto numCos = next<uint32_t>();
        if(name == "vertex")
        {
            currentVertexArray = VERTICES;
        }
        else if(name == "normal")
        {
            currentVertexArray = NORMALS;
        }
        else if(name == "texCo")
        {
            currentVertexArray = TEXCOS;
        }
        else if(name == "weights")
        {
            currentVertexArray = BONEWEIGHTS;
        }
        else
        {
            /* TODO throw exception. */
            assert(false);
        }
        vertexArrayComponentCounts[currentVertexArray] = numCos;
    }

    void parseVertex()
    {
        for(int i = 0; i < vertexArrayComponentCounts[currentVertexArray]; i ++)
        {
            vertexArrays[currentVertexArray].emplace_back(next<float>());
        }
    }

    void parseBinding()
    {
        /* TODO add material parsing. */
        std::string materialName = nextString();
        bindings.emplace_back();
        currentBinding = bindings.size() - 1;
    }

    void parseTriangle()
    {
        for(int i = 0; i < 3; i ++)
        {
            bindings[currentBinding].emplace_back(next<uint32_t>() - 1);
        }
    }

    void parseBone()
    {
        /* TODO actually use these values. */
        float length;
        float mat[16];

        for(int i = 0; i < 16; i ++)
        {
            mat[i] = next<uint32_t>();
        }
    }

    void parseBoneParent()
    {
        /* TODO actually use these values. */
        auto childIndex = next<uint32_t>();
        auto parentIndex = next<uint32_t>();
    }

    void parseBoneName()
    {
        /* TODO actually use these values. */
        auto boneIndex = next<uint32_t>();
        auto name = nextString();
    }

    void parseAction()
    {
        /* TODO actually use these values. */
        auto name = nextString();
        auto sampleCount = next<uint32_t>();
    }

    void parseCurve()
    {
        /* TODO actually use these values. */
        auto boneIndex = next<uint32_t>();
    }

    void parseCurveSample()
    {
        /* TODO actually use these values. */
        float matrix[16];

        for(int i = 0; i < 16; i ++)
        {
            matrix[i] = next<float>();
        }
    }

    void parsePlaybackData()
    {
        /* TODO actually use these values. */
        auto sampleRate = next<uint8_t>();
        auto lerp = next<uint8_t>();
    }

    void parseRecord()
    {
        auto recordType = next<uint8_t>();
        if     (recordType == TMD_RECORD_DONE)         parseDone();
        else if(recordType == TMD_RECORD_VERSION)      parseVersion();
        else if(recordType == TMD_RECORD_VERTEXATTRIB) parseVertexAttrib();
        else if(recordType == TMD_RECORD_VERTEX)       parseVertex();
        else if(recordType == TMD_RECORD_BINDING)      parseBinding();
        else if(recordType == TMD_RECORD_TRIANGLE)     parseTriangle();
        else if(recordType == TMD_RECORD_BONE)         parseBone();
        else if(recordType == TMD_RECORD_BONEPARENT)   parseBoneParent();
        else if(recordType == TMD_RECORD_BONENAME)     parseBoneName();
        else if(recordType == TMD_RECORD_ACTION)       parseAction();
        else if(recordType == TMD_RECORD_CURVE)        parseCurve();
        else if(recordType == TMD_RECORD_CURVESAMPLE)  parseCurveSample();
        else if(recordType == TMD_RECORD_PLAYBACKDATA) parsePlaybackData();
        /* TODO throw exception. */
        else
            assert(false);
    }
};

template<typename Stream>
void TmdParser<Stream>::parse()
{
    while(!isDone)
    {
        parseRecord();
    }
}

template<typename Stream>
template<typename Shader, typename Material>
std::shared_ptr<Node> TmdParser<Stream>::buildRendererNode(
        std::shared_ptr<Shader> shader)
{
    assert(isDone);

    std::vector<float> vertexData;

    /* TODO include other vertex arrays. */
    std::vector<float> &vertices = vertexArrays[VertexArrayName::VERTICES];
    const uint32_t &vertexSize
        = vertexArrayComponentCounts[VertexArrayName::VERTICES];
    std::vector<float> &normals = vertexArrays[VertexArrayName::NORMALS];
    const uint32_t &normalSize
        = vertexArrayComponentCounts[VertexArrayName::NORMALS];

    const std::size_t stride =
        vertexSize * sizeof(float)
        + normalSize * sizeof(float);

    /* Create vertex buffer data from vertex arrays. */
    for(int i = 0; i < vertices.size() / vertexSize; i ++)
    {
        for(int j = 0; j < vertexSize; j ++)
        {
            vertexData.push_back(vertices[i * vertexSize + j]);
        }

        for(int j = 0; j < normalSize; j ++)
        {
            vertexData.push_back(normals[i * normalSize + j]);
        }
    }

    GLuint vbo;
    /* Upload vertex data. */
    glGenBuffers(1, &vbo);

    /* TODO throw exception. */
    assert(vbo != 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
            GL_ARRAY_BUFFER,
            vertexData.size() * sizeof(float),
            vertexData.data(),
            GL_STATIC_DRAW);

    /* Create the node to hold the bindings. */
    auto rootNode = std::make_shared<EmptyNode>();

    for(auto &binding : bindings)
    {
        GLuint vao, ibo;
        /* Generate the vertex array and index buffer. */
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &ibo);

        /* TODO throw exception. */
        assert(vao != 0);
        assert(ibo != 0);

        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        /* Upload the index data. */
        glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                binding.size() * sizeof(uint32_t),
                binding.data(),
                GL_STATIC_DRAW);

        /* Set the attrib pointers for the VAO. */
        glVertexAttribPointer(
                ShaderProgram::ATTRIB_LOCATION_VERTEX,
                vertexSize,
                GL_FLOAT,
                GL_FALSE,
                stride,
                nullptr);
        glEnableVertexAttribArray(ShaderProgram::ATTRIB_LOCATION_VERTEX);

        glVertexAttribPointer(
                ShaderProgram::ATTRIB_LOCATION_NORMAL,
                normalSize,
                GL_FLOAT,
                GL_FALSE,
                stride,
                (void *)(vertexSize * sizeof(float)));
        glEnableVertexAttribArray(ShaderProgram::ATTRIB_LOCATION_NORMAL);

        /* Create the mesh. */
        auto rendererMesh = std::make_shared<RendererMesh>(
                GL_TRIANGLES,
                binding.size(),
                GL_UNSIGNED_INT,
                vao);
        
        /* TODO create actual material. */
        /* Create the material. */
        auto mat = std::make_shared<Material>(shader);

        /* Create the RendererNode and add it to the root node. */
        rootNode->add(std::make_shared<RendererNode>(rendererMesh, mat));
    }

    return rootNode;
}

} /* namespace */

#endif
