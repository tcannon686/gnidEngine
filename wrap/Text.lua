
local PointCloud = require("wrap/PointCloud")
local Mesh = require("wrap/Mesh")
local VertexArray = require("wrap/VertexArray")
local IndexArray = require("wrap/IndexArray")

local materials = require("materials")

local Text = {}
Text.__mt = {}

function Text.__mt.__newindex(t, k, v)
    if k == "text" then
        if v ~= t.__text then
            rawset(t, "__text", v)
            t:update()
        end
    else
        rawset(t, k, v)
    end
end

function Text.__mt.__index(t, k, v)
    if k == "text" then
        return t.__text
    end
end

function Text.new(self, attributes)
    local ret = {
        position = Vector.fourth,
        scale = Vector.one,
    }
    local material = materials.monospace1
    if attributes.material then
        material = attributes.material end
    if attributes.position then
        ret.position = attributes.position end
    if attributes.scale then
        ret.scale = attributes.scale end

    ret.mesh = Mesh:new {
        pointCloud = PointCloud:new {
            vertex = VertexArray:new {},
            texCo = VertexArray:new {}
        },
        bindings = {
            {
                material = material,
                indexArray = IndexArray:new {}
            }
        }
    }

    ret.update = function(self)
        local mesh = self.mesh
        mesh.pointCloud.vertex:clear()
        mesh.pointCloud.texCo:clear()
        mesh.bindings[1].indexArray:clear()

        -- Create squares for each character.
        local vi = 1
        local tex = 1.0 / 16.0

        local x = 0
        local y = 0
        for i = 1, #self.text do
            if self.text:sub(i, i) == '\n' then
                x = 0
                y = y - 1
            else
                local char = self.text:byte(i) - 1
                local texX = (char % 16) * tex
                local texY = 1.0 - math.floor(char / 16) * tex - tex
                
                mesh.pointCloud.vertex[vi]     = Vector(x,       y,       0)
                mesh.pointCloud.vertex[vi + 1] = Vector(x + 1.0, y,       0)
                mesh.pointCloud.vertex[vi + 2] = Vector(x + 1.0, y + 1.0, 0)
                mesh.pointCloud.texCo[vi]      = Vector(texX,       texY)
                mesh.pointCloud.texCo[vi + 1]  = Vector(texX + tex, texY)
                mesh.pointCloud.texCo[vi + 2]  = Vector(texX + tex, texY + tex)
                mesh.bindings[1].indexArray[vi]     = vi
                mesh.bindings[1].indexArray[vi + 1] = vi + 1
                mesh.bindings[1].indexArray[vi + 2] = vi + 2
                vi = vi + 3

                mesh.pointCloud.vertex[vi]     = Vector(x + 1.0, y + 1.0, 0)
                mesh.pointCloud.vertex[vi + 1] = Vector(x,       y + 1.0, 0)
                mesh.pointCloud.vertex[vi + 2] = Vector(x,       y,       0)
                mesh.pointCloud.texCo[vi]      = Vector(texX + tex, texY + tex)
                mesh.pointCloud.texCo[vi + 1]  = Vector(texX,       texY + tex)
                mesh.pointCloud.texCo[vi + 2]  = Vector(texX,       texY)
                mesh.bindings[1].indexArray[vi]     = vi
                mesh.bindings[1].indexArray[vi + 1] = vi + 1
                mesh.bindings[1].indexArray[vi + 2] = vi + 2
                vi = vi + 3

                x = x + 1
            end
        end
        self.lineCount = -y + 1
    end

    ret.render = function(self, scene)
        self.mesh:render(scene,
            Matrix.newTranslate(self.position)
                * Matrix.newScale(self.scale))
    end
    
    setmetatable(ret, self.__mt)
    ret.text = attributes.text

    return ret
end


return Text
