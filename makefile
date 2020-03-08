CXX 			= clang++
LINK 			= clang++
TARGETDIR 		= bin/
INCPATH			= -I/usr/include/SDL2
LIBS			= -L/usr/include -lSDL2 -lOpenGL -lGLEW
TARGET			= $(TARGETDIR)opengl

OBJECTS_DIR   	= obj/
####### Files
SOURCES       	= main.cpp
HEADERS   		= main.h
OBJECTS       	= $(OBJECTS_DIR)main.o


$(OBJECTS): $(SOURCES) $(HEADERS)
	$(CXX) -c $(SOURCES) $(INCPATH) -o $(OBJECTS) 

$(TARGET): $(OBJECTS)
	$(LINK) -o $(TARGET) $(OBJECTS) $(LIBS)

run: $(TARGET)
	$(TARGET)
