CC = g++
CFLAGS = -g -c -O0 -std=c++11 -Wall
INCLUDES =
SRC = src/
OUT = g

all: main.o parser.o jsobject.o astnode.o lexer.o
	$(CC) -g $^ -o $(OUT)

parser.o: $(SRC)parser.cc $(SRC)parser.h
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)parser.cc -o $@

main.o: main/main.cc
	$(CC) $(CFLAGS) $(INCLUDES) main/main.cc -o $@

jsobject.o: $(SRC)jsobject.cc
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

astnode.o: $(SRC)astnode.cc $(SRC)astnode.h
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)astnode.cc -o $@

lexer.o: $(SRC)lexer.cc $(SRC)lexer.h $(SRC)token.h
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC)lexer.cc -o $@

build:
	mkdir build

.PHONY: clean
clean:
	rm *.o $(OUT)
