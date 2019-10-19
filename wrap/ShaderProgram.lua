
local Texture2D = require("wrap/Texture2D")
local VertexArray = require("wrap/VertexArray")

local ShaderProgram = {}
ShaderProgram.__mt = {}

function ShaderProgram.__mt.__index(self, k, v)
    local uniform = rawget(self, "__uniforms")[k]
    if uniform then
        return uniform end
    local attrib = rawget(self, "__vertexAttributes")[k]
    if attrib then
        return attrib end
    return rawget(self, k)
end

function ShaderProgram.__mt.__newindex(self, k, v)
    local uniform = rawget(self, "__uniforms")[k]
    local val

    self:bind()
    
    -- Verify type is correct.
    if uniform == nil then
        -- Maybe it is an attribute.
        local attrib = rawget(self, "__vertexAttributes")[k]
        if attrib then
            va = VertexArray:toVertexArray(v)
            if va then
                gl.vertexAttrib.enable(attrib.location)
                va:bind(attrib.location)
            else
                error("ShaderProgram.__mt.__newindex expects VertexArray.")
            end
        else
            error("unknown shader key '" .. k .. "'.")
        end
    elseif uniform.uniformType == "number" then
        gl.uniform.set(uniform.location, v)
    elseif uniform.uniformType == "integer" then
        gl.uniform.set(uniform.location, math.tointeger(v))
    elseif uniform.uniformType == "boolean" then
        if v then
            gl.uniform.set(uniform.location, 1)
        else
            gl.uniform.set(uniform.location, 0)
        end
    elseif uniform.uniformType == "vector" then
        val = Vector.toVector(v)

        if val then
            gl.uniform.set(uniform.location, val)
        else
            error("shader key " .. k .. " expects vector.")
        end
    elseif uniform.uniformType == "matrix" then
        val = Matrix.toMatrix(v)
        if val then
            gl.uniform.set(uniform.location, val)
        else
            error("shader key " .. k .. " expects matrix.")
        end
    elseif uniform.uniformType == "texture2d" then
        val = Texture2D:toTexture2D(v)
        if val then
            val:bind()
            gl.uniform.set(uniform.location, 0)
        else
            error("shader key " .. k .. " expects Texture2D.")
        end
    end
end

local uniformArrayMt = {}
function uniformArrayMt.__newindex(t, k, v)
    if type(k) == "number" then
        gl.uniform.set(t.location + k - 1, v)
    else
        error("uniform index must be a number.")
    end
end

--[[This function creates a ShaderProgram with the given attributes. Attributes
    should be a table that contains code, a table with entries for vertex and
    fragment shader; attributes, a list of vertex attributes used by the shader,
    and uniforms, a table of uniforms and their corrosponding types of 'vector',
    'integer', 'number', or 'matrix'.]]
function ShaderProgram.new(self, attributes)
    local ret = {}
    local shaders = {}

    if not attributes then
        error("ShaderProgram.new expects table attributes.")
    end

    -- Error if no code argument was provided.
    if not attributes.code then
        error("ShaderProgram.new missing 'code' attribute.")
    end

    -- For each entry in the code, create a corrosponding shader.
    for shaderType, code in pairs(attributes.code) do
        if gl.shader[shaderType] then
           table.insert(shaders, gl.shader[shaderType](code))
        else
            error(string.format("unknown shader type '%s'.", shaderType))
        end
    end

    ret.__program = gl.shader.program(table.unpack(shaders))

    if not attributes.vertexAttributes then
        error("ShaderProgram.new missing 'vertexAttributes' attribute.")
    end

    ret.__vertexAttributes = {}
    for i, v in ipairs(attributes.vertexAttributes) do
        ret.__vertexAttributes[v] = {
            location = gl.vertexAttrib.getLocation(ret.__program, v)
        }
    end

    if not attributes.uniforms then
        error("ShaderProgram.new missing 'uniforms' attribute.")
    end

    ret.__uniforms = {}
    -- Verify each uniform has a valid type.
    for uniform, uniformType in pairs(attributes.uniforms) do
        if uniformType == 'vector' then
        elseif uniformType == 'matrix' then
        elseif uniformType  == 'integer' then
        elseif uniformType == 'number' then
        elseif uniformType == 'texture2d' then
        elseif uniformType == 'boolean' then
        else
            error("ShaderProgram.new attribute 'uniforms' must be a "
               .. "table of names with types of 'vector', 'matrix', 'integer' "
               .. "'number', 'boolean', or 'texture2d'.")
        end
        ret.__uniforms[uniform] = {
            uniformType = uniformType,
            location = gl.uniform.getLocation(ret.__program, uniform)
        }
        if ret.__uniforms[uniform].location < 0 then
            error("invalid uniform " .. uniform)
        end
        setmetatable(ret.__uniforms[uniform], uniformArrayMt)
    end

    ret.bind = function(self)
        if ShaderProgram.__active ~= self then
            ShaderProgram.__active = self
            gl.shader.use(self.__program)
        end
        for name, attrib in pairs(self.__vertexAttributes) do
            if attrib.location < 0 then
                error("invalid vertex attrib " .. name .. ".")
            end
            gl.vertexAttrib.enable(attrib.location)
        end
    end

    ret.isAttrib = function(self, key)
        if self.__vertexAttributes[key] then
            return true
        else
            return false
        end
    end

    ret.isUniform = function(self, key)
        if self.__uniforms[key] then
            return true
        else
            return false
        end
    end

    setmetatable(ret, self.__mt)
    return ret
end

function ShaderProgram.toShaderProgram(self, object)
    if getmetatable(object) == self.__mt then
        return object
    else
        return nil
    end
end

return ShaderProgram
