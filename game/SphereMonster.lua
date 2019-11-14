

local ActionPlayer = require("wrap/ActionPlayer")
local models = require("models")

local Explosion = require("game/Explosion")

local SphereMonster = {}

SphereMonster.previewModel = models.sphereMonster1
SphereMonster.spawnDistance = 0.6

function SphereMonster.new(self, attributes)
    if not attributes then
        error("SphereMonster.new expects table attributes.")
    end
    local ret = {
        mesh = models.sphereMonster1:clone(),
        physics = {
            enabled = true,
            radius = 0.6,
            gravity = Vector(0, -9.8, 0),
            velocity = Vector(),
        },
        trigger = {
            enabled = true,
            radius = 5
        },
        lookX = 0,
        walkSpeed = 4.0
    }

    ret.onTriggerStay = function(self, other)
        if other.player then
            self.target = other
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

    ret.takeDamage = function(self, scene, damage)
        scene:remove(self)
        scene:add(Explosion:new {
            matrix = Matrix.newTranslate(self.position)
        })
    end

    if attributes.position then
        ret.position = attributes.position
    else
        ret.position = Vector(0, 0, 0)
    end

    if attributes.lookX then
        ret.lookX = attributes.lookX
    end
    if attributes.lookY then
        ret.lookY = attributes.lookY
    end

    ret.currentAnimation = ActionPlayer:new {
        target = ret.mesh.skeleton,
        action = ret.mesh.actions.run1,
        loop = true
    }
    ret.render = function(self, scene)
        self.mesh:render(scene,
            Matrix.newTranslate(self.position
                - Vector.up * self.physics.radius)
            * Matrix.newRotate(self.lookX, Vector.up))
    end
    ret.preTick = function(self, deltaT, scene)
        self.physics.velocity = self.physics.velocity
            + self.physics.gravity * deltaT
        if self.target then
            local delta = self.position - self.target.position
            self.lookX = math.atan2(delta.x, delta.z)
            self.position = self.position
                - Matrix.newRotate(self.lookX, Vector.up)
                    * Vector.forward * deltaT * self.walkSpeed
        end
        self.position = self.position + self.physics.velocity * deltaT
    end
    ret.tick = function(self, deltaT, scene)
        self.currentAnimation:tick(deltaT)
    end
    return ret
end

return SphereMonster

