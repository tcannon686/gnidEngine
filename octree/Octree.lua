
local Mesh = require("wrap/Mesh")
local VertexArray = require("wrap/VertexArray")
local IndexArray = require("wrap/IndexArray")

local Octree = {}
Octree.__mt = {}


--[[

            5--------6
           /|       /|
          / |      / |
         1--------2  |
         |  7-----|--8
         | /      | /
         |/       |/
         3--------4

           13--------16
           /|       /|
          / |      / |
         1--------4  |
         | 19-----|--22
         | /      | /
         |/       |/
         7--------10

             1
             |
             |
             +--------0
            /
           /
          2

]]

function Octree.__mt.__index(t, k)
    if type(k) == "number" then
        return t.children[k]
    end

    return rawget(t, k)
end

function Octree.__mt.__newindex(t, k, v)
    if type(k) == "number" then
        if t.children[k] then
            t.children[k].parent = nil
            t.children[k].index = nil
            t.children[k]:recursiveSet("root", t.children[k])
        end
        t.children[k] = v
        if v then
            v.parent = t
            v.index = k
            v:recursiveSet("root", t.root)
        else
            empty = true
            for i = 1, 8 do
                if t.children[i] then
                    empty = false
                    break
                end
            end
            -- If empty remove this octree.
            if empty and t.parent then
                t.parent[t.index] = nil
            end
        end
        return t
    end

    return rawset(t, k, v)
end

function Octree.new(self, attributes)
    local ret = {}
    ret.root = ret
    ret.offsets = {}
    ret.min = Vector(-0.5, -0.5, -0.5)
    ret.max = Vector(0.5, 0.5, 0.5)

    for i = 0, 1 do
        for j = 0, 1 do
            for k = 0, 1 do
                ret.offsets[1 + 1 * i + 2 * j + 4 * k] = Vector(
                    i,
                    j,
                    k)
            end
        end
    end
    if attributes then
        ret.children = {}
        local empty = true
        for i = 1, 8 do
            if attributes[i] then
                empty = false
                ret.children[i] = attributes[i]
                ret.children[i].parent = ret
                ret.children[i]:recursiveSet("root", ret)
                ret.children[i].index = i
            end
        end

        if empty then
            ret.children = nil
        end

        if attributes.min then
            ret.min = attributes.min end
        if attributes.max then
            ret.max = attributes.max end
        if attributes.material then
            ret.material = attributes.material end

        if attributes.offsets then
            for i = 1, 8 do
                ret.offsets[i] = attributes.offsets[i]
            end
        end
    end

    ret.toLua = function(self, out, materials)
        local r, e

        if self.root == self then
            r, e = out:write("return ")
            if not r then return false, e end
        end
        r, e = out:write("Octree:new{")
        if not r then return false, e end
        if self.children then
            for i = 1, 8 do
                child = self[i]
                if child then
                    self[i]:toLua(out, materials)
                else
                    r, e = out:write("nil")
                    if not r then return false, e end
                end
                r, e = out:write(",")
                if not r then return false, e end
            end
        else
            r, e = out:write("offsets={")
            if not r then return false, e end
            for i = 1, 8 do
                r, e = out:write("Vector("
                    .. self.offsets[i].x .. ","
                    .. self.offsets[i].y .. ","
                    .. self.offsets[i].z .. "),")
            end
            r, e = out:write("},")
            if not r then return false, e end
        end
        if self.root == self then
            r, e = out:write("min=Vector("
                .. self.min.x .. ","
                .. self.min.y .. ","
                .. self.min.z .. "),")
            if not r then return false, e end
            r, e = out:write("max=Vector("
                .. self.max.x .. ","
                .. self.max.y .. ","
                .. self.max.z .. "),")
            if not r then return false, e end
        end
        -- If we have a material, find it in the list of materials.
        if self.material then
            local matched = false
            for k, v in pairs(materials) do
                if v == self.material then
                    out:write("material=materials." .. k .. ",")
                    matched = true
                    break
                end
            end
            if not matched then
                print("warning: could not find material.")
            end
        end
        r, e = out:write("}")
        return true
    end

    ret.subdivide = function(self)
        if not self.children then
            self.children = {}
            for i = 1, 8 do
                self[i] = Octree:new {}
            end
        end
    end

    ret.unsubdivide = function(self)
        if self.children then
            for i = 1, 8 do
                if self.children[i] then
                    self.children[i].root = self.children[i]
                    self.children[i].parent = nil
                end
                self.children[i] = nil
            end
            self.children = nil
        end
        self.root:update()
    end

    ret.recursiveSet = function(self, k, v)
        self[k] = v
        if self.children then
            for i = 1, 8 do
                if self[i] then
                    self[i]:recursiveSet(k, v)
                end
            end
        end
    end

    ret.calcBounds = function(self, target, min, max, depth)
        if not min then
            min = self.min end
        if not max then
            max = self.max end
        if not depth then
            depth = 1 end
        local size = max - min
        if self == target then
            return min, max, size, depth
        elseif self.children then
            local rmin, rmax, rsize
            local halfSize = 0.5 * size
            for i = 0, 1 do
                for j = 0, 1 do
                    for k = 0, 1 do
                        local child = self.children[1 + i + j * 2 + k * 4]
                        
                        if child then
                            rmin, rmax, rsize, rdepth = child:calcBounds(
                                target,
                                min + i * halfSize * Vector.right
                                    + j * halfSize * Vector.up
                                    + k * halfSize * Vector.forward,
                                min + (i + 1) * halfSize * Vector.right
                                    + (j + 1) * halfSize * Vector.up
                                    + (k + 1) * halfSize * Vector.forward,
                                depth + 1)
                            if rmin then
                                return rmin, rmax, rsize, rdepth
                            end
                        end
                    end
                end
            end
        end
    end

    ret.calcPointIndex = function(self, point, min, max)
        if not min then
            min = self.min end
        if not max then
            max = self.max end
        nearestIndex = 1
        nearestDistance = Vector.distance(
            min + self.offsets[1] * (max - min),
            point)
        
        for i = 2, 8 do
            p = min + self.offsets[i] * (max - min)
            distance = Vector.distance(p, point)

            if distance < nearestDistance then
                nearestIndex = i
                nearestDistance = distance
            end
        end
        return nearestIndex
    end

    ret.createAround = function(self, point, iterations, min, max)
        if not min then
            min = self.min end
        if not max then
            max = self.max end

        local size = max - min
        if iterations == 1 then
            return self
        else
            if not self.children then
                self.children = {}
            end
            local halfSize = size * 0.5
            for i = 0, 1 do
                for j = 0, 1 do
                    for k = 0, 1 do
                        childIndex = 1 + i * 1 + j * 2 + k * 4
                        child = self[childIndex]

                        local childMin = min + i * halfSize * Vector.right
                                       + j * halfSize * Vector.up
                                       + k * halfSize * Vector.forward
                        local childMax = min + (i + 1) * halfSize * Vector.right
                                       + (j + 1) * halfSize * Vector.up
                                       + (k + 1) * halfSize * Vector.forward
                        if point.x >= childMin.x
                                and point.y >= childMin.y
                                and point.z >= childMin.z

                                and point.x <= childMax.x
                                and point.y <= childMax.y
                                and point.z <= childMax.z then
                            if not child then
                                child = Octree:new {}
                                self[childIndex] = child
                            end
                            return child:createAround(
                                point,
                                iterations - 1,
                                childMin,
                                childMax)
                        end
                    end
                end
            end
        end
    end

    ret.triDoPhysics = function(
            self,
            tri,
            hintNormal,
            object)
        local n = (tri[2] - tri[1]):cross(tri[3] - tri[1]):normalize()
        if n:dot(hintNormal) < 0 then
            n = -n end
        local distance, nearest = math3d.trianglePointDistance(
            tri[1],
            tri[2],
            tri[3],
            object.position)
        
        if distance < object.radius then
            local dir = (object.position - nearest):normalize()
            object.position = nearest + dir * object.radius
            local vPar = object.velocity:dot(dir) * dir
            local vPerp = object.velocity - vPar
            object.velocity = object.velocity - vPar - vPerp * 0.05
            if dir:dot(Vector.up) > 0.5 then
                object.isGrounded = true
            end
        end
    end

    ret.quadDoPhysics = function(
            self,
            quad,
            hintNormal,
            object)
        self:triDoPhysics(
            { quad[1], quad[2], quad[3] }, hintNormal, object)
        self:triDoPhysics(
            { quad[3], quad[4], quad[1] }, hintNormal, object)
    end

    ret.doPhysics = function(self, object, min, max)
        if not min then
            min = self.min end
        if not max then
            max = self.max end
        
        local size = max - min

        local minExpanded = min - object.radius * Vector.one
        local maxExpanded = max + object.radius * Vector.one
        local sizeExpanded = maxExpanded - minExpanded

        if object.position.x >= minExpanded.x and
            object.position.y >= minExpanded.y and
            object.position.z >= minExpanded.z and
            
            object.position.x <= maxExpanded.x and
            object.position.y <= maxExpanded.y and
            object.position.z <= maxExpanded.z then
            
            if self.children then
                local halfSize = (max - min) * 0.5
                for i = 0, 1 do
                    for j = 0, 1 do
                        for k = 0, 1 do
                            local child = self.children[1 + i + j * 2 + k * 4]
                            if child then
                                child:doPhysics(
                                    object,
                                    min + i * halfSize * Vector.right
                                        + j * halfSize * Vector.up
                                        + k * halfSize * Vector.forward,
                                    min + (i + 1) * halfSize * Vector.right
                                        + (j + 1) * halfSize * Vector.up
                                        + (k + 1) * halfSize * Vector.forward)
                            end
                        end
                    end
                end
            else
                self:quadDoPhysics({
                        min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 0] * size
                    }, -Vector.forward, object)

                self:quadDoPhysics({
                        min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 1] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 1] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 1] * size,
                        min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 1] * size
                    }, Vector.forward, object)

                self:quadDoPhysics({
                        min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 1] * size,
                        min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 1] * size
                    }, -Vector.up, object)

                self:quadDoPhysics({
                        min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 1] * size,
                        min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 1] * size
                    }, Vector.up, object)

                self:quadDoPhysics({
                        min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 1] * size,
                        min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 1] * size
                    }, -Vector.right, object)

                self:quadDoPhysics({
                        min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 1] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 1] * size
                    }, Vector.right, object)
            end
        end
    end

    ret.raycast = function(self, hit, ray, min, max)
        if not min then
            min = self.min end
        if not max then
            max = self.max end
        local size = max - min

        -- If this is a leaf node, raycast a cube.
        if not self.children then
            local localHit = {}
            
            if math3d.rayBox(localHit, min, max, ray.o, ray.d) then
                local hitQuad = false

                if self:raycastQuad(hit, {
                        min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 0] * size
                    }, ray) then
                    hit.target = self
                    hitQuad = true
                end

                if self:raycastQuad(hit, {
                        min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 1] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 1] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 1] * size,
                        min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 1] * size
                    }, ray) then
                    hit.target = self
                    hitQuad = true
                end

                if self:raycastQuad(hit, {
                        min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 1] * size,
                        min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 1] * size
                    }, ray) then
                    hit.target = self
                    hitQuad = true
                end

                if self:raycastQuad(hit, {
                        min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 1] * size,
                        min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 1] * size
                    }, ray) then
                    hit.target = self
                    hitQuad = true
                end

                if self:raycastQuad(hit, {
                        min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 1] * size,
                        min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 1] * size
                    }, ray) then
                    hit.target = self
                    hitQuad = true
                end

                if self:raycastQuad(hit, {
                        min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 0] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 1] * size,
                        min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 1] * size
                    }, ray) then
                    hit.target = self
                    hitQuad = true
                end
                return hitQuad
            end
        -- If it is a branch node, continue with the children.
        else
            local halfSize = 0.5 * size
            local hitChild = false
            for i = 0, 1 do
                for j = 0, 1 do
                    for k = 0, 1 do
                        local child = self.children[1 + i + j * 2 + k * 4]
                        
                        if child then
                            if child:raycast(
                                hit,
                                ray,
                                min + i * halfSize * Vector.right
                                    + j * halfSize * Vector.up
                                    + k * halfSize * Vector.forward,
                                min + (i + 1) * halfSize * Vector.right
                                    + (j + 1) * halfSize * Vector.up
                                    + (k + 1) * halfSize * Vector.forward) then
                                hitChild = true
                            end
                        end
                    end
                end
            end
            return hitChild
        end
    end

    ret.createTri = function(self, tri, hintNormal)
        local n123 = Vector.cross(tri[2] - tri[1], tri[3] - tri[1]):normalize()
        if n123:dot(hintNormal) < 0 then
            n123 = -n123
        end
        local normals = { n123, n123, n123 }
        local tan1
        if math.abs(n123:dot(Vector.up)) ~= 1 then
            tan1 = -n123:cross(Vector.up):normalize()
        else
            tan1 = -n123:cross(Vector.forward):normalize()
        end
        local tan2 = n123:cross(tan1):normalize()
        local texCos = {
            Vector(tri[1]:dot(tan1) / 4, tri[1]:dot(tan2) / 4),
            Vector(tri[2]:dot(tan1) / 4, tri[2]:dot(tan2) / 4),
            Vector(tri[3]:dot(tan1) / 4, tri[3]:dot(tan2) / 4)
        }
        return self:createShape(tri, texCos, normals)
    end

    ret.createQuad = function(self, quad, hintNormal)
        self:createTri({ quad[1], quad[2], quad[3] }, hintNormal)
        self:createTri({ quad[3], quad[4], quad[1] }, hintNormal)
    end
    
    ret.createShape = function(self, vertices, texCos, normals)
        local root = self.root
        local mesh = root.mesh

        local binding = nil
        -- Find the material in the bindings.
        for i, b in ipairs(mesh.bindings) do
            if b.material == self.material then
                binding = b
                break
            end
        end

        -- If a binding was not found, create one.
        if not binding then
            binding = {
                material = self.material,
                indexArray = IndexArray:new {}
            }
            table.insert(mesh.bindings, binding)
        end

        for i, v in ipairs(vertices) do
            local n = normals[i]
            local tc = texCos[i]
            local index

            table.insert(mesh.pointCloud.vertex, v)
            table.insert(mesh.pointCloud.texCo, tc)
            table.insert(mesh.pointCloud.normal, n)
            index = #mesh.pointCloud.vertex
            -- Insert the index.
            table.insert(binding.indexArray, index)
        end
    end

    ret.raycastQuad = function(self, hit, quad, ray)
        if math3d.rayTriangle(hit, quad[1], quad[2], quad[3], ray.o, ray.d) then
            return true
        elseif math3d.rayTriangle(hit, quad[3], quad[4], quad[1], ray.o, ray.d) then
            return true
        end
        return false
    end

    ret.update = function(self, min, max, keepSides)
        if not min then
            min = self.min end
        if not max then
            max = self.max end

        -- If we are the root, we may need to create the mesh.
        if self.root == self then
            --[[bindings will hold the material and IndexArray for parts of the
               octree.]]
            if not self.mesh then
                self.mesh = {
                    pointCloud = PointCloud:new {
                        vertex = VertexArray:new {},
                        normal = VertexArray:new {},
                        texCo = VertexArray:new {}
                    },
                    bindings = {},
                }
            else
                self.mesh.pointCloud:clear()
                for i, b in ipairs(self.mesh.bindings) do
                    if b.indexArray then
                        b.indexArray:clear()
                    end
                end
            end
        end

        local size = max - min

        -- If it is a leaf node, create a cube.
        if not self.children then
            
            if not keepSides or keepSides[5] then
                self:createQuad({
                    min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 0] * size,
                    min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 0] * size,
                    min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 0] * size,
                    min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 0] * size
                }, -Vector.forward)
            end

            if not keepSides or keepSides[6] then
                self:createQuad({
                    min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 1] * size,
                    min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 1] * size,
                    min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 1] * size,
                    min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 1] * size
                }, Vector.forward)
            end

            if not keepSides or keepSides[3] then
                self:createQuad({
                    min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 0] * size,
                    min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 0] * size,
                    min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 1] * size,
                    min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 1] * size
                }, -Vector.up)
            end
            if not keepSides or keepSides[4] then
                self:createQuad({
                    min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 0] * size,
                    min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 0] * size,
                    min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 1] * size,
                    min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 1] * size
                }, Vector.up)
            end
            if not keepSides or keepSides[1] then
                self:createQuad({
                    min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 0] * size,
                    min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 0] * size,
                    min + self.offsets[1 + 1 * 0 + 2 * 1 + 4 * 1] * size,
                    min + self.offsets[1 + 1 * 0 + 2 * 0 + 4 * 1] * size
                }, -Vector.right)
            end
            if not keepSides or keepSides[2] then
                self:createQuad({
                    min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 0] * size,
                    min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 0] * size,
                    min + self.offsets[1 + 1 * 1 + 2 * 1 + 4 * 1] * size,
                    min + self.offsets[1 + 1 * 1 + 2 * 0 + 4 * 1] * size
                }, Vector.right)
            end
                
        -- If it is a branch node, continue with the children.
        else
            local halfSize = 0.5 * size
            local originalOffsets = {}

            for i = 0, 1 do
                for j = 0, 1 do
                    for k = 0, 1 do
                        originalOffsets[1 + 1 * i + 2 * j + 4 * k] = Vector(
                            i,
                            j,
                            k)
                    end
                end
            end

            for i = 0, 1 do
                for j = 0, 1 do
                    for k = 0, 1 do
                        local child = self.children[1 + i + j * 2 + k * 4]
                        if child then
                            -- Determine which sides will be visible.
                            local childKeepSides = {}

                            local ai, aj, ak

                            if i == 0 then
                                adjacent = self.children[1 + 1 + j * 2 + k * 4]
                                childKeepSides[1] = keepSides == nil or keepSides[1]
                                childKeepSides[2] = adjacent == nil or
                                    adjacent.children or
                                    adjacent.offsets[1] ~= originalOffsets[1] or
                                    adjacent.offsets[3] ~= originalOffsets[3] or
                                    adjacent.offsets[5] ~= originalOffsets[5] or
                                    adjacent.offsets[7] ~= originalOffsets[7]
                            else
                                adjacent = self.children[1 + 0 + j * 2 + k * 4]
                                childKeepSides[1] = adjacent == nil or
                                    adjacent.children or
                                    adjacent.offsets[2] ~= originalOffsets[2] or
                                    adjacent.offsets[4] ~= originalOffsets[4] or
                                    adjacent.offsets[6] ~= originalOffsets[6] or
                                    adjacent.offsets[8] ~= originalOffsets[8]
                                childKeepSides[2] = keepSides == nil or keepSides[2]
                            end
                            if j == 0 then
                                childKeepSides[3] = keepSides == nil or keepSides[3]
                                adjacent = self.children[1 + i + 1 * 2 + k * 4]
                                childKeepSides[4] = adjacent == nil or
                                    adjacent.children or
                                    adjacent.offsets[1] ~= originalOffsets[1] or
                                    adjacent.offsets[2] ~= originalOffsets[2] or
                                    adjacent.offsets[5] ~= originalOffsets[5] or
                                    adjacent.offsets[6] ~= originalOffsets[6]
                            else
                                adjacent = self.children[1 + i + 0 * 2 + k * 4]
                                childKeepSides[3] = adjacent == nil or
                                    adjacent.children or
                                    adjacent.offsets[3] ~= originalOffsets[3] or
                                    adjacent.offsets[4] ~= originalOffsets[4] or
                                    adjacent.offsets[7] ~= originalOffsets[7] or
                                    adjacent.offsets[8] ~= originalOffsets[8]
                                childKeepSides[4] = keepSides == nil or keepSides[4]
                            end
                            if k == 0 then
                                childKeepSides[5] = keepSides == nil or keepSides[5]
                                adjacent = self.children[1 + i + j * 2 + 1 * 4]
                                childKeepSides[6] = adjacent == nil or
                                    adjacent.children or
                                    adjacent.offsets[1] ~= originalOffsets[1] or
                                    adjacent.offsets[2] ~= originalOffsets[2] or
                                    adjacent.offsets[3] ~= originalOffsets[3] or
                                    adjacent.offsets[4] ~= originalOffsets[4]
                            else
                                adjacent = self.children[1 + i + j * 2 + 0 * 4]
                                childKeepSides[5] = adjacent == nil or
                                    adjacent.children or
                                    adjacent.offsets[5] ~= originalOffsets[5] or
                                    adjacent.offsets[6] ~= originalOffsets[6] or
                                    adjacent.offsets[7] ~= originalOffsets[7] or
                                    adjacent.offsets[8] ~= originalOffsets[8]
                                childKeepSides[6] = keepSides == nil or keepSides[6]
                            end

                            if not child.material then
                                child.material = self.material
                            end
                            child:update(
                                min + i * halfSize * Vector.right
                                    + j * halfSize * Vector.up
                                    + k * halfSize * Vector.forward,
                                min + (i + 1) * halfSize * Vector.right
                                    + (j + 1) * halfSize * Vector.up
                                    + (k + 1) * halfSize * Vector.forward,
                                childKeepSides)
                        end
                    end
                end
            end
        end
    end
    
    ret.render = function(self, scene)
        for i, binding in ipairs(self.mesh.bindings) do
            binding.material.shader.transform = scene.activeCamera.viewMatrix
            binding.material.shader.projection = scene.activeCamera.projection
            binding.material:bind()
            self.mesh.pointCloud:bind(binding.material.shader)
            binding.indexArray:render()
        end
    end
    
    setmetatable(ret, self.__mt)
    return ret
end

return Octree
