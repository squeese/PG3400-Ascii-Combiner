CC = gcc
FLAGS = -Wall -g -Wextra
TARGET = view

$(TARGET): view.o parser.o
	$(CC) $(FLAGS) -o $(TARGET) view.o parser.o

view.o: view.c
	$(CC) $(FLAGS) -c -o view.o view.c

parser.o: parser.c
	$(CC) $(FLAGS) -c -o parser.o parser.c

clean:
	rm -f *.o *.txt $(TARGET)

remake: clean $(TARGET)
