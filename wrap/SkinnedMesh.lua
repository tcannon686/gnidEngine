local SkinnedMesh = {}

SkinnedMesh.new = function(self, attributes)
    local ret = {
        mesh = attributes.mesh,
        skeleton = attributes.skeleton,
        actions = attributes.actions
    }

    ret.render = function(self, scene, transform)
        if self.skeleton then
            for i, binding in ipairs(self.mesh.bindings) do
                self.skeleton:bind(
                    binding.material.shader,
                    "boneCount",
                    "boneHead",
                    "boneMatrix")
            end
            self.mesh:render(scene, transform)
            for i, binding in ipairs(self.mesh.bindings) do
                self.skeleton:unbind(binding.material.shader, "boneCount")
            end
        else
            self.mesh:render()
        end
    end

    ret.clone = function(self)
        return SkinnedMesh:new {
            mesh = self.mesh,
            skeleton = self.skeleton,
            actions = self.actions
        }
    end

    return ret
end
return SkinnedMesh
