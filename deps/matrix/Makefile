
EXE=test
OBJECTS=test.o
SOURCES=test.cpp
HEADERS=matrix.hpp
CC=g++
CFLAGS=-Wall -g

$(EXE) : $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXE)

$(OBJECTS) : $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -c $(SOURCES)

clean:
	rm -f $(EXE) $(OBJECTS)
