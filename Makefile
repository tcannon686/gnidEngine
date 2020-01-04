
CC = gcc
CFLAGS = -g -Wall -c -Iglad/include -I/usr/include/lua5.3\
		 -I/usr/local/include/lua5.3
OBJECTS = matrix.o main.o math3d.o hashtable.o node.o root_node.o linkedlist.o camera_node.o
SOURCES = matrix/matrix.c main.c matrix/math3d.c hashtable.c node.c root_node.c linkedlist.c camera_node.c
HEADERS = matrix/matrix.h matrix/math3d.h hashtable.h node.h root_node.h linkedlist.h camera_node.h

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

