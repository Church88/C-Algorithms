a.out: main.cc
	g++ -g -Wall -std=c++14 main.cc 

memcheck:
	@valgrind --log-fd=1 a.out < inputfile3 | grep "exit:" | cut -f2 -d":" | cut -f2,3 -d" "
	@cowsay "If it says "72,704 bytes" you are cool."

clean:
	rm -f a.out core *.o
