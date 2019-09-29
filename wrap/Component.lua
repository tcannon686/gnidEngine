
local Component = {}

local id = 0

function Component.new(self)
    local ret = {}
    ret.id = id
    id = id + 1
    return ret
end

return Component
