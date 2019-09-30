local ActionPlayer = {}

function ActionPlayer.new(self, attributes)
    if type(attributes) ~= "table" then
        error("ActionPlayer.new expects table attributes.")
    end
    if type(attributes.action) ~= "table" then
        error("ActionPlayer.new expects table with key action.")
    end
    if type(attributes.target) ~= "table" then
        error("ActionPlayer.new expects table with key target.")
    end
    
    local ret = {
        action = attributes.action,
        target = attributes.target,
        time = 0
    }

    if attributes.loop then
        ret.loop = true
    else
        ret.loop = false
    end

    ret.tick = function(self, deltaT)
        self.time = self.time + deltaT
        if self.action.loopEnd and self.loop then
            local loopStart = self.action.loopStart
            if not loopStart then
                loopStart = 0 end
            repeat
                local delta = self.time - self.action.loopEnd
                if delta >= 0 then
                    self.time = loopStart + delta
                end
            until delta < 0
        end
        self.action:apply(self.target, self.time)
    end

    return ret
end

return ActionPlayer
