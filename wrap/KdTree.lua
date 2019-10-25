
local KdTree = {}

function KdTree.new(self, objects, axisIndex)
    if not axisIndex then
        axisIndex = 1 end
    local axis = Vector()
    axis[axisIndex] = 1

    function calcBounds(object)
        if object.max and object.min then
            return {
                max = object.max,
                min = object.min,
                position = (object.max + object.min) * 0.5,
                size = object.max - object.min
            }
        elseif object.position and object.radius then
            return {
                max = object.position + object.radius * Vector.one,
                min = object.position - object.radius * Vector.one,
                position = object.position,
                size = 2 * object.radius * Vector.one
            }
        end
    end

    local ret = {}

    -- Calculate the bounds of the tree.
    for k, object in pairs(objects) do
        ret.bounds = calcBounds(object)
        if ret.bounds then
            break
        end
    end
    for k, object in pairs(objects) do
        local bounds = calcBounds(object)
        if bounds then
            if bounds.max.x > ret.bounds.max.x then
                ret.bounds.max.x = bounds.max.x end
            if bounds.max.y > ret.bounds.max.y then
                ret.bounds.max.y = bounds.max.y end
            if bounds.max.z > ret.bounds.max.z then
                ret.bounds.max.z = bounds.max.z end

            if bounds.min.x < ret.bounds.min.x then
                ret.bounds.min.x = bounds.min.x end
            if bounds.min.y < ret.bounds.min.y then
                ret.bounds.min.y = bounds.min.y end
            if bounds.min.z < ret.bounds.min.z then
                ret.bounds.min.z = bounds.min.z end
        end
    end
    ret.bounds.position = (ret.bounds.min + ret.bounds.max) * 0.5
    ret.bounds.size = ret.bounds.max - ret.bounds.min

    local onLeft = {}
    local onLeftIndex = 0
    local onRight = {}
    local onRightIndex = 0

    for k, object in pairs(objects) do
        local bounds = calcBounds(object)
        if bounds then
            if (bounds.position - ret.bounds.position):dot(axis) < 0 then
                onLeftIndex = onLeftIndex + 1
                onLeft[onLeftIndex] = object
            else
                onRightIndex = onRightIndex + 1
                onRight[onRightIndex] = object
            end
        end
    end

    if onLeftIndex == 0 then
        ret.objects = onRight
    elseif onRightIndex == 0 then
        ret.objects = onLeft
    else
        ret.left = self:new(onLeft, (axisIndex + 1) % 4 + 1)
        ret.right = self:new(onRight, (axisIndex + 1) % 4 + 1)
    end

    function ret.raycast(self, hit, ray, objectFilter)
        -- First determine if this node is hit.
        local localHit = {}
        if math3d.rayBox(
                localHit,
                self.bounds.min,
                self.bounds.max,
                ray.o,
                ray.d) then
            -- If at a leaf node, iterate through the objects at the node.
            if self.objects then
                local ret = false
                for k, object in pairs(self.objects) do
                    if not objectFilter or objectFilter(object) then
                        if object.raycast and object:raycast(hit, ray) then
                            ret = true
                        end
                    end
                end
                return ret
            -- Otherwise continue into the node.
            else
                local ret = false
                if self.left then
                    if self.left:raycast(hit, ray, objectFilter) then
                        ret = true
                    end
                end
                if self.right then
                    if self.right:raycast(hit, ray, objectFilter) then
                        ret = true
                    end
                end
                return ret
            end
        end
        return false
    end
    return ret
end

return KdTree
