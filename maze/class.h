#pragma once
#include <iostream>
#include <queue>
#include <utility>
#include <deque>
#include <cmath>
using namespace std;


class Source{
	public:
	pair<int,int> origin = {};
	priority_queue<float> paths;
	deque<pair<int,int>> fucket;
	Source(){}
	Source(pair<int,int> new_origin){
		origin = new_origin;
	}
	void search (char board[][SIZE]){
		//Identify all unvisisted nodes
		int x = origin.first;
		int y = origin.second;
		pair<int,int> temp;
		for(int i = -1; i <=1; i ++){
			for(int j = -1; j <= 1; j++){
				if(spot_check(board[x+i][y+j]){
													
				}
					
			}
		}
		system ("clear");
		print_board (board);
	}
	pair get_origin() return origin;
};

class Node{
	public:
	pair<int,int> point = {}
	float dist = 0;
	Node(){}
	Node(int x,int y,int d){
		point.first = x;
		point.second = y;
		int x_2 = origin.first;
		sqrt(pow(x-x_2,2) + pow(y-origin.second,2));
	}
};
