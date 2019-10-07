
local Camera = require("wrap/Camera")
local VertexArray = require("wrap/VertexArray")
local PointCloud = require("wrap/PointCloud")
local GameObject = require("wrap/GameObject")
local IndexArray = require("wrap/IndexArray")
local Octree = require("octree/Octree")
local Material = require("wrap/Material")
local Mesh = require("wrap/Mesh")
local SkinnedMesh = require("wrap/SkinnedMesh")
local ActionPlayer = require("wrap/ActionPlayer")
local Scene = require("wrap/Scene")

local SphereMonster = require("game/SphereMonster")
local Player = require("game/Player")

local keys = require("keys")
local cursorMode = require("cursorMode")

local config = require("config")

local cubeMesh = require("mesh/cubeMesh")

local shaders = require("shaders")

local materials = require("materials")
local models = require("models")


function saveMap(name)
    local filename
    if config.mapDir then
        filename = config.mapDir .. "/" .. name .. ".lua"
    else
        filename = name .. ".lua"
    end
    file = io.open(filename, 'w')
    if file then
        octree:toLua(file, materials)
        file:close()
        print(string.format("Wrote file \"%s\".", filename))
    else
        print(string.format("Error writing file \"%s\".",
            filename))
    end
end

function loadMap(name)
    local filename
    if config.mapDir then
        filename = config.mapDir .. "/" .. name .. ".lua"
    else
        filename = name .. ".lua"
    end
    local fun, e = loadfile(filename, "bt", {
        Octree = Octree,
        Vector = Vector,
        Matrix = Matrix,
        materials = materials
    })

    if not fun then
        return nil, e
    end

    local result, map = pcall(fun)
    if result then
        return map
    else
        return result, map
    end
end


if config.map then
    mapName = config.map
    octree, e = loadMap(mapName)
    if not octree then
        print("Error loading map: " .. e)
        octree = Octree:new {}
        octree.min = Vector(-8, -8, -8)
        octree.max = Vector(8, 8, 8)
    end
else
    mapName = os.date("tmp_%Y_%m_%d_%H%M")
    octree = Octree:new {}
    octree.min = Vector(-8, -8, -8)
    octree.max = Vector(8, 8, 8)
end

keysDown = {}
mouseButtonsDown = {}

window.width = 800
window.height = 600

player = Player:new {
    position = Vector(0, 0, octree.min.z - 2)
}

scene = Scene:new {
    objects = {
        player,
        player.camera,
        player.weapon,
        SphereMonster:new {
            position = Vector(0, octree.max.y + 2, 0)
        },
        octree
    },
    activeCamera = player.camera
}

octree.material = materials.default
octree:update()

window.cursorMode = cursorMode.DISABLED

function render()
    gl.polygonMode.fill()
    scene:render()

    -- Draw weapon viewmodel.
    if player.mode == "play" then
        player.weapon.model:render(
            scene,
            player.weapon.matrix)
    end

    -- Draw editing stuff.
    if player.mode == "edit" then
        -- Draw aim.
        if player.aimHit then
            shaders.flat.color = Vector(1, 0, 0, 1)
            shaders.flat.projection = 
                scene.activeCamera.projection
            shaders.flat.transform = 
                scene.activeCamera.viewMatrix
                * Matrix.newTranslate(player.aimHit.position)
                * Matrix.newScale(0.05 * Vector.one)
                * Matrix.newTranslate(-0.5 * Vector.one)
                
            cubeMesh.pointCloudV:bind(shaders.flat)
            cubeMesh.indexArray:render()
        end
        
        -- Draw axes.
        if player.selection then
            drawAxes(scene, player.selection.position)
            gl.polygonMode.line()
            shaders.flat.color = Vector(1, 1, 0, 1)
            shaders.flat.projection =
                scene.activeCamera.projection
            shaders.flat.transform =
                scene.activeCamera.viewMatrix
                * Matrix.newTranslate(player.selection.min
                    - Vector(0.005, 0.005, 0.005))
                * Matrix.newScale(player.selection.size
                    + Vector(0.01, 0.01, 0.01))
            cubeMesh.pointCloudV:bind(shaders.flat)
            cubeMesh.indexArray:render()
        end
    end
end

function drawAxis(origin, axis, color)
    shaders.flat.projection =
        scene.activeCamera.projection
    shaders.flat.transform =
        scene.activeCamera.viewMatrix
        * Matrix.newTranslate(origin)
        * Matrix.newTranslate(Vector(-0.0125, -0.0125, -0.0125))
        * Matrix.newScale(axis + Vector(0.025, 0.025, 0.025))
    shaders.flat.color = color
    cubeMesh.pointCloudV:bind(shaders.flat)
    cubeMesh.indexArray:render()
end

function drawAxes(scene, origin)
    drawAxis(origin, Vector(0.5, 0, 0), Vector(1, 0, 0))
    drawAxis(origin, Vector(0, 0.5, 0), Vector(0, 1, 0))
    drawAxis(origin, Vector(0, 0, 0.5), Vector(0, 0, 1))

    shaders.flat.transform = 
        scene.activeCamera.viewMatrix
        * Matrix.newTranslate(origin)
        * Matrix.newScale(Vector(0.05, 0.05, 0.05))
        * Matrix.newTranslate(Vector(-0.5, -0.5, -0.5))
    shaders.flat.color = Vector(1, 1, 0)
    cubeMesh.pointCloudV:bind(shaders.flat)
    cubeMesh.indexArray:render()
end

local lastUpdateTime = window.time
function window.callbacks.update()
    local time = window.time
    local elapsedTime = time - lastUpdateTime
    scene:preTick(elapsedTime)
    scene:tick(elapsedTime)
    scene:postTick(elapsedTime)

    lastUpdateTime = time
    render()
end

function window.callbacks.key(key, scancode, action, mods)
    if action == keys.PRESS then
        keysDown[key] = true

        if key == keys.KEY_G then
            if player.mode == "edit" then
                player.gravity = Vector(0, -9.8, 0)
                player.mode = "play"
            else
                player.gravity = Vector()
                player.velocity = Vector()
                player.mode = "edit"
            end
        end

        if player.mode == "edit" then
            if keysDown[keys.KEY_LEFT_ALT] then
                if key == keys.KEY_E then
                    saveMap(mapName)
                end
                if player.selection then
                    s = player.selection
                    direction = Vector()
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
                    if keysDown[config.keyDown] then
                        direction.y = direction.y - 1
                    end
                    if keysDown[config.keyUp] then
                        direction.y = direction.y + 1
                    end
                    if #direction > 0 then
                        s.octree.offsets[s.vertex] =
                            (s.octree.offsets[s.vertex] + direction * 0.25):clamp()
                        s.position = s.min + s.octree.offsets[s.vertex] * s.size
                        s.octree.root:update()
                    end
                end
            else
                if key == config.keySubdivide then
                    if player.selection then
                        player.selection.octree:subdivide()
                        player:selectOctree(
                            player.selection.octree[player.selection.vertex])
                        
                    end
                elseif key == config.keyUnsubdivide then
                    if player.selection and
                            player.selection.octree.parent then
                        player:selectOctree(
                            player.selection.octree.parent)
                        player.selection.octree:unsubdivide()
                    end
                elseif key == config.keyScaleUp then
                    octree.min = octree.min * 2
                    octree.max = octree.max * 2
                    octree:update()
                    player.selection = nil
                elseif key == config.keyScaleDown then
                    octree.min = octree.min * 0.5
                    octree.max = octree.max * 0.5
                    octree:update()
                    player.selection = nil
                elseif key == config.keyExtrude then
                    local min, max, size, depth
                    target = player.aimHit.target
                    min, max, size, depth = octree:calcBounds(target)
                    octree:createAround(
                        (min + max) * 0.5
                            - player.aim:normalize() * size,
                        depth)
                    octree:update()
                elseif key == config.keyNextMaterial then
                    if player.selection then
                        local found = false
                        local material = nil
                        local first = nil
                        -- Calculate material index.
                        for k, v in pairs(materials) do
                            if not first then
                                first = v
                            end
                            if found then
                                material = v
                                break
                            end
                            if player.selection.octree.material == v then
                                found = true
                            end
                        end
                        if not material then
                            material = first
                        end
                        player.selection.octree.material = material
                        octree:update()
                    end
                elseif key == config.keyPreviousMaterial then
                    if player.selection then
                        local material = nil
                        -- Calculate material index.
                        for k, v in pairs(materials) do
                            if player.selection.octree.material == v then
                                break
                            end
                            material = v
                        end
                        player.selection.octree.material = material
                        octree:update()
                    end
                end
            end
        end
    elseif action == keys.RELEASE then
        keysDown[key] = false

        if key == keys.KEY_ESCAPE then
            window.cursorMode = cursorMode.NORMAL
        end
    end
end

lastMousePos = nil
function window.callbacks.mouseMove(x, y)
    if not lastMousePos then
        lastMousePos = { x, y }
    end
    
    local deltaX = x - lastMousePos[1]
    local deltaY = y - lastMousePos[2]

    player.lookX = player.lookX - deltaX * config.mouseSensitivity
        * math.pi / 180
    player.lookY = player.lookY - deltaY * config.mouseSensitivity
        * math.pi / 180

    lastMousePos = { x, y }
end

function window.callbacks.mouse(button, action, mods)
    if action == keys.PRESS then
        mouseButtonsDown[button] = true
        window.cursorMode = cursorMode.DISABLED
    elseif action == keys.RELEASE then
        mouseButtonsDown[button] = false
    end
    if player.mode == "edit" then
        if button == config.mouseButtonRemove
                and action == keys.PRESS then
            if player.aimHit then
                -- Delete the target.
                if player.aimHit.target.parent then
                    player.aimHit.target.parent[
                        player.aimHit.target.index] = nil
                end
                octree:update()
                player.selection = nil
            end
        end
    elseif player.mode == "play" then
        if action == keys.PRESS
                and button == config.mouseButtonFire1 then
            if not player.weapon.currentAnimation
                    or not player.weapon.currentAnimation.isPlaying then
                player.weapon:fire1(scene)
            end
        end
    end
end
