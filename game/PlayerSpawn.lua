local PlayerSpawn = {}
local models = require("models")

PlayerSpawn.previewModel = models.playerSpawn1
PlayerSpawn.previewDistance = 0.75

function PlayerSpawn.new(self, attributes)
    local ret = {
        playerSpawn = {},
        position = Vector(),
        lookX = 0,
        lookY = 0,
        previewModel = self.previewModel,
        physics = {
            enabled = false,
            radius = 0.75,
        },
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

    ret.render = function(self, scene)
        if scene.mode == "edit" then
            self.previewModel:render(
                scene,
                Matrix.newTranslate(self.position
                        - self.physics.radius * Vector.up)
                   * Matrix.newRotate(self.lookX, Vector(0, 1, 0))
                   * Matrix.newRotate(self.lookY, Vector(1, 0, 0)))
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
        
        r, e = out:write("objects.PlayerSpawn:new{position=Vector("
            .. self.position.x .. ","
            .. self.position.y .. ","
            .. self.position.z .. "),")
        if not r then return false, e end
        r, e = out:write("lookX=" .. self.lookX .. ",")
        if not r then return false, e end
        r, e = out:write("lookY=" .. self.lookY)
        if not r then return false, e end
        r, e = out:write("}")
        if not r then return false, e end
        return true
    end

    return ret
end

return PlayerSpawn
