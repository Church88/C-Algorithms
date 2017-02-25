#include <iostream>
#include <boost/multiprecision/gmp.hpp>
using namespace std;
using namespace boost::multiprecision;

bool prime_test(mpz_int& x) {
	//YOU: Write your code to test for a prime here
	if(x<=3)return true;
	mpz_int end = sqrt(x);
	for(mpz_int i = 5; i < end; i+=2){
		if(x==i) return true;
		if(x % i == 0) return false;
	}
	return true;
}

int main() {
	mpz_int x;
	cout << "Please enter an integer: ";
	cin >> x;
	cout << "The number: " << x << "... ";
	if (prime_test(x)) cout << "is a prime.\n";
	else cout << "is not a prime.\n";
}
