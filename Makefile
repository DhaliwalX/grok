CC = g++
CFLAGS = -c -O3 -std=c++11 -fpermissive
INCLUDES =
SRC = src/
OUT = build/

all: main.o parser.o vm.o
	$(CC) -g $(OUT)*.o -o grok

vm.o:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)vm.cc -o $(OUT)vm.o

parser.o: build
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)parser.cc -o $(OUT)parser.o

main.o: parser.o
	$(CC) $(CFLAGS) $(INCLUDES) main/main.cc -o $(OUT)main.o

build:
	mkdir build

.PHONY: clean
clean:
	rm $(OUT)*.o
