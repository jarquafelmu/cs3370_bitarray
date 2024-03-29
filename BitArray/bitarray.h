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
	class Bitproxy final
	{
		// Bitproxy is called 'reference' in `bits.cpp
		size_t pos_;
		BitArray b_;
	public:
		Bitproxy( BitArray& bit_array, size_t const& pos ) : b_ { bit_array } { pos_ = pos; }
		Bitproxy& operator=( bool const bit )
		{
			// set the bit in position pos to true or false; per bit
			b_.assign_bit ( pos_, bit );
			return *this;
		}

		operator bool() const
		{
			// return true or false per the bit in position pos_
			// return b_.data & (1u << pos);
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
		auto space_required = words_needed ( nbits );
		if (space_required > bits_.size ())
		{
			bits_.resize ( space_required );
		}
	}

	enum { BITS_PER_WORD = CHAR_BIT * sizeof ( IType ) };

	std::vector<IType> bits_ {}; // a collection to store our bits

	size_t nbits_ {}; // the number of bits currently in use

protected:
	bool read_bit( size_t const& bitpos ) const
	{
		// get which element contains our bit
		// get which element contains our bit

		auto word = read_word ( bitpos );
		auto const offset = bit_offset ( bitpos ); // 18 = 50 % 32

		// get a 1s mask at our bit location and & with it in order to only get that single bit value
		return word &= mask1 ( offset );
	}

	void assign_bit( size_t const& bitpos, bool bit )
	{
		// either sets or resets the bit in logical position `pos`, 
		// depending ont the value of `bit`
		
		auto word = read_word ( bitpos );
		auto const offset = bit_offset ( bitpos ); // 18 = 50 % 32

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
		return bits_.at ( block );
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
		// clean the last word
		bits_[bits_.size() - 1] = clean_word ( bits_.back () );

		auto count { 0 };
		std::for_each ( bits_.begin (), bits_.end (), [&count]( auto const& item )
		{
			auto temp = item;
			for ( auto i { 0 }; i < BITS_PER_WORD; ++i )
			{
				count += temp & 1u;
				temp >>= 1u;
			}
		} );
		return count;
	}

	IType clean_word( IType word, size_t nbits ) // reset unused bits in last word
	{
		auto used_bits = nbits % BITS_PER_WORD;
		if (used_bits)
		{
			IType mask = ( IType ( 1 ) << used_bits ) - 1;
			word &= mask;
		}
		return word;
	} 

	std::string to_string( IType word )
	{
		std::stringstream ss;
		// logic might need to be reversed
		// for ( auto i = sizeof ( IType ) - 1; i >= 0 ; --i) {
		for ( auto i = 0; i < sizeof ( IType ) - 1; ++i )
		{
			// shifts word to the right by the amount indicated by i
			// then ANDs with 1 to remove everything but the least significant bit
			ss << ( ( word >> i ) & 1 );
		}
		return ss.str ();
	}

public:
	// Object Management

	explicit BitArray( size_t const nbits = 0 ) // create an object of nbits if argument > 0
	{
		if ( nbits > 0 ) { grow ( nbits ); }
	};

	explicit BitArray( const std::string& str )
	{
		// expects a string of `0` or `1`. Any other character (including whitespace) gets `runtime_error`
		auto iter = std::find_if ( str.begin (), str.end (), []( int const c ) { return c != '1' && c != '0'; } );
		// if iter is not the end of str then a character other than `0` or `1` was found
		if ( iter != str.end () ) { throw std::runtime_error ( "invalid character found: '" + to_string ( *iter ) + "'" ); }
	}

	BitArray( BitArray const& other ) = default; // Copy Constructor

	auto operator=( BitArray const& other ) -> BitArray& = default; // Copy assignment

	BitArray( BitArray&& other ) noexcept // Move Constructor
		: bits_ ( std::move ( other.bits_ ), nbits_ (std::move ( other.nbits_ )) ) { std::cout << "move constructor" << std::endl; }

	auto operator=( BitArray&& other ) noexcept -> BitArray& // Move Assignment	
	{
		std::cout << "move assignment" << std::endl;
		if ( this == &other ) { return *this; }
		bits_ = std::move ( other.bits_ );
		nbits_ = std::move ( other.nbits_ );
		return *this;
	}

	// Mutators
	BitArray& operator+=( bool const val )          // Append a bit
	{
		// increase our bit storage capacity
		grow ( ++nbits_ );

		// assign the new bit to the new spot
		assign_bit ( nbits_, val );
		return *this;
	}
	BitArray& operator+=( const BitArray& b ) // Append a BitArray
	{		
		// increase our count of bits by how many bits we are inserting
		for ( auto i {}; i < b.size (); ++i )
		{
			*this += b [i];
		}
		return *this;
	}

	void erase( size_t bitpos, size_t nbits = 1 ) {} // Remove "nbits" bits at a position
	void insert( size_t bitpos, bool val )           // Insert a bit at a position (slide "right")
	{
		// if insert at end, just append bit and increment `nbits_`

		// if insert at beginning or somewhere else, slide all bits one to the right,
		// to the right of that spot, insert bit, then increment `nbits_`
		grow ( ++nbits_ );
		left_shift_at ( bitpos );
		assign_bit ( bitpos, val );
	}

	void insert( size_t bitpos, const BitArray& b )
	{
		auto additional_size = b.size ();
		nbits_ += additional_size;
		grow ();
		for ( auto i {}; i < additional_size; ++i)
		{
			
		}
	} // Insert an entire BitArray object

	void shrink_to_fit()
	{
		// Discard unused, trailing vector cells
		// eliminates any unused blocks at the end of the storage vector that may have 
		// accrued after calls to erase (you are in effect, supporting on-demand, "shrink-to-fit"
		// storage allocation policy, but not forcing it).

		// just call vector::resize
		// determine how many words are being used
		bits_.resize (size ());
	}

	// Bitwise ops	
	Bitproxy operator[]( size_t bitpos ) { return Bitproxy ( *this, bitpos ); }

	bool operator[]( size_t const& bitpos ) const { return read_bit ( bitpos ); }

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
			[&mask]( auto const& word ) { return word ^= mask; } );
	}

	BitArray operator~() const
	{
		auto new_b { *this };
		new_b.toggle ();
		return new_b;
	}

	void left_shift_at( size_t const& bitpos ) const
	{
		for ( auto i { bitpos }; i < nbits_; ++i ) {
			assign_bit ( wrap_index ( i ), read_bit ( wrap_index ( i + 1 ) ) );
		}
	}

	void right_shift_at(size_t const& bitpos) const
	{
		for ( auto i { bitpos }; i < nbits_; ++i ) {
			assign_bit ( wrap_index ( i + 1 ), read_bit ( wrap_index ( i ) ) );
		}
	}

	// Shift operators
	BitArray operator<<( unsigned int shift_amt ) const // shift temp left
	{
		auto temp = *this;
		return temp << shift_amt;
	}
	BitArray operator>>( unsigned int shift_amt ) const // shift temp right
	{
		auto temp = *this;
		return temp >> shift_amt;
	}
	BitArray& operator<<=( unsigned int shift_amt ) const // shift self left
	{
		left_shift_at ( 0 );
		return *this;
	}
	BitArray& operator>>=( unsigned int shift_amt ) const // shift self right
	{
		right_shift_at ( 0 );
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
	auto operator==( BitArray<IType> const& b ) -> bool { return to_string () == b.to_string (); }
	auto operator!=( BitArray const& b ) -> bool { return !( operator==(b) ); }
	auto operator<( BitArray const& b ) -> bool { return to_string () < b.to_string (); }
	auto operator<=( BitArray const& b ) -> bool { return operator<( b ) || operator==( b ); }
	auto operator>( BitArray const& b ) -> bool { return to_string () > b.to_string (); }
	auto operator>=( BitArray const& b ) -> bool { return operator>( b ) || operator==( b ); }

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

	// Stream I/O (define these in situ)
	friend auto operator<<( std::ostream& os, BitArray const& obj ) -> std::ostream&
	{
		return os << "bits_: " << obj.to_string ();
	}

	// overwrites it's bitarray argument's container
	friend auto operator>>( std::istream& is, BitArray& obj ) -> std::istream&
	{
		IType word {};
		bits_.clear ();
		bits_.shrink_to_fit ();

		auto count { 0 };
		auto const max_size { sizeof ( IType ) };

		while ( is )
		{
			auto const next_char = is.peek ();
			if ( next_char == static_cast<int> ( is.eof () ) ) { break; }
			if ( next_char != '0' && next_char != '1' ) // it's okay if the character is a whitespace
			{
				if ( !std::isspace ( next_char, std::locale ( "en_US.UTF8" ) ) ) { break; } // illegal character found
				is.seekg ( 1 );                                                             // skip whitespace character
				continue;
			}

			if ( count == max_size )
			{
				obj.bits_.push_back ( word );
				word = 0;
				count = 0;
			}
			else { word <<= 1; }

			if ( is.get () == '1' )
			{
				// if is.get() == '0' then it doesn't matter as it has already been 
				// shifted and so a zero is already in the least significant spot
				word |= 1u;
			}
			count++;
		}

		return is;
	}

	// String conversion
	std::string to_string() const
	{
		std::ostringstream ss;
		if ( !bits_.empty () ) { for ( auto const& item : bits_ ) { ss << to_string ( item ); } }
		return ss.str ();
	}
};
#endif //BIT_ARRAY_H
