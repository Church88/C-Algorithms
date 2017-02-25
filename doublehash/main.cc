#include <iostream>
#include <cstdlib>
#include <ctime>
#include "hash.h"
using namespace std;

void uppercaseify(string& mystr);

int main(int argv, char* argc[]) {
//	Uncomment this section to see how argc and argv works,
//	just be sure to comment it back out before the deadline...
//	cout << "Argv: " << argv << endl;
//	for (int i = 0; i < argv; i++)
//		cout << "Argc[" << i << "]: " << argc[i] << endl;
	

	unsigned int hash_size = 11;

	//If we have any command line parameters, time the run
	bool will_clock = false;
	if (argv > 1) will_clock = true; 

	//If the person does not enter a command line parameter
	//default to array version
	int mode = Hash::LINEAR_MODE;
	if (argv > 1) mode = atoi(argc[1]); //Allow user to change mode from command line.
	if (mode < Hash::LINEAR_MODE || mode > Hash::DOUBLE_MODE)
		mode = Hash::LINEAR_MODE;
	if (argv > 2) hash_size = atoi(argc[2]); //Allow user to change hash size
	if (hash_size < 10) hash_size = 10;

	/* Uncomment this section to see what parameters we'll be using
	 * again, be sure to comment it back out before turning in your homework
	cout << "will_clock: " << boolalpha << will_clock << endl;
	cout << "mode: " << mode << endl;
	*/

	Hash myhash(mode,hash_size);
	string operation;
	int data;

	//Capture the start time
	clock_t start_time = clock();

	while (cin) {
		cin >> operation;
		uppercaseify(operation);
		if (!cin || cin.eof() || operation == "QUIT") {
			break;
		}
		if (operation == "LOAD") {
			cout << myhash.get_load() << endl;
		}
		if (operation == "PRINT") {
			myhash.print();
		}
		if (operation == "INSERT") {
			cin >> data;
			myhash.insert(data);
		}
		if (operation == "DELETE") {
			cout << "Delete not supported!\n" << endl;
			exit(1);
		}
		if (operation == "SEARCH") {
			cin >> data;
			if (myhash.search(data))
				cout << "IN HASH\n";
			else
				cout << "NOT IN HASH\n";
		}
	}

	//Capture the finish time, then output the time elapsed
	//We will write it to cerr so that we can see it even when writing the file to an outputfile
	clock_t end_time = clock();
	if (will_clock) 
		cerr << 1000*(double(end_time - start_time) / CLOCKS_PER_SEC) << endl;

	return 0;
}

void uppercaseify(string& mystr) {
	for (auto& c: mystr)
		c = toupper(c);
}
