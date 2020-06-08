
CC = g++
CFLAGS = -g -Wall -c -Iglad/include -I/usr/include/lua5.3\
		 -I/usr/local/include/lua5.3 -Imatrix
OBJECTS = main.o node.o scene.o camera.o renderer.o phongshader.o\
		  renderernode.o spatialnode.o
SOURCES = main.cpp node.cpp scene.cpp camera.cpp renderer.cpp phongshader.cpp\
		  renderernode.cpp spatialnode.cpp
HEADERS = node.hpp camera.hpp scene.hpp shader.hpp material.hpp renderer.hpp\
		  matrix/matrix.hpp phongshader.hpp renderernode.hpp spatialnode.hpp\
		  tsf_parser.hpp obj_parser.hpp

UNAME = $(shell uname)

ifneq (,$(findstring Linux, $(UNAME)))
LIBS = -lglfw3 -lGL -ldl -lX11 -lm -lpthread
EXE = gnid
else ifneq (,$(findstring Darwin, $(UNAME)))
LIBS = -lglfw -framework OpenGL -lm -lpthread
EXE = gnid
else
LIBS = -lglfw3 -lopengl32 -lglu32 -lgdi32
EXE = gnid.exe
endif

$(EXE) : $(OBJECTS) glad.o lodepng.o
	$(CC) $(OBJECTS) glad.o lodepng.o\
		$(LIBS) -o $(EXE)

glad.o : glad/src/glad.c
	$(CC) -c -Iglad/include glad/src/glad.c -o glad.o

lodepng.o : lodepng.c lodepng.h
	$(CC) -c lodepng.c -o lodepng.o

%.o : %.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

clean :
	rm -f $(OBJECTS) $(EXE) glad.o

