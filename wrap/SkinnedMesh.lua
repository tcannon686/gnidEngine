local SkinnedMesh = {}

SkinnedMesh.new = function(self, mesh, skeleton)
    local ret = {
        mesh = mesh,
        skeleton = skeleton
    }

    ret.render = function(self)
        if self.skeleton then
            for i, binding in ipairs(self.mesh.bindings) do
                self.skeleton:bind(
                    binding.material.shader,
                    "boneCount",
                    "boneHead",
                    "boneMatrix")
            end
            mesh:render()
            for i, binding in ipairs(mesh.bindings) do
                self.skeleton:unbind(binding.material.shader, "boneCount")
            end
        else
            mesh:render()
        end
    end
    return ret
end
return SkinnedMesh
