#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <gmpxx.h>
using namespace std;
 
/* Bernoulli Trials Simulator */
 
//You can change these for debugging, but reset them before the deadline
#define MAX_LUCK 10
#define TRIALS 100
#define FLIPS 40
#define PERCENT_PER_STAR 1
 
//Some sample code on how to flip a biased coin. You MUST use this code, and not the normal rand() function
bool coin_flip(mpz_class bias, gmp_randclass &RNG) {
    mpz_class roll = RNG.get_z_range(MAX_LUCK); //Generates a number from 0 to 9
    if (roll < bias) return true;
    return false;
}
 
mpz_class fact(mpz_class x) {
    mpz_class temp;
    if(x <= 1) return 1;
    temp = x * fact(x-1);
    return temp;
}
 
mpz_class nChoosek( unsigned n, unsigned k ) {
//fix this  
    mpz_class n_fact = fact(n);
    mpz_class k_fact = fact(k);
    mpz_class nsubk_fact = fact(n-k);
    mpz_class result = (n_fact/(k_fact*nsubk_fact));
    return result;
}
 
string asterik(int count) {
    string retval = "";
    for(int z = 0; z < count; z++) {
        retval += "*";
    }
    return retval;
}
 
int main() {
    //Remember not to modify these prompts, or you'll break the autograder and get no points
    cout << "Please input a random seed: ";
    mpz_class my_seed;
    cin >> my_seed;
    gmp_randclass RNG(gmp_randinit_mt);
    RNG.seed(my_seed);
 
    //Your luck score controls how likely you are to get a heads on a flip
    //At 0, you have a 0% chance, at 10 you have a 100% chance.
    //By default, it should be 5 for a 50/50 chance.
    //(This is called a "bias" on a coin flip)
    cout << "Please enter your luck score (from 0 to " << MAX_LUCK << "): ";
    mpz_class luck;
    cin >> luck;
    if (luck < 0 || luck > MAX_LUCK) {
        cout << "Bad input.\n";
        exit(1);
    }
 
    //YOU: Now, you're going to flip a coin FLIPS (default: 40) times, and count how many heads you get
    //You will do this whole process TRIALS (default: 100) times
    int bins[FLIPS+1] = {}; //Holds how many times we get each result, initialized to zero
    //For example, after TRIALS times, bins[20] holds how many times we got exactly 20 heads out of 40 flips
 
 
    //Here's how you flip a coin.
    //You'll need to put it inside a loop or two to make it work with LOOPS and TRIALS
    //The higher your luck, the higher the chance of success
    for(int i = 0; i < TRIALS; i++) {
        int count = 0;
        for(int j = 0; j < FLIPS; j++) {
            bool success = coin_flip(luck, RNG);
            if(success) count++;
        }
        bins[count] += 1;
    }
 
    //YOU: After you have done all 100 coin flips, you will print your results
    //The format is as such:
    for(unsigned long int bin = 0; bin <= FLIPS; bin++){
        mpz_class nChk = nChoosek(FLIPS,bin);
        mpf_class p = luck;
        p = p * .1;
        mpf_class oneSubp = 1-p;
        unsigned long int nSubk = (FLIPS-bin);
        mpf_class reg_1 = 0;
        mpf_pow_ui(reg_1.get_mpf_t(), oneSubp.get_mpf_t(), nSubk);
        mpf_class reg_2 = 0;
        mpf_pow_ui(reg_2.get_mpf_t(), p.get_mpf_t(), bin);
        mpz_class total;
        total = 100 * (nChk * reg_1 * reg_2);
        cout << "Bin " << setw(2) << setfill('0') << bin << ": " << setw(2) << setfill('0') << total << "%" << " vs " << setw(2) << setfill('0') << bins[bin] << "%: " << asterik(bins[bin]) << endl;
    }
    // bin number: expected percent vs actual percent: stars equal to the actual percent
    //Example:
    // ...
    // 19: 11% vs 10%: **********
    // 20: 12% vs 15%: ***************
    // 21: 11% vs 13%: *************
    // ...
    //Explanation:
    // The bin number is how many heads came up, which will range from 0 to FLIPS
    // The expected percent is the expected probability, given by (n choose k) * p^k * (1-p)^(n-k)
    // The actual percent is how many actually got rolled (which will rarely match the expected number)
    return 0;
}
