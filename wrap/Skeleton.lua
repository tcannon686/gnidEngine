local Skeleton = {}

function Skeleton.new(self, attributes)
    local ret = {}

    if type(attributes) ~= "table" then
        error("Skeleton.new expects table attributes.")
    end

    if type(attributes.bones) ~= "table" then
        error("Skeleton.new expects table with key bones.")
    end
    ret.bones = attributes.bones
    ret.namedBones = attributes.namedBones

    -- Update all the leaf bones (will propogate to children).
    for i, bone in ipairs(ret.bones) do
        if not bone.parent then
            bone:update()
            -- Initialize the rest pose.
            bone:initialize()
        end
    end

    ret.bind = function(
            self,
            shader,
            countUniform,
            headUniform,
            matrixUniform)

        -- Update all the leaf bones (will propogate to children).
        for i, bone in ipairs(self.bones) do
            if not bone.parent then
                bone:update()
            end
        end

        shader:bind()
        shader[countUniform] = #self.bones
        for i, bone in ipairs(self.bones) do
            shader[matrixUniform][i] = bone.shaderMatrix
        end
    end
    ret.unbind = function(self, shader, countUniform)
        shader[countUniform] = 0
    end
    return ret
end

return Skeleton
