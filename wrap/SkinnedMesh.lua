local SkinnedMesh = {}

SkinnedMesh.new = function(self, mesh, skeleton, actions)
    local ret = {
        mesh = mesh,
        skeleton = skeleton,
        actions = actions
    }

    ret.render = function(self, scene)
        if self.skeleton then
            for i, binding in ipairs(self.mesh.bindings) do
                self.skeleton:bind(
                    binding.material.shader,
                    "boneCount",
                    "boneHead",
                    "boneMatrix")
            end
            mesh:render(scene)
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
