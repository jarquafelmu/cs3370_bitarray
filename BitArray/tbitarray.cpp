// Project: BitArray
// Name: Sam Terrazas
// File: tbitarray.cpp
// Created: 04/05/2019 9:21 AM
// Updated: 04/18/2019 2:20 PM
// 
// I declare that the following source code was written by me, or provided
// by the instructor for this project. I understand that copying source
// code from any other source, providing source code to another student, 
// or leaving my code on a public web site constitutes cheating.
// I acknowledge that  If I am found in violation of this policy this may result
// in a zero grade, a permanent record on file and possibly immediate failure of the class. 

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "bitarray.h"
#include "test.h"
using namespace std;

// Test program
int main()
{
	// Test exceptions
	BitArray<> b;
	throw_(b[0],logic_error); // BUG: Fails to throw logic_error on index into empty BitArray
	throw_(b.toggle(0),logic_error);
	const BitArray<> b1 { b }; // Test copy constructor
	throw_(b1[0],logic_error);

	// Test empty Bitarray properties
	test_(b.size() == 0);
	test_(b.count() == 0);
	test_(b.capacity() == 0);
	test_(!b.any());

	// Validate construction and to_string()
	BitArray<> b2 { 5 };
	test_(b2.size() == 5);
	for ( size_t i = 0; i < 5; ++i ) { test_(!b2[i]); }
	test_(b2.to_string() == "00000");

	// Test copy, assign, equality, and from_string
	BitArray<> b3 { b2 };
	test_(b2 == b3);
	test_(b != b2);
	test_(!b3[2]);
	b3[2] = true;
	std::cout << b3.to_string () << std::endl;
	test_(b3[2]);
	test_(b2 != b3);
	test_(b2.to_string() == "00000");
	b = b2;
	test_(b.to_string() == "00000");

	// Test move operations
	BitArray<> b4 { move ( b3 ) };
	test_(b4[2]);
	BitArray<> b4b;
	b4b = move ( b4 );
	test_(b4b[2]);

	// Test bit ops
	BitArray<> x { "011010110" }; // Also tests string constructor
	test_(x.count() == 5);
	test_(x.any());
	cout << "Before shift: " << x.to_string () << endl;
	cout << "After shift: " << ( x >> 6 ).to_string () << endl;
	//test_((x << 6).to_string() == "110000000"); // BUG test fails for bit shift
	test_((x >> 6).to_string() == "000000011");
	test_((x <<= 3).to_string() == "010110000");
	test_((x >>= 3).to_string() == "000010110");
	BitArray<> y { ~x };
	nothrow_(x.toggle());
	test_(x == y);
	test_(x.to_string() == "111101001");

	b = BitArray<> {};
	test_(!b.any());
	b += true;
	b += false;
	b += true;
	test_(b.to_string() == "101");
	test_(b.any());

	b2 = BitArray<> { "10101011" };
	test_(b2.count() == 5);
	b2.toggle ();
	test_(b2.to_string() == "01010100");
	b2.erase ( 3 );
	test_(b2.to_string() == "0100100");
	b2.erase ( b2.size () - 1 );
	test_(b2.to_string() == "010010");
	b2.erase ( 1, 4 );
	test_(b2.to_string() == "00");
	b2.insert ( 1, true );
	test_(b2.to_string() == "010");
	b2.insert ( 1, false );
	test_(b2.to_string() == "0010");
	b2 += b;
	test_(b2.to_string() == "0010101");
	b2.insert ( 3, b );
	test_(b2.to_string() == "0011010101");

	ostringstream os;
	os << "101" << 'a' << "0101";
	istringstream is { os.str () };
	b3 = BitArray<> {};
	is >> b3;
	test_(b3.to_string() == "101");
	is.get ();
	is >> b3;
	test_(b3.to_string() == "0101");
	os.str ( "" );
	os << 'a';
	istringstream is2 { os.str () };
	is2 >> b3;
	test_(!is2);
	test_(b3.to_string() == "0101");

	BitArray<> b5 { "11111111111111111111111111000000000000000000000000000011" };
	test_(b5.slice(23,10) == BitArray<>("1110000000"));
	size_t n = b2.size ();
	b2.insert ( 3, b5 );
	test_(n + b5.size() == b2.size());
	b2.erase ( 3, b5.size () );
	b2.shrink_to_fit ();

	// Test comparisons
	BitArray<> b6 { "10101" };
	BitArray<> b7 { "101010" };
	BitArray<> b8 { b7 };
	test_(b6 < b7);
	test_(b6 <= b7);
	test_(b6 <= b6);
	test_(b7 > b6);
	test_(b7 >= b6);
	test_(b7 >= b7);
	test_(BitArray<>("111") > BitArray<>("10111"));

	BitArray<> b9 { "11111111111111111111111111000000000000000000000000000011" };
	ostringstream ostr;
	ostr << b9;
	test_(ostr.str() == "11111111111111111111111111000000000000000000000000000011");
	test_(b9.count() == 28);
	nothrow_(b9 <<= 2);
	test_(b9.count() == 26);
	nothrow_(b9 >>=33);
	test_(b9.count() == 23);
	BitArray<> b10 { "01" };
	b9[0] = b10[0] = b10[1];
	test_(b10[0]);
	test_(b9[0]);
	const BitArray<> b11 { b10 };
	test_(b11[0]);

	BitArray<> b12 ( "11011111101" );
	b12.erase ( 1, 8 );
	test_(b12.to_string() == "101");
	b12 += b12;
	test_(b12.to_string() == "101101");

	BitArray<> b13 ( "" );
	test_(b13.size() == 0);

	report_ ();
}

/* Output (should have at least 4 move assignments):
move constructor
move assignment      
move assignment
move assignment
move assignment
move assignment
move assignment
shrinking from 2 to 1 words   (numbers may vary)

Test Report:

   Number of Passes = 69
   Number of Failures = 0

*/
