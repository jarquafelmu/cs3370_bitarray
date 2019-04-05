// Project: BitArray
// Name: Sam Terrazas
// File: bitarray.h
// Created: 04/05/2019 9:12 AM
// Updated: 04/05/2019 10:38 AM
// 
// I declare that the following source code was written by me, or provided
// by the instructor for this project. I understand that copying source
// code from any other source, providing source code to another student, 
// or leaving my code on a public web site constitutes cheating.
// I acknowledge that  If I am found in violation of this policy this may result
// in a zero grade, a permanent record on file and possibly immediate failure of the class. 

#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H
#include <vector>
#include <climits>
#include <ostream>

template <class IType = size_t>
// throw `logic_error` if any out-of-range indexing is attempted anywhere
class BitArray {
	class bitproxy {
		// define nested private `bitproxy` class to accommodate intelligent use of operator[]. See "bits.cpp"
	};

	enum { BITS_PER_WORD = CHAR_BIT * sizeof ( IType ) };

	std::vector<unsigned> bits_ {};

	bool read_bit( size_t bitpos ); // test bit in logical position `pos`
	static void assign_bit( size_t bitpos, bool val ) {
		// either sets or resets the bit in logical position `pos`, 
		// depending ont the value of `val`
	}

public:
	// Object Management

	explicit BitArray( size_t nbits = 0 ) {
		// create an object of nbits if argument > 0
	};
	explicit BitArray( const std::s /*unused*/tring&  )
 {
	// expects a string of `0` or `1`. Any other character (including whitespace) gets `runtime_error`
		};
	BitArray( BitArray const& other ) = default;                    // Copy Constructor
	auto operator=( BitArray const& other ) -> BitArray& = default; // Copy assignment
	BitArray( BitArray&& other ) noexcept                           // Move Constructor
		: bits_ ( std::move ( other.bits_ ) ) {}

	auto operator=( BitArray&& other ) noexcept -> BitArray& {
		// Move Assignment
		if ( this == &other ) { return *this; }
		bits_ = std::move ( other.bits_ );
		return *this;
	}

	static size_t capacity() {}; // # of bits the current allocation can hold

	// Mutators
	BitArray& operator+=( bool val );                // Append a bit
	BitArray& operator+=( const BitArray& b );       // Append a BitArray
	void erase( size_t bitpos, size_t nbits = 1 );   // Remove "nbits" bits at a position
	void insert( size_t bitpos, bool val );          // Insert a bit at a position (slide "right")
	void insert( size_t bitpos, const BitArray& b ); // Insert an entire BitArray object
	static void shrink_to_fit() {
		// Discard unused, trailing vector cells
		// eliminates any unused blocks at the end of the storage vector that may have 
		// accrued after calls to erase (you are in effect, supporting on-demand, "shrink-to-fit"
		// storage allocation policy, but not forcing it).

		// just call vector::resize
	};

	// Bitwise ops	
	bitproxy operator[]( size_t bitpos ) {
		// return item stored at `bitpos`
		auto block = bitpos / BITS_PER_WORD;  // 1 = 50 / 32
		auto offset = bitpos % BITS_PER_WORD; // 18 = 50 % 32
		return 0;
	}

	bool operator[]( size_t bitpos ) const;
	void toggle( size_t bitpos );
	void toggle(); // Toggles all bits
	BitArray operator~() const;

	// Shift operators
	BitArray operator<<( unsigned int ) const;
	BitArray operator>>( unsigned int ) const;
	BitArray& operator<<=( unsigned int ) const;
	BitArray& operator>>=( unsigned int ) const;

	// Extraction ops
	BitArray slice( size_t bitpos, size_t count ) const; // Extracts a new sub-array

	// Comparison ops
	// these should compare the objects lexicographically
	// (as if they were strings, in dictionary order)
	auto operator==( BitArray const& b ) -> bool;
	auto operator!=( BitArray const& b ) -> bool;
	auto operator<( BitArray const& b ) -> bool;
	auto operator<=( BitArray const& b ) -> bool;
	auto operator>( BitArray const& b ) -> bool;
	auto operator>=( BitArray const& b ) -> bool;

	// Counting ops
	size_t size() const;  // Number of bits in use in the vector
	size_t count() const; // The number of 1-bits present
	bool any() const;     // Optimized version of count() > 0

	// Stream I/O (define these in situ)
	friend auto operator<<( std::ostream& os, BitArray const& obj ) -> std::ostream& {
		return os << "bits_: " << obj.bits_;
	}

	friend auto operator>>( std::istream& is, BitArray& obj ) -> std::istream& {
		// read only `0` or `1` into the objects, other characters are to stay in the stream
		// set stream to fail state if there are no valid bit characters to be read in the input stream
		// after skipping whitespace
	}

	// String conversion
	std::string to_string() const;
};
#endif //BIT_ARRAY_H
