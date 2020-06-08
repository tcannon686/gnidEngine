
#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

class ObjParser
{
private:
    vector<Vector3> vertices;
    vector<Vector3> normals;
    vector<Vector2> texCos;
    vector<int> indices;

public:
    ObjParser();
    virtual ~ObjParser();
};

#endif /* OBJ_PARSER_H */

