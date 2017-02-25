#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <cstdlib>
using namespace std;

void uppercaseify(string &str) {//Modify string to all uppercase
	for(char &ch : str) ch = toupper(ch);
}

void die() {
	cout << "Invalid Input!" << endl;
	exit(EXIT_FAILURE); //Same as exit(1) except sexier
}


class Table {//Tables will hold VINS and designated by a name
		string name = "";
		vector<string> data = {};
	public:
		Table() {//default will not be used but here anyways
			name = "BILL";
			data = {};
		}
		Table(string new_name) {
			name = new_name;
			uppercaseify(new_name); //uppercaseify input
			if(name != new_name) die(); //check to see if oringinal input was uppercase Bob != BOB
		}
		string get_name() {//becuase privates
			return name;
		}
		bool contains(string key) {//See if a VIN is inside a table
			for(string i : data) {
				if(i == key) return true;
			}
			return false;
		}
		void add_table(const Table &rhs) { //Used for inserting entire tables into another...giggity
			for(auto i : rhs.data) { //Iterating through table being passed in
				this->insert(i); //This is the table that is invoking this function ex: this.add_table(rhs)
			}
		}
		void insert(string new_car_or_vin) { //Insert individual values into a table
			string temp = new_car_or_vin;
			uppercaseify(temp);
			if(temp != new_car_or_vin) die(); //Die if not uppercase
			for(char c : new_car_or_vin){//Damn thing better not have special characters
				if(!isdigit(c) && !isalpha(c)){
					die();
				}
			}
			if(contains(temp)) return; //No duplicates, we dont't take kindly to their kind
			data.push_back(temp);//otherwise push on back
			return;
		}
		void print_table() {//Syntax for printing out a table with its name and values
			cout << name << ": ";
			if(data_size() == 0){
				cout << endl;
				return;
			}
			unsigned int index = 0;
			for(auto i : data) {
				if(index == data_size()-1) cout << i << endl;
				if(index != data_size()-1) cout << i  << ", ";
				index++;
			}
		}
		void print_UI() {//For when we unionize or intersect and want to print out results
			unsigned int index = 0;
			for(auto i : data) {
				if(index == data_size()-1) cout << i << endl;
				if(index != data_size()-1) cout << i << ", ";
				index++;
			}
		}
		//Union means to add the tables together without duplicates because I forgot this twice in one afternoon!
		Table union_fun(Table second){//Cause its fun 
			for(auto i : second.data){//Going through the second
				auto it  = find(data.begin(),data.end(),i);//Searching with values from the first
				if(it == data.end()){//If it wasnt in there put it in the first
					data.push_back(i);
				}
			}
			return *this;//We want to return a table which must be dereferenced
		}
			void intersex(Table second){//That intersection action only pushing back values found in both
			Table temp("INTER");
			for(string i : data){
				auto it = find(second.data.begin(),second.data.end(),i);
				if(it != second.data.end()){
					temp.data.push_back(i);
				}
			}
			temp.print_UI();//And print in proper format
		}
    bool subset(Table second){//If we find a value that isnt in the second its no good
      for(string i : data){
        auto it = find(second.data.begin(),second.data.end(),i);
        if(it == second.data.end()){
          return false;
        }
      }
      return true;
    }
		int data_size() {//Just cause
			return data.size();
		}

};

class MasterSet {//Class to hold all tables
	private:
		vector <Table> superset = {};
	public:
		MasterSet() {
			superset = {};
		}
		bool search(string name) {///One function to find them
			for(Table t : superset) {
				if(t.get_name() == name) {
					return true;
				}
			}
	   		return false;
		}
		Table &search_id(string name) {//One funtion to return them
			for(Table &t : superset) {
				if(t.get_name() == name) {
					return t;
				}
			}
			die();
		}
		void insert(Table new_table) {//And in the darkness combine them!
			superset.push_back(new_table);
		}
		int unique() {//Just need the number of unique values
			int total;
			Table t("UNIQUE");
			for(Table x : superset) {//For every table we need to add them together minus duplicates...
				t.union_fun(x); //Union prints out shit, but it got fixed, like a stray dog
			}
			total = t.data_size();//...and grab its size
			return total;//Tada
		}
    void poset(){//Proper subset is a subset smaller than the parent set
      for(Table y : superset){//
        //Table temp_1 = y;
        for(Table z : superset){
            if(y.data_size() < z.data_size()){//Same deal as subset but we only evaluate if its smaller than the one we are running through
              if(y.subset(z)){
                cout << y.get_name() << " " << z.get_name() << endl;
              }
            }
        }
      }
      return;
    }
		void print_masterset() {//Print that whole damn thing
			for(Table t : superset) {
				t.print_table();//With another fancy print function
			}
		}
};



MasterSet haystack;//Its the global haystack

int main() {
	string user_input = "";
	while(true) {
		string command = "";//Going to be reading in a bunch of different strings
		string table_id = "" , table_id_2 = "";
		string values = "";
		string name = "";
		getline(cin, user_input); //Grab the entire line of user input
		istringstream input(user_input);//Set up our own input stream with one line of user input
		input >> command; //Read in the first word from the user input
		if(command == "EXIT" || command == "") exit(1);//Just for me
		else if(command == "CREATE"){
			input >> command;
			if(command == "TABLE") {
				input >> name; //Now we have a table name coming in
				if(name == "") die();
				if(!haystack.search(name)) { //No duplicate tables
					Table temp(name); //Create a table with our captured name
					haystack.insert(temp); //insert our table into the haystack
				} else die();//Die after every...single...if statement
			} else die();
		}
		else if(command == "INSERT"){
			input >> command;
			if(command == "INTO") {
				input >> table_id; //INSERT INTO <ID>
				if(haystack.search(table_id)) {
					//We've found the table we wish to insert values into
					Table &temp = haystack.search_id(table_id);//Inserting into a table so we grab that table by reference to modify it
					input >> command;
					if(command == "VALUES") {
						string needle;
						//ws(input); //Strip white spacea might not need this one
						getline(input,needle);//Set up a new input stream 
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
				if(!haystack.search(table_id)) {//Cause we cant insert into a table we dont have
					die();
				}
			} 
			else die();
		}
		else if(command == "SELECT"){
			input >> command;
			if(command == "FROM"){
				input >> table_id;
				if(haystack.search(table_id)){//Make sure we have the table
					input >> command;
					if(command == "UNION"){
						input >> table_id_2;
						if(haystack.search(table_id_2)){//Make sure we have the other one
							Table temp_1 = haystack.search_id(table_id);//Now grab the actual tables 
							Table temp_2 = haystack.search_id(table_id_2);
							Table c = temp_1.union_fun(temp_2);//And smush them together
							c.print_UI();//Vomit to screen
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
						haystack.print_masterset();//Print out everything
						cout << haystack.unique() << endl;//Print out number of uniques
					}
					else if(haystack.search(command)){
						table_id = command;
						input >> command;
						if(command == "INNER"){
							input >> command;
							if(command == "JOIN"){
								input >> command;
								if(haystack.search(command)){
									table_id_2 = command;
									Table c = haystack.search_id(table_id);//Grab the tables again
									Table d = haystack.search_id(table_id_2);
									c.intersex(d);//Cause its funnier than intersection
								}
								else die();//Die mother trucker die
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
		else if(command == "PRINT"){
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
    else if(command == "IS"){
    input >> table_id;
    if(haystack.search(table_id)){
      input >> command;
      if(command == "SUBSET"){
        input >> table_id_2;
        if(haystack.search(table_id_2)){
          Table temp_1 = haystack.search_id(table_id);
          Table temp_2 = haystack.search_id(table_id_2);
          if(temp_1.subset(temp_2)){
              cout << "TRUE" << endl;
          }
          else cout << "FALSE" << endl;
        }
        else die();
      }
      else die();
    }
    else die();
  }
  else if(command == "POSET"){
    haystack.poset();
  }
  else die();
  //WE LIVE WE DIE, WE LIVE AGAIN!!!!
  int x = 42; //For luck
}
return 0;
}
