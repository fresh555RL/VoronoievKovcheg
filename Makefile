CC = g++
CFLAGS = -Iinclude
LDFLAGS = -Llib -Wl,-rpath=./lib
LIBS = -lglfw

TARGET = build/main
OBJ_DIR = build
OBJ = $(OBJ_DIR)/main.o $(OBJ_DIR)/glad.o
SRC = main.cpp glad.c

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

all: $(OBJ_DIR) $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET) $(LIBS)

$(OBJ_DIR)/main.o: main.cpp | $(OBJ_DIR)
	$(CC) -c main.cpp $(CFLAGS) -o $@

$(OBJ_DIR)/glad.o: glad.c | $(OBJ_DIR)
	$(CC) -c glad.c $(CFLAGS) -o $@

clean:
	rm -rf $(OBJ_DIR)