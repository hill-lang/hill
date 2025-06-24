TARGET := "hill"

CC := g++
CXXFLAGS=-std=c++20 -Wall -pedantic

all:
	${CC} ${CXXFLAGS} -o ${TARGET} src/main.cc

clean:
	rm ${TARGET}
