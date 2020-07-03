
INCLUDEDIR = include
SRCDIR = src
DEPSDIR = deps
BINDIR = bin

CC = g++
CFLAGS = -g -Wall -c -I$(DEPSDIR)/glad/include -I/usr/include/lua5.3\
		 -I/usr/local/include/lua5.3 -Imatrix -I$(INCLUDEDIR) -I$(DEPSDIR)

SOURCES = $(shell find $(SRCDIR) -name "*.cpp")
HEADERS = $(shell find $(INCLUDEDIR) -name "*.hpp")
OBJECTS = $(shell find $(SRCDIR) -name "*.cpp" | sed -e 's/.cpp$$/.o/')

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

$(EXE) : $(OBJECTS) $(BINDIR)/glad.o $(BINDIR)/lodepng.o
	$(CC) $(OBJECTS) $(BINDIR)/glad.o $(BINDIR)/lodepng.o\
		$(LIBS) -o $(EXE)

$(BINDIR)/glad.o : $(DEPSDIR)/glad/src/glad.c
	$(CC) -c -I$(DEPSDIR)/glad/include $(DEPSDIR)/glad/src/glad.c -o $(BINDIR)/glad.o

$(BINDIR)/lodepng.o : $(DEPSDIR)/lodepng.c $(DEPSDIR)/lodepng.h
	$(CC) -c $(DEPSDIR)/lodepng.c -o $(BINDIR)/lodepng.o

%.o : %.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

clean :
	rm -f $(OBJECTS) $(EXE) $(BINDIR)/glad.o $(BINDIR)/lodepng.o

