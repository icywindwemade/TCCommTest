/*
*  ContainerAllocationPolicies.h
*  Core
*
*  Created by Youngmin Kim on 10. 07. 25.
*  Copyright 2010 SwallowStudio. All rights reserved.
*
*/
#ifndef __CONTAINERALLOCATIONPOLICIES_H__
#define __CONTAINERALLOCATIONPOLICIES_H__

/** The default slack calculation heuristic. */
//extern INT DefaultCalculateSlack(INT NumElements,INT NumAllocatedElements,UINT BytesPerElement);

/** Whether array slack is being tracked. */
#define TRACK_ARRAY_SLACK	0

inline INT DefaultCalculateSlack(INT NumElements,INT NumAllocatedElements,UINT BytesPerElement)
{
	if (NumElements < NumAllocatedElements)
	{
		// If the container has too much slack, shrink it to exactly fit the number of elements.
		const UINT CurrentSlackElements = NumAllocatedElements - NumElements;
		const UINT CurrentSlackBytes = (NumAllocatedElements - NumElements) * BytesPerElement;
		const UBOOL bTooManySlackBytes = CurrentSlackBytes >= 16384;
		const UBOOL bTooManySlackElements = 3 * NumElements < 2 * NumAllocatedElements;
		if ((bTooManySlackBytes || bTooManySlackElements) && (CurrentSlackElements > 64 || !NumElements))
		{
			return NumElements;
		}
		else
		{
			return NumAllocatedElements;
		}
	}
	else if (NumElements > 0)
	{
		// Allocate slack for the array proportional to its size.
		return NumElements + 3 * NumElements / 8 + 16;
	}
	else
	{
		return 0;
	}
}


/** Used to determine the alignment of an element type. */
template<typename ElementType>
class TElementAlignmentCalculator
{
private:
	
	// We disable this warning as we're intentionally expecting packing to affect the alignment
#pragma warning(push)
#pragma warning(disable : 4121) // alignment of a member was sensitive to packing
	
	/**
	 * This is a dummy type that's used to calculate the padding added between the byte and the element
	 * to fulfill the type's required alignment.
	 */
	struct NAlignedElements
	{
		BYTE MisalignmentPadding;
		ElementType Element;
		
		/** FAlignedElement's default constructor is declared but never implemented to avoid the need for a ElementType default constructor. */
		NAlignedElements();
	};
	
#pragma warning(pop)
	
public:
	
	enum { Alignment = sizeof(NAlignedElements) - sizeof(ElementType) };
};

/** A type which is used to represent a script type that is unknown at compile time. */
struct NScriptContainerElement
{
};


/**
 * Used to declare an untyped array of data with compile-time alignment.
 * It needs to use template specialization as the MS_ALIGN and GCC_ALIGN macros require literal parameters.
 */
template<INT Size,DWORD Alignment>
class TAlignedBytes
{
	BYTE Data[-Size]; // this intentionally won't compile, we don't support the requested alignment
};

/** Unaligned storage. */
template<INT Size>
struct TAlignedBytes<Size,1>
{
	BYTE Pad[Size];
};


//// C++/CLI doesn't support alignment of native types in managed code, so we enforce that the element
//// size is a multiple of the desired alignment
//#ifdef __cplusplus_cli
//	#define IMPLEMENT_ALIGNED_STORAGE(Align) \
//	template<INT Size>        \
//	struct TAlignedBytes<Size,Align> \
//	{ \
//		BYTE Pad[Size]; \
//		checkAtCompileTime( Size % Align == 0, CLRInteropTypesMustNotBeAligned ); \
//	};
//#else
	/** A macro that implements TAlignedBytes for a specific alignment. */
	#define IMPLEMENT_ALIGNED_STORAGE(Align) \
	template<INT Size>        \
	struct TAlignedBytes<Size,Align> \
	{ \
		struct MS_ALIGN(Align) TPadding \
		{ \
			BYTE Pad[Size]; \
		} GCC_ALIGN(Align); \
		TPadding Padding; \
	};
//#endif

// Implement TAlignedBytes for these alignments.
IMPLEMENT_ALIGNED_STORAGE(16);
IMPLEMENT_ALIGNED_STORAGE(8);
IMPLEMENT_ALIGNED_STORAGE(4);
IMPLEMENT_ALIGNED_STORAGE(2);

#undef IMPLEMENT_ALIGNED_STORAGE


/** An untyped array of data with compile-time alignment and size derived from another type. */
template<typename ElementType>
class TTypeCompatibleBytes :
public TAlignedBytes<
sizeof(ElementType),
TElementAlignmentCalculator<ElementType>::Alignment
>
{};


/** The indirect allocation policy always allocates the elements indirectly. */
class NHeapAllocator
{
public:
	
	enum { NeedsElementType = FALSE };
	
	class ForAnyElementType
	{
	public:
		
		/** Default constructor. */
		ForAnyElementType()
		: Data(NULL)
		{}
		
		/** ENoInit constructor. */
		ForAnyElementType(ENoInit)
		{}
		
		/** Destructor. */
		~ForAnyElementType()
		{
			if(Data)
			{
				appFree(Data);
			}
		}
		
		// NContainerAllocatorInterface
		FORCEINLINE NScriptContainerElement* GetAllocation() const
		{
			return Data;
		}
		void ResizeAllocation(INT PreviousNumElements,INT NumElements,INT NumBytesPerElement)
		{
			// Avoid calling appRealloc( NULL, 0 ) as ANSI C mandates returning a valid pointer which is not what we want.
			if( Data || NumElements )
			{
				//checkSlow(((QWORD)NumElements*(QWORD)ElementTypeInfo.GetSize() < (QWORD)INT_MAX));
				Data = (NScriptContainerElement*)appRealloc( Data, NumElements*NumBytesPerElement, DEFAULT_ALIGNMENT );
			}
		}
		INT CalculateSlack(INT NumElements,INT NumAllocatedElements,INT NumBytesPerElement) const
		{
			return DefaultCalculateSlack(NumElements,NumAllocatedElements,NumBytesPerElement);
		}
		
	private:
		
		/** A pointer to the container's elements. */
		NScriptContainerElement* Data;
	};
	
	template<typename ElementType>
	class ForElementType : public ForAnyElementType
	{
	public:
		
		/** Default constructor. */
		ForElementType()
		{}
		
		/** ENoInit constructor. */
		ForElementType(ENoInit)
		:	ForAnyElementType(E_NoInit)
		{}
		
		FORCEINLINE ElementType* GetAllocation() const
		{
			return (ElementType*)ForAnyElementType::GetAllocation();
		}
	};
};

/** The indirect allocation policy with DEFAULT_ALIGNMENT is used by default. */
class NDefaultAllocator : public NHeapAllocator
{
};


/**
 * The inline allocation policy allocates up to a specified number of bytes in the same allocation as the container.
 * Any allocation needed beyond that causes all data to be moved into an indirect allocation.
 * It always uses DEFAULT_ALIGNMENT.
 */
template<UINT NumInlineElements,typename SecondaryAllocator = NDefaultAllocator >
class TInlineAllocator
{
public:
	
	enum { NeedsElementType = TRUE };
	
	template<typename ElementType>
	class ForElementType
	{
	public:
		
		/** Default constructor. */
		ForElementType()
		{
		}
		
		/** ENoInit constructor. */
		ForElementType(ENoInit)
		{}
		
		/** Destructor. */
		~ForElementType()
		{
		}
		
		// NContainerAllocatorInterface
		FORCEINLINE ElementType* GetAllocation() const
		{
			return IfAThenAElseB<ElementType>(SecondaryData.GetAllocation(),GetInlineElements());
		}
		void ResizeAllocation(INT PreviousNumElements,INT NumElements,INT NumBytesPerElement)
		{
			const INT PreviousNumBytes = PreviousNumElements * NumBytesPerElement;
			
			// Check if the new allocation will fit in the inline data area.
			if(NumElements <= NumInlineElements)
			{
				// If the old allocation wasn't in the inline data area, move it into the inline data area.
				if(SecondaryData.GetAllocation())
				{
					appMemcpy(GetInlineElements(),SecondaryData.GetAllocation(),PreviousNumBytes);
					
					// Free the old indirect allocation.
					SecondaryData.ResizeAllocation(0,0,NumBytesPerElement);
				}
			}
			else
			{
				if(!SecondaryData.GetAllocation())
				{
					// Allocate new indirect memory for the data.
					SecondaryData.ResizeAllocation(0,NumElements,NumBytesPerElement);
					
					// Move the data out of the inline data area into the new allocation.
					appMemcpy(SecondaryData.GetAllocation(),GetInlineElements(),PreviousNumBytes);
				}
				else
				{
					// Reallocate the indirect data for the new size.
					SecondaryData.ResizeAllocation(PreviousNumElements,NumElements,NumBytesPerElement);
				}
			}
		}
		INT CalculateSlack(INT NumElements,INT NumAllocatedElements,INT NumBytesPerElement) const
		{
			// If the elements use less space than the inline allocation, only use the inline allocation as slack.
			return NumElements <= NumInlineElements ?
			NumInlineElements :
			SecondaryData.CalculateSlack(NumElements,NumAllocatedElements,NumBytesPerElement);
		}
		
	private:
		
		/** The data is stored in this array if less than NumInlineBytes is needed. */
		TTypeCompatibleBytes<ElementType> InlineData[NumInlineElements];
		
		/** The data is allocated through the indirect allocation policy if more than NumInlineBytes is needed. */
		typename SecondaryAllocator::template ForElementType<ElementType> SecondaryData;
		
		/** @return the base of the aligned inline element data */
		ElementType* GetInlineElements() const
		{
			return (ElementType*)InlineData;
		}
	};
	
	typedef ForElementType<NScriptContainerElement> ForAnyElementType;
};

/** Bit arrays use a 4 DWORD inline allocation policy by default. */
class NDefaultBitArrayAllocator : public TInlineAllocator<4>
{
};




enum ENumBitsPerDWORD		{ NumBitsPerDWORD = 32		};
enum ENumBitsPerDWORDLogTwo	{ NumBitsPerDWORDLogTwo = 5	};

//
// Sparse array allocation definitions
//

/** Encapsulates the allocators used by a sparse array in a single type. */
template<typename InElementAllocator = NDefaultAllocator,typename InBitArrayAllocator = NDefaultBitArrayAllocator>
class TSparseArrayAllocator
{
public:
	
	typedef InElementAllocator ElementAllocator;
	typedef InBitArrayAllocator BitArrayAllocator;
};

/** An inline sparse array allocator that allows sizing of the inline allocations for a set number of elements. */
template<
UINT NumInlineElements,
typename SecondaryAllocator = TSparseArrayAllocator<NDefaultAllocator,NDefaultAllocator>
>
class TInlineSparseArrayAllocator
{
private:
	
	/** The size to allocate inline for the bit array. */
	enum { InlineBitArrayDWORDs = (NumInlineElements + NumBitsPerDWORD - 1) / NumBitsPerDWORD};
	
public:
	
	typedef TInlineAllocator<NumInlineElements,typename SecondaryAllocator::ElementAllocator>		ElementAllocator;
	typedef TInlineAllocator<InlineBitArrayDWORDs,typename SecondaryAllocator::BitArrayAllocator>	BitArrayAllocator;
};

class NDefaultSparseArrayAllocator : public TSparseArrayAllocator<>
{
};

//
// Set allocation definitions.
//

#define DEFAULT_NUMBER_OF_ELEMENTS_PER_HASH_BUCKET	2
#define DEFAULT_BASE_NUMBER_OF_HASH_BUCKETS			8
#define DEFAULT_MIN_NUMBER_OF_HASHED_ELEMENTS		4

/** Encapsulates the allocators used by a set in a single type. */
template<
	typename InSparseArrayAllocator = TSparseArrayAllocator<>,
	typename InHashAllocator = TInlineAllocator<1,NDefaultAllocator>,
	UINT AverageNumberOfElementsPerHashBucket = DEFAULT_NUMBER_OF_ELEMENTS_PER_HASH_BUCKET,
	UINT BaseNumberOfHashBuckets = DEFAULT_BASE_NUMBER_OF_HASH_BUCKETS,
	UINT MinNumberOfHashedElements = DEFAULT_MIN_NUMBER_OF_HASHED_ELEMENTS
>
class TSetAllocator
{
public:
	
	/** Computes the number of hash buckets to use for a given number of elements. */
	static FORCEINLINE UINT GetNumberOfHashBuckets(UINT NumHashedElements)
	{
		if(NumHashedElements >= MinNumberOfHashedElements)
		{
			return appRoundUpToPowerOfTwo(NumHashedElements / AverageNumberOfElementsPerHashBucket + BaseNumberOfHashBuckets);
		}
		else
		{
			return 1;
		}
	}
	
	typedef InSparseArrayAllocator SparseArrayAllocator;
	typedef InHashAllocator HashAllocator;
};

/** An inline set allocator that allows sizing of the inline allocations for a set number of elements. */
template<
UINT NumInlineElements,
typename SecondaryAllocator = TSetAllocator<TSparseArrayAllocator<NDefaultAllocator,NDefaultAllocator>,NDefaultAllocator>,
UINT AverageNumberOfElementsPerHashBucket = DEFAULT_NUMBER_OF_ELEMENTS_PER_HASH_BUCKET,
UINT MinNumberOfHashedElements = DEFAULT_MIN_NUMBER_OF_HASHED_ELEMENTS
>
class TInlineSetAllocator
{
private:
	
	enum { NumInlineHashBuckets = (NumInlineElements + AverageNumberOfElementsPerHashBucket - 1) / AverageNumberOfElementsPerHashBucket };
	
public:
	
	/** Computes the number of hash buckets to use for a given number of elements. */
	static FORCEINLINE UINT GetNumberOfHashBuckets(UINT NumHashedElements)
	{
		const UINT NumDesiredHashBuckets = appRoundUpToPowerOfTwo(NumHashedElements / AverageNumberOfElementsPerHashBucket);
		if(NumDesiredHashBuckets < NumInlineHashBuckets)
		{
			return NumInlineHashBuckets;
		}
		else if(NumHashedElements < MinNumberOfHashedElements)
		{
			return NumInlineHashBuckets;
		}
		else
		{
			return NumDesiredHashBuckets;
		}
	}
	
	typedef TInlineSparseArrayAllocator<NumInlineElements,typename SecondaryAllocator::SparseArrayAllocator> SparseArrayAllocator;
	typedef TInlineAllocator<NumInlineHashBuckets,typename SecondaryAllocator::HashAllocator> HashAllocator;
};

class NDefaultSetAllocator : public TSetAllocator<>
{
};

#endif	// __CONTAINERALLOCATIONPOLICIES_H__