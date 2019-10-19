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
| G                     | Switch mode (edit/game).           |
| Left mouse            | Select octree.                     |
| Right mouse           | Remove octree.                     |
| R                     | Subdivide octree.                  |
| Alt W                 | Move corner forward.               |
| Alt A                 | Move corner left.                  |
| Alt S                 | Move corner back.                  |
| Alt D                 | Move corner right.                 |
| Alt left shift        | Move corner up.                    |
| Alt left control      | Move corner down.                  |
| E                     | Extrude octree.                    |
| Alt E                 | Save map.                          |
| P                     | Double map size.                   |
| O                     | Halve map size.                    |
| M                     | Use selected material.             |
| M left                | Select next material.              |
| M right               | Select previous material.          |
| Q                     | Place object (hold for preview).   |
| Q left                | Select next object.                |
| Q right               | Select previous object.            |
| \`                    | Open console.                      |

## Editing
See above for editing controls. Currently, there is no undo button. The name of
the map will be the name of the map specified in the config file. To save the
map with that name, press Alt E. If no map name is specified, a temporary one
will be saved.

## Exporting models
The engine features its own file format for models and skeletons called tmd,
which stands for Tom's Model Data. The format is super simple, and not very
efficient, but it gets the job done. A blender script is included to export
models. It is `operator_export_tmd.py`. It only supports exporting a single
model and armature. The format also exports all of the animation actions! Yay!

