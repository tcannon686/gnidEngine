local Material = {}

local materialId = 1
Material.__active = nil

function Material.new(self, shader, values)
    local ret = {}
    ret.values = {}
    ret.shader = shader
    ret.values = values
    ret.id = materialId

    materialId = materialId + 1

    ret.bind = function(self)        
        if Material.currentBoundMaterial ~= self then
            self.shader:bind()
            for key, value in pairs(self.values) do
                self.shader[key] = value
            end
            Material.__active = self
        end
    end
    return ret
end

return Material
