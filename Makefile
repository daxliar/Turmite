
SOURCES= *.cpp ./pugixml/src/pugixml.cpp ./lodepng-master/lodepng.cpp
INCLUDE= -I./pugixml/src -I./lodepng-master

all:
	g++ $(SOURCES) -o Turmite $(INCLUDE) -lGL -lstdc++ -lc -lm -lglut -lGLU -lGLEW

clean:
	rm -f Turmite *.o
