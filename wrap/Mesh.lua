
local VertexArray = require("wrap/VertexArray")
local ShaderProgram = require("wrap/ShaderProgram")

local Mesh = {}

function Mesh.new(self, attributes)
    local ret = {}
    
    ret.pointCloud = attributes.pointCloud
    if not ret.pointCloud then
        error("Mesh.new expects PointCloud.")
    end
    ret.bindings = attributes.bindings
    if not ret.bindings then
        error("Mesh.new expects bindings.")
    end

    ret.render = function(self)
        for i, binding in ipairs(self.bindings) do
            binding.material:bind()
            self.pointCloud:bind(binding.material.shader)
            binding.indexArray:render()
        end
    end
    return ret
end

return Mesh
