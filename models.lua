
local models = {}

modelPaths = {
    shotgun1 = "models/shotgun1.tmd"
}

-- Load each model.
for key, path in modelPaths do
    local results, e = tmd.loadTmd(path)
    if not results then
        error("error: tmd: " .. e .. ".")
    else
        models[key] = results
    end
end

return models
