/* Copyright (C) 2011 Wildfire Games.
 * This file is part of 0 A.D.
 *
 * 0 A.D. is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * 0 A.D. is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 0 A.D.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDED_MATHS_RANDOM
#define INCLUDED_MATHS_RANDOM

// Number of elements in an array.
#define ARRAY_COUNT(Array) \
	(sizeof(Array) / sizeof((Array)[0]))

#define FACT 2.32830643653869628906e-10

/**
 * Random number generator with period 2^{512}-1;
 * effectively a better version of MT19937 (smaller state, similarly fast,
 * simpler code, better distribution).
 * Implements Boost.Random's PseudoRandomNumberGenerator concept.
 */
class WELL512
{
private:
    UINT state[16];
    UINT index;

public:
	WELL512()
    {
		seed((UINT)0);
	}

	UINT operator()()
	{
		// WELL512 implementation by Chris Lomont (Game Programming Gems 7;
		// http://lomont.org/Math/Papers/2008/Lomont_PRNG_2008.pdf)

		UINT a, b, c, d;
		a = state[index];
		c = state[(index + 13) & 15];
		b = a ^ c ^ (a << 16) ^ (c << 15);
		c = state[(index + 9) & 15];
		c ^= (c >> 11);
		a = state[index] = b ^ c;
		d = a ^ ((a << 5) & 0xDA442D24UL);
		index = (index + 15) & 15;
		a = state[index];
		state[index] = a ^ b ^ d ^ (a << 2) ^ (b << 18) ^ (c << 28);
		return state[index];
	}

	double Random()
	{
		// WELL512 implementation by Chris Lomont (Game Programming Gems 7;
		// http://lomont.org/Math/Papers/2008/Lomont_PRNG_2008.pdf)

		UINT a, b, c, d;
		a = state[index];
		c = state[(index + 13) & 15];
		b = a ^ c ^ (a << 16) ^ (c << 15);
		c = state[(index + 9) & 15];
		c ^= (c >> 11);
		a = state[index] = b ^ c;
		d = a ^ ((a << 5) & 0xDA442D24UL);
		index = (index + 15) & 15;
		a = state[index];
		state[index] = a ^ b ^ d ^ (a << 2) ^ (b << 18) ^ (c << 28);
		return ((double)state[index]) * FACT;
	}
	
	void seed(UINT value)
	{
		index = 0;

		// Expand the seed with the same algorithm as boost::random::mersenne_twister
		const UINT mask = ~0u;
		state[0] = value & mask;
		for (UINT i = 1; i < ARRAY_COUNT(state); ++i)
			state[i] = (1812433253UL * (state[i - 1] ^ (state[i - 1] >> 30)) + i) & mask;
	}

	void seed(UINT values[16])
	{
		index = 0;

		for (UINT i = 0; i < ARRAY_COUNT(state); ++i)
			state[i] = values[i];
	}
		
	// Implement UniformRandomNumberGenerator concept:
	//typedef UINT result_type;

	//UINT min() const
	//{
	//return std::numeric_limits<UINT>::min();
	//}
	//
	//UINT max() const
	//{
	//return std::numeric_limits<UINT>::max();
	//}
	
	// Implement EqualityComparable concept:
	
	friend bool operator==(const WELL512& x, const WELL512& y)
	{
		if (x.index != y.index)
			return false;
		for (UINT i = 0; i < ARRAY_COUNT(x.state); ++i)
			if (x.state[i] != y.state[i])
				return false;
		return true;
	}

	friend bool operator!=(const WELL512& x, const WELL512& y)
	{
		return !(x == y);
	}

	// Implement Streamable concept (based on boost::random::mersenne_twister):
	//template<class CharT, class Traits>
	//friend std::basic_ostream<CharT, Traits>&
	//	operator<<(std::basic_ostream<CharT, Traits>& os, const WELL512& rng)
	//{
	//	os << rng.index << " ";
	//	for (UINT i = 0; i < ARRAY_COUNT(rng.state); ++i)
	//		os << rng.state[i] << " ";
	//	return os;
	//}
	//				
	//template<class CharT, class Traits>
	//friend std::basic_istream<CharT, Traits>&
	//	operator>>(std::basic_istream<CharT, Traits>& is, WELL512& rng)
	//{
	//	is >> rng.index >> std::ws;
	//	for (UINT i = 0; i < ARRAY_COUNT(rng.state); ++i)
	//		is >> rng.state[i] >> std::ws;
	//	return is;
	//}
	
	INT RandomI( INT MinI, INT MaxI )
	{
			// Output random integer in the interval min <= x <= max
			// Relative error on frequencies < 2^-32
			if ( MaxI <= MinI )
			{
				if ( MaxI == MinI) return MinI; else return 0x80000000;
			}

			// Multiply interval with random and truncate
			int r = int((double)(unsigned int)(MaxI - MinI + 1) * Random() + MinI); 
			if (r > MaxI) r = MaxI;
			return r;
	}
};

#endif // INCLUDED_MATHS_RANDOM

///* ***************************************************************************** */
///* Copyright:      Francois Panneton and Pierre L'Ecuyer, University of Montreal */
///*                 Makoto Matsumoto, Hiroshima University                        */
///* Notice:         This code can be used freely for personal, academic,          */
///*                 or non-commercial purposes. For commercial purposes,          */
///*                 please contact P. L'Ecuyer at: lecuyer@iro.UMontreal.ca       */
///* ***************************************************************************** */
//#ifndef __RANDOM_H__
//#define __RANDOM_H__
//
//#include <time.h>
//
//#define W 32
//#define R 16
//#define P 0
//#define M1 13
//#define M2 9
//#define M3 5
//
//#define MAT0POS(t,v) (v^(v>>t))
//#define MAT0NEG(t,v) (v^(v<<(-(t))))
//#define MAT3NEG(t,v) (v<<(-(t)))
//#define MAT4NEG(t,b,v) (v ^ ((v<<(-(t))) & b))
//
//#define V0            STATE[state_i                   ]
//#define VM1           STATE[(state_i+M1) & 0x0000000fU]
//#define VM2           STATE[(state_i+M2) & 0x0000000fU]
//#define VM3           STATE[(state_i+M3) & 0x0000000fU]
//#define VRm1          STATE[(state_i+15) & 0x0000000fU]
//#define VRm2          STATE[(state_i+14) & 0x0000000fU]
//#define newV0         STATE[(state_i+15) & 0x0000000fU]
//#define newV1         STATE[state_i                 ]
//#define newVRm1       STATE[(state_i+14) & 0x0000000fU]
//
//#define FACT 2.32830643653869628906e-10
//
//extern void InitWELLRNG512a();// (unsigned int *init = NULL);
//extern double Random (void);
//extern int RandomI( int mini, int maxi );
//
//
//#endif	// __RANDOM_H__