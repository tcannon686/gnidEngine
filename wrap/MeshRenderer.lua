
local Component = require("wrap/Component")

local MeshRenderer = Component.new()

function MeshRenderer.new(self, mesh, bindings)
    local ret = {}

    ret.mesh = mesh
    ret.bindings = {}

    for i, item in ipairs(bindings) do
        if not item.mesh then
            error("MeshRenderer.new expects list of items with mesh key.")
        end
        if not item.material then
            error("MeshRenderer.material expects list of items with "
               .. "material key.")
        end
        ret.bindings[i] = {
            mesh = item.mesh,
            material = item.material
        }
    end

    ret.render = function(self) 
        for i, item in ipairs(self.items) do
            item.material:bind()
            item.material.shader.transform =
                ret.gameObject.transform.worldMatrix
            item.material.shader.projection =
                ret.gameObject.scene.camera.components.camera.projection
            item.mesh:bind(item.material.shader)

            -- TODO draw.
        end
    end
end

return MeshRenderer
