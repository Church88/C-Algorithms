#define cimg_display 0
#include "CImg.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <cassert>
using namespace cimg_library;
using namespace std;

//These are helper functions that will return the red, green, or blue value at
// any given location, or allow you to set the value there as well.
//We're accessing a 1D array as if it was a 3D array
#define WIDTH 1000
#define HEIGHT 500
#define STRIDE 500000 //1000*500
#define R(i,j) (j*WIDTH+i)
#define G(i,j) (STRIDE+j*WIDTH+i)
#define B(i,j) (STRIDE+STRIDE+j*WIDTH+i)
void clear_image(unsigned char out[]) {
	//Note: i is the x coordinate, j is the y coordinate
	for (int i = 0; i < WIDTH; i++) { 
		for (int j = 0; j < HEIGHT; j++) { //At each pixel...
			out[R (i, j)] /= 2; //Site White
			out[G (i, j)] /= 2; //Site White
			out[B (i, j)] /= 2; //Site White
		}
	}
}

class Rectangle {
	public:
		int x_min = 0;
		int x_max = 0;
		int y_min = 0;
		int y_max = 0;
		Rectangle(int new_x_min, int new_x_max, int new_y_min, int new_y_max) : x_min(new_x_min), x_max(new_x_max), y_min(new_y_min), y_max(new_y_max) {
			assert(x_min >= 0);
			assert(x_min < WIDTH);
			assert(x_max >= 0);
			assert(x_max < WIDTH);
			assert(y_min >= 0);
			assert(y_min < HEIGHT);
			assert(y_max >= 0);
			assert(y_max < HEIGHT);
			assert (x_min <= x_max);
			assert (y_min <= y_max);
		}
};

void draw_rect(unsigned char out[], Rectangle rekt) {
	for (int i = rekt.x_min; i <= rekt.x_max; i++) { 
		for (int j = rekt.y_min; j <= rekt.y_max; j++) { //At each pixel...
			out[R (i, j)] = 148; //Moonlit Orchid
			out[G (i, j)] = 145; //Moonlit Orchid
			out[B (i, j)] = 148; //Moonlit Orchid
		}
	}
}

//Rewrite this function five different ways and graph the output of each
//Compare it with the predicted output of the master theorem (M.T.)
//You are to vary:
// A) The number of times f calls itself (i.e. 'A' in the M.T.)
// B) The fraction of the vector in each call to f (i.e. 'B' in the M.T.)
// C) The exponent of the running time of the work done at each step ('C')
//If you don't mix it up enough, I will be miffed. And you will lose a grade. Or two.
//WHAT TO TURN IN:
// 1) Five recursion relations (email them to me in like a Word doc or something)
// 2) Email me the animated gifs for them (embed them in the Word doc)
// 3) If anything weird happened, try to explain yourself here also
vector<unsigned int> fib_mem(4000001);
unsigned int fib(unsigned int x){
	fib_mem.at(0) = 0;
	fib_mem.at(1) = 1;
	fib_mem.at(2) = 1;
	if(fib_mem.at(x) == 0) fib_mem.at(x) = (fib(x-1) + fib(x-2));
	return fib_mem.at(x);
}
template <class T> 
void a(T start, T finish) { //By quarters *******FIB
	size_t size = distance(start,finish);
	if (size <= 4) return; //Base condition
	T mid = start+(size/2);
	a(start, start+(size/4)); 
	a(start+(size/4)+1, mid);
	a(mid+1, mid+(size/4));
	a(mid+(size/4)+1, finish);
	unsigned int y = fib(size); //Fib number
	//cout << y << endl;
}
float cos(int n, float x){
	if(n<2) return n;
	float c = x*x/(2*(n-1)*2*n);
	return (1-c)*cos(n-1,x)+c*cos(n-2,x);
}
template <class R>
void b(R start, R finish) { //By thirds ********TAYLOR SERIES
	size_t size = distance(start,finish);
	if (size <= 3) return; //Base condition
	R third = start+size/3;
	b(start, third);
	b(third+1, third+(size/3)); 
	b(third+(size/3)+1, finish);
	//Calculate cos(n,x) via taylor expansion	
	float key = 33.5;
	float junk = cos(size,key);
}
vector<unsigned int> primes (int n){
	vector<bool> poss(n+1,true);
	poss.at(0) = poss.at(1) = false;
	int end = (int)sqrt(n);//type cast as int cause sqrt gives a float
	int count;
	for(int i = 2; i <= end+1; i++){
		count=i;
		while(count*i <= n){
			poss.at(count*i) = false;
			count++;
		}
	}
	vector<unsigned int> primes;
	for(int i = 0; i < poss.size(); i ++){
		if(poss.at(i)) primes.push_back((unsigned int)i);
	}
	return primes;
}

template <class Q>
void c(Q start, Q finish) { //By halves ********PRIME NUMBER
	size_t size = distance(start,finish);
	if (size <= 100) return; //Base condition
	Q mid = start+size/2;
	c(start, mid); 
	c(mid, finish);
	unsigned int reload = size%10000;
	if(reload == 0) reload = 10;
	vector<unsigned int> trash = primes(reload); //O(N) merge
}
template <class P>
void d(P start, P finish) { //
	size_t size = distance(start,finish);
	if (size <= 1) return; //Base condition
	P mid = start+size/2;
	d(start, mid); 
	d(mid, finish);
	random_shuffle(start,finish); //O(N) merge
}
template <class O>
void e(O start, O finish) { //
	size_t size = distance(start,finish);
	if (size <= 1) return; //Base condition
	O mid = start+size/2;
	e(start, mid); 
	e(mid, finish);
	inplace_merge(start,mid,finish); //O(N) merge
}
class Results {
	public:
		int size = 0;
		double time = 0;
		bool operator<(const Results &rhs) const {
			return time < rhs.time;
		}
};

int main () {
	CImgList<unsigned char> wallhaven(0,WIDTH, HEIGHT, 1, 3);
	CImg<unsigned char> image (WIDTH, HEIGHT, 1, 3, 0);
	//CImg<unsigned char> image2 ("harambe.jpg");
	//cout << image2.height() << " " << image2.width() << endl;
	//clear_image(image);

	//A vector to hold our timing results
	vector<Results> rec;
	const int BEGIN = 10000;
	const int END = 4000000;
	const int STEP = 10000;
	for (int i = BEGIN; i <= END; i += STEP) {
		vector<int> vec(i);
		generate(vec.begin(), vec.end(), rand); //Fill it with random numbers
		clock_t start_time = clock();
		a(vec.begin(), vec.end()); //Mergesort
		clock_t end_time = clock();
		double time =  double(end_time-start_time) / CLOCKS_PER_SEC;
		cout << "Size = " << i << "\nTime = " << time << endl; 
		rec.push_back({i,time}); //Save our results
	}

	if (rec.size() < 1) exit(1);
	int w = WIDTH / rec.size(); //Width per bar in the bar chart
	//Grab the longest running time in the records
	auto max = max_element(rec.begin(),rec.end())->time;

	//Now graph results
	//YOU: +1 grade for making this pretty and/or funny (max 110%)
	for (int i = 0; i < rec.size(); i++) {
		int x_min = w*i; //Left edge
		int x_max = w*(i+1)-1; //Right edge
		int h = (rec[i].time / max) * image.height(); //Ratio of our time vs. max
		if (h < 1) h = 1;
		int y_min = image.height() - h; //Upper edge
		int y_max = image.height() - 1; //Lower edge
		draw_rect(image,{x_min,x_max,y_min,y_max});
		wallhaven.push_back(image);
	}

	wallhaven.save_gif_external("output.gif", 60, 1); //Animated gif at 25 fps & INFINITE LOOOPS
	//harambe.save_gif_external("output.gif", 60, 0); //Animated gif at 25 fps & INFINITE LOOOPS
}

