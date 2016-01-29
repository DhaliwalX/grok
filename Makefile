CC = g++
CFLAGS = -c -O3 -std=c++11
INCLUDES =
SRC = src/
OUT = build/

all: main.o vm.o
	$(CC) -g $(OUT)*.o -o grok

interpreter.o: parser.o
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)interpreter.cc -o $(OUT)interpreter.o

vm.o:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)vm.cc -o $(OUT)vm.o

parser.o: build
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)parser.cc -o $(OUT)parser.o

main.o: interpreter.o
	$(CC) $(CFLAGS) $(INCLUDES) main/main.cc -o $(OUT)main.o

build:
	mkdir build

.PHONY: clean
clean:
	rm $(OUT)*.o
