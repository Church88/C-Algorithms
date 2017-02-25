#include <iostream>
#include <vector>
using namespace std;

bool me(bool i, bool j, bool k) {
	if (!(i && j || k) || (j && k) && (!i || k) || j)
		return true;
	else
		return false;
}

//Everything between these two comments will have their character counted, so don't write comments or whatever.
//CUTPOINT1
bool you(bool i,bool j,bool k){return j||!(j^k);}
//CUTPOINT2

bool me2(bool i, bool j, bool k) {
	if (!(i && j && k) && (j || k) || ((i && k) || j))
		return true;
	else
		return false;
}

//Everything between these two comments will have their character counted, so don't write comments or whatever.
//CUTPOINT3
bool you2(bool i,bool j,bool k){return j||k;}
//CUTPOINT4

//Don't modify anything below here
int main() {
	vector<bool> bools = {true,false};
	cout << "Truth table (me vs you):\n";
	cout << "i" << "\t|\t" << "j" << "\t|\t" << "k" << "\t|\t" << "me()" << "\t|\t" << "you()" << endl;
	for (bool i : bools) 
		for (bool j : bools)
			for (bool k : bools)
				cout << boolalpha << i << "\t|\t" << j << "\t|\t" << k << "\t|\t" << me(i,j,k) << "\t|\t" << you(i,j,k) << endl;
	cout << "\nTruth table (me2 vs you2):\n";
	cout << "i" << "\t|\t" << "j" << "\t|\t" << "k" << "\t|\t" << "me2()" << "\t|\t" << "you2()" << endl;
	for (bool i : bools) 
		for (bool j : bools)
			for (bool k : bools)
				cout << boolalpha << i << "\t|\t" << j << "\t|\t" << k << "\t|\t" << me2(i,j,k) << "\t|\t" << you2(i,j,k) << endl;
	system("sed -n '/\\/\\/CUTPOINT1/,/CUTPOINT2/{/CUTPOINT1/b;/CUTPOINT2/b;p}' main.cc | wc -c > you.txt");
	system("sed -n '/\\/\\/CUTPOINT3/,/CUTPOINT4/{/CUTPOINT3/b;/CUTPOINT4/b;p}' main.cc | wc -c > you2.txt");
}
