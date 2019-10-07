local Bone = {}

function Bone.new(self, attributes)
    local ret = {}

    ret.name = attributes.name
    ret.matrix = attributes.matrix
    ret.length = attributes.length
    ret.shaderMatrix = Matrix.identity
    ret.restMatrix = matrix
    ret.restMatrixLocal = Matrix.identity
    ret.restMatrixLocalInverse = Matrix.identity

    if attributes then
        -- Add the children field if we have children.
        if attributes.children then
            local isEmpty = true
            ret.children = {}
            for i, child in ipairs(attributes.children) do
                ret.children[i] = child
                child.parent = ret
                isEmpty = false
            end
            if isEmpty then
                ret.children = nil
            end
        end
    end

    ret.clone = function(self)
        local args = {
            matrix = self.restMatrix,
            length = self.length,
            name = self.name
        }
        return Bone:new(args)
    end

    ret.initialize = function(self)
        self.restHead = self.head
        self.restTail = self.tail
        self.restMatrixLocal = self.matrixLocal
        self.restMatrixLocalInverse = self.restMatrixLocal:inverse()
        self.restMatrix = self.matrix
        
        if self.children then
            for i, child in ipairs(self.children) do
                child:initialize()
            end
        end
    end

    ret.update = function(self, matrixLocal)
        if not matrixLocal then
            matrixLocal = Matrix.identity end

        self.matrixLocal = matrixLocal
            * self.matrix
        self.shaderMatrix = self.matrixLocal
            * self.restMatrixLocalInverse
        self.head = self.matrixLocal * Vector.fourth
        self.tail = self.matrixLocal * Vector(0, self.length, 0, 1)

        matrixLocal = self.matrixLocal
            * Matrix.newTranslate(Vector(0, self.length, 0))
        if self.children then
            for i, child in ipairs(self.children) do
                child:update(matrixLocal)
            end
        end
    end
    
    return ret
end

return Bone
