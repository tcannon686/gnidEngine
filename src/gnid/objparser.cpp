#include "gnid/objparser.hpp"
#include <unordered_map>

using namespace gnid;

void ObjParser::nextToken()
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

void ObjParser::parseString()
{
    int c;
    std::stringstream s;
    while((c = stream.get()) != '\n' && c != '\r')
    {
        s << (char) c;
    }
    stream.unget();

    token = s.str();
    token_type = ObjParser::TokenType::STRING;
}

bool ObjParser::accept(ObjParser::TokenType type)
{
    if(token_type == type)
    {
        nextToken();
        return true;
    }
    else
        return false;
}

bool ObjParser::expect(ObjParser::TokenType type)
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

ObjParser::Mesh &ObjParser::mesh()
{
    if (meshes.empty())
    {
        meshes.emplace_back();
    }
    return meshes.back();
}

bool ObjParser::parseVertex()
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

bool ObjParser::parseNormal()
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

bool ObjParser::parseTexCo()
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

bool ObjParser::parseFace()
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
        for(std::size_t i = 1; i < vIndices.size() - 1; i ++)
        {
            this->mesh().vIndices.push_back(vIndices[0]);
            if(nIndices.size() > 0)
                this->mesh().nIndices.push_back(nIndices[0]);
            if(tIndices.size() > 0)
                this->mesh().tIndices.push_back(tIndices[0]);
            for(std::size_t j = 0; j < 2; j ++)
            {
                this->mesh().vIndices.push_back(vIndices[i + j]);
                if(nIndices.size() > 0)
                    this->mesh().nIndices.push_back(nIndices[i + j]);
                if(tIndices.size() > 0)
                    this->mesh().tIndices.push_back(tIndices[i + j]);
            }
        }

        if(this->mesh().nIndices.size() > 0)
            assert(this->mesh().nIndices.size() == this->mesh().vIndices.size());

        if(this->mesh().tIndices.size() > 0)
            assert(this->mesh().tIndices.size() == this->mesh().vIndices.size());

        return true;
    }
    return false;
}

bool ObjParser::parseUseMtl()
{
    if(accept(TokenType::USEMTL))
    {
        parseString();
        expect(TokenType::STRING);
        std::string name = token;

        meshes.emplace_back();
        mesh().material = name;

        return true;
    }
    return false;
}

bool ObjParser::parseMtlLib()
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

bool ObjParser::parseObject()
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

bool ObjParser::parseGroup()
{
    if(accept(TokenType::GROUP))
    {
        parseString();
        expect(TokenType::STRING);
        std::string name = token;

        return true;
    }
    return false;
}

bool ObjParser::parseShade()
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

std::ostream &ObjParser::error() const
{
    return std::cerr << "error parsing obj[" << line << "]: ";
}

std::ostream &ObjParser::warning() const
{
    return std::cerr << "warning parsing obj[" << line << "]: ";
}

void ObjParser::parse()
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

std::shared_ptr<Node> ObjParser::buildPhysicsNode()
{
    std::shared_ptr<EmptyNode> ret = std::make_shared<EmptyNode>();
    for (const Mesh &mesh : meshes) {
        for (int i = 0; i < mesh.vIndices.size(); i += 3) {
            std::vector<tmat::Vector3f> points;
            for (int j = 0; j < 3; j ++) {
                points.push_back(vertices[mesh.vIndices[i + j]].cut());
            }
            std::shared_ptr<Collider> collider = std::make_shared<Collider>(
                std::make_shared<Hull>(points));
            ret->add(collider);
        }
    }
    return ret;
}

std::shared_ptr<Node> ObjParser::buildRendererNode(
    const Mesh &mesh,
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

    for(std::size_t i = 0; i < mesh.vIndices.size(); i ++)
    {
        std::string table_index =
            std::to_string(mesh.vIndices[i])
            + "/" + (mesh.tIndices.size() > 0 ?
                    std::to_string(mesh.tIndices[i])
                    : "")
            + "/" + (mesh.nIndices.size() > 0 ?
                    std::to_string(mesh.nIndices[i])
                    : "");

        auto in_table = index_table.find(table_index);

        /* If the vertex has not been added yet, add it. */
        if(in_table == end(index_table))
        {
            /* Add the data. */
            for(int j = 0; j < 3; j ++)
                data.push_back(vertices[mesh.vIndices[i]][j]);
            if(mesh.tIndices.size() > 0)
            {
                for(int j = 0; j < 2; j ++)
                    data.push_back(texCos[mesh.tIndices[i]][j]);
            }
            if(mesh.nIndices.size() > 0)
            {
                for(int j = 0; j < 3; j ++)
                    data.push_back(normals[mesh.nIndices[i]][j]);
            }

            index_table[table_index] = cur_index;
            cur_index ++;
        }
        index_array.push_back(index_table[table_index]);
    }

    const std::size_t vertex_size = 3 * sizeof(float);
    const std::size_t normal_size = (
            mesh.nIndices.size() > 0? 3 * sizeof(float) : 0);
    const std::size_t texco_size = (
            mesh.tIndices.size() > 0? 2 * sizeof(float) : 0);
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

    if(mesh.tIndices.size() > 0)
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

    if(mesh.nIndices.size() > 0)
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

    auto renderMesh = std::make_shared<RendererMesh>(
        GL_TRIANGLES,
        index_array.size(),
        GL_UNSIGNED_INT,
        vao);
    return std::make_shared<RendererNode>(renderMesh, material);
}

std::shared_ptr<Node> ObjParser::buildRendererNode(
    std::shared_ptr<Material> material)
{
    auto node = std::make_shared<EmptyNode>();
    for (const Mesh &mesh : meshes)
    {
        node->add(buildRendererNode(mesh, material));
    }
    return node;
}

std::shared_ptr<Node> ObjParser::buildRendererNode(
    const MaterialMapping &materials)
{
    auto node = std::make_shared<EmptyNode>();
    for (const Mesh &mesh : meshes)
    {
        node->add(buildRendererNode(mesh, materials.at(mesh.material)));
    }
    return node;
}

const std::string ObjParser::tokenStrings[] =
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
