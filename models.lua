
local tmd = require("tmd")
local models = {}

modelPaths = {
    shotgun1 = "models/shotgun1.tmd",
    explosion1 = "models/explosion1.tmd",
    sphereMonster1 = "models/sphereMonster1.tmd"
}

-- Load each model.
for key, path in pairs(modelPaths) do
    local results, e = tmd.loadTmd(path)
    if not results then
        error("error: tmd: " .. e .. ".")
    else
        models[key] = results
    end
end

return models
