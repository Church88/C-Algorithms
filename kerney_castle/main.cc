#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <queue>
#include <ncurses.h>
#include <utility>
#include <limits>
#include "splash.h"
using namespace std;

const unsigned char OPEN = '.'; //An open space
const unsigned char WALL = '#'; //A wall space; impassable
const unsigned char KING = 'K'; //Represents the King
const unsigned char PAWN = 'p'; //Represents the enemy pawns trying to capture the King
const unsigned char TREASURE = '$'; //Represents money, which the pawns will move to capture and move off screen
const unsigned char PAWN_WITH_TREASURE = 'P'; //A pawn carrying money off the screen - if it touches an edge it drops off the gold and becomes a normal pawn again

//Globals holding game state
int SIZE_X, SIZE_Y; //Dimensions of the castle
int cursor_x, cursor_y; //Cursor for drawing stuff
unsigned char* castle; //A character array holding our castle
unsigned int max_kings, king_count; //Holds number of kings in the world
unsigned int max_pawns, pawn_count; //Holds number of pawns in the world
bool game_on; //Holds if the simulation is running

const unsigned int MIN_SIZE = 8; //Minimum size of the castle
const unsigned int TIMEOUT = 300; //Milliseconds to wait for a getch to finish
const int UP = 65; //Key code for up arrow
const int DOWN = 66;
const int LEFT = 68;
const int RIGHT = 67;

//2D to 1D array mapping
//NOTE: creates a circular array
//For example, access the castle location (3,2) like this:
//  castle[index(3,2)]
int index (int i, int j) {
	while (i < 0) i += SIZE_X;
	while (j < 0) j += SIZE_Y;
	if (i >= SIZE_X) i %= SIZE_X;
	if (j >= SIZE_Y) j %= SIZE_Y;
	return (i * SIZE_Y + j);
}

void reset_castle() {
	for (int i = 0; i < SIZE_X; i++)
		for (int j = 0; j < SIZE_Y; j++)
			if (i == 0 || j == 0 || i == SIZE_X - 1 || j == SIZE_Y - 1) //Is edge
				castle[index (i, j)] = WALL;
			else
				castle[index (i, j)] = OPEN;
}

//Prints walls surrounding the castle, and then the contents of castle inside it
void print_castle() {
	for (int i = 0; i < SIZE_X; i++) {
		for (int j = 0; j < SIZE_Y; j++) {
			if (i == cursor_x && j == cursor_y)
				attron (A_UNDERLINE | A_BOLD);
			//if (i == 0 || j == 0 || i == SIZE_X - 1 || j == SIZE_Y - 1) //Is edge
			//	mvaddch(i,j,WALL);
			//else
			mvaddch (i, j, castle[index (i, j)]);
			attroff (A_UNDERLINE | A_BOLD);
		}
	}
}

void die() {
	cout << "Invalid input.\n";
	exit (EXIT_FAILURE);
}


float distance (float fin_x, float fin_y, float start_x, float start_y) {
	return float (sqrt (pow ( (fin_x - start_x), 2) + pow ( (fin_y - start_y), 2)));
}

struct Node {
	int x = 0;
	int y = 0;
	float dist = 0.0;
	Node *parent;
	Node (int new_x, int new_y, float d) {
		x = new_x;
		y = new_y;
		dist = d;
	}
	bool operator== (const Node &rhs) const {
		return (x == rhs.x) && (y == rhs.y);
	}
};
struct to_from {
	bool treasure;
	to_from(const bool &flag=false){
		treasure = flag;
	}
	bool operator() (const Node &lhs, const Node &rhs) const {
		if(treasure) return lhs.dist < rhs.dist; //Run from
		return lhs.dist > rhs.dist; // Run to
	}
};

pair <int, int> seek (int start_x, int start_y, int fin_x, int fin_y, bool has_treasure) {
	pair<int, int> next_step = make_pair (start_x, start_y); //The return value at the very end
	//BFS to find nearest target
	deque<pair<int, int>> BFS; //BFS positions
	BFS.push_back (make_pair (start_x, start_y)); //Grab the first one
	pair<int, int> king_pos, treasure_pos, edge_pos; //Need to mark target locations for A*
	bool king_found = false, treasure_found = false, edge_found = false;
	while (true) {
		unsigned int discard = BFS.size();//Capture for discarding checked positions
		if (discard == 0) return make_pair (start_x, start_y);
//		cout << "Outer for loop\n";
		for (int i = 0; i < discard; i++) {
			int x_check = BFS.at(i).first;
			int y_check = BFS.at(i).second;
			for (int a = -1; a < 2; a ++) {
				for (int b = -1; b < 2; b++) {
					if(a==0 && b==0) continue;
					if(castle[index(x_check+a,y_check+b)] != WALL){ //Everytime we get a valid position we need to create a new Node 
					//Implement a parent system to the nodes 
					pair<int, int> temp = make_pair (x_check + a, y_check + b);
					BFS.push_back (temp);
					}
					if (castle[index (x_check + a, y_check + b)] == KING) {
						king_pos = make_pair (x_check + a, y_check + b);
						king_found = true;
					}
				}
			}
		}
		if (king_found == true) break;
		for (unsigned int i = 0; i < discard; i++) {
			BFS.pop_front();//Scrap all of the checked positions
		}
	}//Rinse and repeat
//After we generate 


	//After we find the target set up priority queue
	Node start (start_x, start_y, 0);
	typedef priority_queue<Node, vector<Node>, to_from> my_queue;
	my_queue search;
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			if(i==0 && j==0) continue;
			if ((start_x == king_pos.first) && (start_y == king_pos.second)) { //target spotted, move there
				next_step = make_pair (king_pos.first, king_pos.second);
				return next_step;
			}
			if (castle[index (start_x + i, start_y + j)] == OPEN) { //if it hasn't been visited and the node is OPEN, visit it
				float distance_from_start = distance (start_x, start_y, start_x+i, start_y+j); //calculate the distance we are away from start
				float distance_to_target = distance (king_pos.first, king_pos.second, start_x + i, start_y + j);
				float travel = distance_from_start + distance_to_target;
				Node processing (start_x + i, start_y + j,travel);
				search.push (processing);
			}
		}
	}
	if (search.size() == 0) { //we're trapped trying to close distance
		return next_step;
	}
	if (search.size() > 0) {
		next_step = make_pair (search.top().x, search.top().y);
	}
	return next_step;
}

//YOU: You write this code. It will control the pawns, and will be called every frame the game is running
//The rules for pawns are as follows:
//1. They must move one square each frame, diagonals included
// 1.1. If they can move toward their nearest target (see #3 below) they will do so
// 1.2. If they cannot move toward their nearest target they will move randomly if they can
// 1.3. If they are completely blocked in, they do not move at all
//2. They cannot pass through other pawns or walls.
// (i.e. They can only move through open squares or into KING squares to capture them)
//3. Every frame, they calculate their closest target, which is the nearest (shortest path)
// TREASURE or KING square.
// 3.1. If they are adjacent to a KING, that takes precedence over all else, and will capture the KING
//  Capturing a king will decrement the king_count.
// 3.2. Otherwise, if there is a tie (equal distance, greater than 1) between a TREASURE and a KING they
//  will choose to move to the TREASURE over the KING
//4. If there are no treasures or Kings and there are Pawns, the Pawns win, and print "Game Over Pawns Win"
//5. If a pawn moves over a TREASURE, it becomes a PAWN_WITH_TREASURE, which changes how it moves:
// 5.1. Pawns with treasure ignore kings entirely (and will refuse to capture them, and path around them)
// 5.2. Pawns with treasure will path to the nearest edge, moving one square each round
// 5.3. If a Pawn reaches an edge, it will drop off the treasure, becoming a normal PAWN again
//If all that is too complicated, ignore the TREASURES and just go after KINGs for a loss of one grade.
void ai_pawn (int i, int j) { //i and j are the coordinates the pawn is at
	if (castle[index (i, j)] != PAWN_WITH_TREASURE) {
		for (int a = -1; a < 2; a ++) {
			for (int b = -1; b < 2; b++) {
				if (castle[index (a + i, b + j)] == KING) { //Woot
					castle[index (i, j)] = OPEN; //Vacate current spot
					castle[index (a + i, b + j)] = PAWN; //Put us in new spot
					king_count--;
					return;
				}
			}
		}
	}
	pair<int, int> step;
	int target_x = 0, target_y = 0;
	bool has_treasure = false;
	if (castle[index (i, j)] == PAWN_WITH_TREASURE) { //Seek to edge and avoid kings
		has_treasure = true;
		step = seek (i, j, target_x, target_y, has_treasure);
		castle[index (i, j)] = OPEN;
		castle[index (step.first, step.second)] = PAWN_WITH_TREASURE;
	}
	if (castle[index (i, j)] != PAWN_WITH_TREASURE) { //Seek towards nearest king or treasure, treasure takes priority
		has_treasure = false;
		step = seek (i, j, target_x, target_y, has_treasure);
		castle[index (i, j)] = OPEN;
		castle[index (step.first, step.second)] = PAWN;
	//	cout << "I am here: " << i << " " << j << endl;
	//	cout << "Found next step: " << step.first << " " << step.second << endl;
	//	exit(1);
	}
}

//The AI for Kings is simple. If there is a pawn next to us, we move on top of them and capture them
// This removes them from the game, and decrements pawn_counter.
//If there are at least one kings in the game and no pawns, then print a "Game Over Kings Win" message
void ai_king (int i, int j) {
	for (int a = -1; a < 2; a ++) {
		for (int b = -1; b < 2; b++) {
			if ( (castle[index (a + i, b + j)] == PAWN) || (castle[index (a + i, b + j)] == PAWN_WITH_TREASURE)) { //Woot
				castle[index (i, j)] = OPEN; //Vacate current spot
				castle[index (a + i, b + j)] = KING; //Put us in new spot
				pawn_count--;
				return;
			}
		}
	}
}

int main() {
	cout << "How big a castle do you want? (Enter x size followed by y then hit return): ";
	cin >> SIZE_Y >> SIZE_X; //Everything is backwards (x and y) in NCURSES
	if (!cin || SIZE_X < MIN_SIZE || SIZE_Y < MIN_SIZE) {
		cout << "Error: castle size must be at least " << MIN_SIZE << " in each dimension.\n";
		return 1;
	}
	castle = new unsigned char[SIZE_X * SIZE_Y];
	reset_castle();

	//Set up NCURSES
	initscr();//Start curses mode
	clear();
	noecho();
	cbreak();
	timeout (TIMEOUT); //Set a max delay for key entry

	splash();
	clear();

	//MAIN LOOP
	int frame = 0;
	cursor_x = SIZE_X / 2, cursor_y = SIZE_Y / 2; //Cursor for drawing stuff
	game_on = false;
	while (true) {
		if (!game_on)
			mvprintw (SIZE_X, 0, "Design Mode - Game Paused                                        ");
		else
			mvprintw (SIZE_X, 0, "Game Running. Kings: %i Pawns: %i Frame: %i", king_count, pawn_count, frame++);
		mvprintw (SIZE_X + 1, 0, "Use arrow keys to move the cursor around");
		mvprintw (SIZE_X + 2, 0, "Type Space to clear the square or set a wall");
		mvprintw (SIZE_X + 3, 0, "Type K to place a King");
		mvprintw (SIZE_X + 4, 0, "Type P to place a Pawn");
		mvprintw (SIZE_X + 5, 0, "Type $ to place a Treasure");
		mvprintw (SIZE_X + 6, 0, "Type Enter to run or pause the game");
		mvprintw (SIZE_X + 7, 0, "Type Q to quit");
		int ch = getch(); // Wait for user input, with TIMEOUT delay
		if (ch == 'q' || ch == 'Q') break;
		else if (ch == ' ' || ch == '#') {
			if (castle[index (cursor_x, cursor_y)] == OPEN)
				castle[index (cursor_x, cursor_y)] = WALL;
			else
				castle[index (cursor_x, cursor_y)] = OPEN;
		} else if (ch == 'K' || ch == 'k') {
			if (castle[index (cursor_x, cursor_y)] == OPEN) {
				castle[index (cursor_x, cursor_y)] = KING;
				king_count++;
				max_kings++;
			}
		} else if (ch == 'p' || ch == 'P') {
			if (castle[index (cursor_x, cursor_y)] == OPEN) {
				castle[index (cursor_x, cursor_y)] = PAWN;
				pawn_count++;
				max_pawns++;
			}
		} else if (ch == '4' || ch == '$') {
			if (castle[index (cursor_x, cursor_y)] == OPEN) {
				castle[index (cursor_x, cursor_y)] = TREASURE;
			}
		}
		//Handle arrow keys, the NCURSES #defs don't work here for some reason
		//KEY_UP, for example, is set to 0403 (259), not 65
		else if (ch == RIGHT) {
			cursor_y++;
			if (cursor_y >= SIZE_Y) cursor_y = SIZE_Y - 1; //Clamp value
		} else if (ch == LEFT) {
			cursor_y--;
			if (cursor_y < 0) cursor_y = 0;
		} else if (ch == UP) { //Should be KEY_UP, grr
			cursor_x--;
			if (cursor_x < 0) cursor_x = 0;
		} else if (ch == DOWN) {
			cursor_x++;
			if (cursor_x >= SIZE_X) cursor_x = SIZE_X - 1; //Clamp value
		} else if (ch == ERR) { //No keystroke
			; //Do nothing
		} else if (ch == '\n') {
			game_on = !game_on; //Pause or unpause the game
		}
		//else
		//mvprintw(SIZE_Y+5,0,"Unknown character: %i\n",ch);

		//Run the AI movement for pawns and kings
		if (game_on) {
			for (int i = 0; i < SIZE_X; i++) {
				for (int j = 0; j < SIZE_Y; j++) {
					if (castle[index (i, j)] == PAWN || castle[index (i, j)] == PAWN_WITH_TREASURE)
						ai_pawn (i, j); //Run the AI on all pawns in the castle
					else if (castle[index (i, j)] == KING)
						ai_king (i, j); //Run the AI on all kings in the castle
				}
			}
		}

		//YOU: If the above code works, then the count will cause the game to end.
		//Check for victory -
		// At least one king and pawn was placed in the game, and now there's none
		if (max_kings > 0 && max_pawns > 0 && (king_count <= 0 || pawn_count <= 0)) {
			clear();
			break;
		}

		print_castle();
		refresh(); // Print it on to the real screen
		if(game_on) wait_ticks(300000); //Additional pause if you want
	}
	endwin(); // End curses mode

	//You could delete this and put in a better win screen in CURSES
	system ("clear");
	if (pawn_count <= 0 && max_pawns > 0) {
		cout << "GAME OVER: The Kings Win!\n";
	}
	if (king_count <= 0 && max_kings > 0) {
		cout << "GAME OVER: The Pawns Win!\n";
	}

	return 0;
}
