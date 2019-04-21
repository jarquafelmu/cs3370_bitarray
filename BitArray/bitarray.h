// Project: BitArray
// Name: Sam Terrazas
// File: bitarray.h
// Created: 04/05/2019 9:12 AM
// Updated: 04/15/2019 9:09 AM
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
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <locale>
#include <iostream>

template <class IType = size_t>
// throw `logic_error` if any out-of-range indexing is attempted anywhere
class BitArray
{
	enum { BITS_PER_WORD = CHAR_BIT * sizeof ( IType ) };
	std::vector<IType> bits_ {}; // a collection to store our bits
	size_t nbits_ {}; // the number of bits currently in use

	class Bitproxy final
	{
		// Bitproxy is called 'reference' in `bits.cpp
		size_t pos_;
		BitArray& b_;
	public:
		Bitproxy( BitArray& bit_array, size_t const& pos ) : b_ { bit_array } { pos_ = pos; }
		Bitproxy& operator=( bool const bit )
		{
			// set the bit in position pos to true or false; per bit
			b_.assign_bit ( pos_, bit );
			return *this;
		}

		Bitproxy& operator=( Bitproxy& obj )
		{
			if ( this == &obj ) return *this;
			b_ = obj.b_;
			pos_ = obj.pos_;
			return *this;
		}

		// ReSharper disable once CppNonExplicitConversionOperator
		operator bool() const
		{
			if ( b_.empty () ) throw std::logic_error ( "out of bounds" );
			return b_.read_bit ( pos_ );
		}
	};

	IType wrap_index(size_t const& index)
	{
		return index % nbits_;
	}

	void grow ()
	{
		grow ( nbits_ );
	}

	void grow(size_t const& nbits)
	{
		auto space_required = words_needed ( nbits ) + 1;

		for ( auto i {0}; i < space_required - bits_.size (); ++i)
		{
			bits_.push_back ( IType {} );
		}
		// if (space_required > bits_.size ())
		// {
		// 	bits_.resize ( space_required );
		// }
	}

/// <summary>
/// Compares the provided BitArray against this BitArray
/// </summary>
/// <param name="b">The BitArray to be compared.</param>
/// <returns>
///		Returns an integer which denotes the comparison result.
/// <list type="bullet">
/// <item>
/// <description><c>-1:</c> Provided BitArray is less than this BitArray</description>
/// </item>
/// <item>
/// <description><c>0:</c> Provided BitArray is equal to this BitArray</description>
/// </item>
/// <item>
/// <description><c>1:</c> Provided BitArray is greater than this BitArray</description>
/// </item>
/// </list>
/// </returns>
	int comp ( BitArray const& b )
	{
		auto retVal { 0 };
		auto comp_length { 0 };
		auto this_bit_str {to_string ()};
		auto bit_str = b.to_string ();
			
		// determine our comparison length and default compare result.
		if ( this_bit_str.length () < bit_str.length ())
		{
			retVal = 1;
			comp_length = this_bit_str.length ();
		} else if ( this_bit_str.length () > bit_str.length () )
		{
			retVal = -1;
			comp_length = bit_str.length ();
		} else
		{
			retVal = 0;
			comp_length = bit_str.length ();
		}

		for ( auto i {0}; i < comp_length; ++i )
		{
			if ( bit_str [i] != this_bit_str [i] )
			{
				// 1 - 0 = 1
				// 0 - 1 = -1
				retVal = bit_str [i] - this_bit_str [i];
				break;
			}
		}
			 
		return retVal;
	}

protected:
	int read_bit( size_t const& bitpos ) const
	{
		if ( bitpos >= nbits_ || bitpos < 0 || nbits_ == 0 ) throw std::logic_error ( "index out of range" );

		// get which element contains our bit
		auto word = read_word ( bitpos );
		auto const offset = bit_offset ( bitpos ); // 18 = 50 % 32

		// get a 1s mask at our bit location and & with it in order to only get that single bit value
		return word &= mask1 ( offset );
	}

	void assign_bit( size_t const& bitpos, bool bit )
	{
		// gets the word to assign the bit to gets a new word if we are empty

		// since we have nothing, create our self big enough to handle the incoming bit
		grow ();

		// get the word and bit offset represented by the bitpos
		auto word = read_word ( bitpos );
		auto const offset = bit_offset ( bitpos );

		// if we are setting the bit then we want a 1 mask
		// if we are resetting the bit then we want a 0 mask
		auto mask = ( bit ? mask1 ( offset ) : mask0 ( offset ) );

		if ( bit )
		{
			// setting
			word |= mask;
		}
		else
		{
			// resetting
			word &= mask;
		}

		set_word ( word, bitpos );
	}

	// how big does the vector need to be
/// <summary>
/// Calculates how many words is required to house the number of bits provided.
/// </summary>
/// <param name="nbits">The number of bits.</param>
/// <returns>The number of words required.</returns>
	static size_t words_needed ( size_t const& nbits ) { return word_offset ( nbits ); }

	// which word in vector for given offset
/// <summary>
/// Calculates the word offset of the position.
/// </summary>
/// <param name="bitpos">The bitpos.</param>
/// <returns>size_t.</returns>
	static size_t word_offset ( size_t bitpos ) { return bitpos / BITS_PER_WORD; }

/// <summary>
/// Calculates the bit offset in a word for the position.
/// </summary>
/// <param name="bitpos">The bitpos.</param>
/// <returns>size_t.</returns>
	static size_t bit_offset ( size_t bitpos ) { return bitpos % BITS_PER_WORD; }  // offset into word for creating mask

	IType read_word( size_t const& bitpos ) const
	{
		auto block = word_offset ( bitpos );
		try {
			return bits_.at ( block );
		} catch (std::out_of_range& e)
		{
			throw std::logic_error ( e.what () );
		}
	}

	void set_word( IType word, size_t const& bitpos )
	{
		auto block = word_offset ( bitpos ); 
		bits_[block] = word;
	}

	static IType mask1( size_t const& bitpos ) { return 1u << bitpos; } // returns a 1 mask shifted properly

	static IType mask0( size_t const& bitpos ) { return ~mask1 ( bitpos ); } // returns a 0 mask shifted properly
	
	// counts bits set in bitarray
	size_t count_ones( IType word ) const
	{
		//tidy ();
		auto temp = bits_;
		clean_word ( temp.back () );

		auto count { 0 };
		std::for_each ( temp.begin (), temp.end (), [&]( auto& item )
		{
			for ( auto i { 0 }; i < BITS_PER_WORD; ++i )
			{
				count += item & 1u;
				item >>= 1u;
			}
		} );
		return count;
	}

	void clean_word ( IType& word ) const // reset unused bits in last word
	{
		auto used_bits = nbits_ % BITS_PER_WORD;
		if (used_bits)
		{
			// create a mask which places 1's where are used bits are
			// and 0's where our dirty bits are
			IType mask = ( IType ( 1 ) << used_bits ) - 1;
			word &= mask;
		}
	}

	static std::string display_word( IType word, size_t& bits_to_display )
	{
		if ( bits_to_display <= 0 ) return "";

		std::stringstream ss;
		// logic might need to be reversed
		// for ( auto i = sizeof ( IType ) - 1; i >= 0 ; --i) {
		for ( auto i = 0; i < BITS_PER_WORD - 1 && bits_to_display > 0; ++i )
		{
			// shifts word to the right by the amount indicated by i
			// then ANDs with 1 to remove everything but the least significant bit
			ss << ( ( word >> i ) & 1 );
			--bits_to_display;
		}
		return ss.str ();
	}

public:
	// Object Management

	explicit BitArray( size_t const nbits = 0 ) // create an object of nbits if argument > 0
	{
		if ( nbits > 0 )
		{
			grow ( nbits );
			nbits_ = nbits;
		}
	};

	explicit BitArray( const std::string& str )
	{
		// expects a string of `0` or `1`. Any other character (including whitespace) gets `runtime_error`
		auto iter = std::find_if ( str.begin (), str.end (), []( int const c ) { return c != '1' && c != '0'; } );
		// if iter is not the end of str then a character other than `0` or `1` was found
		if ( iter != str.end () ) { throw std::runtime_error ( "invalid character found." ); }

		// if reached then our string was good, now we need to bring it in
		std::for_each ( str.begin (), str.end (), [&]( auto const& character )
		{
			operator+= ( character == '1' );
		});
	}

	BitArray( BitArray const& other ) = default; // Copy Constructor

	auto operator=( BitArray const& other ) -> BitArray& = default; // Copy assignment

	BitArray( BitArray&& other ) noexcept
		: bits_ ( std::move(other.bits_) ),
			nbits_ ( other.nbits_ ) {
		std::cout << "move constructor" << std::endl;
	}

	auto operator=( BitArray&& other ) noexcept -> BitArray&
	{
		if ( this == &other )
			return *this;
		bits_ = std::move ( other.bits_ );
		nbits_ = other.nbits_;
		std::cout << "move assignment" << std::endl;
		return *this;
	}

	// Mutators
	BitArray& operator+=( bool const val )          // Append a bit
	{
		// increase our bit storage capacity
		grow ( ++nbits_ );

		// assign the new bit to the new spot
		assign_bit ( nbits_ - 1, val );
		return *this;
	}

	BitArray& operator+=( const BitArray& b ) // Append a BitArray
	{		
		// increase our count of bits by how many bits we are inserting
		for ( size_t i {}; i < b.size (); ++i )
		{
			*this += b [i];
		}
		return *this;
	}

	// Remove "nbits" bits at a position
	void erase( size_t const& bitpos, size_t const& nbits = 1 )
	{
		BitArray temp {};
		auto const end = bitpos + nbits - 1;
		for ( auto i { 0 }; i < nbits_; ++i)
		{
			if ( i < bitpos || i > end )
				temp += read_bit ( i );
		}
		//
		// std::swap ( bits_, temp.bits_ );
		// std::swap ( nbits_, temp.nbits_ );

		*this = std::move ( temp );
	} 

	void insert( size_t const bitpos, bool val )           // Insert a bit at a position (slide "right")
	{
		BitArray temp {};

		for ( auto i { 0 }; i < nbits_; ++i ) {
			if ( i == bitpos )
				temp += val;
			temp += read_bit ( i );
		}

		*this = std::move ( temp );
	}

	 // Insert an entire BitArray object
	void insert( size_t const bitpos, const BitArray& b )
	{
		BitArray temp {};

		for ( size_t i {}; i < nbits_; ++i ) {
			if ( i == bitpos )
				temp += b;
			temp += read_bit ( i );
		}

		std::swap ( bits_, temp.bits_ );
		std::swap ( nbits_, temp.nbits_ );
	}

		// Discard unused, trailing vector cells
		// eliminates any unused blocks at the end of the storage vector that may have 
		// accrued after calls to erase (you are in effect, supporting on-demand, "shrink-to-fit"
		// storage allocation policy, but not forcing it).
	void shrink_to_fit()
	{
		std::cout << "shrinking from " << capacity ();
		bits_.resize (capacity ());
		clean_word ( bits_.back () );
		std::cout << " to" << capacity () << " words";
	}

	// Bitwise ops	
	Bitproxy operator[]( size_t bitpos )
	{
		if ( empty () ) throw std::logic_error ( "out of bounds" );
		return Bitproxy ( *this, bitpos );
	}

	bool operator[]( size_t const& bitpos ) const
	{
		if ( empty () ) throw std::logic_error ( "out of bounds" );
		return read_bit ( bitpos );
	}

	bool at( size_t const& bitpos ) const { return read_bit ( bitpos ); }

	void toggle( size_t const& bitpos )
	{
		// get a 1 mask at the bit position
		// XOR the word with the mask
		assign_bit ( bitpos, !read_bit ( bitpos ) );
	}

	void toggle() // Toggles all bits
	{
		// for each word XOR it with a full one mask
		auto mask { ~0u };
		std::transform ( bits_.begin (), bits_.end (), bits_.begin (),
			[&mask]( auto& word ) { return word ^= mask; } );
	}

	BitArray operator~() const
	{
		auto new_b { *this };
		new_b.toggle ();
		return new_b;
	}

	// Shift operators
	BitArray operator<<( unsigned int shift_amt ) const // shift temp left
	{
		auto temp = *this;
		return temp.operator<<=(shift_amt);
	}
	BitArray operator>>( unsigned int shift_amt ) const // shift temp right
	{
		auto temp = *this;
		return temp.operator>>=( shift_amt );
	}

	BitArray& operator<<=( unsigned int const& shift_amt ) // shift self left
	{
		if (shift_amt >= nbits_)
		{
			reset ();
		}
		else {
			for ( size_t index {0}; index < nbits_; ++index ) {
				if ( index < nbits_ - shift_amt )
					assign_bit ( index, read_bit ( index + shift_amt ) );
				else
					assign_bit ( index, false );
			}
		}
		return *this;
	}

	BitArray& operator>>=( unsigned int const& shift_amt ) // shift self right
	{
		if ( shift_amt >= nbits_ ) {
			reset ();
		}
		else {
			for ( auto index { nbits_ }; index > 0; --index ) {
				auto const relative_index = index - 1;
				if ( relative_index >= shift_amt )
					assign_bit ( relative_index, read_bit ( relative_index - shift_amt ) );
				else
					assign_bit ( relative_index, false );
			}
		}

		return *this;
	}

	// Extraction ops
	BitArray slice( size_t const& bitpos, size_t const& count ) // Extracts a new sub-array
	{
		BitArray new_array {};
		auto current { bitpos };

		// start at bitpos and end at bitpos + count
		while (current != bitpos + count)
		{
		// read each bit from `*this` and copy it to `new_array`
			new_array += read_bit ( current++ );
		}		
		
		// return `new_array`		
		return new_array;
	}

	// Comparison ops
	// these should compare the objects lexicographically
	// (as if they were strings, in dictionary order)
	auto operator==( BitArray<IType> const& b ) -> bool { return comp ( b ) == 0; }
	auto operator!=( BitArray const& b ) -> bool { return comp ( b ) != 0; }
	auto operator<( BitArray const& b ) -> bool { return comp ( b ) > 0; }
	auto operator<=( BitArray const& b ) -> bool { return comp ( b ) >= 0; }
	auto operator>( BitArray const& b ) -> bool { return comp ( b ) < 0; }
	auto operator>=( BitArray const& b ) -> bool { return comp ( b ) <= 0; }

	// Counting ops
	size_t size() const { return nbits_; } // Number of bits in use in the vector

	size_t capacity() const { return words_needed ( nbits_ ); } // # of bits the current allocation can hold

	size_t count() const // The number of 1-bits present
	{
		auto count { 0 };
		for ( auto const& word : bits_ ) { count += count_ones ( word ); }
		return count;
	}

	bool any() const // Optimized version of count() > 0
	{
		for ( auto const& item : bits_ ) { if ( item ) { return true; } }
		return false;
	}

/// <summary>
/// Resets this instance by making every bit zero.
/// </summary>
	void reset ()
	{
		for ( auto i { 0 }; i < nbits_; ++i)
		{
			assign_bit ( i, false );
		}
	}

	// Stream I/O (define these in situ)
	friend auto operator<<( std::ostream& os, BitArray const& obj ) -> std::ostream&
	{
		return os << "bits_: " << obj.to_string ();
	}

	// overwrites it's bitarray argument's container
	friend auto operator>>( std::istream& is, BitArray& obj ) -> std::istream& {
		BitArray temp;
		char c;
		while ( is && is >> std::skipws >> c ) {
			if ( c != '0' && c != '1' ) {
				is.unget ();
				if ( temp.empty () ) {
					is.setstate ( std::ios::failbit );
					return is;
				}
				break;
			}

			temp += c == '1';
		}
		obj = std::move ( temp );
		return is;
	}

	bool empty () const
	{
		return size () == 0;
	}

	void clear ()
	{
		bits_.clear ();
		bits_.shrink_to_fit ();
		nbits_ = 0;
	}

	// String conversion
	std::string to_string() const
	{
		std::ostringstream ss;
		if ( !bits_.empty () )
		{
			auto bits_to_display = nbits_;
			for ( auto const& item : bits_ ) { ss << display_word ( item, bits_to_display ); }
		}
		return ss.str ();
	}
};
#endif //BIT_ARRAY_H
