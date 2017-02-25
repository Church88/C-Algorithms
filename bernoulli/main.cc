#include <iostream>
#include <cstdlib>
#include <cmath>
#include <gmpxx.h>
#include <iomanip>
using namespace std;

/* Bernoulli Trials Simulator */

//You can change these for debugging, but reset them before the deadline
#define MAX_LUCK 10
#define TRIALS 100
#define FLIPS 40
#define PERCENT_PER_STAR 1

//Some sample code on how to flip a biased coin. You MUST use this code, and not the normal rand() function
bool coin_flip (mpz_class bias, gmp_randclass &RNG) {
	mpz_class roll = RNG.get_z_range (MAX_LUCK); //Generates a number from 0 to 9
	if (roll < bias) return true;
	return false;
}

mpz_class fac(mpz_class x){
	mpz_class temp;
	if(x <= 1) return 1;
	temp = x * fac(x-1);
	return temp;
}

mpz_class nCk (unsigned n, unsigned k) {//n!/(k!(n-k)!
	mpz_class a,b,c;
	a = fac(n);
	b = fac(k);
	c = fac(n-k);
	return (a/(b*c));
}

int main() {
	//Remember not to modify these prompts, or you'll break the autograder and get no points
	cout << "Please input a random seed: ";
	mpz_class my_seed;
	cin >> my_seed;
	gmp_randclass RNG (gmp_randinit_mt);
	RNG.seed (my_seed);

	//Your luck score controls how likely you are to get a heads on a flip
	//At 0, you have a 0% chance, at 10 you have a 100% chance.
	//By default, it should be 5 for a 50/50 chance.
	//(This is called a "bias" on a coin flip)
	cout << "Please enter your luck score (from 0 to " << MAX_LUCK << "): ";
	mpz_class luck;
	cin >> luck;
	if (luck < 0 || luck > MAX_LUCK) {
		cout << "Bad input.\n";
		exit (1);
	}

	//YOU: Now, you're going to flip a coin FLIPS (default: 40) times, and count how many heads you get
	//You will do this whole process TRIALS (default: 100) times
	int bins[FLIPS + 1] = {}; //Holds how many times we get each result, initialized to zero
	//For example, after TRIALS times, bins[20] holds how many times we got exactly 20 heads out of 40 flips


	//Here's how you flip a coin.
	//You'll need to put it inside a loop or two to make it work with LOOPS and TRIALS
	for (int i = 0; i < TRIALS; i ++) {
		int heads = 0;
		for (int j = 0; j < FLIPS; j++) {
			bool success = coin_flip (luck, RNG); //The higher your luck, the higher the chance of success
			if (success) heads++;
		}
		bins[heads]+=1;
	}
	//YOU: After you have done all 100 coin flips, you will print your results
	//The format is as such:
	// bin number: expected percent vs actual percent: stars equal to the actual percent
	//Example:
	// ...
	// 19: 11% vs 10%: **********
	// 20: 12% vs 15%: ***************
	// 21: 11% vs 13%: *************
	// ...
	for (unsigned long int n = 0; n <= FLIPS; n++) {
		mpz_class nck = nCk(FLIPS,n);
		mpf_class x = 0;
		mpf_class y = 0;
		mpf_class lucky = luck;
		lucky = lucky * .1;
		mpf_class lesslucky = 1-lucky;
		unsigned long int nsubk = (FLIPS-n);
		mpf_pow_ui(y.get_mpf_t(),lesslucky.get_mpf_t(),nsubk);
		mpf_pow_ui(x.get_mpf_t(),lucky.get_mpf_t(),n);
		mpz_class expect;
		expect = 100 * (nck*x*y);
		cout << "Bin " << setw(2) << setfill('0') << n << ": "  << setw(2) << setfill('0') << expect << "% vs " << setw(2) << setfill('0') << bins[n] << "%: ";
		for (int i = 0; i < bins[n]; i++) cout << "*";
		cout << endl;
	}
	//Explanation:
	// The bin number is how many heads came up, which will range from 0 to FLIPS
	// The expected percent is the expected probability, given by (n choose k) * p^k * (1-p)^(n-k)
	// The actual percent is how many actually got rolled (which will rarely match the expected number)
	//

	return 0;
}
