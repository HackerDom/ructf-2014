CXX=clang++
CXXFLAGS=-std=c++0x -O2

md: input.o main.o queue.o
	$(CXX) -o md input.o main.o queue.o

main.o: main.C input.h queue.h

input.o: input.C input.h

queue.o: queue.C queue.h input.h

.PHONY: clean
clean:
	rm -f *.o md
