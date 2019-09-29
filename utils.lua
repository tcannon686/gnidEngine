

function vectorHash(v)
    mix = {
        string.unpack("i", string.pack("f", v.x))
        string.unpack("i", string.pack("f", v.y))
        string.unpack("i", string.pack("f", v.z))
        string.unpack("i", string.pack("f", v.w))
    }

    for i = 1, 8 do
        mix[i % 4]
    end
end
