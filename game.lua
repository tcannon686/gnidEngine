
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
local Text = require("wrap/Text")

local SphereMonster = require("game/SphereMonster")
local Player = require("game/Player")

local keys = require("keys")
local cursorMode = require("cursorMode")

local config = require("config")

local cubeMesh = require("mesh/cubeMesh")

local shaders = require("shaders")

local materials = require("materials")
local models = require("models")
local objects = require("objects")


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
        octree
    },
    activeCamera = player.camera
}

uiCamera = Camera:new {
    projection = Matrix.newOrthographic(
        0, window.width,
        0, window.height,
        -1, 1),
    matrix = Matrix.identity
}

historyText = Text:new {
    text = "",
    position = Vector(0, 0),
    scale = Vector.one * 10
}
commandText = Text:new {
    text = "",
    position = Vector(0, 0),
    scale = Vector.one * 10
}
uiScene = Scene:new {
    objects = {
        uiCamera,
        commandText,
        historyText
    },
    activeCamera = uiCamera
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

        -- Draw preview of object to add.
        if keysDown[config.keyCreateObject] and not player.waitForCommand then
            gl.polygonMode.fill()
            previewModel = objects[player.currentObjectKey].previewModel
            if previewModel and player.aimHit then
                previewModel:render(
                    scene,
                    Matrix.newTranslate(player.aimHit.position + Vector.up))
            end
        end
        
        gl.polygonMode.fill()
        uiScene:render()
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

function cycleForward(list, key)
    local ret = key
    repeat
        ret = next(
            list,
            ret)
    until ret ~= nil
    return ret
end

function cycleBackward(list, key)
    local ret = key
    local cur = next(list, key)
    while cur ~= key do
        if cur ~= nil then
            ret = cur
        end
        cur = next(list, cur)
    end
    return ret
end

function window.callbacks.char(char)
    if player.waitForCommand then
        commandText.text = commandText.text .. string.char(char)
    end
end

function window.callbacks.key(key, scancode, action, mods)
    if action == keys.PRESS or action == keys.REPEAT then
        -- Command line
        if player.waitForCommand then
            if key == keys.KEY_BACKSPACE then
                commandText.text = commandText.text:sub(0, -2)
                if #commandText.text == 0 then
                    player.waitForCommand = false
                    historyText.text = ''
                end
            elseif key == keys.KEY_ENTER then
                local env = {
                    selection = player.selection,
                    player = player,
                    models = models,
                    materials = materials,
                    objects = objects,
                    scene = scene,
                    Matrix = Matrix,
                    Vector = Vector,
                    table = table,
                    string = string,
                    math = math,
                    pairs = pairs,
                    ipairs = ipairs,
                    print = function(...)
                        for i, v in pairs { ... } do
                            historyText.text = historyText.text
                                .. tostring(v) .. " "
                        end
                        historyText.text = historyText.text .. "\n"
                    end
                }
                if player.selection then
                    env.object = player.selection.object
                    env.octree = player.selection.octree
                end
                local command = commandText.text:sub(2)
                commandText.text = "`"
                historyText.text = historyText.text .. "`" .. command .. "\n"
                local chunk, e = load(command, command, 't', env)
                if not chunk then
                    historyText.text = historyText.text .. e .. "\n"
                else
                    local status, result = pcall(chunk)
                    if result ~= nil then
                        historyText.text = historyText.text
                            .. tostring(result) .. "\n" end
                end
                historyText.position.y = 10 * (historyText.lineCount - 1)
            end
        end
    end

    if action == keys.PRESS then
        keysDown[key] = true

        if key == keys.KEY_G then
            if player.mode == "edit" and not player.waitForCommand then
                player.gravity = Vector(0, -9.8, 0)
                player.mode = "play"
            else
                player.gravity = Vector()
                player.velocity = Vector()
                player.mode = "edit"
            end
        end


        if player.mode == "edit" and not player.waitForCommand then
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
                        if s.octree then
                            s.octree.offsets[s.vertex] =
                                (s.octree.offsets[s.vertex]
                                    + direction * 0.25):clamp()
                            s.position = s.min
                                + s.octree.offsets[s.vertex] * s.size
                            s.octree.root:update()
                        else
                            s.object.position = s.object.position
                                + direction * 0.25
                        end
                    end
                end
            elseif not player.waitForCommand then
                if key == config.keySubdivide then
                    if player.selection and player.selection.octree then
                        player.selection.octree:subdivide()
                        player:selectOctree(
                            player.selection.octree[player.selection.vertex])
                        
                    end
                elseif key == config.keyUnsubdivide then
                    if player.selection and player.selection.octree and
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
                elseif key == config.keyMaterial then
                    if player.selection and player.selection.octree then
                        player.selection.octree.material =
                            materials[player.currentMaterialKey]
                        octree:update()
                    end
                elseif key == config.keyNext then
                    if keysDown[config.keyMaterial] then
                        player.currentMaterialKey = cycleForward(
                            materials,
                            player.currentMaterialKey)
                        if player.selection and player.selection.octree then
                            player.selection.octree.material =
                                materials[player.currentMaterialKey]
                            octree:update()
                        end
                    elseif keysDown[config.keyCreateObject] then
                        player.currentObjectKey = cycleForward(
                            objects,
                            player.currentObjectKey)
                    elseif keysDown[config.keyLight] then
                        if player.selection and player.selection.object
                                and player.selection.object.light then
                            player.selection.object.light.distance = 
                                player.selection.object.light.distance + 1
                        end
                    end
                elseif key == config.keyPrevious then
                    if keysDown[config.keyMaterial] then
                        if player.selection and player.selection.octree then
                            player.currentMaterialKey = cycleBackward(
                                materials,
                                player.currentMaterialKey)
                            player.selection.octree.material =
                                materials[player.currentMaterialKey]
                            octree:update()
                        end
                    elseif keysDown[config.keyCreateObject] then
                        player.currentObjectKey = cycleBackward(
                            objects,
                            player.currentObjectKey)
                    elseif keysDown[config.keyLight] then
                        if player.selection and player.selection.object
                                and player.selection.object.light then
                            player.selection.object.light.distance = 
                                player.selection.object.light.distance - 1
                        end
                    end
                elseif key == config.keyCommand then
                    player.waitForCommand = true
                end
            end
        end
    elseif action == keys.RELEASE then
        keysDown[key] = false

        if key == keys.KEY_ESCAPE then
            window.cursorMode = cursorMode.NORMAL
        end

        if player.mode == 'edit' and not player.waitForCommand then
            if key == config.keyCreateObject then
                if player.aimHit then
                    scene:add(objects[player.currentObjectKey]:new {
                        position = player.aimHit.position + Vector.up
                    })
                end
            end
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
                -- If it is an octree, remove it from its parent.
                if Octree:isOctree(player.aimHit.target) then
                    if player.aimHit.target.parent then
                        player.aimHit.target.parent[
                            player.aimHit.target.index] = nil
                    end
                -- Otherwise remove it from the scene.
                else
                    scene:remove(player.aimHit.target)
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
