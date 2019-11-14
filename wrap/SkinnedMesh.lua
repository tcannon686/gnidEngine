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
            self.mesh:render(scene, transform)
        end
    end

    ret.clone = function(self)
        local t = {
            mesh = self.mesh,
            actions = self.actions
        }
        if self.skeleton then
            t.skeleton = self.skeleton:clone()
        end
        return SkinnedMesh:new(t)
    end

    return ret
end
return SkinnedMesh
