
local Pistol = {}
local ActionPlayer = require("wrap/ActionPlayer")
local Light = require("game/Light")
local models = require("models")

function Pistol.new(self)
    local weapon = {
        arms = models.arm1:clone(),
        model = models.pistol1:clone(),
        matrix = Matrix.identity,
    }
    weapon.armsIdleAnimation = ActionPlayer:new {
        target = weapon.arms.skeleton,
        action = weapon.arms.actions.pistolHold1,
        loop = true
    }
    weapon.weaponIdleAnimation = ActionPlayer:new {
        target = weapon.model.skeleton,
        action = weapon.model.actions.idle1,
        loop = true
    }
    weapon.muzzleFlareLight = Light:new {
        distance = 0.5
    }

    weapon.fire1 = function(self, scene)
        if (not self.armsFireAnimation
                or not self.armsFireAnimation.isPlaying)
            and (not self.weaponFireAnimation
                or not self.weaponFireAnimation.isPlaying) then
            if self.player.aimHit then
                if self.player.aimHit.target.takeDamage then
                    self.player.aimHit.target:takeDamage(scene, 10)
                end
            end
            scene:add(self.muzzleFlareLight)
            self.armsFireAnimation = ActionPlayer:new {
                target = self.arms.skeleton,
                action = self.arms.actions.pistolFire1,
            }
            self.weaponFireAnimation = ActionPlayer:new {
                target = self.model.skeleton,
                action = self.model.actions.fire1,
                events = {
                    {
                        time = 0.05,
                        callback = function()
                            scene:remove(self.muzzleFlareLight)
                        end
                    }
                }
            }
        end
    end

    weapon.postTick = function(self, deltaT, scene)
        if self.armsIdleAnimation then
            self.armsIdleAnimation:tick(deltaT)
        end
        if self.weaponIdleAnimation then
            self.weaponIdleAnimation:tick(deltaT)
        end
        if self.armsFireAnimation
                and self.armsFireAnimation.isPlaying then
            self.armsFireAnimation:tick(deltaT)
            self.armsIdleAnimation.time = 0
        end
        if self.weaponFireAnimation
                and self.weaponFireAnimation.isPlaying then
            self.weaponFireAnimation:tick(deltaT)
            self.weaponIdleAnimation.time = 0
        end
        self.muzzleFlareLight.position = 
                    self.matrix
                        * self.model.skeleton.namedBones
                            .muzzleFlare.head
    end

    weapon.render = function(self, scene, pass)
        if scene.mode == "play" then
            if pass == scene.passes.opaque then
                self.arms:render(
                    scene,
                    self.matrix)
            elseif pass == scene.passes.transparent then
                self.model:render(
                    scene,
                    self.matrix
                        * self.arms.skeleton.namedBones
                            .trigger.matrixLocal)
            end
        end
    end

    return weapon
end

return Pistol

