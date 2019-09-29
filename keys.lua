-- Taken from GLFW.h

keys = {}

--[[ The unknown key ]]--
keys.KEY_UNKNOWN          = -1

--[[ Printable keys ]]--
keys.KEY_SPACE            = 32
keys.KEY_APOSTROPHE       = 39  --[[ ' ]]--
keys.KEY_COMMA            = 44  --[[ , ]]--
keys.KEY_MINUS            = 45  --[[ - ]]--
keys.KEY_PERIOD           = 46  --[[ . ]]--
keys.KEY_SLASH            = 47  --[[ / ]]--
keys.KEY_0                = 48
keys.KEY_1                = 49
keys.KEY_2                = 50
keys.KEY_3                = 51
keys.KEY_4                = 52
keys.KEY_5                = 53
keys.KEY_6                = 54
keys.KEY_7                = 55
keys.KEY_8                = 56
keys.KEY_9                = 57
keys.KEY_SEMICOLON        = 59  --[[ ; ]]--
keys.KEY_EQUAL            = 61  --[[ = ]]--
keys.KEY_A                = 65
keys.KEY_B                = 66
keys.KEY_C                = 67
keys.KEY_D                = 68
keys.KEY_E                = 69
keys.KEY_F                = 70
keys.KEY_G                = 71
keys.KEY_H                = 72
keys.KEY_I                = 73
keys.KEY_J                = 74
keys.KEY_K                = 75
keys.KEY_L                = 76
keys.KEY_M                = 77
keys.KEY_N                = 78
keys.KEY_O                = 79
keys.KEY_P                = 80
keys.KEY_Q                = 81
keys.KEY_R                = 82
keys.KEY_S                = 83
keys.KEY_T                = 84
keys.KEY_U                = 85
keys.KEY_V                = 86
keys.KEY_W                = 87
keys.KEY_X                = 88
keys.KEY_Y                = 89
keys.KEY_Z                = 90
keys.KEY_LEFT_BRACKET     = 91  --[[ [ ]]--
keys.KEY_BACKSLASH        = 92  --[[ \ ]]--
keys.KEY_RIGHT_BRACKET    = 93  --[[ ] ]]--
keys.KEY_GRAVE_ACCENT     = 96  --[[ ` ]]--
keys.KEY_WORLD_1          = 161 --[[ non-US #1 ]]--
keys.KEY_WORLD_2          = 162 --[[ non-US #2 ]]--

--[[ Function keys ]]--
keys.KEY_ESCAPE           = 256
keys.KEY_ENTER            = 257
keys.KEY_TAB              = 258
keys.KEY_BACKSPACE        = 259
keys.KEY_INSERT           = 260
keys.KEY_DELETE           = 261
keys.KEY_RIGHT            = 262
keys.KEY_LEFT             = 263
keys.KEY_DOWN             = 264
keys.KEY_UP               = 265
keys.KEY_PAGE_UP          = 266
keys.KEY_PAGE_DOWN        = 267
keys.KEY_HOME             = 268
keys.KEY_END              = 269
keys.KEY_CAPS_LOCK        = 280
keys.KEY_SCROLL_LOCK      = 281
keys.KEY_NUM_LOCK         = 282
keys.KEY_PRINT_SCREEN     = 283
keys.KEY_PAUSE            = 284
keys.KEY_F1               = 290
keys.KEY_F2               = 291
keys.KEY_F3               = 292
keys.KEY_F4               = 293
keys.KEY_F5               = 294
keys.KEY_F6               = 295
keys.KEY_F7               = 296
keys.KEY_F8               = 297
keys.KEY_F9               = 298
keys.KEY_F10              = 299
keys.KEY_F11              = 300
keys.KEY_F12              = 301
keys.KEY_F13              = 302
keys.KEY_F14              = 303
keys.KEY_F15              = 304
keys.KEY_F16              = 305
keys.KEY_F17              = 306
keys.KEY_F18              = 307
keys.KEY_F19              = 308
keys.KEY_F20              = 309
keys.KEY_F21              = 310
keys.KEY_F22              = 311
keys.KEY_F23              = 312
keys.KEY_F24              = 313
keys.KEY_F25              = 314
keys.KEY_KP_0             = 320
keys.KEY_KP_1             = 321
keys.KEY_KP_2             = 322
keys.KEY_KP_3             = 323
keys.KEY_KP_4             = 324
keys.KEY_KP_5             = 325
keys.KEY_KP_6             = 326
keys.KEY_KP_7             = 327
keys.KEY_KP_8             = 328
keys.KEY_KP_9             = 329
keys.KEY_KP_DECIMAL       = 330
keys.KEY_KP_DIVIDE        = 331
keys.KEY_KP_MULTIPLY      = 332
keys.KEY_KP_SUBTRACT      = 333
keys.KEY_KP_ADD           = 334
keys.KEY_KP_ENTER         = 335
keys.KEY_KP_EQUAL         = 336
keys.KEY_LEFT_SHIFT       = 340
keys.KEY_LEFT_CONTROL     = 341
keys.KEY_LEFT_ALT         = 342
keys.KEY_LEFT_SUPER       = 343
keys.KEY_RIGHT_SHIFT      = 344
keys.KEY_RIGHT_CONTROL    = 345
keys.KEY_RIGHT_ALT        = 346
keys.KEY_RIGHT_SUPER      = 347
keys.KEY_MENU             = 348

keys.KEY_LAST             = GLFW_KEY_MENU

--[[! @} ]]--

--[[! @defgroup mods Modifier key flags
 *
 *  See [key input](@ref input_key) for how these are used.
 *
 *  @ingroup input
 *  @{ ]]--

--[[! @brief If this bit is set one or more Shift keys were held down.
 ]]--
keys.MOD_SHIFT         = 0x0001
--[[! @brief If this bit is set one or more Control keys were held down.
 ]]--
keys.MOD_CONTROL       = 0x0002
--[[! @brief If this bit is set one or more Alt keys were held down.
 ]]--
keys.MOD_ALT           = 0x0004
--[[! @brief If this bit is set one or more Super keys were held down.
 ]]--
keys.MOD_SUPER         = 0x0008

--[[! @} ]]--

--[[! @defgroup buttons Mouse buttons
 *
 *  See [mouse button input](@ref input_mouse_button) for how these are used.
 *
 *  @ingroup input
 *  @{ ]]--
keys.MOUSE_BUTTON_1       = 0
keys.MOUSE_BUTTON_2       = 1
keys.MOUSE_BUTTON_3       = 2
keys.MOUSE_BUTTON_4       = 3
keys.MOUSE_BUTTON_5       = 4
keys.MOUSE_BUTTON_6       = 5
keys.MOUSE_BUTTON_7       = 6
keys.MOUSE_BUTTON_8       = 7
keys.MOUSE_BUTTON_LAST    = keys.MOUSE_BUTTON_8
keys.MOUSE_BUTTON_LEFT    = keys.MOUSE_BUTTON_1
keys.MOUSE_BUTTON_RIGHT   = keys.MOUSE_BUTTON_2
keys.MOUSE_BUTTON_MIDDLE  = keys.MOUSE_BUTTON_3
--[[! @} ]]--

--[[! @defgroup joysticks Joysticks
 *
 *  See [joystick input](@ref joystick) for how these are used.
 *
 *  @ingroup input
 *  @{ ]]--
keys.JOYSTICK_1           = 0
keys.JOYSTICK_2           = 1
keys.JOYSTICK_3           = 2
keys.JOYSTICK_4           = 3
keys.JOYSTICK_5           = 4
keys.JOYSTICK_6           = 5
keys.JOYSTICK_7           = 6
keys.JOYSTICK_8           = 7
keys.JOYSTICK_9           = 8
keys.JOYSTICK_10          = 9
keys.JOYSTICK_11          = 10
keys.JOYSTICK_12          = 11
keys.JOYSTICK_13          = 12
keys.JOYSTICK_14          = 13
keys.JOYSTICK_15          = 14
keys.JOYSTICK_16          = 15
keys.JOYSTICK_LAST        = GLFW_JOYSTICK_16

keys.RELEASE              = 0
keys.PRESS                = 1
keys.REPEAT               = 2

return keys
