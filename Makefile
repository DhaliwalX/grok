CC = g++
CFLAGS = -g -c -O0 -std=c++11
INCLUDES =
SRC = src/
OUT = build/

all: build main.o parser.o
	$(CC) -g $(OUT)*.o -o g

parser.o:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)parser.cc -o $(OUT)parser.o

main.o: main/main.cc
	$(CC) $(CFLAGS) $(INCLUDES) main/main.cc -o $(OUT)main.o

build:
	mkdir build

.PHONY: clean
clean:
	rm $(OUT)*.o
