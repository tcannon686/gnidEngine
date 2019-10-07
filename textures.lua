local Texture2D = require("wrap/Texture2D")

local textures = {
    test1 = Texture2D:loadPng("textures/Test1.png"),
    brick1 = Texture2D:loadPng("textures/Brick1.png"),
    concrete1 = Texture2D:loadPng("textures/Concrete1.png"),
    road1 = Texture2D:loadPng("textures/Road1.png"),
    sand1 = Texture2D:loadPng("textures/Sand1.png"),
    snow1 = Texture2D:loadPng("textures/Snow1.png"),
    wood1 = Texture2D:loadPng("textures/Wood1.png"),
    wood2 = Texture2D:loadPng("textures/Wood2.png"),
    wood3 = Texture2D:loadPng("textures/Wood3.png"),
    sphereMonster1 = Texture2D:loadPng("textures/SphereMonster1.png"),
}
return textures
