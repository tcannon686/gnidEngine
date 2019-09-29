local Bone = {}

function Bone.new(self, attributes)
    local ret = {}

    ret.matrix = attributes.matrix
    ret.length = attributes.length
    ret.shaderMatrix = Matrix.identity
    ret.restMatrix = Matrix.identity
    ret.restMatrixInverse = Matrix.identity

    if attributes then
        -- Add the children field if we have children.
        local isEmpty = true
        ret.children = {}
        for i, child in ipairs(attributes) do
            ret.children[i] = child
            child.parent = ret
            isEmpty = false
        end
        if isEmpty then
            ret.children = nil
        end
    end

    ret.initialize = function(self)
        self.restHead = self.head
        self.restTail = self.tail
        self.restMatrix = self.worldMatrix
        self.restMatrixInverse = self.restMatrix:inverse()
        
        if self.children then
            for i, child in ipairs(self.children) do
                child:initialize()
            end
        end
    end

    ret.update = function(self, worldMatrix)
        if not worldMatrix then
            worldMatrix = Matrix.identity end

        self.worldMatrix = worldMatrix
            * self.matrix
        self.shaderMatrix = self.worldMatrix
            * self.restMatrixInverse
        self.head = self.worldMatrix * Vector.fourth
        self.tail = self.worldMatrix * Vector(0, self.length, 0, 1)

        worldMatrix = self.worldMatrix
            * Matrix.newTranslate(Vector(0, self.length, 0))
        if self.children then
            for i, child in ipairs(self.children) do
                child:update(worldMatrix)
            end
        end
    end
    
    return ret
end

return Bone
