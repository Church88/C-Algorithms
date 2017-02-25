#include <iostream>
#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/cpp_int.hpp>
using namespace std;
using namespace boost::multiprecision;

int main() {
	//Try switching mpz_int and cpp_int and see which is faster
	mpz_int x;
	//cpp_int x;
	x = 3;
	for (int i = 0; i < 4; i++) 
		x = pow(x,100);
	cout << "X = " << x << endl;
	if (x % 2 == 0) cout << "X is even!\n";
	else cout << "X is odd!\n";
}
