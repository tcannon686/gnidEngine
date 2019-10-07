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
    if attributes.events and type(attributes.events) ~= "table" then
        error("ActionPlayer.new expects table events with events.")
    end
    
    local ret = {
        action = attributes.action,
        target = attributes.target,
        events = attributes.events,
        time = 0,
        isPlaying = false
    }

    if attributes.events then
        for k, event in pairs(attributes.events) do
            if event.time == nil or not event.callback then
                error("ActionPlayer.new expects table events with key event "
                    .."and key callback")
            end
        end
    end

    if attributes.loop then
        ret.loop = true
    else
        ret.loop = false
    end

    ret.tick = function(self, deltaT)
        if self.events then
            for i, event in ipairs(self.events) do
                if self.time < event.time
                        and self.time + deltaT > event.time then
                    event.callback()
                end
            end
        end
        self.time = self.time + deltaT
        self.isPlaying = true
        if self.action.loopEnd then
            if self.loop then
                local loopStart = self.action.loopStart
                if not loopStart then
                    loopStart = 0 end
                repeat
                    local delta = self.time - self.action.loopEnd
                    if delta >= 0 then
                        self.time = loopStart + delta
                    end
                until delta < 0
            elseif self.time > self.action.loopEnd then
                self.isPlaying = false
            end
        end
        self.action:apply(self.target, self.time)
    end

    ret.clone = function(self)
        return ActionPlayer:new {
            action = self.action,
            target = self.target,
            loop = self.loop
        }
    end

    return ret
end

return ActionPlayer
