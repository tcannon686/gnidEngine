#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <glad/glad.h>
#include "matrix.h"
#include "luamatrix.h"
#include "luagl.h"

typedef enum
{
    SHADER,
    PROGRAM,
    BUFFER,
    TEXTURE,
    VERTEX_ARRAY
} luagl_type_t;

typedef struct
{
    luagl_type_t luagl_type;           // Should be SHADER.
    GLenum type;
    GLuint id;
} luagl_shader_t;

typedef struct
{
    luagl_type_t luagl_type;           // Should be PROGRAM.
    GLuint id;
} luagl_program_t;

typedef struct
{
    luagl_type_t luagl_type;           // Should be BUFFER
    GLuint id;
} luagl_buffer_t;

typedef struct
{
    luagl_type_t luagl_type;           // Should be BUFFER
    GLuint id;
} luagl_vertex_array_t;

typedef struct
{
    luagl_type_t luagl_type;           // Should be TEXTURE
    GLuint id;
} luagl_texture_t;

static void LuaGlThrowGlErrors(lua_State *L)
{
    GLenum error;
    int error_count;
    char error_string[256];
    char *str_ptr;
    error_string[0] = 0;
    error_count = 0;
    
    sprintf(error_string, "OpenGL errors: ");
    str_ptr = error_string;
    str_ptr = str_ptr + strlen(str_ptr);
    while((error = glGetError()) != GL_NO_ERROR)
    {
        sprintf(str_ptr, "0x%x, ", error);
        error_count ++;
        str_ptr = str_ptr + strlen(str_ptr);
    }

    if(error_count > 0)
    {
        luaL_error(L, error_string);
    }
}

static int _G_gl_shader_source(lua_State *L)
{
    luagl_shader_t *luagl_shader;
    const char *source;
    size_t lensource;
    int source_length;

    // Arguments.
    luagl_shader = lua_touserdata(L, 1);

    if(lua_isstring(L, 2))
    {
        source = lua_tolstring(L, 2, &lensource);
    }
    else
    {
        return luaL_error(L, "shader.source expects string.");
    }

    if(luagl_shader == NULL || luagl_shader->luagl_type != SHADER)
    {
        return luaL_error(L, "shader.source expects luagl_shader_t.");
    }
    
    source_length = (int) lensource;
    glShaderSource(luagl_shader->id, 1, &source, &source_length);

    LuaGlThrowGlErrors(L);
    return 0;
}

static int _G_gl_shader_compile(lua_State *L)
{
    luagl_shader_t *luagl_shader;
    int compile_status, info_log_length;
    char *log;

    luagl_shader = lua_touserdata(L, 1);
    if(luagl_shader == NULL || luagl_shader->luagl_type != SHADER)
    {
        return luaL_error(L, "shader.compile expects luagl_shader_t.");
    }

    glCompileShader(luagl_shader->id);

    glGetShaderiv(luagl_shader->id, GL_COMPILE_STATUS, &compile_status);
    // If compilation succeeded return nothing.
    if(compile_status == GL_TRUE)
        return 0;
    
    // Otherwise.
    // Read the info log.
    glGetShaderiv(luagl_shader->id, GL_INFO_LOG_LENGTH, &info_log_length);
    log = (char *) malloc(info_log_length);
    glGetShaderInfoLog(luagl_shader->id, info_log_length, NULL, log);
    
    // Report the error.
    lua_pushstring(L, log);
    free(log);
    return lua_error(L);
}
static int _G_gl_shader_vertex(lua_State *L)
{
    luagl_shader_t *luagl_shader;
    const char *source;


    if(lua_isstring(L, 1))
        source = lua_tostring(L, 1);
    else
        source = NULL;
    
    // Create the shader holder.
    luagl_shader = (luagl_shader_t *)
        lua_newuserdata(
            L,
            sizeof(luagl_shader_t));

    luagl_shader->luagl_type = SHADER;
    luagl_shader->type = GL_VERTEX_SHADER;

    luagl_shader->id = glCreateShader(luagl_shader->type);

    // Return nil if shader creation failed.
    if(luagl_shader->id == 0)
        return 0;

    // If the source was included, compile the shader.
    if(source != NULL)
    {
        // Source the shader.
        lua_pushcfunction(L, _G_gl_shader_source);
        lua_pushvalue(L, -2);
        lua_pushstring(L, source);
        lua_call(L, 2, 0);
        // Compile.
        lua_pushcfunction(L, _G_gl_shader_compile);
        lua_pushvalue(L, -2);
        lua_pushstring(L, source);
        lua_call(L, 2, 0);
    }

    // Return the created shader.
    return 1;
}

static int _G_gl_shader_fragment(lua_State *L)
{
    luagl_shader_t *luagl_shader;
    const char *source;

    if(lua_isstring(L, 1))
        source = lua_tostring(L, 1);
    else
        source = NULL;
    
    // Create the shader holder.
    luagl_shader = (luagl_shader_t *)
        lua_newuserdata(
            L,
            sizeof(luagl_shader_t));

    luagl_shader->luagl_type = SHADER;
    luagl_shader->type = GL_FRAGMENT_SHADER;

    luagl_shader->id = glCreateShader(luagl_shader->type);
    
    // Return nil if shader creation failed.
    if(luagl_shader->id == 0)
        return 0;

    // If the source was included, compile the shader.
    if(source != NULL)
    {
        // Source the shader.
        lua_pushcfunction(L, _G_gl_shader_source);
        lua_pushvalue(L, -2);
        lua_pushstring(L, source);
        lua_call(L, 2, 0);
        // Compile.
        lua_pushcfunction(L, _G_gl_shader_compile);
        lua_pushvalue(L, -2);
        lua_pushstring(L, source);
        lua_call(L, 2, 0);
    }

    // Return the created shader.
    return 1;
}


static int _G_gl_shader_program(lua_State *L)
{
    int i;
    size_t lastarg;
    int link_status, info_log_length;
    char *info_log;
    luagl_program_t *program;
    luagl_shader_t *shader;

    lastarg = lua_gettop(L);

    program = (luagl_program_t *)
        lua_newuserdata(
            L,
            sizeof(luagl_program_t));
    program->luagl_type = PROGRAM;

    program->id = glCreateProgram();
    if(program->id == 0)
    {
        return 0;
    }

    // Link the following shaders on the stack.
    for(i = 1; i <= lastarg; i++)
    {
        shader = lua_touserdata(L, i);
        if(shader == NULL || shader->luagl_type != SHADER)
        {
            return luaL_error(L, "gl.shader.link expects luagl_shader_t as "
                              "arguments.");
        }
        
        glAttachShader(program->id, shader->id);
    }

    glLinkProgram(program->id);

    // If the link was successfull, return the program.
    glGetProgramiv(program->id, GL_LINK_STATUS, &link_status);
    if(link_status == GL_TRUE)
    {
        return 1;
    }


    // Otherwise throw an error.
    info_log = (char *) malloc(info_log_length);

    glGetProgramiv(program->id, GL_INFO_LOG_LENGTH, &info_log_length);
    glGetProgramInfoLog(program->id, info_log_length, NULL, info_log);

    lua_pushstring(L, info_log);
    free(info_log);
    
    return lua_error(L);
}

int _G_gl_shader_use(lua_State *L)
{
    luagl_program_t *program;

    program = (luagl_program_t *) lua_touserdata(L, 1);
    if(program == NULL || program->luagl_type != PROGRAM)
    {
        return luaL_error(
            L,
            "gl.shader.use expects luagl_program_t.");
    }
    glUseProgram(program->id);
    LuaGlThrowGlErrors(L);
    return 0;
}


int _G_gl_buffer_new(lua_State *L)
{
    luagl_buffer_t *buf;

    buf = (luagl_buffer_t *) lua_newuserdata(L, sizeof(luagl_buffer_t));
    buf->luagl_type = BUFFER;
    glGenBuffers(1, &buf->id);

    LuaGlThrowGlErrors(L);
    return 1;
}

int _G_gl_buffer_array_subData(lua_State *L)
{
    const char *data;
    size_t data_length;
    GLintptr offset;

    if(lua_isstring(L, 1))
        data = lua_tolstring(L, 1, &data_length);
    else
        return luaL_error(L, "gl.buffer.array.subData expects string.");

    if(lua_isinteger(L, 2))
        offset = (GLintptr) lua_tointeger(L, 2);
    else
        return luaL_error(L, "gl.buffer.array.subData expects integer.");

    // TODO more than just static draw.
    glBufferSubData(GL_ARRAY_BUFFER,
                    offset,
                    data_length,
                    (const GLvoid *) data);
    LuaGlThrowGlErrors(L);
    return 0;
}

int _G_gl_buffer_array_data(lua_State *L)
{
    const char *data;
    size_t data_length;

    if(lua_isstring(L, 1))
        data = lua_tolstring(L, 1, &data_length);
    else if(lua_isnil(L, 1))
    {
        data = NULL;
        if(lua_isinteger(L, 2))
            data_length = lua_tointeger(L, 2);
        else
            return luaL_error(L, "gl.buffer.array.data expects integer.");
    }
    else
        return luaL_error(L, "gl.buffer.array.data expects string.");

    // TODO more than just static draw.
    glBufferData(GL_ARRAY_BUFFER,
                 data_length,
                 (const GLvoid *) data,
                 GL_STATIC_DRAW);
    LuaGlThrowGlErrors(L);
    return 0;
}

int _G_gl_buffer_array_bind(lua_State *L)
{
    luagl_buffer_t *buf;

    buf = lua_touserdata(L, 1);
    if(buf == NULL || buf->luagl_type != BUFFER)
    {
        return luaL_error(L, "gl.buffer.array.bind expects luagl_buffer_t.");
    }

    glBindBuffer(GL_ARRAY_BUFFER, buf->id);
    LuaGlThrowGlErrors(L);
    return 0;
}


int _G_gl_vertexAttrib_pointer(lua_State *L)
{
    GLuint index;
  	GLint size;
  	GLenum type;
  	GLboolean normalized;
  	GLsizei stride;
  	const GLvoid * pointer;

    const char *type_str;

    if(lua_isinteger(L, 1))
        index = lua_tointeger(L, 1);
    else
        return luaL_error(
            L,
            "gl.vertexAttrib.pointer expects int.");

    if(lua_isinteger(L, 2))
        size = lua_tointeger(L, 2);
    else
        return luaL_error(
            L,
            "gl.buffer.array.vertexAttribPointer expects int.");

    if(size < 1 || size > 4)
        return luaL_error(
            L,
            "gl.buffer.array.vertexAttribPointer size must be 1, 2, 3, or 4.");
    
    if(lua_isstring(L, 3))
        type_str = lua_tostring(L, 3);
    else
        return luaL_error(
            L,
            "gl.buffer.array.vertexAttribPointer expects string.");

    if(*type_str == 'b')
        type = GL_BYTE;
    else if(*type_str == 'B')
        type = GL_UNSIGNED_BYTE;
    else if(*type_str == 'h')
        type = GL_SHORT;
    else if(*type_str == 'H')
        type = GL_UNSIGNED_SHORT;
    else if(*type_str == 'i')
        type = GL_INT;
    else if(*type_str == 'I')
        type = GL_UNSIGNED_INT;
    else if(*type_str == 'f')
        type = GL_FLOAT;
    else if(*type_str == 'd')
        type = GL_DOUBLE;
    else
        return luaL_error(
            L,
            "gl.buffer.array.vertexAttribPointer unknown type string %s/",
            type_str);
    
    if(lua_isboolean(L, 4))
        normalized = lua_toboolean(L, 4) ? GL_TRUE : GL_FALSE;
    else
        return luaL_error(
            L,
            "gl.buffer.array.vertexAttribPointer expects boolean.");
    
    if(lua_isinteger(L, 5))
        stride = lua_tointeger(L, 5);
    else
        return luaL_error(
            L,
            "gl.buffer.array.vertexAttribPointer expects int.");
    
    if(lua_isinteger(L, 6))
        pointer = (const GLvoid *) lua_tointeger(L, 6);
    else if(lua_isnil(L, 6))
        pointer = NULL;
    else
        return luaL_error(
            L,
            "gl.buffer.array.vertexAttribPointer expects int.");

    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    LuaGlThrowGlErrors(L);
    return 0;
}

int _G_gl_vertexAttrib_enable(lua_State *L)
{
    int index;
    if(!lua_isinteger(L, 1))
        return luaL_error(
            L,
            "gl.vertexAttrib.enable expects integer.");
    index = lua_tointeger(L, 1);

    glEnableVertexAttribArray(index);
    LuaGlThrowGlErrors(L);
    return 0;
}

int _G_gl_vertexAttrib_getLocation(lua_State *L)
{
    luagl_program_t *program;
    const char *attrib;

    program = (luagl_program_t *) lua_touserdata(L, 1);
    if(program == NULL || program->luagl_type != PROGRAM)
    {
        return luaL_error(
            L,
            "gl.vertexAttrib.getLocation expects luagl_program_t.");
    }

    if(lua_isstring(L, 2))
        attrib = lua_tostring(L, 2);
    else
        return luaL_error(
            L,
            "gl.vertexAttrib.getLocation expects string.");

    lua_pushinteger(L, (lua_Integer) glGetAttribLocation(program->id, attrib));
    LuaGlThrowGlErrors(L);
    return 1;
}

int _G_gl_vertexArray_new(lua_State *L)
{
    luagl_vertex_array_t *va;

    va = (luagl_vertex_array_t *) lua_newuserdata(
            L,
            sizeof(luagl_vertex_array_t));
    va->luagl_type = VERTEX_ARRAY;
    glGenVertexArrays(1, &va->id);

    LuaGlThrowGlErrors(L);
    return 1;
}

int _G_gl_vertexArray_bind(lua_State *L)
{
    luagl_vertex_array_t *va;

    va = lua_touserdata(L, 1);
    if(va == NULL || va->luagl_type != VERTEX_ARRAY)
    {
        return luaL_error(L,
                "gl.buffer.array.bind expects luagl_vertex_array_t.");
    }

    glBindVertexArray(va->id);
    LuaGlThrowGlErrors(L);
    return 0;
}

int _G_gl_buffer_element_subData(lua_State *L)
{
    const char *data;
    size_t data_length;
    GLintptr offset;

    if(lua_isstring(L, 1))
        data = lua_tolstring(L, 1, &data_length);
    else
        return luaL_error(L, "gl.buffer.element.subData expects string.");

    if(lua_isinteger(L, 2))
        offset = (GLintptr) lua_tointeger(L, 2);
    else
        return luaL_error(L, "gl.buffer.element.subData expects integer.");

    // TODO more than just static draw.
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                    offset,
                    data_length,
                    (const GLvoid *) data);
    LuaGlThrowGlErrors(L);
    return 0;
}

int _G_gl_buffer_element_data(lua_State *L)
{
    const char *data;
    size_t data_length;

    if(lua_isstring(L, 1))
        data = lua_tolstring(L, 1, &data_length);
    else if(lua_isnil(L, 1))
    {
        data = NULL;
        if(lua_isinteger(L, 2))
            data_length = lua_tointeger(L, 2);
        else
            return luaL_error(L, "gl.buffer.element.data expects integer.");
    }
    else
        return luaL_error(L, "gl.buffer.element.data expects string.");

    // TODO more than just static draw.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 data_length,
                 (const GLvoid *) data,
                 GL_STATIC_DRAW);
    LuaGlThrowGlErrors(L);
    return 0;
}

int _G_gl_buffer_element_bind(lua_State *L)
{
    luagl_buffer_t *buf;

    buf = lua_touserdata(L, -1);
    if(buf == NULL || buf->luagl_type != BUFFER)
    {
        return luaL_error(L, "gl.buffer.element.bind expects luagl_buffer_t.");
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->id);
    LuaGlThrowGlErrors(L);
    return 0;
}

int _G_gl_triangles_drawArrays(lua_State *L)
{
  	GLsizei count;

    if(lua_isinteger(L, 1))
        count = (GLsizei) lua_tointeger(L, 1);
    else
        return luaL_error(L, "gl.triangles.drawArrays expects integer count.");

    glDrawArrays(GL_TRIANGLES, 0, count);
    LuaGlThrowGlErrors(L);
    return 0;
}

int _G_gl_triangles_drawElements(lua_State *L)
{
    GLsizei count;
    GLenum type;
    const char *type_str;

    if(lua_isinteger(L, 1))
        count = (GLsizei) lua_tointeger(L, 1);
    else
        return luaL_error(L, "gl.triangles.drawElements expects integer count.");

    if(lua_isstring(L, 2))
        type_str = lua_tostring(L, 2);
    else
        return luaL_error(L, "gl.triangles.drawElements expects string type.");

    if(*type_str == 'B')
        type = GL_UNSIGNED_BYTE;
    else if(*type_str == 'H')
        type = GL_UNSIGNED_SHORT;
    else if(*type_str == 'I')
        type = GL_UNSIGNED_INT;
    else
        return luaL_error(
            L,
            "gl.triangles.drawElements unknown type string %s/",
            type_str);
    glDrawElements(GL_TRIANGLES, count, type, NULL);
    LuaGlThrowGlErrors(L);
    return 0;
}


int _G_gl_uniform_getLocation(lua_State *L)
{
    luagl_program_t *program;
    const char *uniform;

    program = (luagl_program_t *) lua_touserdata(L, 1);
    if(program == NULL || program->luagl_type != PROGRAM)
    {
        return luaL_error(
            L,
            "gl.uniform.getLocation expects luagl_program_t.");
    }

    if(!lua_isstring(L, 2))
        return
            luaL_error(
                L, "gl.uniform.getLocation expects string.");
    uniform = lua_tostring(L, 2);

    lua_pushinteger(
        L,
        (lua_Integer) glGetUniformLocation(program->id, uniform));
    LuaGlThrowGlErrors(L);
    return 1;
}

int _G_gl_uniform_set(lua_State *L)
{
    int location;

    if(!lua_isinteger(L, 1))
        return
            luaL_error(
                L, "gl.uniform.set expects integer.");
    location = (int) lua_tointeger(L, 1);

    // Set the uniform according to the passed value.

    if(lua_isinteger(L, 2))
    {
        int value;
        value = (int) lua_tointeger(L, 2);
        glUniform1i(location, value);
    }
    else if(lua_isnumber(L, 2))
    {
        float value;
        value = (float) lua_tonumber(L, 2);
        glUniform1f(location, value);
    }
    else if(lua_isvector(L, 2))
    {
        float glmat[4];
        int i;
        vector_t *vector;

        vector = lua_tovectorptr(L, 2);

        for(i = 0; i < 4; i ++)
        {
            glmat[i] = (float) vector->m[i];
        }

        glUniform4fv(location, 1, glmat);
    }
    else if(lua_ismatrix(L, 2))
    {
        // Upload the correctly ordered matrix.
        float glmat[16];
        int i, j;
        matrix_t *matrix;

        matrix = lua_tomatrixptr(L, 2);

        for(i = 0; i < 4; i ++)
        {
            for(j = 0; j < 4; j ++)
            {
                glmat[i * 4 + j] = (float) matrix->m[i + j * 4];
            }
        }

        glUniformMatrix4fv(location, 1, GL_FALSE, glmat);
    }
    else
        return luaL_error(L, "gl.uniform.set unknown uniform type.");
    
    LuaGlThrowGlErrors(L);
    return 0;
}

static int _G_gl_polygonMode_fill(lua_State *L)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    LuaGlThrowGlErrors(L);
    return 0;
}
static int _G_gl_polygonMode_line(lua_State *L)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    LuaGlThrowGlErrors(L);
    return 0;
}
static int _G_gl_polygonMode_point(lua_State *L)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    LuaGlThrowGlErrors(L);
    return 0;
}


static int _G_gl_texture_new(lua_State *L)
{
    luagl_texture_t *tex;

    tex = (luagl_texture_t *) lua_newuserdata(L, sizeof(luagl_texture_t));
    tex->luagl_type = TEXTURE;
    glGenTextures(1, &tex->id);

    LuaGlThrowGlErrors(L);
    return 1;
}

int _G_gl_texture2d_bind(lua_State *L)
{
    luagl_texture_t *tex;

    tex = lua_touserdata(L, 1);
    if(tex == NULL || tex->luagl_type != TEXTURE)
    {
        return luaL_error(L, "gl.texture2d.bind expects luagl_texture_t.");
    }
    glBindTexture(GL_TEXTURE_2D, tex->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    LuaGlThrowGlErrors(L);
    return 0;
}

// gl.texture2d.data(data, width, height, componentstr, typestr)
int _G_gl_texture2d_data(lua_State *L)
{
    GLenum format, type;
    GLsizei width, height;
    const char *data, *component_str, *type_str;
    size_t len_data;
    int isnum;

    data = lua_tolstring(L, 1, &len_data);
    if(data == NULL)
        return luaL_error(L, "gl.texture2d.data expects string data.");

    width = lua_tointegerx(L, 2, &isnum);
    if(!isnum)
        return luaL_error(L, "gl.texture2d.data expects integer width.");
    height = lua_tointegerx(L, 3, &isnum);
    if(!isnum)
        return luaL_error(L, "gl.texture2d.data expects integer height.");

    component_str = lua_tostring(L, 4);
    if(component_str == NULL)
        return luaL_error(L, "gl.texture2d.data expects string component_str.");
    type_str = lua_tostring(L, 5);
    if(type_str == NULL)
        return luaL_error(L, "gl.texture2d.data expects string type_str.");

    // Determine the components and type.
    if(strcmp(component_str, "rgba") == 0)
        format = GL_RGBA;
    else if(strcmp(component_str, "bgra") == 0)
        format = GL_BGRA;
    else if(strcmp(component_str, "rgb") == 0)
        format = GL_RGB;
    else if(strcmp(component_str, "bgr") == 0)
        format = GL_BGR;
    else if(strcmp(component_str, "r") == 0)
        format = GL_RED;
    else if(strcmp(component_str, "si") == 0)
        format = GL_STENCIL_INDEX;
    else if(strcmp(component_str, "dc") == 0)
        format = GL_DEPTH_COMPONENT;
    else
        return luaL_error(L, "gl.texture2d.data unknown components.");

    if(strcmp(type_str, "B") == 0)
        type = GL_UNSIGNED_BYTE;
    else if(strcmp(type_str, "b") == 0)
        type = GL_BYTE;
    else if(strcmp(type_str, "H") == 0)
        type = GL_UNSIGNED_SHORT;
    else if(strcmp(type_str, "h") == 0)
        type = GL_SHORT;
    else if(strcmp(type_str, "I") == 0)
        type = GL_UNSIGNED_INT;
    else if(strcmp(type_str, "i") == 0)
        type = GL_INT;
    else if(strcmp(type_str, "f") == 0)
        type = GL_FLOAT;
    else
        return luaL_error(L, "gl.texture2d.data unknown format.");
    glTexImage2D(
        GL_TEXTURE_2D,
        0,                             // Level
        GL_RGBA,
        width,
        height,
        0,                             // Border
        format,
        type,
        data);

    LuaGlThrowGlErrors(L);
    return 0;
}

static const luaL_Reg gl_buffer_element_funcs[] = {
    { "bind", _G_gl_buffer_element_bind },
    { "data", _G_gl_buffer_element_data },
    { "subData", _G_gl_buffer_element_subData },
    { NULL, NULL }
};

static const luaL_Reg gl_buffer_array_funcs[] = {
    { "bind", _G_gl_buffer_array_bind },
    { "data", _G_gl_buffer_array_data },
    { "subData", _G_gl_buffer_array_subData },
    { NULL, NULL }
};

static const luaL_Reg gl_buffer_funcs[] = {
    { "new", _G_gl_buffer_new },
    { NULL, NULL }
};

static const luaL_Reg gl_shader_funcs[] = {
    { "source", _G_gl_shader_source },
    { "compile", _G_gl_shader_compile },
    { "vertex", _G_gl_shader_vertex },
    { "fragment", _G_gl_shader_fragment },
    { "program", _G_gl_shader_program },
    { "use", _G_gl_shader_use },
    { NULL, NULL }
};

static const luaL_Reg gl_triangles_funcs[] = {
    { "drawArrays", _G_gl_triangles_drawArrays },
    { "drawElements", _G_gl_triangles_drawElements },
    { NULL, NULL }
};

static const luaL_Reg gl_vertexAttrib_funcs[] = {
    { "pointer", _G_gl_vertexAttrib_pointer },
    { "getLocation", _G_gl_vertexAttrib_getLocation },
    { "enable", _G_gl_vertexAttrib_enable },
    { NULL, NULL }
};

static const luaL_Reg gl_vertexArray_funcs[] = {
    { "new", _G_gl_vertexArray_new },
    { "bind", _G_gl_vertexArray_bind }
};

static const luaL_Reg gl_uniform_funcs[] = {
    { "getLocation", _G_gl_uniform_getLocation },
    { "set", _G_gl_uniform_set },
    { NULL, NULL }
};

static const luaL_Reg gl_polygonMode_funcs[] = {
    { "fill", _G_gl_polygonMode_fill },
    { "line", _G_gl_polygonMode_line },
    { "point", _G_gl_polygonMode_point },
    { NULL, NULL }
};

static const luaL_Reg gl_texture_funcs[] = {
    { "new", _G_gl_texture_new }
};

static const luaL_Reg gl_texture2d_funcs[] = {
    { "bind", _G_gl_texture2d_bind },
    { "data", _G_gl_texture2d_data }
};

static const luaL_Reg gl_funcs[] = {
    { NULL, NULL }
};

int luaopen_gl(lua_State *L)
{
    luaL_newlib(L, gl_funcs);

    luaL_newlib(L, gl_shader_funcs);
    lua_setfield(L, -2, "shader");

    luaL_newlib(L, gl_buffer_funcs);
    lua_pushvalue(L, -1);
    lua_setfield(L, -3, "buffer");
    luaL_newlib(L, gl_buffer_array_funcs);
    lua_setfield(L, -2, "array");
    luaL_newlib(L, gl_buffer_element_funcs);
    lua_setfield(L, -2, "element");
    lua_pop(L, 1);

    luaL_newlib(L, gl_triangles_funcs);
    lua_setfield(L, -2, "triangles");

    luaL_newlib(L, gl_vertexAttrib_funcs);
    lua_setfield(L, -2, "vertexAttrib");

    luaL_newlib(L, gl_uniform_funcs);
    lua_setfield(L, -2, "uniform");

    luaL_newlib(L, gl_polygonMode_funcs);
    lua_setfield(L, -2, "polygonMode");

    luaL_newlib(L, gl_texture_funcs);
    lua_setfield(L, -2, "texture");

    luaL_newlib(L, gl_texture2d_funcs);
    lua_setfield(L, -2, "texture2d");

    luaL_newlib(L, gl_vertexArray_funcs);
    lua_setfield(L, -2, "vertexArray");

    return 1;
}


