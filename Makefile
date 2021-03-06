CC = gcc
FLAGS = -Wall -Wextra -Werror -O0 -g
TARGET = combine

$(TARGET): directories build/main.o build/parser.o
	$(CC) $(FLAGS) -o $(TARGET) build/main.o build/parser.o -lm

directories:
	mkdir -p build

build/main.o: src/main.c
	$(CC) $(FLAGS) -c -o build/main.o src/main.c

build/parser.o: src/parser.c
	$(CC) $(FLAGS) -c -o build/parser.o src/parser.c

clean:
	rm -rf build grind.log *.txt $(TARGET)

remake: clean $(TARGET)

build: clean directories
	$(CC) -O3 -lm -o $(TARGET) src/main.c src/parser.c -lm
