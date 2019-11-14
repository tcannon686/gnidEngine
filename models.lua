
local tmd = require("tmd")
local models = {}

modelPaths = {
    arm1 = "models/arm1.tmd",
    shotgun1 = "models/shotgun1.tmd",
    pistol1 = "models/pistol1.tmd",
    explosion1 = "models/explosion1.tmd",
    sphereMonster1 = "models/sphereMonster1.tmd",
    redDot1 = "models/redDot1.tmd",
    playerSpawn1 = "models/playerSpawn1.tmd",
    objectSpawn1 = "models/objectSpawn1.tmd",
    light1 = "models/light1.tmd",
}

-- Load each model.
for key, path in pairs(modelPaths) do
    local results, e = tmd.loadTmd(path)
    if not results then
        error("error: tmd: " .. path .. ": " .. e .. ".")
    else
        models[key] = results
    end
end

return models
