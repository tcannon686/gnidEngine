
INCLUDEDIR = include
SRCDIR = src
DEPSDIR = deps
BINDIR = bin
TESTDIR = tests

CC = g++
CFLAGS = -fPIC -g -Wall -c -I$(DEPSDIR)/glad/include -I/usr/include/lua5.3\
		 -I/usr/local/include/lua5.3 -I$(INCLUDEDIR) -I$(DEPSDIR)

SOURCES = $(shell find $(SRCDIR) -name "*.cpp")
HEADERS = $(shell find $(INCLUDEDIR) -name "*.hpp")
OBJECTS = $(shell find $(SRCDIR) -name "*.cpp" | sed -e 's|.cpp$$|.o|' | sed -e 's|^$(SRCDIR)|$(BINDIR)|')
TEST_OBJECTS = $(shell find $(TESTDIR) -name "*.cpp" | sed -e 's|.cpp$$|.o|')
EXE = libgnid.so

UNAME = $(shell uname)

ifneq (,$(findstring Linux, $(UNAME)))
LIBS = -lglfw -lGL -ldl -lX11 -lm -lpthread
else ifneq (,$(findstring Darwin, $(UNAME)))
LIBS = -lglfw -framework OpenGL -lm -lpthread
else
LIBS = -lglfw -lopengl32 -lglu32 -lgdi32 -lpthread
endif

$(EXE) : $(OBJECTS) $(BINDIR)/glad.o $(BINDIR)/lodepng.o
	$(CC) -shared $(OBJECTS) $(BINDIR)/glad.o $(BINDIR)/lodepng.o\
		$(LIBS) -o $(EXE)

$(BINDIR)/glad.o : $(DEPSDIR)/glad/src/glad.c
	$(CC) -c -fPIC -I$(DEPSDIR)/glad/include $(DEPSDIR)/glad/src/glad.c -o $(BINDIR)/glad.o

$(BINDIR)/lodepng.o : $(DEPSDIR)/lodepng.c $(DEPSDIR)/lodepng.h
	$(CC) -c -fPIC $(DEPSDIR)/lodepng.c -o $(BINDIR)/lodepng.o

$(BINDIR)/%.o : $(SRCDIR)/%.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

$(TESTDIR)/%.o : $(TESTDIR)/%.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

clean :
	rm -f $(OBJECTS) $(EXE) $(BINDIR)/glad.o $(BINDIR)/lodepng.o

test: $(EXE) $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) -L./ -o $(TESTDIR)/test -lgnid
	export LD_LIBRARY_PATH=./; $(TESTDIR)/test

