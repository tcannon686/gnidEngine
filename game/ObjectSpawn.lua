
local ObjectSpawn = {}
local models = require("models")

ObjectSpawn.previewModel = models.objectSpawn1
ObjectSpawn.spawnDistance = 0.75

function ObjectSpawn.new(self, attributes)
    local ret = {
        objectSpawn = {
            object = nil,
            objectKey = nil,
            count = 1,
            spawnCount = 0,
            spawnedObjects = {},
        },
        position = Vector(),
        lookX = 0,
        lookY = 0,
        previewModel = models.objectSpawn1,
        physics = {
            enabled = false,
            radius = 0.75,
        },
        object = nil,
        editOnly = true,
    }

    if attributes.position then
        ret.position = attributes.position
    end
    if attributes.lookX then
        ret.lookX = attributes.lookX
    end
    if attributes.lookY then
        ret.lookY = attributes.lookY
    end
    if attributes.object then
        ret.objectSpawn.object = attributes.object
        if attributes.objectKey then
            ret.objectSpawn.objectKey = attributes.objectKey
        else
            error("ObjectSpawn:new expects table with key objectKey.")
        end
    elseif attributes.objectKey then
        error("ObjectSpawn:new expects table with key object.")
    end

    ret.acceptObject = function(self, key, object)
        self.objectSpawn.object = object
        self.objectSpawn.objectKey = key
    end

    ret.render = function(self, scene)
        if scene.mode == "edit" then
            self.previewModel:render(
                scene,
                Matrix.newTranslate(self.position
                        - self.physics.radius * Vector.up)
                   * Matrix.newRotate(self.lookX, Vector(0, 1, 0))
                   * Matrix.newRotate(self.lookY, Vector(1, 0, 0)))
            local object = self.objectSpawn.object
            if object and object.previewModel then
                object.previewModel:render(
                    scene,
                    Matrix.newTranslate(self.position +
                            (object.spawnDistance - self.physics.radius)
                                * Vector.up)
                       * Matrix.newRotate(self.lookX, Vector(0, 1, 0))
                       * Matrix.newRotate(self.lookY, Vector(1, 0, 0)))
            end
        end
    end

    ret.preTick = function(self, deltaT, scene)
        if scene then
            local objectSpawn = self.objectSpawn
            if scene.mode == "edit" then
                if objectSpawn.spawnCount > 0 then
                    for i, spawnedObject in ipairs(objectSpawn.spawnedObjects) do
                        scene:remove(spawnedObject)
                    end
                    objectSpawn.spawnCount = 0
                    objectSpawn.spawnedObjects = {}
                end
            elseif scene.mode == "play" then
                if objectSpawn.spawnCount < objectSpawn.count then
                    local spawnedObject = objectSpawn.object:new {
                        position = self.position +
                                (objectSpawn.object.spawnDistance
                                        - self.physics.radius)
                                    * Vector.up,
                        lookX = self.lookX,
                        lookY = self.lookY
                    }
                    scene:add(spawnedObject)

                    objectSpawn.spawnCount = objectSpawn.spawnCount + 1
                    objectSpawn.spawnedObjects[objectSpawn.spawnCount] = spawnedObject
                end
            end
        end
    end

    ret.raycast = function(self, hit, ray)
        if math3d.raySphere(
                hit,
                self.position,
                self.physics.radius,
                ray.o,
                ray.d) then
            hit.target = self
            return true
        else
            return false
        end
    end

    ret.toLua = function(self, out)
        local r, e
        
        r, e = out:write("objects.ObjectSpawn:new{position=Vector("
            .. self.position.x .. ","
            .. self.position.y .. ","
            .. self.position.z .. "),")
        if not r then return false, e end
        r, e = out:write("lookX=" .. self.lookX .. ",")
        if not r then return false, e end
        r, e = out:write("lookY=" .. self.lookY .. ",")
        if not r then return false, e end
        r, e = out:write("object=objects['" .. self.objectSpawn.objectKey
                .. "'],")
        if not r then return false, e end
        r, e = out:write("objectKey='" .. self.objectSpawn.objectKey .. "'")
        if not r then return false, e end
        r, e = out:write("}")
        if not r then return false, e end
        return true
    end

    return ret
end

return ObjectSpawn
