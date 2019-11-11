# gnidEngine
This is the most sofisticated game engine I've written yet, and is loosely based
on the Cube 2 engine. It simple collision detection and physics, octree based
maps, and skeletal animation. It is very much a work in progress, but hopefully
I will have a playable demo up and running soon.

## Name
The name gnid comes from a song I heard by jazz musician Tadd Dameron. As far as
I can tell, the word was made up. It has nothing to do with the engine, I just
needed a name, and gnid sounded cool.

## Compiling
The program can be compiled using GCC or MinGW on Windows. I have not tested it
on other platforms, although it should be possible with a little changing of the
Makefile. Install the dependencies listed below, and then run `make linux` or
`make windows` depending on your platform. I have only tested it on Ubuntu and
Windows 10.

### Dependencies
 - Lua 5.3
 - GLFW3
 - OpenGL

### Ubuntu
To install the dependencies on ubuntu, run the following:
```
sudo apt install libmesa-common-dev
sudo apt install liblua5.3
sudo apt install libglfw3
```

## Running
To run, simply make, and then run the outputted `game` or `game.exe`. You can
specify a map by changing the `config.lua` file, and adding a `map` key, which
should be a string of the name of the path of the map, without `.lua` at the
end. When the program starts, press escape to regain control of your cursor.

## Default controls
### Basics
| Key                   | Action                             |
| --------------------- | ---------------------------------- |
| W                     | Move forward.                      |
| A                     | Move left.                         |
| S                     | Move back.                         |
| D                     | Move right.                        |
| Left shift            | Fly up.                            |
| Left control          | Fly down.                          |
| Space                 | Jump.                              |

### Editing
| Key                   | Action                             |
| --------------------- | ---------------------------------- |
| Alt W                 | Move corner forward.               |
| Alt A                 | Move corner left.                  |
| Alt S                 | Move corner back.                  |
| Alt D                 | Move corner right.                 |
| Alt Left Shift        | Move corner up.                    |
| Alt Left Control      | Move corner down.                  |
| Alt W                 | Move object forward.               |
| Alt A                 | Move object left.                  |
| Alt S                 | Move object back.                  |
| Alt D                 | Move object right.                 |
| Alt Left              | Rotate object left.                |
| Alt Right             | Rotate object right.               |
| Alt E                 | Save map.                          |
| E                     | Extrude octree.                    |
| G                     | Switch mode (edit/game).           |
| L Left                | Decrease light distance.           |
| L Right               | Increase light distance.           |
| Left Mouse            | Select.                            |
| M                     | Use selected material.             |
| M Left                | Select previous material.          |
| M Right               | Select next material.              |
| Q                     | Place object (hold for preview).   |
| Q Left                | Select previous object.            |
| Q Right               | Select next object.                |
| R                     | Subdivide octree.                  |
| Right Mouse           | Remove.                            |
| Z Left                | Halve map size.                    |
| Z Right               | Double map size.                   |
| \`                    | Open console.                      |

## Editing
See above for editing controls. Currently, there is no undo button. The name of
the map will be the name of the map specified in the config file. To save the
map with that name, press Alt E. If no map name is specified, a temporary one
will be saved.

## Exporting models
The engine features its own file format for models and skeletons called tmd,
which stands for Tom's Model Data. The format is super simple, and not very
efficient, but it gets the job done. A Blender script is included to export
models. It is `operator_export_tmd.py`. It only supports exporting a single
model and armature. The format also exports all of the animation actions! Yay!

