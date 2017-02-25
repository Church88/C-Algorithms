//Lee
#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
using namespace std;

//BEGIN GOLF
//All semicolons between here and the end count as a stroke against you

//This function will sort the given vector from vec[start] to vec[end], LEAST TO GREATEST
//You must do it recursively, and can only write code in this section (between the BEGIN and END)
void merge (vector<unsigned int> &vec, vector<unsigned int> left, vector<unsigned int> right) {
	for (unsigned int i = 0,R=0,L=0; i < vec.size(); i++) {
		if (L <= left.size()-1 && R <= right.size()-1) {
			(left.at(L) < right.at(R)) ? ((vec.at(i) = left.at(L)), L++) : ((vec.at(i) = right.at(R)), R++);
		}
		else if (L == left.size()){
			vec.at (i) = right.at (R),R++;
		}
		else if (R == right.size()){
			vec.at (i) = left.at (L),L++;
		}
	}
}
void mergesort (vector<unsigned int> &vec, int start, int end) {
	if (vec.size() > 1) {
		vector<unsigned int> left (vec.begin(), vec.begin() + vec.size() / 2), right (vec.begin() + vec.size() / 2, vec.end());
		mergesort (left, 0, (start + end) / 2),
		          mergesort (right, (start + end + 1) / 2, vec.size()),
		          merge (vec, left, right);
	}
}
//END GOLF

int main() {
	vector<unsigned int> vec;
	cout << "Please insert natural numbers to sort (0 to end): ";
	while (cin) {
		unsigned int temp;
		cin >> temp;
		if (!cin) break;
		if (temp == 0) break;
		vec.push_back (temp);
	}
	if (vec.size() > 0)
		mergesort (vec, 0, vec.size() - 1);
	for (unsigned int i : vec) cout << i << " ";
	cout << endl;
	system ("grep -c ';' main.cc >> temp");
	ifstream ins ("temp");
	int golf_score;
	ins >> golf_score;
	cerr << "Number of strokes: " << golf_score - 17 << endl;
	system ("rm temp");
}
