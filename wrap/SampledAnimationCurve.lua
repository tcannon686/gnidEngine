local SampledAnimationCurve = {}

function SampledAnimationCurve.new(self, attributes)
    if not attributes then
        error("SampledAnimationCurve.new expects table attributes.")
    end
    if not attributes.samples then
        error("SampledAnimationCurve.new expects table with key samples.")
    end
    if not attributes.sampleRate then
        error("SampledAnimationCurve.new expects table with key sampleRate.")
    end
    if not attributes.dataPath then
        error("SampledAnimationCurve.new expects table with key dataPath.")
    end

    local ret = {
        samples = attributes.samples,
        sampleRate = attributes.sampleRate,
        sampleCount = #attributes.samples,
        dataPath = attributes.dataPath,
        lerp = attributes.lerp
    }

    ret.evaluate = function(self, time)
        local index = math.tointeger(math.floor(time * self.sampleRate)) + 1
        if index > self.sampleCount then
            index = self.sampleCount end
        if index < 1 then
            index = 1 end
        if self.lerp then
            local t = (time - (index - 1) / self.sampleRate) * self.sampleRate
            if t < 0 then
                t = 0
            elseif t > 1 then
                t = 1
            end
            return math3d.lerp(
                self.samples[index],
                self.samples[index % self.sampleCount + 1],
                t)
        else
            return self.samples[index]
        end
    end

    ret.apply = function(self, target, time)
        local item = target
        for i, key in ipairs(self.dataPath) do
            if self.dataPath[i + 1] then
                item = item[key]
                if not item then
                    error("SampledAnimationCurve object.apply target does not "
                        .."have key \"" .. key .. "\".")
                end
            -- If we are at the last key, set that key.
            else
                if not item[key] then
                    error("SampledAnimationCurve object.apply target does not "
                        .."have key \"" .. key .. "\".")
                end
                item[key] = self:evaluate(time)
            end
        end
    end

    return ret
end

return SampledAnimationCurve
