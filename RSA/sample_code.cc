#include <iostream>
#include <cstdlib>
#include <cmath>
#include <boost/multiprecision/gmp.hpp>
using namespace std;
using namespace boost::multiprecision;

//////////////////////////////////////
//To build this code type: make sample
//To run this code type: sample
//////////////////////////////////////

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

int main()
{
	//Example of how to do modular exponentiation
	mpz_int b = 5;
	mpz_int p = 2;
	mpz_int m = 10;
	mpz_int r = powm(b,p,m); //r = b^p % m
	cout << "5^2 % 10 = " << r << endl;

	//Example of how to calculate a modular inverse:
	cout << "3 x ?? = 1 mod 7: " << calculate_inverse(7,3) << endl; //Returns 5

	//Example of how to encode and decode strings to integers
	string test;
	cout << "Please enter a string to encode: ";
	getline(cin,test);
	mpz_int foo = encode(test);
	cout << "Encoded, it is: " << foo << endl;
	cout << "Decoded, it is: " << decode(foo) << endl;
	return 0;
}
