CC = gcc
FLAGS = -Wall -g -Wextra
TARGET = main

$(TARGET): directories build/main.o build/parser.o
	$(CC) $(FLAGS) -o $(TARGET) build/main.o build/parser.o

directories:
	mkdir -p build

build/main.o: src/main.c
	$(CC) $(FLAGS) -c -o build/main.o src/main.c

build/parser.o: src/parser.c
	$(CC) $(FLAGS) -c -o build/parser.o src/parser.c

clean:
	rm -f build/*.o *.txt $(TARGET)

remake: clean $(TARGET)
