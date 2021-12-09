#include "gnid/objparser.hpp"

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
            this->vIndices.push_back(vIndices[0]);
            if(nIndices.size() > 0)
                this->nIndices.push_back(nIndices[0]);
            if(tIndices.size() > 0)
                this->tIndices.push_back(tIndices[0]);
            for(std::size_t j = 0; j < 2; j ++)
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

bool ObjParser::parseUseMtl()
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
    std::shared_ptr<SpatialNode> ret = std::make_shared<SpatialNode>();
    for (int i = 0; i < vIndices.size(); i += 3) {
        std::vector<tmat::Vector3f> points;
        for (int j = 0; j < 3; j ++) {
            points.push_back(vertices[vIndices[i + j]].cut());
        }
        std::shared_ptr<Collider> collider = std::make_shared<Collider>(
            std::make_shared<Hull>(points));
        ret->add(collider);
    }
    return ret;
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
