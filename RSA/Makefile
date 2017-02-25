a.out: main.cc
	g++ -O3 -std=c++11 -Wall -Wextra -fsanitize=address main.cc -lgmp -lgmpxx

sample: sample_code.cc
	g++ -O3 -std=c++11 -Wall -Wextra -fsanitize=address sample_code.cc -lgmp -lgmpxx -o sample

clean:
	rm a.out
