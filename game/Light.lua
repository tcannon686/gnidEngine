local Light = {}
local models = require("models")

Light.previewModel = models.light1
Light.spawnDistance = 0.5

function Light.new(self, attributes)
    local ret = {
        light = { distance = 1.0 },
        position = Vector(),
        previewModel = self.previewModel,
        physics = {
            enabled = false,
            radius = 0.5,
        },
        editOnly = true
    }

    if attributes.position then
        ret.position = attributes.position
    end
    if attributes.distance then
        ret.light.distance = attributes.distance
    end

    ret.toLua = function(self, out)
        local r, e
        
        r, e = out:write("objects.Light:new{position=Vector("
            .. self.position.x .. ","
            .. self.position.y .. ","
            .. self.position.z .. "),")
        if not r then return false, e end
        r, e = out:write("distance=" .. self.light.distance)
        if not r then return false, e end
        r, e = out:write("}")
        if not r then return false, e end
        return true
    end

    ret.render = function(self, scene)
        if scene.mode == "edit" then
            self.previewModel:render(
                scene,
                Matrix.newTranslate(self.position))
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

    return ret
end

return Light
