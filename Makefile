CC = g++
CFLAGS = -Iinclude 
LDFLAGS = -Llib -Wl,-rpath=./lib
LIBS = -lglfw

TARGET = main
OBJ = main.o glad.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET) $(LIBS)

main.o: main.cpp
	$(CC) -c main.cpp $(CFLAGS)

glad.o: glad.c
	$(CC) -c glad.c $(CFLAGS)

clean:
	rm -f $(TARGET) $(OBJ)