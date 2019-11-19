
CC = gcc
CFLAGS = -g -Wall -c -Iglad/include -I/usr/include/lua5.3\
		 -I/usr/local/include/lua5.3
OBJECTS = matrix.o luamatrix.o luagl.o main.o math3d.o luamath3d.o luapng.o
SOURCES = matrix.c luamatrix.c luagl.c main.c math3d.c luamath3d.c luapng.c
HEADERS = matrix.h luamatrix.h luagl.h math3d.h luamath3d.h luapng.h

UNAME = $(shell uname)

ifneq (,$(findstring Linux, $(UNAME)))
LIBS = -llua5.3 -lglfw3 -lGL -ldl -lX11 -lm -lpthread
EXE = gnid
else ifneq (,$(findstring Darwin, $(UNAME)))
LIBS = -llua5.3 -lglfw -framework OpenGL -lm -lpthread
EXE = gnid
else
LIBS = -llua -lglfw3 -lopengl32 -lglu32 -lgdi32
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

