#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <cstdlib>
using namespace std;

void uppercaseify(string &str) {
	for(char &ch : str) ch = toupper(ch);
}

void die() {
	cout << "INVALID INPUT" << endl;
	exit(EXIT_FAILURE); //same as exit(1)
}


class Table {
	private:
		string name = "";
		vector<string> data = {};
	public:
		Table() {
			name = "BILLKERNEY1864712049423024129";
			data = {};
		}
		Table(string new_name) {
			name = new_name;
			uppercaseify(new_name); //uppercaseify our input
			if(name != new_name) die(); //check to see if oringinal input was uppercase Bob != BOB
		}
		string get_name() {
			return name;
		}
		bool contains(string key) {
			for(string i : data) {
				if(i == key) return true;
			}
			return false;
		}
		void add_table(const Table &rhs) { //Bob is rhs E.g. INSERT INTO LITHIA VALUES BOB,JEEP,RANGER
			//For every string in bob.data lithia->insert(string);
			//temp.add_table(unionT);
			for(auto i : rhs.data) { //interating over bob's data
				this->insert(i);
			}	
		}
		void insert(string new_car_or_vin) { //Table x; x.isnert(value);
			string temp = new_car_or_vin;
			uppercaseify(temp);
			if(temp != new_car_or_vin) die(); //Die if not uppercase
			if(contains(temp)) die(); //No duplicates 
			data.push_back(temp);
			return;
		}
		void print_table() {
			cout << name << ": ";
			int index = 0;
			for(auto i : data) {
				if(index == data.size()-1) cout << i << endl;
				if(index != data.size()-1) cout << i << ", ";
				index++;
			}
		}
		void print_UI() {
			unsigned int index = 0;
			for(auto i : data) {
				if(index == data.size()-1) cout << i << endl;	
				if(index != data.size()-1) cout << i << ", ";
				index++;
			}
		}
		//Union means to add the tables together without duplicates ie COMBINE
		Table union_fun(Table second){
			for(auto i : second.data){
				auto it  = find(data.begin(),data.end(),i);
				if(it == data.end()){
					data.push_back(i);
				}
			}
			return *this;
		}
		//function.call(parameters);
		//function(invoke);
		void intersex(Table second){
			Table temp("INTER");
			for(string i : second.data){
				auto it = find(data.begin(),data.end(),i);
				if(it != data.end()){
					temp.data.push_back(i);
				}
			}
			temp.print_UI();
		}
		int data_size() {
			return data.size();	
		}

}; //End of class

class MasterSet {
	private:
		vector <Table> superset = {};
	public:
		MasterSet() {
			superset = {};
		}
		bool search(string name) {
			for(Table t : superset) {
				if(t.get_name() == name) {
					return true;
				}
			}
	   		return false;
		}
		Table &search_id(string name) {
			for(Table &t : superset) {
				if(t.get_name() == name) {
					return t;
				}
			}
			die();
		}
		void insert(Table new_table) {
			superset.push_back(new_table);
		}
		int unique() {
			int total;
			Table t("UNIQUE");
			for(Table x : superset) {
				t.union_fun(x); //Union prints out shit
			}
			total = t.data_size();
			return total;
		}
		void print_masterset() {
			for(Table t : superset) {
				t.print_table();
			}
			//Output unique number of cars in the system
		}
}; //End of class



MasterSet haystack;

int main() {
	string user_input = "";
	while(true) {
		string command = "";
		string table_id = "" , table_id_2 = "";
		string values = "";
		string name = "";
		getline(cin, user_input); //get full command string i.e. "CREATE TABLE BOB"
		istringstream input(user_input);
		input >> command; //We've read in the first word of the string i.e. "CREATE"
		if(command == "EXIT" || command == "") exit(1);
		else if (command == "CREATE") {
			input >> command;
			if(command == "TABLE") {
				input >> name; //We've read "BOB" or "Cynthia"	
				if(name == "") die();		
				if(!haystack.search(name)) { //returns a bool
					Table temp(name); //Create a table with our captured name
					haystack.insert(temp); //insert our table into the haystack
				} else die();
			} else die();
		}
		else if(command == "INSERT") {
			input >> command;
			if(command == "INTO") {
				input >> table_id; //INSERT INTO <ID>
				if(haystack.search(table_id)) {						
					//We've found the table we wish to insert values into
					Table &temp = haystack.search_id(table_id);
					input >> command;	
					if(command == "VALUES") {
						string needle; //INPUT == VALUES^ VIN1, VIN2, VIN3
						ws(input); //INPUT == VIN1,VIN2,VIN3
						getline(input,needle);
						istringstream csv(needle);
						string values;
						while( getline(csv,values,',') ) { //thanks stack overflow
							//Strip the whitespace if any from values
							values.erase(remove(values.begin(),values.end(),' '),values.end());
							if(haystack.search(values)) {
								//The value we're inserting is a table
								Table unionT = haystack.search_id(values);
								temp.add_table(unionT);	
							}
							else {
								temp.insert(values);	
							}
						}
					} else die();
				}
				if(!haystack.search(table_id)) {
					die();
				}
			} else die();
		}
		else if(command == "SELECT"){
			input >> command;
			if(command == "FROM"){
				input >> table_id;
				if(haystack.search(table_id)){
					input >> command;
					if(command == "UNION"){
						input >> table_id_2;
						if(haystack.search(table_id_2)){
							Table temp_1 = haystack.search_id(table_id);
							Table temp_2 = haystack.search_id(table_id_2);
							Table c = temp_1.union_fun(temp_2);
							c.print_UI();
						}
						else die();
					}
					else die();
				}
				else die();
			}
			else if(command == "*"){
				input >> command;
				if(command == "FROM"){
					input >> command;
					if(command == "*"){
						haystack.print_masterset();
						cout << haystack.unique() << endl;
					}
					else if(haystack.search(command)){
						table_id = command;
						input >> command;
						//string tempAdd = "";
						//input >> tempAdd;
						//command += " " + tempAdd;
						if(command == "INNER"){
							input >> command;
							if(command == "JOIN"){
								input >> command;
								if(haystack.search(command)){
									table_id_2 = command;
									Table c = haystack.search_id(table_id);
									Table d = haystack.search_id(table_id_2);
									c.intersex(d);
								}
								else die();
							}
							else die();
						} 
						else die();
					}
					else die();
				}
				else die();
			} 
			else die();
		}
		else if(command == "PRINT") {
			input >> command;
			if(command == "TABLE") {
				input >> name;
				if(haystack.search(name)) {
					Table temp = haystack.search_id(name);
					temp.print_table();
				}
			} 
			else if(command == "MASTER") {
				haystack.print_masterset();
			}
			else die();
		}
		else die();
	}
	return 0;
}
