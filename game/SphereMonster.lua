

local ActionPlayer = require("wrap/ActionPlayer")
local models = require("models")

local Explosion = require("game/Explosion")

local SphereMonster = {}

SphereMonster.previewModel = models.sphereMonster1
SphereMonster.previewDistance = 0.6

function SphereMonster.new(self, attributes)
    if not attributes then
        error("Explosion.new expects table attributes.")
    end
    local ret = {
        mesh = models.sphereMonster1:clone(),
        radius = 0.6,
        enablePhysics = true,
        gravity = Vector(0, -9.8, 0),
        lookX = 0,
        walkSpeed = 4.0
    }

    ret.raycast = function(self, hit, ray)
        if math3d.raySphere(
                hit,
                self.position,
                self.radius,
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
    ret.velocity = Vector()

    ret.currentAnimation = ActionPlayer:new {
        target = ret.mesh.skeleton,
        action = ret.mesh.actions.run1,
        loop = true
    }
    ret.render = function(self, scene)
        self.mesh:render(scene,
            Matrix.newTranslate(self.position
                - Vector.up * self.radius)
            * Matrix.newRotate(self.lookX, Vector.up))
    end
    ret.preTick = function(self, deltaT, scene)
        self.velocity = self.velocity
            + self.gravity * deltaT
        self.lookX = self.lookX + deltaT
        self.position = self.position
            - Matrix.newRotate(self.lookX, Vector.up)
                * Vector.forward * deltaT * self.walkSpeed
        self.position = self.position + self.velocity * deltaT
    end
    ret.tick = function(self, deltaT, scene)
        self.currentAnimation:tick(deltaT)
    end
    return ret
end

return SphereMonster

