#pragma once
#include <iostream>
#include <assert.h>
#include <vector>
#include <cmath>
using namespace std;

class Hash {
	int mode; //Holds how we are implementing the queue, either as linked list, array, or vector
	int max_size; //Holds the size of our array
	int contains = 0; //Holds how many elements we contain
	int R;

	int *hash_table;

  public:
	static const int LINEAR_MODE = 0;
	static const int QUADRATIC_MODE = 1;
	static const int DOUBLE_MODE = 2;
	Hash (int new_mode = LINEAR_MODE, int new_size = 1000) : mode (new_mode), max_size (new_size) {
		if (!prime_test (new_size)) {
			cout << "Bad user, no cookie. Size must be a prime.\n";
			exit (1);
		}
		if (mode == DOUBLE_MODE) {
			R = 7;
		}
		hash_table = new int[max_size];
		for (int i = 0; i < max_size; i++) hash_table[i] = 0;
	}

	//Returns percentage the array is full
	float get_load() {
		return contains / float (max_size);
	}

	//YOU: Print out the hash table for debugging purposes
	void print() {
		for (int i = 0; i < max_size; i++) {
			cout << i << endl;
		}
	}

	//YOU: Insert a new int into the hash table using the rules found in mode
	void insert (int newdata) {
		//Initial hash location is the same for all three
		int h1 = newdata % max_size;
		if (hash_table[h1] == 0) {
			hash_table[h1] = newdata;
			counter++;
			return;
		}

		if (mode == LINEAR_MODE) {
			int step = 1;
			while (true) {
				h1 = (h1 + step) % max_size;

				//Test to see if we've wrapped all the way around
				if (h1 == newdata % max_size) {
					cout << "Bad user, NO COOKIE, array is full.\n";
					exit (1);
				}

				if (hash_table[h1] == 0) {
					hash_table[h1] = newdata;
					counter++;
					return;
				}
			}
		}
		if (mode == QUADRATIC_MODE) {
			int step = 1;
			while (true) {
				h1 = (h1 + pow (step, 2)) % max_size;
				if (h1 == newdata % max_size) {
					cout << "Bad user, NO COOKIE, array is full.\n";
					exit (1);
				}
				if (hash_table[h1] == 0) {
					hash_table[h1] = newdata;
					counter++;
					return;
				}
				step ++;
			}
		}
		if (mode == DOUBLE_MODE) {
			cerr << "Entering Double Mode\n";
			int step = 1 + (newdata % R);
			while (true) {
				h1 += step;
				if (h1 == newdata % max_size) {
					cout << "Bad user, NO COOKIE, array is full.\n";
					exit (1);
				}
				if (hash_table[h1] == 0) {
					hash_table[h1] = newdata;
					counter++;
					return;
				}
			}
		}
		//Update the number of elements in the hash_table
		//contains++;
	}

	//YOU: Returns true if the key is in the hash table, false otherwise
	bool search (int key) {
		if (hash_table[h1] == key) return true;
		if (mode == LINEAR_MODE) {
			int step = 1;
			while (true) {
				h1 = (h1 + step) % max_size;
				if (h1 == newdata % max_size) {
					return false;
				}
				if (hash_table[h1] == key) {
					return true;
				}
			}
		}
		if (mode == QUADRATIC_MODE) {
			int step = 1;
			while (true) {
				h1 = (h1 + pow (step, 2)) % max_size;
				if (h1 == newdata % max_size) {
					return false;
				}
				if (hash_table[h1] == key) {
					return true;
				}
				step ++;
			}
		}
		if (mode == DOUBLE_MODE) {
			int step = 1 + (newdata % R);
			while (true) {
				h1 += step;
				if (h1 == newdata % max_size) {
					return false;
				}
				if (hash_table[h1] == key) {
					return true;
				}
			}
		}
	}
const Hash& = delete;
Hash operator= (const Hash&) = delete;
};
