
local ActionPlayer = require("wrap/ActionPlayer")
local models = require("models")

local Explosion = {}

function Explosion.new(self, attributes)
    if not attributes then
        error("Explosion.new expects table attributes.")
    end
    local ret = {
        matrix = attributes.matrix,
        mesh = models.explosion1:clone(),
        light = {
            enabled = true,
            distance = 1.0
        },
        position = attributes.matrix * Vector(0, 0, 0, 1)
    }
    ret.currentAnimation = ActionPlayer:new {
        target = ret.mesh.skeleton,
        action = ret.mesh.actions.fire1,
        loop = false
    }
    ret.currentAnimation:tick(0)
    ret.render = function(self, scene, pass)
        if pass == scene.passes.opaque then
            self.mesh:render(scene, self.matrix)
        end
    end
    ret.tick = function(self, deltaT, scene)
        self.currentAnimation:tick(deltaT)
        self.light.distance = self.light.distance - deltaT * 2
        if not self.currentAnimation.isPlaying then
            scene:remove(self)
        end
    end
    return ret
end

return Explosion

