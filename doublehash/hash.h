#pragma once
#include <iostream>
#include <assert.h>
#include <vector>
#include <cmath>
using namespace std;

class Hash {
	int mode; //Holds how we are implementing the queue, either as linked list, array, or vector
	int max_size = 11; //Holds the size of our array
	int contains = 0; //Holds how many elements we contain
	int R = 7;
	int *hash_table;

	public:
	static const int LINEAR_MODE = 0;
	static const int QUADRATIC_MODE = 1;
	static const int DOUBLE_MODE = 2;
	Hash (int new_mode = LINEAR_MODE, int new_size = 1000) : mode (new_mode), max_size (new_size) {
		hash_table = new int[max_size];
		for (int i = 0; i < max_size; i++) hash_table[i] = 0;
	}
	//Returns percentage the array is full
	float get_load() {
		return contains / float (max_size);
	}
	//YOU: Print out the hash table for debugging purposes
	void print() {
		for (int i = 0; i < max_size; i ++) {
		cout << "Index " << i << ": " << hash_table[i] << endl;
		}
	}
	//YOU: Insert a new int into the hash table using the rules found in mode
	void insert (int newdata) {
		int h1 = newdata % max_size;
		if (hash_table[h1] == 0 || hash_table[h1] == newdata) {
			hash_table[h1] = newdata;
			contains ++;
			return;
		}
		if (mode == LINEAR_MODE) {
			int step = 1;
			while (true) {
				h1 += step;
				h1 %= max_size;
				if (h1 == newdata % max_size) {
					cout << "Hash table full!" << endl;
					exit (1);
				}
				if (hash_table[h1] == 0) {
					hash_table[h1] = newdata;
					contains++;
					return;
				}
			}
		}
		if (mode == QUADRATIC_MODE) {
			int step = 1;
			int originalHashLocation = h1;
			while (true) {
				int x = pow(step,2); //Collisions+1^2
				h1 = ((originalHashLocation + x) % max_size);
				if (h1 == newdata % max_size) {
					cout << "Hash table full!" << endl;
					exit (1);
				}
				if (hash_table[h1] == 0) {
					hash_table[h1] = newdata;
					contains++;
					return;
				}
				step++;
			}
		}
		if (mode == DOUBLE_MODE) {
			int step = 1 + (newdata % R);
			while (true) {
				h1 += step;
				h1 %= max_size;
				if (h1 == newdata % max_size) {
					cout << "Hash table full!" << endl;
					exit (1);
				}
				if (hash_table[h1] == 0) {
					hash_table[h1] = newdata;
					contains++;
					return;
				}
			}
		}
		cout << "Something went wrong.\n";
		exit(1);
	}
	//YOU: Returns true if the key is in the hash table, false otherwise
	bool search (int key) {
		int h1 = key % max_size;
		if (hash_table[h1] == key) return true;
		if (hash_table[h1] == 0) return false;
		if (mode == LINEAR_MODE) {
			int step = 1;
			while (true) {
				h1 += step;
				h1 %= max_size;
				if (h1 == key % max_size || hash_table[h1] == 0) {
					return false;
				}
				if (hash_table[h1] == key) {
					return true;
				}
			}
		}
		if (mode == QUADRATIC_MODE) {
			int step = 1;
			int originalHashLocation = h1;
			while (true) {
				int x = pow(step,2);
				h1 = (originalHashLocation + x) % max_size;
				if (h1 == key % max_size || hash_table[h1] == 0) {
					return false;
				}
				if (hash_table[h1] == key) {
					return true;
				}
				step ++;
			}
		}
		if (mode == DOUBLE_MODE) {
			int step = 1 + (key % R);
			while (true) {
				h1 += step;
				h1 %= max_size;
				if (h1 == key % max_size || hash_table[h1] == 0) {
					return false;
				}
				if (hash_table[h1] == key) {
					return true;
				}
			}
		}
		return false;
	}
	Hash (const Hash&) = delete;
	Hash operator= (const Hash&) = delete;
};
