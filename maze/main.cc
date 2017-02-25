//LEE
#include <iostream>
#include <cctype>
#include <deque>
#include <queue>
#include <functional>
#include <cmath>
#include <algorithm>
#include <utility>
#include <array>
#include <map>
using namespace std;
int end_x = 0;
int end_y = 0;
const char FRONTLINE = 'O';
const char PLAYER = 'x';
const int SIZE = 20;
const char EMPTY = '.';
const char START = '@';
const char END = '$';
const char CHECKER = '*';
const int REFRESH = 100000;
const char WALL = '#';
void print_board (char board[][SIZE]) { //Print the board with a border that is not included on the board itself
	for (int i = 0; i < SIZE + 2; i ++)
		cout << WALL;
	cout << endl;
	for (int i = 0; i < SIZE; i ++) {
		cout << WALL;
		for (int j = 0; j < SIZE; j ++) {
			cout << board[i][j];
		}
		cout << WALL << endl;
	}
	for (int i = 0; i < SIZE + 2; i ++)
		cout << WALL;
	cout << endl;
}//Bounds checking for the edge will be for going off the board not if there is a wall there
void pause (int ticks) { //Wait pause
	clock_t start = clock();
	while ( (clock() - start) < ticks);
}
bool spot_check (int x, int y, char board[][SIZE], bool out) { //Check bounds and walls
	if (x >= SIZE || x < 0 || y >= SIZE || y < 0) {
		if (out) cout << "Invalid input, please try again\n"; //Only need outputs for user input
		return false;
	}
	if (board[x][y] == WALL) {
		if (out) cout << "Invalid input, space is a wall\n";
		return false;
	}
	if (board[x][y] != EMPTY) return false;
	return true;
}
bool win_check (int x, int y, char board[][SIZE]) {
	for (int a = -1; a <= 1; a ++) { //Double loop here checks all positions around point
		for (int b = -1; b <= 1; b ++) {
			if (board[x + a][y + b] == END) {
				return true;
			}
		}
	}

	return false;
}

class Node {
  public:
	pair<int, int> point = {};
	float dist = 0;
	vector<pair<int, int>> linked = {};
	Node() {}
	Node (int x, int y, float new_dist) {
		point = make_pair(x,y);
		dist = new_dist;
	}
};
float dist_calc (pair<int, int> point2, pair<int, int> point1) {
	return sqrt (pow (point2.first - point1.first, 2) + pow (point2.second - point1.second, 2));
}

void seek (char board[][SIZE], pair<int, int>start) {
	multimap<float, Node> solution;//Probably need something other than multimap 
	int travel = 0;
	Node temp = Node (start.first, start.second, travel);
	solution.insert (make_pair(temp.dist, temp));
	deque<pair<int, int>> frontline;
	frontline.push_back (temp.point);
	bool msg = false; //No error messages when checking positions
	while (true) {
		unsigned int front = frontline.size();//Capture for later popping off
		if (front == 0) {
			cout << "Houston we have a problem\n";
			return;
		}
		for (unsigned int i = 0; i < front; i ++) {
			pair<int, int> temp = frontline.at (i);
			int x = temp.first;
			int y = temp.second;
			for (int a = -1; a <= 1; a ++) { //Double loop here checks all positions around point
				for (int b = -1; b <= 1; b ++) {
					if (spot_check (x + a, y + b, board, msg)){
						frontline.push_back (make_pair(x + a, y + b)); //Take all open spots around point and put them in the frontline
						board[x+a][y+b] = FRONTLINE;
						system ("clear");
						print_board (board);
						pause (REFRESH);
					}
				}

			}
		}
		for (unsigned int i = 0; i < front; i ++) {
			frontline.pop_front();
		}
		travel++;
		//Iterate over frontline pushing back nodes assigning distance based on repition of while loop (travel) not crows flight
		for (unsigned int i = 0; i < frontline.size(); i ++){
			int x = frontline.at(i).first;
			int y = frontline.at(i).second;
			temp = Node(x,y,travel);
			solution.insert (make_pair(travel, temp));//Using distance based on iterations from the origin rather than calculated distance
			board[x][y] = CHECKER;
			system ("clear");
			print_board (board);
			pause (REFRESH);
		}
	}
}


bool solve (char board[][SIZE], deque<pair<int, int>> start) {
	bool msg = false; //Do not want to output anything from spot checker
	//Set up a deque for each start position
	deque<deque<pair<int, int>>> master;
	for (auto i : start) {
		deque<pair<int, int>> temp;
		pair<int, int> temp2 (i.first, i.second);
		temp.push_back (temp2);
		master.push_back (temp);
	}
	//Check all positions around start and fill in
	while (true) {
		for (unsigned int i = 0; i < master.size(); i ++) {//Checking from every start position
			int dec_size = master.at (i).size(); //Grab original size to later discard
			if (dec_size == 0) return false;
			bool win = false;
			for (int j = 0; j < dec_size; j ++) {//Iterate only through the orginal values
				pair<int, int> temp_spot = master.at (i).at (j);
				//Run a spot check on each position around temp_spot
				int x = temp_spot.first;
				int y = temp_spot.second;
				pair<int, int> temp_pair;
				for (int a = -1; a <= 1; a ++) { //Double loop here checks all positions around point
					for (int b = -1; b <= 1; b ++) {
						if (spot_check (x + a, y + b, board, msg)) {
							board[x + a][y + b] = CHECKER;
							temp_pair.first = x + a;
							temp_pair.second = y + b;
							master.at (i).push_back (temp_pair);
						}
							if (board[x+a][y+b] == END) win = true;
						}
				}
			}
			system ("clear");
			print_board (board);
			pause (REFRESH);
			if (win) return true;
			for (int j = 0; j < dec_size; j++) { //Discard all of the original positions
				master.at (i).pop_front();
			}
		}//Rinse and repeat
	}
	return false;
}
void die() {
	cout << "Invalid input.\n";
	exit (EXIT_FAILURE);
}
void clear_board (char board [][SIZE]) {
	for (int i = 0; i < SIZE; i ++) {
		for (int j = 0; j < SIZE; j ++) {
			char pos = board[i][j];
			if (pos == WALL || pos == START || pos == END) continue;
			board[i][j] = EMPTY;
		}
	}
}
void waiter (string error = " ") {
	cout << error << "Press any key to continue.\n";
	string nothing;
	cin >> nothing;
}
int main () {
	system ("clear");
	char board[SIZE][SIZE] = {};
	clear_board (board);
	print_board (board);
	bool out = true;
	int x = 0;
	int y = 0;
	while (cin) {
		x = 0;
		y = 0;
		cout << "Please enter the walls of the maze (-1 to continue): ";
		cin >> x;
		if (!cin) {
			cin.clear();
			cin.ignore();
			cout << "Invalid input, reenter coordinates\n";
			continue;
		}
		if (x == -1) break;
		cin >> y;
		if (!cin) {
			cin.clear();
			cin.ignore();
			cout << "Invalid input, reenter coordinates\n";
			continue;
		}
		if (!spot_check (x, y, board, out)) continue;
		else {
			board[x][y] = WALL;
		}
		system ("clear");
		print_board (board);
	}
	system ("clear");
	print_board (board);
	int start_x = 0;
	int start_y = 0;
	deque<pair<int, int>> start_pos; //Setting up for multiple start positions **Nope
	while (cin) {
		cout << "Please enter the coordinates for the start point (-1 to continue): ";
		cin >> start_x;
		if (!cin) {
			cin.clear();
			cin.ignore();
			cout << "Invalid input, reenter coordinates\n";
			continue;
		}
		if (start_x == -1) break;
		cin >> start_y;
		if (!cin) {
			cin.clear();
			cin.ignore();
			cout << "Invalid input, reenter coordinates\n";
			continue;
		}
		if (!spot_check (start_x, start_y, board, out)) continue;
		board[start_x][start_y] = START;
		start_pos.push_back (make_pair (start_x, start_y));
		system ("clear");
		print_board (board);
		break;
	}
	system ("clear");
	print_board (board);
	while (cin) {
		cout << "Please enter the coordinates for the end point: ";
		cin >> end_x >> end_y;
		if (!cin) {
			cin.clear();
			cin.ignore();
			cout << "Invalid input, reenter coordinates\n";
			continue;
		}
		if (!spot_check (end_x, end_y, board, out))	continue;
		board[end_x][end_y] = END;
		system ("clear");
		print_board (board);
		break;
	}
	out = false;//No longer need to output error statements
	if (solve (board, start_pos)) {
		cout << "Maze is solvable.\n";
	} else {
		cout << "Maze is unsovable :( \n";
		return 0;
	}

//	seek(board,start_pos.at(0));
	//Enter open world mode
	cout << "Now it is your turn. First select your start location:\n";
	x = 0;
	for (auto i : start_pos) {
		cout << x << ": " << i.first << " " << i.second << endl;
		x++;
	}
	cout << "Enter corresponding number for start position\n";
	unsigned int input;
	while (true) {
		cin >> input;
		if (!cin || input >= start_pos.size()) {
			cout << "Invalid input, try again.\n";
			cin.clear();
			cin.ignore();
			continue;
		} else break;
	}
	char move;
	int curr_x = start_pos.at (input).first;
	int curr_y = start_pos.at (input).second;
	clear_board (board);
	while (true) {
		board[curr_x][curr_y] = PLAYER;
		system ("clear");
		print_board (board);
		if (win_check (curr_x, curr_y, board)) {
			cout << "Congratulations you win! :)\n";
			break;
		}
		cin >> move;
		if (!cin) die();
		if (move == 'w' && spot_check (curr_x - 1, curr_y, board, out)) {
			board[curr_x][curr_y] = EMPTY;
			curr_x--;
		}
		if (move == 'a' && spot_check (curr_x, curr_y - 1, board, out)) {
			board[curr_x][curr_y] = EMPTY;
			curr_y--;
		}
		if (move == 's' && spot_check (curr_x + 1, curr_y, board, out)) {
			board[curr_x][curr_y] = EMPTY;
			curr_x++;
		}
		if (move == 'd' && spot_check (curr_x, curr_y + 1, board, out)) {
			board[curr_x][curr_y] = EMPTY;
			curr_y++;
		}
	}
}
