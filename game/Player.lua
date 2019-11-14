
local Octree = require("octree/Octree")
local Camera = require("wrap/Camera")
local ActionPlayer = require("wrap/ActionPlayer")
local Scene = require("wrap/Scene")
local Explosion = require("game/Explosion")

local keys = require("keys")

local config = require("config")

local materials = require("materials")
local models = require("models")
local objects = require("objects")

local Player = {}

function getLookMatrix(lookX, lookY)
    return Matrix.newRotate(lookX, Vector(0, 1, 0))
           * Matrix.newRotate(lookY, Vector(1, 0, 0))
end

function Player.new(self, attributes)
    player = {
        lookY = 0,
        lookX = 0,
        speed = 5,

        physics = {
            enabled = true,
            radius = 0.75,
            velocity = Vector(),
            gravity = Vector(0, -9.8, 0),
        },
        jumpSpeed = 4,
        headHeight = 1.7,

        currentMaterialKey = "default",
        currentObjectKey = "SphereMonster",

        camera = Camera:new {},
    }

    if attributes.position then
        player.position = attributes.position
    end

    player.weapon = {
        arms = models.arm1:clone(),
        model = models.pistol1:clone(),
        matrix = Matrix.identity,
        player = player,
    }
    player.weapon.armsIdleAnimation = ActionPlayer:new {
        target = player.weapon.arms.skeleton,
        action = player.weapon.arms.actions.pistolHold1,
        loop = true
    }

    player.weapon.fire1 = function(self, scene)
        if self.player.aimHit then
            if self.player.aimHit.target.takeDamage then
                self.player.aimHit.target:takeDamage(scene, 10)
            end
        end
        self.armsFireAnimation = ActionPlayer:new {
            target = self.arms.skeleton,
            action = self.arms.actions.pistolFire1,
        }
    end

    player.weapon.tick = function(self, deltaT, scene)
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
    end

    player.getHeadPosition = function(self)
        return self.position
            + (self.headHeight - self.physics.radius) * Vector.up
    end

    player.selectOctree = function(self, target, point)

        if point == nil then
            if self.selection then
                point = self.selection.position
            else
                point = Vector()
            end
        end
        local min, max, size, depth
        min, max, size, depth = target.root:calcBounds(target)
        local pointIndex = target:calcPointIndex(
            point, min, max)
        local octree = target
        local position = min + octree.offsets[pointIndex] * size

        self.selection = {
            octree = octree,
            vertex = pointIndex,
            position = position,
            min = min,
            max = max,
            size = size,
            depth = depth
        }
    end

    player.selectObject = function(self, target)
        self.selection = {
            position = target.position,
            min = target.position - target.physics.radius * Vector.one,
            size = Vector.one * target.physics.radius * 2,
            object = target
        }
    end

    player.preTick = function(self, deltaT, scene)
        local direction = Vector()

        if not keysDown[keys.KEY_LEFT_ALT] and not self.waitForCommand then
            if keysDown[config.keyLeft] then
                direction.x = direction.x - 1
            end
            if keysDown[config.keyRight] then
                direction.x = direction.x + 1
            end
            if keysDown[config.keyForward] then
                direction.z = direction.z - 1
            end
            if keysDown[config.keyBackward] then
                direction.z = direction.z + 1
            end
            if scene.mode == "edit" then
                if keysDown[config.keyDown] then
                    direction.y = direction.y - 1
                    self.physics.velocity.y = 0
                end
                if keysDown[config.keyUp] then
                    direction.y = direction.y + 1
                    self.physics.velocity.y = 0
                end
            end
            if scene.mode == "play" then
                if keysDown[config.keyJump] then
                    if self.isGrounded then
                        self.physics.velocity.y = player.jumpSpeed
                    end
                end
            end
        end

        lenDirection = #direction
        if lenDirection > 0 then
            direction = Matrix.newRotate(self.lookX, Vector(0, 1, 0))
                * direction * (self.speed * deltaT / lenDirection)
            self.position = player.position + direction
            self.physics.velocity = self.physics.velocity * Vector.up
        end

        if self.physics.enabled then
            self.physics.velocity = self.physics.velocity
                + self.physics.gravity * deltaT
        end

        self.position = self.position + self.physics.velocity * deltaT
    end

    player.postTick = function(self, deltaT, scene)
        self.aim = 
                Vector.three(Matrix.newRotate(self.lookX, Vector(0, 1, 0))
                * Matrix.newRotate(self.lookY, Vector(1, 0, 0))
                * -Vector.forward)

        ray = {
            o = self:getHeadPosition(),
            d = self.aim
        }

        self.aimHit = {}
        if scene.mode == "play" then
            if not scene:raycast(
                    self.aimHit,
                    ray,
                    function(object) return not object.editorOnly end) then
                self.aimHit = nil
            end
        else
            if not scene:raycast(
                    self.aimHit,
                    ray) then
                self.aimHit = nil
            end
        end

        self.camera.matrix =
            Matrix.newTranslate(self:getHeadPosition())
                * getLookMatrix(self.lookX, player.lookY)

        self.weapon.matrix =
            Matrix.newTranslate(self:getHeadPosition())
            * Matrix.newTranslate(Vector(0, -0.05, 0))
            * getLookMatrix(self.lookX, player.lookY)
            * Matrix.newRotate(math.pi, Vector.up)

        if scene.mode == "edit" then
            -- Update selection sizes.
            if self.selection and self.selection.object then
                self.selection.position = self.selection.object.position
                self.selection.min = self.selection.object.position
                    - self.selection.object.physics.radius * Vector.one
                self.selection.size = self.selection.object.physics.radius
                    * Vector.one * 2
            end
            -- Update selection.
            if mouseButtonsDown[config.mouseButtonSelect] then
                if self.aimHit then
                    -- If aimed at an octree, select it.
                    if Octree:toOctree(self.aimHit.target) then
                        self:selectOctree(
                            self.aimHit.target,
                            self.aimHit.position)
                    -- Otherwise select as an object.
                    else
                        self:selectObject(self.aimHit.target)
                    end
                else
                    self.selection = nil
                end
            end
        end
    end
    player.render = function(self, scene)
        if scene.mode == "play" then
            self.weapon.arms:render(
                scene,
                self.weapon.matrix)

            self.weapon.model:render(
                scene,
                self.weapon.matrix
                    * self.weapon.arms.skeleton.namedBones.trigger.matrixLocal)
        end
    end
    return player
end

return Player
