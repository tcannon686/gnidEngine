
keys = require("keys")

config = {
    mouseSensitivity = 0.1,
    lookSpeed = math.pi * 1,
    keyLeft = keys.KEY_A,
    keyRight = keys.KEY_D,
    keyBackward = keys.KEY_S,
    keyForward = keys.KEY_W,
    keyDown = keys.KEY_LEFT_CONTROL,
    keyUp = keys.KEY_LEFT_SHIFT,
    keyJump = keys.KEY_SPACE,

    mouseButtonFire1 = keys.MOUSE_BUTTON_LEFT,

    mapDir = "maps",
    map = "tmp",

-- Editor keys
    keySubdivide = keys.KEY_R,
    keyUnsubdivide = keys.KEY_Y,
    keyScaleUp = keys.KEY_P,
    keyScaleDown = keys.KEY_O,
    keyExtrude = keys.KEY_E,
    keyNext = keys.KEY_RIGHT,
    keyMaterial = keys.KEY_M,
    keyPrevious = keys.KEY_LEFT,
    keyCreateObject = keys.KEY_Q,
    keyCommand = keys.KEY_GRAVE_ACCENT,
    keyLight = keys.KEY_L,
    mouseButtonSelect = keys.MOUSE_BUTTON_LEFT,
    mouseButtonRemove = keys.MOUSE_BUTTON_RIGHT
}

return config
