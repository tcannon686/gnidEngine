local Texture2D = {}

Texture2D.__mt = {}

function Texture2D.new(self, data, width, height, typeStr, formatStr)
    local ret = {
        width = width,
        height = height,
    }
    ret.__texture = gl.texture.new()
    ret.bind = function(self)
        gl.texture2d.bind(self.__texture)
    end

    gl.texture2d.bind(ret.__texture)
    gl.texture2d.data(data, width, height, typeStr, formatStr)

    setmetatable(ret, self.__mt)

    return ret
end

function Texture2D.fromPng(self, data)
    local image = png.flipY(png.decode(data))
    return self:new(image.data, image.width, image.height, "rgba", "B")
end

function Texture2D.loadPng(self, path)
    local f = io.open(path, "rb")
    if f == nil then
        error("could not open\"" .. path .. "\".") end
    local data, e = f:read("a")
    f:close()

    if not data then
        error("error reading data: " .. e) end
    return self:fromPng(data)
end

function Texture2D.toTexture2D(self, v)
    if getmetatable(v) == self.__mt then
        return v
    end
    return nil
end

return Texture2D
