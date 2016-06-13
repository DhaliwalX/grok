CC = g++
CFLAGS = -g -c -O0 -std=c++11
INCLUDES =
SRC = src/
OUT = build/

all: build main.o parser.o vm.o codegen.o
	$(CC) -g $(OUT)*.o -o grok

vm.o: $(SRC)vm.cc $(SRC)bytecode.h $(SRC)astnode.h
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)vm.cc -o $(OUT)vm.o

parser.o:
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
