local KdTree = {}

function KdTree.new(self, objects)
    function calcBounds(object)
        if object.max and object.min then
            return {
                max = object.max * Vector.one,
                min = object.min * Vector.one,
                position = (object.max + object.min) * 0.5,
                size = object.max - object.min
            }
        elseif object.position and object.physics and
                object.physics.radius then
            return {
                max = object.position + object.physics.radius * Vector.one,
                min = object.position - object.physics.radius * Vector.one,
                position = object.position * Vector.one,
                size = 2 * object.physics.radius * Vector.one
            }
        end
    end

    local ret = {}
    local average = Vector.new(0, 0, 0)

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
            average = average + bounds.position
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

    average = average * (1 / #objects)
    local axis

    -- Split along longest axis.
    if ret.bounds.size.x >= ret.bounds.size.y then
        if ret.bounds.size.x >= ret.bounds.size.z then
            axis = Vector.right
        else
            axis = Vector.forward
        end
    else
        if ret.bounds.size.y >= ret.bounds.size.z then
            axis = Vector.up
        else
            axis = Vector.forward
        end
    end

    local onLeft = {}
    local onLeftIndex = 0
    local onRight = {}
    local onRightIndex = 0

    for k, object in pairs(objects) do
        local bounds = calcBounds(object)
        if bounds then
            if (bounds.position - average):three():dot(axis) < 0 then
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
        ret.left = self:new(onLeft)
        ret.right = self:new(onRight)
    end

    -- Find leaf nodes that intersect the given bounds. callback should be a
    -- function of function(node) -> true|false.
    function ret.intersect(self, callback, bounds)
        if self:overlaps(bounds) then
            -- If a leaf node, call the callback.
            if self.objects then
                if callback then
                    return callback(self)
                else
                    return true
                end
            else
                local ret = false
                if self.left and self.left:intersect(callback, bounds) then
                    ret = true end
                if self.right and self.right:intersect(callback, bounds) then
                    ret = true end
                return ret
            end
        end
    end

    -- Returns whether the bounds of the tree overlap the given bounds.
    function ret.overlaps(self, bounds)
        if self.bounds.max.x >= bounds.min.x
                and self.bounds.max.y >= bounds.min.y
                and self.bounds.max.z >= bounds.min.z

                and self.bounds.min.x <= bounds.max.x
                and self.bounds.min.y <= bounds.max.y
                and self.bounds.min.z <= bounds.max.z then
            return true
        end
        return false
    end

    -- Calculates all of the intersections. One intersection per object.
    -- callback should be a function(a, b).
    function ret.calcIntersections(self, callback, root, visited)
        if not visited then
            visited = {} end
        if not root then
            root = self end

        if self.objects then
            -- When we find an intersection, do the callback funciton.
            visited[tostring(self)] = true
            root:intersect(function(node)
                if not visited[tostring(node)] then
                    callback(self, node)
                    return true
                end
            end, self.bounds)
        else
            -- If the left and right nodes don't overlap, then we only have to
            -- check from each respective node downward.
            local newRoot = root
            self.left:calcIntersections(callback, newRoot, visited)
            self.right:calcIntersections(callback, newRoot, visited)
        end
    end

    function ret.printTree(self, indentation)
        if not indentation then
            indentation = ''
        end
        print(indentation .. tostring(self.bounds.size))
        if self.objects then
            print(indentation .. "#objects: " .. #self.objects)
        else
            if self.left then
                print(indentation .. "left: ")
                self.left:printTree(indentation .. ' ')
            end
            if self.right then
                print(indentation .. "right: ")
                self.left:printTree(indentation .. ' ')
            end
        end
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
