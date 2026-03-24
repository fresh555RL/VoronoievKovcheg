CC = g++
CFLAGS = -Iinclude 
LDFLAGS = -Llib -Wl,-rpath=./lib
LIBS = -lglfw

TARGET = main
OBJ = main.o glad.o
GLFW_LIB = lib/libglfw.so

all: $(TARGET)

$(TARGET): $(OBJ) $(GLFW_LIB)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET) $(LIBS)

$(GLFW_LIB):
	cmake -S ./glfw -B ./glfw/build \
		-D BUILD_SHARED_LIBS=ON \
		-D GLFW_LIBRARY_TYPE=SHARED \
		-D GLFW_BUILD_EXAMPLES=OFF \
		-D GLFW_BUILD_TESTS=OFF \
		-D GLFW_BUILD_DOCS=OFF
	cmake --build ./glfw/build/
	mkdir -p lib
	cp glfw/build/src/libglfw.so lib/

main.o: main.cpp
	$(CC) -c main.cpp $(CFLAGS)

glad.o: glad.c
	$(CC) -c glad.c $(CFLAGS)

clean:
	rm -f $(TARGET) $(OBJ)