#include <iostream>
#include <functional>
#include <cmath>
#include <queue>
#include <cstdlib>
using namespace std;

const double  AVG = 100000;
const int DIE_SIDES = 10;

//For this assignment, you will calculate 1k1 up through 10k10
//You will output the average values in a 10x10 table
//Any time you keep more dice than you roll, output a --

//A single d10 can explode, if you roll a 10
double rolls_keeps_sides (int dice_rolls, int keeps, int DIE_SIDES) {
	int total = 0;
	int roll_total = 0;//Total per roll to be stored
	for (int i = 0; i < AVG; i ++) {
		priority_queue<double> top_rolls;
		for (int i = 0; i < dice_rolls; i ++) {
			roll_total = 0; //Reset every time
			while (true) {
				int roll = rand() % DIE_SIDES + 1;
				roll_total += roll;
				if (roll != DIE_SIDES) break;
			}
			top_rolls.push (roll_total);
		}
		for (int i = 0; i < keeps; i ++) {
			double temp = top_rolls.top();
			total += temp;
			top_rolls.pop();
		}
	}
	return total / AVG;
}

int main() {
	srand (time (NULL));
	cout << endl << '\t';
	for (int i = 0; i < DIE_SIDES; i++) {
		cout << "KEEP " << i + 1 << '\t';
	}
	cout << endl << endl;
	for (int i = 0; i < DIE_SIDES; i ++) {
		cout << "ROLL " << i + 1 << '\t';
		for (int j = 0; j < DIE_SIDES; j ++) {
			if (j <= i) cout << rolls_keeps_sides (i + 1, j + 1, DIE_SIDES) << '\t';
			else cout << "--" << '\t';
		}
		cout << endl << endl;
	}
}
