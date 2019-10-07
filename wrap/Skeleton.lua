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

    ret.clone = function(self)
        local bones = {}
        local namedBones = nil
        local lookup = {}

        if self.namedBones then
            namedBones = {}
        end

        for i, bone in ipairs(self.bones) do
            bones[i] = bone:clone()
            if bone.children then
                bones[i].children = {}
            end
            lookup[tostring(bone)] = bones[i]
        end
        for i, bone in ipairs(self.bones) do
            if bone.children then
                for j, child in ipairs(bone.children) do
                    local newChild = lookup[tostring(child)]
                    bones[i].children[j] = newChild
                    newChild.parent =bones[i].children[j]
                end
            end
        end
        if self.namedBones then
            for k, bone in pairs(self.namedBones) do
                namedBones[k] = lookup[tostring(bone)]
            end
        end
        return Skeleton:new {
            bones = bones,
            namedBones = namedBones
        }
    end
    return ret
end

return Skeleton
