/**
 * @file	TBitArray.h
 *
 * Declares the bit array class.
 */
#ifndef __TBITARRAY_H__
#define __TBITARRAY_H__

//
// Forward declaration.
template<typename Allocator = NDefaultBitArrayAllocator>
class TBitArray;

template<typename Allocator = NDefaultBitArrayAllocator>
class TConstSetBitIterator;

template<typename Allocator = NDefaultBitArrayAllocator, typename OtherAllocator = NDefaultBitArrayAllocator>
class TConstDualSetBitIterator;

/** Used to read/write a bit in the array as a UBOOL. */
class NBitReference
{
public:
	NBitReference(DWORD& InData, DWORD InMask)
		: Data(InData)
		, Mask(InMask)
	{
	}
	
	FORCEINLINE operator UBOOL() const
	{
		return (Data & Mask) != 0;
	}
	
	FORCEINLINE void operator=(const UBOOL NewValue)
	{
		if (NewValue)
		{
			Data |= Mask;
		}
		else
		{
			Data &= ~Mask;
		}
	}
	
private:
	DWORD&	Data;
	DWORD	Mask;
};

/** Used to read a bit in the array as a UBOOL. */
class NConstBitReference
{
public:
	NConstBitReference(const DWORD& InData, DWORD InMask)
		: Data(InData)
		, Mask(InMask)
	{
	}
	
	FORCEINLINE operator UBOOL() const
	{
		return (Data & Mask) != 0;
	}
	
private:
	const DWORD& Data;
	DWORD Mask;
};

/**	Used to reference a bit in an unspecified bit array. */
class NRelativeBitReference
{
	template<typename>
	friend class TBitArray;
	template<typename>
	friend class TConstSetBitIterator;
	template<typename, typename>
	friend class TConstDualSetBitIterator;
	
public:
	NRelativeBitReference(INT BitIndex)
		: DWORDIndex(BitIndex >> NumBitsPerDWORDLogTwo)
		, Mask(1 << (BitIndex & (NumBitsPerDWORD - 1)))
	{
	}

protected:
	INT DWORDIndex;
	DWORD Mask;
};

/**
 * A dynamically sized bit array.
 * An array of Booleans.  They stored in one bit/Boolean.  There are iterators that efficiently iterate over only set bits.
 */
template<typename Allocator /*= NDefaultBitArrayAllocator*/>
class TBitArray : protected Allocator::template ForElementType<DWORD>
{
public:
	
	template<typename>
	friend class TConstSetBitIterator;
	
	template<typename,typename>
	friend class TConstDualSetBitIterator;
	
	/**
	 * Minimal initialization constructor.
	 * @param Value - The value to initial the bits to.
	 * @param InNumBits - The initial number of bits in the array.
	 */
	TBitArray( const UBOOL Value = FALSE, const INT InNumBits = 0 )
	:	NumBits(0)
	,	MaxBits(0)
	{
		Init(Value,InNumBits);
	}
	
	/**
	 * Copy constructor.
	 */
	TBitArray(const TBitArray& Copy)
	:	NumBits(0)
	,	MaxBits(0)
	{
		*this = Copy;
	}
	
	/**
	 * Assignment operator.
	 */
	TBitArray& operator=(const TBitArray& Copy)
	{
		// check for self assignment since we don't use swamp() mechanic
		if( this == &Copy )
		{
			return *this;
		}
		
		Empty(Copy.Num());
		NumBits = MaxBits = Copy.NumBits;
		if(NumBits)
		{
			const INT NumDWORDs = (MaxBits + NumBitsPerDWORD - 1) / NumBitsPerDWORD;
			Realloc(0);
			appMemcpy(GetData(),Copy.GetData(),NumDWORDs * sizeof(DWORD));
		}
		return *this;
	}
	
	/**
	 * Adds a bit to the array with the given value.
	 * @return The index of the added bit.
	 */
	INT AddItem(const UBOOL Value)
	{
		const INT Index = NumBits;
		const UBOOL bReallocate = (NumBits + 1) > MaxBits;
		
		NumBits++;
		
		if(bReallocate)
		{
			// Allocate memory for the new bits.
			const UINT MaxDWORDs = this->CalculateSlack(
														(NumBits + NumBitsPerDWORD - 1) / NumBitsPerDWORD,
														(MaxBits + NumBitsPerDWORD - 1) / NumBitsPerDWORD,
														sizeof(DWORD)
														);
			MaxBits = MaxDWORDs * NumBitsPerDWORD;
			Realloc(NumBits - 1);
		}
		
		(*this)(Index) = Value;
		
		return Index;
	}
	
	/**
	 * Removes all bits from the array, potentially leaving space allocated for an expected number of bits about to be added.
	 * @param ExpectedNumBits - The expected number of bits about to be added.
	 */
	void Empty(INT ExpectedNumBits = 0)
	{
		NumBits = 0;
		
		// If the expected number of bits doesn't match the allocated number of bits, reallocate.
		if(MaxBits != ExpectedNumBits)
		{
			MaxBits = ExpectedNumBits;
			Realloc(0);
		}
	}
	
	/**
	 * Resets the array's contents.
	 * @param Value - The value to initial the bits to.
	 * @param NumBits - The number of bits in the array.
	 */
	void Init(UBOOL Value,INT InNumBits)
	{
		Empty(InNumBits);
		if(InNumBits)
		{
			NumBits = InNumBits;
			appMemset(GetData(),Value ? 0xff : 0,(NumBits + NumBitsPerDWORD - 1) / NumBitsPerDWORD * sizeof(DWORD));
		}
	}
	
	/**
	 * Removes bits from the array.
	 * @param BaseIndex - The index of the first bit to remove.
	 * @param NumBitsToRemove - The number of consecutive bits to remove.
	 */
	void Remove(INT BaseIndex,INT NumBitsToRemove = 1)
	{
		//check(BaseIndex >= 0 && BaseIndex + NumBitsToRemove <= NumBits);
		
		// Until otherwise necessary, this is an obviously correct implementation rather than an efficient implementation.
		NIterator WriteIt(*this);
		for(NConstIterator ReadIt(*this);ReadIt;++ReadIt)
		{
			// If this bit isn't being removed, write it back to the array at its potentially new index.
			if(ReadIt.GetIndex() < BaseIndex || ReadIt.GetIndex() >= BaseIndex + NumBitsToRemove)
			{
				if(WriteIt.GetIndex() != ReadIt.GetIndex())
				{
					WriteIt.GetValue() = (UBOOL)ReadIt.GetValue();
				}
				++WriteIt;
			}
		}
		NumBits -= NumBitsToRemove;
	}
	
	/** 
	 * Helper function to return the amount of memory allocated by this container 
	 * @return number of bytes allocated by this container
	 */
	DWORD GetAllocatedSize( void ) const
	{
		return (MaxBits / NumBitsPerDWORD) * sizeof(DWORD);
	}
	
	// Accessors.
	FORCEINLINE INT Num() const { return NumBits; }
	FORCEINLINE NBitReference operator()(INT Index)
	{
		/*checkSlowish*///appCheckSlow(Index>=0 && Index<NumBits);
		return NBitReference(
							 GetData()[Index / NumBitsPerDWORD],
							 1 << (Index & (NumBitsPerDWORD - 1))
							 );
	}
	FORCEINLINE const NConstBitReference operator()(INT Index) const
	{
		/*checkSlowish*///appCheckSlow(Index>=0 && Index<NumBits);
		return NConstBitReference(
								  GetData()[Index / NumBitsPerDWORD],
								  1 << (Index & (NumBitsPerDWORD - 1))
								  );
	}
	FORCEINLINE NBitReference AccessCorrespondingBit(const NRelativeBitReference& RelativeReference)
	{
		//appCheckSlow(RelativeReference.Mask);
		//appCheckSlow(RelativeReference.DWORDIndex >= 0);
		//appCheckSlow(((UINT)RelativeReference.DWORDIndex + 1) * NumBitsPerDWORD - 1 - appCountLeadingZeros(RelativeReference.Mask) < (UINT)NumBits);
		return NBitReference(
							 GetData()[RelativeReference.DWORDIndex],
							 RelativeReference.Mask
							 );
	}
	FORCEINLINE const NConstBitReference AccessCorrespondingBit(const NRelativeBitReference& RelativeReference) const
	{
		//appCheckSlow(RelativeReference.Mask);
		//appCheckSlow(RelativeReference.DWORDIndex >= 0);
		//appCheckSlow(((UINT)RelativeReference.DWORDIndex + 1) * NumBitsPerDWORD - 1 - appCountLeadingZeros(RelativeReference.Mask) < (UINT)NumBits);
		return NConstBitReference(
								  GetData()[RelativeReference.DWORDIndex],
								  RelativeReference.Mask
								  );
	}
	
	/** BitArray iterator. */
	class NIterator : public NRelativeBitReference
	{
	public:
		NIterator(TBitArray<Allocator>& InArray,INT StartIndex = 0)
		:	NRelativeBitReference(StartIndex)
		,	Array(InArray)
		,	Index(StartIndex)
		{
		}
		FORCEINLINE NIterator& operator++()
		{
			++Index;
			this->Mask <<= 1;
			if(!this->Mask)
			{
				// Advance to the next DWORD.
				this->Mask = 1;
				++this->DWORDIndex;
			}
			return *this;
		}
		/** conversion to "bool" returning TRUE if the iterator is valid. */
		typedef bool PrivateBooleanType;
		FORCEINLINE operator PrivateBooleanType() const { return Index < Array.Num() ? &NIterator::Index : NULL; }
		FORCEINLINE bool operator !() const { return !operator PrivateBooleanType(); }
		
		FORCEINLINE NBitReference GetValue() const { return NBitReference(Array.GetData()[this->DWORDIndex],this->Mask); }
		FORCEINLINE INT GetIndex() const { return Index; }
	private:
		TBitArray<Allocator>& Array;
		INT Index;
	};
	
	/** Const BitArray iterator. */
	class NConstIterator : public NRelativeBitReference
	{
	public:
		NConstIterator(const TBitArray<Allocator>& InArray,INT StartIndex = 0)
		:	NRelativeBitReference(StartIndex)
		,	Array(InArray)
		,	Index(StartIndex)
		{
		}
		FORCEINLINE NConstIterator& operator++()
		{
			++Index;
			this->Mask <<= 1;
			if(!this->Mask)
			{
				// Advance to the next DWORD.
				this->Mask = 1;
				++this->DWORDIndex;
			}
			return *this;
		}
		/** conversion to "bool" returning TRUE if the iterator is valid. */
		typedef bool PrivateBooleanType;
		FORCEINLINE operator PrivateBooleanType() const { return Index < Array.Num() ? &NConstIterator::Index : NULL; }
		FORCEINLINE bool operator !() const { return !operator PrivateBooleanType(); }
		
		FORCEINLINE NConstBitReference GetValue() const { return NConstBitReference(Array.GetData()[this->DWORDIndex],this->Mask); }
		FORCEINLINE INT GetIndex() const { return Index; }
	private:
		const TBitArray<Allocator>& Array;
		INT Index;
	};
	
private:
	INT NumBits;
	INT MaxBits;
	
	FORCEINLINE const DWORD* GetData() const
	{
		return (DWORD*)this->GetAllocation();
	}
	
	FORCEINLINE DWORD* GetData()
	{
		return (DWORD*)this->GetAllocation();
	}
	
	void Realloc(INT PreviousNumBits)
	{
		const INT PreviousNumDWORDs = (PreviousNumBits + NumBitsPerDWORD - 1) / NumBitsPerDWORD;
		const INT MaxDWORDs = (MaxBits + NumBitsPerDWORD - 1) / NumBitsPerDWORD;
		
		this->ResizeAllocation(PreviousNumDWORDs,MaxDWORDs,sizeof(DWORD));
		
		if(MaxDWORDs)
		{
			// Reset the newly allocated slack DWORDs.
			appMemzero((DWORD*)this->GetAllocation() + PreviousNumDWORDs,(MaxDWORDs - PreviousNumDWORDs) * sizeof(DWORD));
		}
	}
};

/** An iterator which only iterates over set bits. */
template<typename Allocator>
class TConstSetBitIterator : public NRelativeBitReference
{
public:
	
	/** Constructor. */
	TConstSetBitIterator(const TBitArray<Allocator>& InArray,INT StartIndex = 0)
	:	NRelativeBitReference(StartIndex)
	,	Array(InArray)
	,	UnvisitedBitMask((~0) << (StartIndex & (NumBitsPerDWORD - 1)))
	,	CurrentBitIndex(StartIndex)
	,	BaseBitIndex(StartIndex & ~(NumBitsPerDWORD - 1))
	{
		FindFirstSetBit();
	}
	
	/** Advancement operator. */
	FORCEINLINE TConstSetBitIterator& operator++()
	{
		// Mark the current bit as visited.
		UnvisitedBitMask &= ~this->Mask;
		
		// Find the first set bit that hasn't been visited yet.
		FindFirstSetBit();
		
		return *this;
	}
	
	/** conversion to "bool" returning TRUE if the iterator is valid. */
	typedef bool PrivateBooleanType;
	FORCEINLINE operator PrivateBooleanType() const { return CurrentBitIndex < Array.Num() ? &TConstSetBitIterator::CurrentBitIndex : NULL; }
	FORCEINLINE bool operator !() const { return !operator PrivateBooleanType(); }
	
	/** Index accessor. */
	FORCEINLINE INT GetIndex() const
	{
		return CurrentBitIndex;
	}
	
private:
	
	const TBitArray<Allocator>& Array;
	
	DWORD UnvisitedBitMask;
	INT CurrentBitIndex;
	INT BaseBitIndex;
	
	/** Find the first set bit starting with the current bit, inclusive. */
	void FindFirstSetBit()
	{
		static const DWORD EmptyArrayData = 0;
		const DWORD* ArrayData = IfAThenAElseB(Array.GetData(),&EmptyArrayData);
		
		// We're almost guaranteed to hit a load-hit-store in the while(...)
		// Issue an opportunistic prefetch; best case scenario is we offset the L2 miss by the guaranteed LHS
		//CONSOLE_PREFETCH(ArrayData);
		
		// Advance to the next non-zero DWORD.
		DWORD RemainingBitMask = ArrayData[this->DWORDIndex] & UnvisitedBitMask;
		while(!RemainingBitMask)
		{
			this->DWORDIndex++;
			BaseBitIndex += NumBitsPerDWORD;
			const INT LastDWORDIndex = (Array.Num() - 1) / NumBitsPerDWORD;
			if(this->DWORDIndex <= LastDWORDIndex)
			{
				RemainingBitMask = ArrayData[this->DWORDIndex];
				UnvisitedBitMask = ~0;
			}
			else
			{
				// We've advanced past the end of the array.
				CurrentBitIndex = Array.Num();
				return;
			}
		};
		
		// We can assume that RemainingBitMask!=0 here.
		//appCheckSlow(RemainingBitMask);
		
		// This operation has the effect of unsetting the lowest set bit of BitMask
		const DWORD NewRemainingBitMask = RemainingBitMask & (RemainingBitMask - 1);
		
		// This operation XORs the above mask with the original mask, which has the effect
		// of returning only the bits which differ; specifically, the lowest bit
		this->Mask = NewRemainingBitMask ^ RemainingBitMask;
		
		// If the Nth bit was the lowest set bit of BitMask, then this gives us N
		CurrentBitIndex = BaseBitIndex + NumBitsPerDWORD - 1 - appCountLeadingZeros(this->Mask);
	}
};

/** An iterator which only iterates over the bits which are set in both of two bit-arrays. */
template<typename Allocator,typename OtherAllocator>
class TConstDualSetBitIterator : public NRelativeBitReference
{
public:
	
	/** Constructor. */
	TConstDualSetBitIterator(
							 const TBitArray<Allocator>& InArrayA,
							 const TBitArray<OtherAllocator>& InArrayB,
							 INT StartIndex = 0
							 )
	:	NRelativeBitReference(StartIndex)
	,	ArrayA(InArrayA)
	,	ArrayB(InArrayB)
	,	UnvisitedBitMask((~0) << (StartIndex & (NumBitsPerDWORD - 1)))
	,	CurrentBitIndex(StartIndex)
	,	BaseBitIndex(StartIndex & ~(NumBitsPerDWORD - 1))
	{
		check(ArrayA.Num() == ArrayB.Num());
		
		FindFirstSetBit();
	}
	
	/** Advancement operator. */
	FORCEINLINE TConstDualSetBitIterator& operator++()
	{
		//appCheckSlow(ArrayA.Num() == ArrayB.Num());
		
		// Mark the current bit as visited.
		UnvisitedBitMask &= ~this->Mask;
		
		// Find the first set bit that hasn't been visited yet.
		FindFirstSetBit();
		
		return *this;
		
	}
	
	/** conversion to "bool" returning TRUE if the iterator is valid. */
	typedef bool PrivateBooleanType;
	FORCEINLINE operator PrivateBooleanType() const { return CurrentBitIndex < ArrayA.Num() ? &TConstDualSetBitIterator::CurrentBitIndex : NULL; }
	FORCEINLINE bool operator !() const { return !operator PrivateBooleanType(); }
	
	/** Index accessor. */
	FORCEINLINE INT GetIndex() const
	{
		return CurrentBitIndex;
	}
	
private:
	
	const TBitArray<Allocator>& ArrayA;
	const TBitArray<OtherAllocator>& ArrayB;
	
	DWORD UnvisitedBitMask;
	INT CurrentBitIndex;
	INT BaseBitIndex;
	
	/** Find the first bit that is set in both arrays, starting with the current bit, inclusive. */
	void FindFirstSetBit()
	{
		static const DWORD EmptyArrayData = 0;
		const DWORD* ArrayDataA = IfAThenAElseB(ArrayA.GetData(),&EmptyArrayData);
		const DWORD* ArrayDataB = IfAThenAElseB(ArrayB.GetData(),&EmptyArrayData);
		
		// Advance to the next non-zero DWORD.
		DWORD RemainingBitMask = ArrayDataA[this->DWORDIndex] & ArrayDataB[this->DWORDIndex] & UnvisitedBitMask;
		while(!RemainingBitMask)
		{
			this->DWORDIndex++;
			BaseBitIndex += NumBitsPerDWORD;
			const INT LastDWORDIndex = (ArrayA.Num() - 1) / NumBitsPerDWORD;
			if(this->DWORDIndex <= LastDWORDIndex)
			{
				RemainingBitMask = ArrayDataA[this->DWORDIndex] & ArrayDataB[this->DWORDIndex];
				UnvisitedBitMask = ~0;
			}
			else
			{
				// We've advanced past the end of the array.
				CurrentBitIndex = ArrayA.Num();
				return;
			}
		};
		
		// We can assume that RemainingBitMask!=0 here.
		//appCheckSlow(RemainingBitMask);
		
		// This operation has the effect of unsetting the lowest set bit of BitMask
		const DWORD NewRemainingBitMask = RemainingBitMask & (RemainingBitMask - 1);
		
		// This operation XORs the above mask with the original mask, which has the effect
		// of returning only the bits which differ; specifically, the lowest bit
		this->Mask = NewRemainingBitMask ^ RemainingBitMask;
		
		// If the Nth bit was the lowest set bit of BitMask, then this gives us N
		CurrentBitIndex = BaseBitIndex + NumBitsPerDWORD - 1 - appCountLeadingZeros(this->Mask);
	}
};

/** A specialization of the exchange macro that avoids reallocating when exchanging two bit arrays. */
template<typename Allocator>
inline void Exchange(TBitArray<Allocator>& A,TBitArray<Allocator>& B)
{
	appMemswap(&A,&B,sizeof(TBitArray<Allocator>));
}


#endif	// __TBITARRAY_H__