
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
    
    -- If the key is a vertex attribute.
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
    -- If the key is a uniform.
    else
        if rawget(uniform, "count") ~= nil then
            error("shader key '" .. k
                .. "' cannot be set because it is an array.")
        else
            rawget(uniform, "set")(uniform,
                rawget(uniform, "location"), v)
        end
    end
end

local uniformArrayMt = {}
function uniformArrayMt.__newindex(t, k, v)
    if type(k) == "number" then
        rawget(t, "set")(t, rawget(t, "locations")[k], v)
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
    for uniformName, uniformTypeStr in pairs(attributes.uniforms) do
        local uniformType, arrStart = uniformTypeStr:match("([^%[]+)()")
        if not uniformType then
            error("invalid uniform type string '" .. uniformTypeStr .. "'")
        end

        local uniform = {
            uniformType = uniformType,
            name = uniformName
        }
        
        -- Set the set function for the uniform based on the type.
        if uniformType == 'vector' then
            uniform.set = function(self, location, value)
                local v = Vector.toVector(value)
                if v then
                    gl.uniform.set(location, v)
                else
                    error("uniform '" .. self.name .. "' expects vector")
                end
            end
        elseif uniformType == 'matrix' then
            uniform.set = function(self, location, value)
                local v = Matrix.toMatrix(value)
                if v then
                    gl.uniform.set(location, v)
                else
                    error("uniform '" .. self.name .. "' expects matrix")
                end
            end
        elseif uniformType  == 'integer' then
            uniform.set = function(self, location, value)
                if type(value) == 'number'
                        and math.type(value) == 'integer' then
                    gl.uniform.set(location, math.tointeger(value))
                else
                    error("uniform '" .. self.name .. "' expects integer")
                end
            end
        elseif uniformType == 'number' then
            uniform.set = function(self, location, value)
                if type(value) == 'number' then
                    gl.uniform.set(location, value + 0.0)
                end
            end
        elseif uniformType == 'texture2d' then
            uniform.set = function(self, location, value)
                local val = Texture2D:toTexture2D(value)
                if val then
                    val:bind()
                    gl.uniform.set(location, 0)
                else
                    error("uniform '" .. self.name .. "' expects Texture2D")
                end
            end
        elseif uniformType == 'boolean' then
            uniform.set = function(self, location, value)
                if value then
                    gl.uniform.set(location, 1)
                else
                    gl.uniform.set(location, 0)
                end
            end
        else
            error("ShaderProgram.new attribute 'uniforms' must be a "
               .. "table of names with types of 'vector', 'matrix', 'integer' "
               .. "'number', 'boolean', or 'texture2d', got '"
               .. uniformType .. "'")
        end

        -- If the uniform is an array, find the location for each item in the
        -- array.
        if arrStart - 1 ~= #uniformTypeStr then
            local count = tonumber(uniformTypeStr:match("%[(%d+)%]", arrStart))
            if not count then
                error("invalid uniform type string '" .. uniformTypeStr .. "'")
            end
            uniform.count = count
            uniform.locations = {}
            for i = 1, count do
                uniform.locations[i] = gl.uniform.getLocation(
                    ret.__program,
                    uniformName .. "[" .. (i - 1) .. "]")
                if uniform.locations[i] == -1 then
                    error("invalid uniform "
                        .. uniformName .. "[" .. (i - 1) .. "]")
                end
            end
            setmetatable(uniform, uniformArrayMt)
        -- Otherwise just get the uniform's location.
        else
            uniform.location = gl.uniform.getLocation(
                ret.__program, uniformName)
            if uniform.location < 0 then
                error("invalid uniform " .. uniformName)
            end
        end

        ret.__uniforms[uniformName] = uniform
    end

    ret.bind = function(self)
        if ShaderProgram.__active ~= self then
            ShaderProgram.__active = self
            gl.shader.use(self.__program)
            for name, attrib in pairs(self.__vertexAttributes) do
                if attrib.location < 0 then
                    error("invalid vertex attrib " .. name .. ".")
                end
                gl.vertexAttrib.enable(attrib.location)
            end
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
