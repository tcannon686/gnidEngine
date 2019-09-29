local Action = {}

function Action.new(self, attributes)
    local ret = {
        curves = {},
    }
    if not attributes then
        error("Action.new expects table attributes.")
    end
    if not attributes.curves then
        error("Action.new expects table curves.")
    end
    for i, curve in ipairs(attributes.curves) do
        if type(curve.apply) ~= "function" then
            error("Action.new curves must be table of AnimationCurve.")
        end
        ret.curves[i] = curve
    end

    if attributes.loopEnd then
        ret.loopEnd = attributes.loopEnd end
    if attributes.loopStart then
        ret.loopStart = attributes.loopStart end

    ret.apply = function(self, target, time)
        for i, curve in ipairs(self.curves) do
            curve:apply(target, time)
        end
    end

    return ret
end

return Action
