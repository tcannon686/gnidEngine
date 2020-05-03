
CC = g++
CFLAGS = -g -Wall -c -Iglad/include -I/usr/include/lua5.3\
		 -I/usr/local/include/lua5.3 -Imatrix
OBJECTS = main.o node.o scene.o camera.o renderer.o phongshader.o\
		  renderernode.o
SOURCES = main.cpp node.cpp scene.cpp camera.cpp renderer.cpp phongshader.cpp\
		  renderernode.cpp
HEADERS = node.hpp camera.hpp scene.hpp shader.hpp material.hpp renderer.hpp\
		  matrix/matrix.hpp phongshader.hpp renderernode.hpp

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

$(OBJECTS) : $(SOURCES) $(HEADERS)
	gcc $(CFLAGS) $(SOURCES)

clean :
	rm -f $(OBJECTS) $(EXE) glad.o

