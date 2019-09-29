local Material = {}

local materialId = 1

function Material.new(self, shader, values)
    local ret = {}
    ret.values = {}
    ret.shader = shader
    ret.values = values
    ret.id = materialId

    materialId = materialId + 1

    ret.bind = function(self)        
        self.shader:bind()
        for key, value in pairs(self.values) do
            self.shader[key] = value
        end
    end
    return ret
end

return Material
