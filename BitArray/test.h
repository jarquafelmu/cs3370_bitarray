// Project: BitArray
// Name: Sam Terrazas
// File: test.h
// Created: 04/05/2019 9:21 AM
// Updated: 04/18/2019 2:20 PM
// 
// I declare that the following source code was written by me, or provided
// by the instructor for this project. I understand that copying source
// code from any other source, providing source code to another student, 
// or leaving my code on a public web site constitutes cheating.
// I acknowledge that  If I am found in violation of this policy this may result
// in a zero grade, a permanent record on file and possibly immediate failure of the class. 

#ifndef TEST_H
#define TEST_H
#include <cstddef>
#include <iostream>
using std::size_t;

// Unit Test Scaffolding: Users call test_, fail_, succeed_, throw_, nothrow_, and report_
// AUTHOR: Chuck Allison (Creative Commons License, 2001 - 2014)

namespace
{
	size_t nPass { 0 };
	size_t nFail { 0 };

	void do_fail( const char* text, const char* fileName, long lineNumber )
	{
		std::cout << "FAILURE: " << text << " in file " << fileName
				<< " on line " << lineNumber << std::endl;
		++nFail;
	}

	void do_test( const char* condText, bool cond, const char* fileName, long lineNumber )
	{
		if ( !cond ) { do_fail ( condText, fileName, lineNumber ); }
		else { ++nPass; }
	}

	void succeed_() { ++nPass; }

	void report_()
	{
		std::cout << "\nTest Report:\n\n";
		std::cout << "\tNumber of Passes = " << nPass << std::endl;
		std::cout << "\tNumber of Failures = " << nFail << std::endl;
	}
} // namespace
#define test_(cond) do_test(#cond, cond, __FILE__, __LINE__)
#define fail_(text) do_fail(text, __FILE__, __LINE__)
#define throw_(text,T)                      \
    try {                                   \
        text;                               \
        std::cout << "THROW ";              \
        do_fail(#text,__FILE__,__LINE__);   \
    } catch (T) {                           \
        ++nPass;                            \
    } catch (...) {                         \
        std::cout << "THROW ";              \
        do_fail(#text,__FILE__,__LINE__);   \
    }

#define nothrow_(text)                      \
    try {                                   \
        text;                               \
        ++nPass;                            \
    } catch (...) {                         \
        std::cout << "NOTHROW ";            \
        do_fail(#text,__FILE__,__LINE__);   \
    }
#endif
