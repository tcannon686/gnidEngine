# gnidEngine
This is the most sofisticated game engine I've written yet, and is loosely based
on the Cube 2 engine. It simple collision detection and physics, octree based
maps, and skeletal animation. It is very much a work in progress, but hopefully
I will have a playable demo up and running soon.

## Name
The name gnid comes from a song I heard by jazz musician Tadd Dameron. As far as
I can tell, the word was made up. It has nothing to do with the engine, I just
needed a name, and gnid sounded cool.

## Exporting models
The engine features its own file format for models and skeletons called tmd,
which stands for Tom's Model Data. The format is super simple, and not very
efficient, but it gets the job done. A blender script is included to export
models. It is `operator_export_tmd.py`. It only supports exporting a single
model and armature. The format also exports all of the animation actions! Yay!

