CC = g++
CFLAGS = -c -g -std=c++11 -fpermissive
INCLUDES =
SRC = src/
OUT = build/

all: main.o parser.o vm.o
	$(CC) -g $(OUT)*.o -o grok

vm.o: codegen.o
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)vm.cc -o $(OUT)vm.o

parser.o: build
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)parser.cc -o $(OUT)parser.o

main.o: main/main.cc
	$(CC) $(CFLAGS) $(INCLUDES) main/main.cc -o $(OUT)main.o

codegen.o: $(SRC)codegen.cc $(SRC)codegen.h $(SRC)program.h
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)/codegen.cc -o $(OUT)codegen.o

build:
	mkdir build

.PHONY: clean
clean:
	rm $(OUT)*.o
