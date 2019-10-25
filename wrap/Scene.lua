
local Octree = require("octree/Octree")

local Scene = {}

local shaders = require("shaders")

function Scene.new(self, attributes)
    if type(attributes.objects) ~= "table" then
        error("Scene.new expects table with key objects.")
    end

    local objectListMt = {}
    objectListMt.__pairs = function(t, k)
        return next, rawget(t, "__data")
    end
    objectListMt.__newindex = function(t, k, v)
        error("Scene objects list cannot be modified directly.")
    end
    objectListMt.__index = function(t, k)
        if type(k) == "number" then
            local i = 1
            for k1, v in pairs(t) do
                if i >= k then return v end
                i = i + 1
            end
        else
            return rawget(t, k)
        end
    end
    objectListMt.__len = function(t, k)
        local count = 0
        for t, v in pairs(t, "__data") do
            count = count + 1
        end
        return count
    end

    objects = { __data = {} }
    lights = { __data = {} }
    octrees = { __data = {} }
    for k, object in pairs(attributes.objects) do
        objects.__data[tostring(object)] = object
        if object.light then
            lights.__data[tostring(object)] = object
        end
        if Octree:isOctree(object) then
            octrees.__data[tostring(object)] = object
        end
    end
    setmetatable(objects, objectListMt)
    setmetatable(lights, objectListMt)
    setmetatable(octrees, objectListMt)

    local ret = {
        objects = objects,
        lights = lights,
        octrees = octrees,
        activeCamera = attributes.activeCamera
    }
    
    ret.add = function(self, object)
        if type(object) ~= "table" then
            error("Scene.add expects object to add.")
        end
        rawget(self.objects, "__data")[tostring(object)] = object
        if object.light then
            rawget(self.lights, "__data")[tostring(object)] = object
        end
        if Octree:isOctree(object) then
            rawget(self.octrees, "__data")[tostring(object)] = object
        end
    end

    ret.remove = function(self, object)
        if type(object) ~= "table" then
            error("Scene.remove expects object to remove.")
        end
        local name = tostring(object)
        rawget(self.objects, "__data")[name] = nil
        rawget(self.lights, "__data")[name] = nil
        rawget(self.octrees, "__data")[name] = nil
    end

    ret.render = function(self, objectFilter)
        if self.activeCamera == nil then
            print("warning: no active camera.")
            return
        end
        -- Update the lights for each shader.
        for name, shader in pairs(shaders) do
            if shader.lightCount ~= nil then
                shader.lightCount = #self.lights
                lightIndex = 1
                for k, object in pairs(self.lights) do
                    local position = 
                        self.activeCamera.viewMatrix
                            * object.position:homo()
                    shader.lightPositionAndDistance[lightIndex] =
                        position:three()
                            + Vector.fourth * object.light.distance
                    lightIndex = lightIndex + 1
                end
            end
        end
        for k, object in pairs(self.objects) do
            if not objectFilter or objectFilter(object) then
                if object.render then
                    object:render(self)
                end
            end
        end
    end

    ret.preTick = function(self, deltaT, objectFilter)
        for k, object in pairs(self.objects) do
            if not objectFilter or objectFilter(object) then
                if object.preTick then
                    object:preTick(deltaT, self)
                end
            end
        end
    end

    ret.tick = function(self, deltaT, objectFilter)
        for k, object in pairs(self.objects) do
            if not objectFilter or objectFilter(object) then
                if object.tick then
                    object:tick(deltaT, self)
                end
            end
        end
    end

    ret.postTick = function(self, deltaT, objectFilter)
        for k, object in pairs(self.objects) do
            if not objectFilter or objectFilter(object) then
                if object.postTick then
                    object:postTick(deltaT, self)
                end
            end
        end
    end

    ret.raycast = function(self, hit, ray, objectFilter)
        result = false
        for k, object in pairs(self.objects) do
            if not objectFilter or objectFilter(object) then
                if object.raycast then
                    if object:raycast(hit, ray) then
                        result = true
                    end
                end
            end
        end
        return result
    end

    ret.toLua = function(self, out, materials)
        local r, e

        r, e = out:write("return Scene:new{objects={")
        if not r then return false, e end
        for k, object in pairs(self.objects) do
            if object.toLua then
                r, e = object:toLua(out, materials)
                if not r then return false, e end
                r, e = out:write(",")
                if not r then return false, e end
            end
        end
        r, e = out:write("}}")
        if not r then return false, e end
        return true
    end

    return ret
end

return Scene
