CXX = g++
TARGET = hierarchy
OBJECTS = $(TARGET).o textfile.o
SOURCE = $(TARGET).cpp textfile.cpp
CFLAGS =
LIBS =-lGL -lglut -lm -lGLEW
INCLUDE =-I/glm/gtc
all:$(OBJECTS)
	$(CXX) $(INCLUDE) $(OBJECTS) $(LIBS) -o $(TARGET)

$(OBJECTS): $(SOURCE)

clean:
	-rm -f $(OBJECTS)
	-rm -f *~ core

