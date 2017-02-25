#include <vector>
#include <iostream>
#include <deque>
#include <cmath>
using namespace std;

const int LINE_SIZE = 5;
const int CHAR_PER_LINE = 4;


deque<char> hexi = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

deque<char> convert (int deci, int base, int vecsize) { //Hex value of index (0-9,A-F)
	deque<char> ret (vecsize, '0');
	if (deci == 0) return ret;
	// *********SECOND ATTEMPT*************
	int spot = vecsize - 1;
	while (true) {
		int rem = deci % base;
		ret.at (spot) = hexi.at (rem);
		deci /= base;
		if (deci == 0) return ret;
		spot--;
	}
	return ret;
}

int main() {
	int line = 0;
	int hex = 16;
	int oct = 8;
	int bin = 2;
	int bits = 8;
	deque<char> vomit_cha = {};
	deque<char> vomit_bin = {};
	deque<char> vomit_oct = {};
	deque<char> vomit_hex = {};
	char ch;
	while (true) {
		ch = cin.get();
		int as = ch;
		if (as < 0) break;
		vomit_cha.push_back (ch);
		for (auto i : convert (as, bin, bits)) {
			vomit_bin.push_back (i);
		}
		for (auto i : convert (as, oct, 3)) {
			vomit_oct.push_back (i);
		}
		for (auto i : convert (as, hex, bits / 4)) {
			vomit_hex.push_back (i);
		}
	}
	line = vomit_cha.size() / CHAR_PER_LINE;
	for (int i = 0; i < line; i++) {//Print off the last set of each vomit and pop each value as it comes out
		deque<char> line_num = convert (i * 4, hex, LINE_SIZE);
		for (unsigned int i = 0; i < LINE_SIZE; i++) {
			cout << line_num.at (0);
			line_num.pop_front();
		}
		cout << '\t';
		for (unsigned int i = 0; i < 32; i++) {
			cout << vomit_bin.at (0);
			vomit_bin.pop_front();
		}
		cout << '\t';
		for (unsigned int i = 0; i < 12; i ++) {
			cout << vomit_oct.at (0);
			vomit_oct.pop_front();
		}
		cout << '\t';
		for (unsigned int i = 0; i < 8; i ++) {
			cout << vomit_hex.at (0);
			vomit_hex.pop_front();
		}
		cout << '\t';
		for (unsigned int i = 0; i < 4; i ++) {
			if (isprint (vomit_cha.at (0))) cout << vomit_cha.at (0);
			else cout << ".";
			vomit_cha.pop_front();
		}
		cout << endl;
	}
	return 0;
}
