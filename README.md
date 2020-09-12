# gnidEngine
gnidEngine is a simple 3D game engine I have been developing in C++. Goals for
the project include clean code, fast runtime, and quality documentation. I
haven't used it for any projects yet, but hopefully something useful will come
from it. The target platform is Linux, but it should be portable and work with
most OSs.

## Compiling
To compile, install the dependencies, then run `make`. The project uses CMake,
but a wrapper makefile is included for convenience. CMake will build a static
library you can link with your game project. You will need to link with a few
libraries to use the engine in your own project.

You can also build the docs by running `make doc`. This will output index.html
in the docs/html.

The easiest way to build with your own projects is to install the library using
the CMake install system. This can be done by running `sudo make install`, and
will install libgnid.a to /usr/local/lib and a gnid folder to
/usr/local/include. The libgnid.a option can then be linked with by passing
`-lgnid` to gcc. You will also need to link with `-lglfw`, and `-ldl`.

### Dependencies
 - GLFW3
 - OpenGL

#### Ubuntu
To install the dependencies on Ubuntu, run the following:
```
sudo apt install libmesa-common-dev
sudo apt install libglfw3
```

#### Arch Linux
To install the dependencies on Arch, run the following:
```
sudo pacman -S glfw-x11
```

## Running
The program will not run standalone, you need a separate project to do this. An
example project will be provided shortly.

## Features
gnidEngine is a very simple game engine, but nevertheless, it still includes
some features:
 - Collision detection using the GJK and EPA methods
 - Collision pruning using k-D trees
 - Loading of Wavefront OBJ files
 - Very limited lighting using a simple phong shader

Here are a few features I am planning on implementing but haven't gotten around
to yet:
 - Skeletal meshes
 - Actual physics
 - Textures (this should be easy but takes a bit of time to implement)
 - More complex rendering system

