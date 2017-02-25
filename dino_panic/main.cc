#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <ncurses.h>
#include <utility>
#include <limits>
#include <vector>
#include <queue>
#include <set>
#include "splash.h"
using namespace std;

class Herb;
class Herbivore;
class Carnivore;

const unsigned char OPEN = '.'; //An open space
const unsigned char WALL = '#'; //A wall space; impassable
const unsigned char WATER = '~'; //Water, refreshes thirst
const unsigned char HERB = '!'; //A plant, eaten by herbivores
const unsigned char HERBIVORE = 'H'; //A plant eating dinosaur
const unsigned char CARNIVORE = 'R'; //Eats herbivores

//Globals holding game state
int SIZE_X,SIZE_Y; //Dimensions of the world
int cursor_x, cursor_y; //Cursor for drawing stuff
unsigned char* world; //A character array holding our world
vector<pair<int,int>> water;
vector<Herb> herbs;
vector<Herbivore> herbivores; //Array of all herbivores in the world
vector<Carnivore> carnivores; //Array of all carnivores in the world
bool game_on; //Holds if the simulation is running

const unsigned int MIN_SIZE = 8; //Minimum size of the world
const unsigned int TIMEOUT = 300; //Milliseconds to wait for a getch to finish
const int UP = 65; //Key code for up arrow
const int DOWN = 66;
const int LEFT = 68;
const int RIGHT = 67;

//2D to 1D array mapping
//NOTE: creates a circular array
//For example, access the world location (3,2) like this:
//  world[index(3,2)]
int index(int i, int j) {
	while (i < 0) i+=SIZE_X;
	while (j < 0) j+=SIZE_Y;
	if (i >= SIZE_X) i %= SIZE_X;
	if (j >= SIZE_Y) j %= SIZE_Y;
	return (i*SIZE_Y+j);
}

float distance(float fin_x, float fin_y, float start_x, float start_y) {
	return float(sqrt(pow((fin_x-start_x),2) + pow((fin_y-start_y),2)));
}

class Herb {
	public:
		Herb () {}
		Herb (int new_x, int new_y) : x(new_x), y(new_y) {}
		int x = 0, y = 0; //Location
		int fecundity = 3; //3% each frame to spread
		int spread_radius = 2; //How many squares away we can create a new Herb
		int food_value = 10; //How many moves of energy it provides
		void think() { //AI For the plant
			if (rand() % 100 < fecundity) {
				int new_x = x + (rand() % (1 + spread_radius * 2)) - spread_radius; //Between -2 and +2 offset
				int new_y = y + (rand() % (1 + spread_radius * 2)) - spread_radius; //Between -2 and +2 offset
				if (world[index(new_x,new_y)] == OPEN) {
					world[index(new_x,new_y)] = HERB; //Mark the spot on the map
					herbs.push_back(Herb(new_x,new_y)); //Add our new herb to the herbs vector
				}
			}
		}
};

struct Node {
	int x_coor;
	int y_coor;
	float dist;
	Node(int x, int y, float d) {
		x_coor = x;
		y_coor = y;
		dist = d;
	}
	bool operator==(const Node &rhs) const {
		return (x_coor == rhs.x_coor) && (y_coor == rhs.y_coor);
	}
};
struct to_from {
	bool safe;
	to_from(const bool &flag=true){
		safe = flag;
	}
	bool operator()(const Node &lhs, const Node &rhs) const {
		if(safe) return lhs.dist < rhs.dist;//Closer
		return lhs.dist > rhs.dist;//Farther
	}
};
pair <int,int> seek(int start_x, int start_y, int final_x, int final_y, float distanceToTarget, bool safe) {
	pair<int,int> step = make_pair(start_x,start_y);
	Node start(start_x,start_y,0);
	Node target(final_x,final_y,distanceToTarget);
 	typedef priority_queue<Node,vector<Node>,to_from> custom_queue;
	custom_queue farther_path;
	custom_queue closer_path(to_from(false)); //If it is not safe use the less than comparison
	//cout << "Start_coors: " << start.x_coor << " " << start.y_coor << endl;
	//cout << "Target_coors:" << target.x_coor << " " << target.y_coor << endl;
	for(int i = -1; i < 2; i++) {
		for(int j = -1; j < 2; j++) {
			if((start.x_coor+i == target.x_coor) && (start.y_coor+j == target.y_coor)) { //target spotted, move there
				step = make_pair(target.x_coor,target.y_coor);
				//cout << "Evaluated: " << target.x_coor << " " << target.y_coor << endl;
				return step;
			} 
			else if(world[index(start.x_coor+i,start.y_coor+j)] == OPEN) { //if it hasn't been visited and the node is OPEN, visit it
				float distance_from_start = distance(start.x_coor+i,start.y_coor+j,start.x_coor,start.y_coor); //calculate the distance we are away from start
				float distance_to_target = distance(target.x_coor,target.y_coor,start.x_coor+i,start.y_coor+j);
				Node temp(start.x_coor+i,start.y_coor+j,(distance_from_start+distance_to_target));
				if(safe) closer_path.push(temp);
				else farther_path.push(temp);
			}	
		}
	}
	if(safe && closer_path.size() == 0) { //we're trapped trying to close distance
		return step;
	} 
	if(!safe && farther_path.size() == 0) { //we're trapped trying to run
		return step;
	} 
	if(closer_path.size() > 0) {
		step = make_pair(closer_path.top().x_coor,closer_path.top().y_coor);
	}
	else {
		step = make_pair(farther_path.top().x_coor,farther_path.top().y_coor);
	}
	return step;
}

class Herbivore {
	public:
		Herbivore () {}
		Herbivore (int new_x, int new_y) : x(new_x), y(new_y) {}
		int x = 0, y = 0; //Location
		int hunger = 0; //Increases from 0 to 100, +1 per frame. At 100 it dies
		int thirst = 0; //Ditto
		int breeding = 0; //Ditto
		int sleep = 0; //Ditto
		int fear = 0; //Fear should be equal to 100 if we have a predator one square away, falling off to 0 if there are no predators within 5 squares.
		bool sleeping = false;
		bool dead = false;

		void think() { //YOU: Write this AI Think function
			//Handle dead dinosaurs - they stick around for 10 rounds
			if (dead) { 
				if (hunger > 0) hunger = 0; //Hunger becomes our decay counter
				hunger -= 10; //It will get removed by the game code if hunger drops below -100
				return;
			}

			//Handle sleeping dinosaurs - they will do nothing until they wake up
			if (sleeping) {
				sleep -= 10;
				if (sleep < 0) {
					sleep = 0;
					sleeping = false;
					return;
				}
			}
			//Phase 1 - update input states
			hunger++;
			thirst++;
			breeding++;
			sleep++;
			fear = 0; //YOU: Calculate the fear level by searching for nearby predators
			int pred_x,pred_y; 
			for(int i = -5; i < 6; i++) {
				for(int j = -5; j < 6; j++) {
					int temp = fear;
					if(int(distance(x+i,y+j,x,y)) <= 5) {
						if(world[index(x+i,y+j)] == CARNIVORE) {
							if(temp < 100/distance(x+i,y+j,x,y)) fear = 100/distance(x+i,y+j,x,y);
							pred_x = x+i;
							pred_y = y+j;
						}
					}	
				}
			}
			//PHASE 2 - Kill it off if hunger, thirst, breeding, or sleep hit 100
			if (hunger >= 100 || thirst >= 100 || breeding >= 100 || sleep >= 100) {
				dead = true;
				return;
			}

			//PHASE 3 - Use a simple graph to calculate which actions we should take based on states
			//Hunger should encourage trying to find food to eat
			//Sleep should encourage us to sleep 
			//Fear should inhibit hunger a bit and sleeping a lot
			int control = 20;
			int should_run = fear;
			int should_eat = hunger - fear/8;
			int should_sleep = sleep - fear/2;
			int should_drink = thirst - fear/6;
			int should_breed = breeding - fear - control;
			//Etc. Do this for all the possible actions
			priority_queue<int, vector<int>, std::less<int> > actions;
			actions.push(should_eat);	
			actions.push(should_drink);	
			actions.push(should_run);	
			actions.push(should_breed);	
			actions.push(should_sleep);
			actions.push(control);
			//PHASE 4 - Based on which outcome has the highest activation, perform that action here
			//Possible actions are:
			// 1) go to sleep
			bool safe = true;
			if(actions.top() == should_eat && herbs.size()) {
				//Find closest herb that has a valid path, and move towards it.
				//Find closest herb
				float min = distance(herbs[0].x,herbs[0].y,x,y);
				int pos = 0;
				for(size_t i = 0; i < herbs.size(); i++) {
					if(min > distance(herbs[i].x,herbs[i].y,x,y)) {
						min = distance(herbs[i].x,herbs[i].y,x,y);
						pos = i;
					}
				}
				pair <int,int> next_step = seek(x,y,herbs[pos].x,herbs[pos].y,min,safe);
				if(next_step.first != x || next_step.second != y) {  //We found a valid position
					world[index(x,y)] = OPEN;
					x = next_step.first;
					y = next_step.second;
					world[index(x,y)] = HERBIVORE;
				}
				if(x == herbs[pos].x && y == herbs[pos].y) {
					hunger -= herbs[pos].food_value;
					herbs.erase(herbs.begin()+pos);
				}
			}
			else if(actions.top() == should_drink && water.size()){
				float min = distance(water.at(0).first,water.at(0).second,x,y);
				int pos = 0;
				for(size_t i = 0; i < water.size(); i++){
					if(min > distance(water.at(i).first,water.at(i).second,x,y)){
							min = distance(water.at(i).first,water.at(i).second,x,y);
							pos = i;
					}
				}
				pair <int,int> next_step = seek(x,y,water.at(pos).first,water.at(pos).second,min,safe);
				if((next_step.first != x || next_step.second != y) && (min >= 2.0)) {  //We found a valid position but not consume
					world[index(x,y)] = OPEN;
					x = next_step.first;
					y = next_step.second;
					world[index(x,y)] = HERBIVORE;
				}
				else if(min < 2.0) {
					thirst = 0;
				}
			} 
			else if(actions.top() == should_run){
				float min = distance(pred_x,pred_y,x,y);
				pair <int,int> next_step = seek(x,y,pred_x,pred_y,min,!safe);
				if(next_step.first != x || next_step.second != y) {  //We found a valid position
					world[index(x,y)] = OPEN;
					x = next_step.first;
					y = next_step.second;
					world[index(x,y)] = HERBIVORE;
				}
				if(next_step.first != x || next_step.second != y) { 
					world[index(x,y)] = OPEN;
					x = next_step.first;
					y = next_step.second;
					world[index(x,y)] = HERBIVORE;
				}
			}
			else if(actions.top() == should_sleep){
				sleeping = true;
			}
			else if((actions.top() == should_breed) && (herbivores.size() > 1)){
				float min = distance(herbivores.at(0).x,herbivores.at(0).y,x,y);
				int pos = 0;
				for(size_t i = 0; i < herbivores.size(); i++){
					if(min > distance(herbivores.at(i).x,herbivores.at(i).y,x,y)){
							min = distance(herbivores.at(i).x,herbivores.at(i).y,x,y);
							pos = i;
					}
				}
				pair <int,int> next_step = seek(x,y,herbivores.at(pos).x,herbivores.at(pos).y,min,safe);
				if((next_step.first != x || next_step.second != y) && (min >= 2.0)) {  //We found a valid position but not consume
					world[index(x,y)] = OPEN;
					x = next_step.first;
					y = next_step.second;
					world[index(x,y)] = HERBIVORE;
				}
				else if(min < 2.0) {
					for(int i = -1; i < 2; i ++){
						for(int j = -1; j < 2; j ++){
							if(world[index(x+i,y+j)] == OPEN){
								Herbivore temp(x+i,y+j);
								herbivores.push_back(temp);
								world[index(x+i,y+j)] = HERBIVORE;
							}
						}
					}
					breeding = 0;
				}
			}
			// 2) eat (if we are next to a herb, otherwise move to herb)
			//  When we eat an herb, hunger will be reduced by the food_value of the herb
			// 3) breed (if we are next to another herbivore, otherwise move to an herbivore)
			//   Breed should place a new herbivore on the map and set breeding to 0.
			// 4) drink (if we are next to water, or move toward water)
			//  Drinking water should reduce thirst by 50 (minimum 0)
			// 5) run away (if we are near a predaor, move away from it)
			// 6) If nothing else, move randomly (implemented for you here)
			else {
				int new_x = x + rand() % 3 - 1;
				int new_y = y + rand() % 3 - 1;
				if (world[index(new_x,new_y)] == OPEN) {
					world[index(new_x,new_y)] = HERBIVORE;
					world[index(x,y)] = OPEN;
					x = new_x;
					y = new_y;
				}
			}
		}
};


class Carnivore {
	public:
		Carnivore () {}
		Carnivore (int new_x, int new_y) : x(new_x), y(new_y) {}
		int x = 0, y = 0; //Location
		int hunger = 0; //Increases from 0 to 100, +1 per frame. At 100 it dies
		int thirst = 0; //Ditto
		int breeding = 0; //Ditto
		int sleep = 0; //Ditto
		bool sleeping = false;
		bool dead = false;

		void think() { //YOU: Write this AI Think function
			if (dead) return;

			//Handle sleeping dinosaurs - they will do nothing until they wake up
			if (sleeping) {
				sleep -= 10;
				if (sleep < 0) {
					sleep = 0;
					sleeping = false;
					return;
				}
			}

			//Phase 1 - update input states
			hunger++;
			thirst++;
			breeding++;
			sleep++;

			//PHASE 2 - Kill it off if hunger, thirst, breeding, or sleep hit 100
			if (hunger >= 100 || thirst >= 100 || breeding >= 100 || sleep >= 100) {
				dead = true;
				return;
			}

			//PHASE 3 - Use a simple graph to calculate which actions we should take based on states
			//Hunger should encourage trying to find food to eat
			//Sleep should encourage us to sleep 
			//Etc. Do this for all the possible actions
			//You should increase the weight for nearby targets, like a nearby herbivore would boost eat

			//PHASE 4 - Based on which outcome has the highest activation, perform that action here
			//Possible actions are:
			// 1) go to sleep
			// 2) eat (if we are next to a herbivore, otherwise move to herbivore)
			// 3) breed (if we are next to another carnivore, otherwise move to an carnivore)
			//   Breed should place a new carnivore on the map
			// 4) drink (if we are next to water, or move toward water)
			// 5) If nothing else, move randomly (implemented for you here)
			int control = 20;
			int should_eat = hunger;
			int should_sleep = sleep;
			int should_drink = thirst;
			int should_breed = breeding-control;
			//Etc. Do this for all the possible actions
			priority_queue<int, vector<int>, std::less<int> > actions;
			actions.push(control);
			actions.push(should_eat);	
			actions.push(should_drink);	
			actions.push(should_breed);	
			actions.push(should_sleep);
			//PHASE 4 - Based on which outcome has the highest activation, perform that action here
			//Possible actions are:
			// 1) go to sleep
			bool safe = true;
			if(actions.top() == should_eat && herbivores.size()) {
				//Find closest herb that has a valid path, and move towards it.
				//Find closest herb
				float min = distance(herbivores[0].x,herbivores[0].y,x,y);
				int pos = 0;
				for(size_t i = 0; i < herbivores.size(); i++) {
					if(min > distance(herbivores[i].x,herbivores[i].y,x,y)) {
						min = distance(herbivores[i].x,herbivores[i].y,x,y);
						pos = i;
					}
				}
				pair <int,int> next_step = seek(x,y,herbivores[pos].x,herbivores[pos].y,min,safe);
				if(next_step.first != x || next_step.second != y) {  //We found a valid position
					world[index(x,y)] = OPEN;
					x = next_step.first;
					y = next_step.second;
					world[index(x,y)] = CARNIVORE;
				}
				if(x == herbivores[pos].x && y == herbivores[pos].y) {
					hunger = 0;
					herbivores.erase(herbivores.begin()+pos);
				}
			}
			else if(actions.top() == should_drink && water.size()){
				float min = distance(water.at(0).first,water.at(0).second,x,y);
				int pos = 0;
				for(size_t i = 0; i < water.size(); i++){
					if(min > distance(water.at(i).first,water.at(i).second,x,y)){
							min = distance(water.at(i).first,water.at(i).second,x,y);
							pos = i;
					}
				}
				pair <int,int> next_step = seek(x,y,water.at(pos).first,water.at(pos).second,min,safe);
				if((next_step.first != x || next_step.second != y) && (min >= 2.0)) {  //We found a valid position but not consume
					world[index(x,y)] = OPEN;
					x = next_step.first;
					y = next_step.second;
					world[index(x,y)] = CARNIVORE;
				}
				else if(min < 2.0) {
					thirst = 0;
				}
			} 
			else if(actions.top() == should_sleep){
				sleeping = true;
			}
			else if((actions.top() == should_breed) && (carnivores.size() > 1)){
				float min = distance(carnivores.at(0).x,carnivores.at(0).y,x,y);
				int pos = 0;
				for(size_t i = 0; i < carnivores.size(); i++){
					if(min > distance(carnivores.at(i).x,carnivores.at(i).y,x,y)){
							min = distance(carnivores.at(i).x,carnivores.at(i).y,x,y);
							pos = i;
					}
				}
				pair <int,int> next_step = seek(x,y,carnivores.at(pos).x,carnivores.at(pos).y,min,safe);
				if((next_step.first != x || next_step.second != y) && (min >= 2.0)) {  //We found a valid position but not consume
					world[index(x,y)] = OPEN;
					x = next_step.first;
					y = next_step.second;
					world[index(x,y)] = CARNIVORE;
				}
				else if(min < 2.0) {
					for(int i = -1; i < 2; i ++){
						for(int j = -1; j < 2; j ++){
							if(world[index(x+i,y+j)] == OPEN){
								Carnivore temp(x+i,y+j);
								carnivores.push_back(temp);
								world[index(x+i,y+j)] = CARNIVORE;
							}
						}
					}
					breeding = 0;
				}
			}
			int new_x = x + rand() % 3 - 1;
			int new_y = y + rand() % 3 - 1;
			if (world[index(new_x,new_y)] == OPEN) {
				world[index(new_x,new_y)] = CARNIVORE;
				world[index(x,y)] = OPEN;
				x = new_x;
				y = new_y;
			}
		}
};

//Builds an empty world with walls on the edges and open squares in the middle
//Randomly populate it with herbs, herbivores, and carnivores
const int HERB_PERCENTAGE = 10;
const int HERBIVORE_PERCENTAGE = 5;
const int CARNIVORE_PERCENTAGE = 2;
void reset_world() {
	for (int i = 0; i < SIZE_X; i++) {
		for (int j = 0; j < SIZE_Y; j++) {
			if (i == 0 || j == 0 || i == SIZE_X - 1 || j == SIZE_Y - 1) //Is edge
				world[index(i,j)] = WALL;
			else
				world[index(i,j)] = OPEN;
			//Put a lake in the center
			if ( (i == SIZE_X / 2 || i == SIZE_X/2 - 1 || i == SIZE_X/2 + 1) &&
					(j == SIZE_Y / 2 || j == SIZE_Y/2 - 1 || j == SIZE_Y/2 + 1) )
				world[index(i,j)] = WATER;

			//Now populate it with stuff
			if (world[index(i,j)] == OPEN && rand() % 100 < HERB_PERCENTAGE) {
				world[index(i,j)] = HERB;
				herbs.push_back(Herb(i,j));
			}
			else if (world[index(i,j)] == OPEN && rand() % 100 < HERBIVORE_PERCENTAGE) {
				world[index(i,j)] = HERBIVORE;
				herbivores.push_back(Herbivore(i,j));
			}
			else if (world[index(i,j)] == OPEN && rand() % 100 < CARNIVORE_PERCENTAGE) {
				world[index(i,j)] = CARNIVORE;
				carnivores.push_back(Carnivore(i,j));
			}
		}
	}
}

//Prints the entire world, bolding the square the cursor is on
void print_world() {
	for (int i = 0; i < SIZE_X; i++) {
		for (int j = 0; j < SIZE_Y; j++) {
			if (i == cursor_x && j == cursor_y)
				attron(A_UNDERLINE | A_BOLD);
			int color = 1; 
			if (world[index(i,j)] == WALL)
				color = 5;
			else if (world[index(i,j)] == WATER)
				color = 2;
			else if (world[index(i,j)] == HERB)
				color = 3;
			else if (world[index(i,j)] == HERBIVORE)
				color = 4;
			else if (world[index(i,j)] == CARNIVORE)
				color = 6;
			attron(COLOR_PAIR(color));
			mvaddch(i,j,world[index(i,j)]);
			attroff(COLOR_PAIR(color));
			attroff(A_UNDERLINE | A_BOLD);
		}
	}
}

int main()
{
	//Initialize random number generator
	srand(time(NULL));

	cout << "How big a world do you want? (Enter x size followed by y then hit return): ";
	cin >> SIZE_Y >> SIZE_X; //Everything is backwards (x and y) in NCURSES
	if (!cin || SIZE_X < MIN_SIZE || SIZE_Y < MIN_SIZE) {
		cout << "Error: world size must be at least " << MIN_SIZE << " in each dimension.\n";
		return 1;
	}

	//Set up NCURSES
	initscr();//Start curses mode
	start_color(); //Enable Colors if possible
	init_pair(1,COLOR_WHITE,COLOR_BLACK); //Set up some color pairs
	init_pair(2,COLOR_CYAN,COLOR_BLACK);
	init_pair(3,COLOR_GREEN,COLOR_BLACK);
	init_pair(4,COLOR_YELLOW,COLOR_BLACK);
	init_pair(5,COLOR_RED,COLOR_BLACK);
	init_pair(6,COLOR_MAGENTA,COLOR_BLACK);
	clear();
	noecho();
	cbreak();
	timeout(TIMEOUT); //Set a max delay for key entry

	//Splash Screen
	splash();
	timeout(TIMEOUT); //Set a max delay for key entry
	clear();

	world = new unsigned char[SIZE_X * SIZE_Y];
	reset_world();

	//MAIN LOOP
	int frame = 0;
	cursor_x = SIZE_X/2, cursor_y = SIZE_Y/2; //Cursor for drawing stuff
	game_on = false;
	while (true) {
		if (!game_on) 
			mvprintw(SIZE_X,0,"Design Mode - Game Paused                                        ");
		else
			mvprintw(SIZE_X,0,"Game Running. Herbs: %i Herbivores: %i Carnivores: %i Frame: %i",herbs.size(), herbivores.size(), carnivores.size(), frame++);
		mvprintw(SIZE_X+1,0,"Use arrow keys to move the cursor around");
		mvprintw(SIZE_X+2,0,"Type Space to clear the square or set a wall");
		mvprintw(SIZE_X+3,0,"Type ~ to place a water");
		mvprintw(SIZE_X+4,0,"Type ! to place an Herb");
		mvprintw(SIZE_X+5,0,"Type H to place an Herbivore");
		mvprintw(SIZE_X+6,0,"Type R to place a Carnivore");
		mvprintw(SIZE_X+7,0,"Type Enter to run or pause the game");
		mvprintw(SIZE_X+8,0,"Type Q to quit");
		mvprintw(SIZE_X+9,0,"Type c to clear");
		int ch = getch(); // Wait for user input, with TIMEOUT delay
		if (ch == 'q' || ch == 'Q') break;
		else if (ch == 'c') {
			for(int i = 0; i < SIZE_X*SIZE_Y; i++) {
				if(world[i] != WALL) {
					world[i] = OPEN;
				}
			}
			herbs.erase(herbs.begin(),herbs.end());
			carnivores.erase(carnivores.begin(),carnivores.end());
			herbivores.erase(herbivores.begin(),herbivores.end());
			water.erase(water.begin(),water.end());
		}
		else if (ch == ' ' || ch == '#') {
			if (world[index(cursor_x,cursor_y)] == OPEN)
				world[index(cursor_x,cursor_y)] = WALL;
			else if (world[index(cursor_x,cursor_y)] == WALL)
				world[index(cursor_x,cursor_y)] = OPEN;
			//TODO: Be able to clear other things from the world
		}
		else if (ch == '!' || ch == '1') {
			if (world[index(cursor_x,cursor_y)] == OPEN) {
				world[index(cursor_x,cursor_y)] = HERB;
				herbs.push_back(Herb(cursor_x,cursor_y));
			}
		}
		else if (ch == 'h' || ch == 'H') {
			if (world[index(cursor_x,cursor_y)] == OPEN) {
				world[index(cursor_x,cursor_y)] = HERBIVORE;
				herbivores.push_back(Herbivore(cursor_x,cursor_y));
			}
		}
		else if (ch == 'r' || ch == 'R') {
			if (world[index(cursor_x,cursor_y)] == OPEN) {
				world[index(cursor_x,cursor_y)] = CARNIVORE;
				carnivores.push_back(Carnivore(cursor_x,cursor_y));
			}
		}
		else if (ch == '`' || ch == '~') {
			if (world[index(cursor_x,cursor_y)] == OPEN) {
				world[index(cursor_x,cursor_y)] = WATER;
				water.push_back(make_pair(cursor_x,cursor_y));
			}
		}
		//Handle arrow keys, the NCURSES #defs don't work here for some reason
		//KEY_UP, for example, is set to 0403 (259), not 65
		else if (ch == RIGHT) { 
			cursor_y++;
			if (cursor_y >= SIZE_Y) cursor_y = SIZE_Y - 1; //Clamp value
		}
		else if (ch == LEFT) {
			cursor_y--;
			if (cursor_y < 0) cursor_y = 0; 
		}
		else if (ch == UP) { //Should be KEY_UP, grr
			cursor_x--;
			if (cursor_x < 0) cursor_x = 0;
		}
		else if (ch == DOWN) {
			cursor_x++;
			if (cursor_x >= SIZE_X) cursor_x = SIZE_X - 1; //Clamp value
		}
		else if (ch == ERR) { //No keystroke
			; //Do nothing
		} 
		else if (ch == '\n') {
			game_on = !game_on; //Pause or unpause the game
		}
		//else
		//mvprintw(SIZE_Y+5,0,"Unknown character: %i\n",ch);

		//Run the AI
		if (game_on) {
			for (Herb &h : herbs) {
				h.think();
			}
			for (Herbivore &h : herbivores) {
				h.think();
			}
			for (Carnivore &c : carnivores) {
				c.think();
			}
			//Clean up at most one corpse per frame, since the iterator goes invalid after erase
			//TODO: Fix this so it cleans up all corpses
			auto iter = herbivores.begin();
			while (iter != herbivores.end()) {
				if (iter->dead && iter->hunger <= -100) {
					world[index(iter->x,iter->y)] = OPEN;
					herbivores.erase(iter);
					break;
				}
				iter++;
			}
			auto iter2 = carnivores.begin();
			while (iter2 != carnivores.end()) {
				if (iter2->dead) {
					world[index(iter2->x,iter2->y)] = OPEN;
					carnivores.erase(iter2);
					break;
				}
				iter2++;
			}
		}

		//Redraw the screen
		clear();
		print_world();
		refresh(); 
	}
	clear();
	print_world();
	refresh();
	wait_ticks(300000);
	endwin(); // End curses mode
	system("clear");

	return 0;
}
