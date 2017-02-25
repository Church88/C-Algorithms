#include <iostream>
#include <cstdlib>
#include <cmath>
#include <gmpxx.h>
#include <boost/multiprecision/gmp.hpp>
using namespace std;
using namespace boost::multiprecision;

//This function takes a string and returns it as a really really big integer
//For example, "Hi" might convert to 18537
mpz_int encode(string in) {
	mpz_int temp = 0;
	for (unsigned char ch : in) {
		temp *= 256;
		temp += ch;
	}
	return temp;
}

//This function takes a really really big integer and returns it as a string
//For example "18537" might turn into "Hi"
string decode(mpz_int in) {
	string temp;
	while (in > 0) {
		mpz_int r;
		mpz_int divisor = 256;
		divide_qr(in,divisor,in,r); //in /= 256, ch is the remainder
		unsigned char ch = (unsigned char)r;
		temp += ch;
	}
	reverse(temp.begin(),temp.end());
	return temp;
}

//Return the multiplicative inverse of e in the world of mod t
//Credit - http://stackoverflow.com/questions/12826114/euclids-extended-algorithm-c
mpz_int calculate_inverse(mpz_int t, mpz_int e) {
	mpz_int a,b,q,x,lastx,y,lasty,temp,temp1,temp2,temp3;
	a = t;
	b = e;
	if (b>a) {//we switch them
		swap(b,a);
	}

	x=0;
	y=1;
	lastx=1;
	lasty=0;
	while (b!=0) {
		q= a/b;
		temp1= a%b;
		a=b;
		b=temp1;

		temp2=x;
		x=lastx-q*x;
		lastx=temp2;

		temp3=y;
		y=lasty-q*y;
		lasty=temp3;
	}

	while (lasty < 0) lasty += t;
	return lasty;
}

//A wrapper around the GMP library calls to set P and Q to large prime numbers
//Based on the seed, this generation is deterministic, so that we can autograde
void generate_primes(mpz_class my_seed, mpz_int &P, mpz_int &Q) {
	gmp_randclass RNG(gmp_randinit_mt);
	RNG.seed(my_seed);

	mpz_class A = RNG.get_z_bits(512); //Get 512 random bits
	mpz_class B = RNG.get_z_bits(512); 
	mpz_nextprime(A.get_mpz_t(),A.get_mpz_t()); //This searches for primes
	mpz_nextprime(B.get_mpz_t(),B.get_mpz_t());
	mpz_int foo(A.get_mpz_t()); //Awkwardly convert from mpz_class to mpz_int
	mpz_int bar(B.get_mpz_t());
	P = foo; //Copy into our out parameters
	Q = bar;
}


#define GENERATE 0
#define ENCRYPT 1
#define DECRYPT 2
int main() {
	cout << "Do you want to (0) generate private/public keys, (1) encrypt a message or (2) decrypt a message:\n";
	int choice;
	cin >> choice;
	if (choice == GENERATE) {
		mpz_int P,Q,E,D,N,T;
		cout << "Please input a random seed: ";
		mpz_class my_seed = 0;
		cin >> my_seed;
		generate_primes(my_seed,P,Q);
		//cout << "P:\n" << P << "\nQ:\n" << Q << endl;

		//YOU: Given the primes P and Q, compute N, T, E, D.
		N = P * Q;
		T = (P-1)*(Q-1);
		E = 65537;
		D = calculate_inverse(T,E);
		//Now we output everything
		cout << "Public Key (E and N):\n";
		cout << "E:\n";
		cout << E << endl;
		cout << "N:\n";
		cout << N << endl;
		cout << "Private Key (D and N):\n";
		cout << "D:\n";
		cout << D << endl;
		cout << "N:\n";
		cout << N << endl;
	}
	else if (choice == ENCRYPT) {
		mpz_int M,E,N,S;
		cout << "Please enter E (the public key):\n";
		cin >> E;
		cout << "Please enter N (P * Q):\n";
		cin >> N;
		cout << "Please enter M (the message to encrypt):\n";
		while (cin.peek() == '\n') cin.get(); //Discard whitespace
		string input;
		getline(cin,input);

		//YOU: Convert a string to a big integer
		M = encode(input);

		//YOU: Encrypt M, using the formula S = M^E mod N
		S = powm(M,E,N);


		cout << "The secret message S is:\n";
		cout << S << endl;
	}
	else if (choice == DECRYPT) {
		mpz_int D,N,S,M;
		string output;
		cout << "Please enter D (your private key):\n";
		cin >> D;
		cout << "Please enter N (P * Q):\n";
		cin >> N;
		cout << "Please enter S (the encrypted message):\n";
		cin >> S;

		//YOU: Decrypt S to get M back, using the formula M = S^D mod N
		M = powm(S,D,N);

		//YOU: Decode the integer M back into a string called output
		output = decode(M); 

		cout << "The original message M is:\n";
		cout << output << endl;
	}
	else {
		cerr << "BAD INPUT!\n";
		exit(1);
	}
	return 0;
}
