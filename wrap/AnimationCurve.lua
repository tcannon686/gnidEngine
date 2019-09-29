local AnimationCurve = {}
AnimationCurve.__mt = {}

function AnimationCurve.new(self, attributes)
    local ret = {
        keys = {}
    }

    ret.insertKey = function(self, key)
        local index = 1

        if not key then
            error("AnimationCurve object.insertKey expects key.")
        end
        if not key.time then
            error("AnimationCurve object.insertKey expects key time.")
        end
        if not key.value then
            error("AnimationCurve object.insertKey expects key value.")
        end

        for i, lastKey in ipairs(self.keys) do
            if key.time > lastKey.time then
                index = i
            else
                break
            end
        end
        table.insert(self.keys, index, key)
    end

    if attributes.dataPath then
        if type(attributes.dataPath) ~= "table" then
            error("AnimationCurve.new dataPath must be list of keys.")
        else
            if not attributes.dataPath[1] then
                error("AnimationCurve.new dataPath must not be empty.")
            end
        end
    else
        error("AnimationCurve.new expects table with key dataPath.")
    end

    if attributes.keys then
        if type(attributes.keys) == "table" then
            for i, key in ipairs(attributes.keys) do
                ret:insertKey(key)
            end
        else
            error("AnimationCurve.new keys must be list of values.")
        end
    else
        error("AnimationCurve.new expects table with key keys.")
    end

    ret.dataPath = attributes.dataPath

    ret.evaluate = function(self, time)
        -- Find the two closest keys.
        local lastKey, nextKey
        index = 0
        for i, key in ipairs(self.keys) do
            nextKey = key
            if time < key.time then
                break
            else
                lastKey = key
                index = i
            end
        end
        if not lastKey then
            lastKey = nextKey end

        if nextKey.time ~= lastKey.time then
            return math3d.lerp(lastKey.value, nextKey.value,
                (time - lastKey.time) / (nextKey.time - lastKey.time))
        else
            return lastKey.value
        end
    end

    ret.apply = function(self, target, time)
        local item = target
        for i, key in ipairs(self.dataPath) do
            if self.dataPath[i + 1] then
                item = item[key]
            -- If we are at the last key, set that key.
            else
                item[key] = self:evaluate(time)
            end
        end
    end

    setmetatable(ret, self.__mt)
    return ret
end

return AnimationCurve
