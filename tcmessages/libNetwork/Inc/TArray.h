/**
 * @file	TArray.h
 *
 * Declares the array class.
 */
#ifndef __TARRAY_H__
#define __TARRAY_H__

#include "Core.h"

#ifndef check
#define check(x)	assert(x)
#endif	// check

#ifndef appCheckSlow
#define appCheckSlow(x)	assert(x)
#endif	// appCheckSlow

/**
 * Generic non-const iterator which can operate on types that expose the following:
 * - A type called ElementType representing the contained type.
 * - A method IndexType Num() const that returns the number of items in the container.
 * - A method UBOOL IsValidIndex(IndexType index) which returns whether a given index is valid in the container.
 * - A method T& operator(IndexType index) which returns a reference to a contained object by index.
 */
template< typename ContainerType, typename IndexType = INT >
class TIndexedContainerIterator
{
public:
	typedef typename ContainerType::ElementType ElementType;
	
	TIndexedContainerIterator(ContainerType& InContainer)
	:	Container( InContainer )
	,	Index(0)
	{
	}
	
	/** Advances iterator to the next element in the container. */
	TIndexedContainerIterator& operator++()
	{
		++Index;
		return *this;
	}
	TIndexedContainerIterator operator++(int)
	{
		TIndexedContainerIterator Tmp(*this);
		++Index;
		return Tmp;
	}
	
	/** Moves iterator to the previous element in the container. */
	TIndexedContainerIterator& operator--()
	{
		--Index;
		return *this;
	}
	TIndexedContainerIterator operator--(int)
	{
		TIndexedContainerIterator Tmp(*this);
		--Index;
		return Tmp;
	}
	
	/** pointer arithmetic support */
	TIndexedContainerIterator& operator+=(INT Offset)
	{
		Index += Offset;
		return *this;
	}
	
	TIndexedContainerIterator operator+(INT Offset) const
	{
		TIndexedContainerIterator Tmp(*this);
		return Tmp += Offset;
	}
	
	TIndexedContainerIterator& operator-=(INT Offset)
	{
		return *this += -Offset;
	}
	
	TIndexedContainerIterator operator-(INT Offset) const
	{
		TIndexedContainerIterator Tmp(*this);
		return Tmp -= Offset;
	}
	
	TIndexedContainerIterator operator()(INT Offset) const
	{
		return *this + Offset;
	}
	
	/** @name Element access */
	//@{
	ElementType& operator* () const
	{
		return Container( Index );
	}
	
	ElementType* operator-> () const
	{
		return &Container( Index );
	}
	//@}
	
	/** conversion to "UBOOL" returning TRUE if the iterator has not reached the last element. */
	typedef UBOOL PrivateBooleanType;
	operator PrivateBooleanType() const
	{
		return Container.IsValidIndex(Index) ? &TIndexedContainerIterator::Index : NULL;
	}
	
	/** inverse of the "UBOOL" operator */
	UBOOL operator !() const
	{
		return !operator PrivateBooleanType();
	}
	
	/** Returns an index to the current element. */
	IndexType GetIndex() const
	{
		return Index;
	}
	
	/** Resets the iterator to the first element. */
	void Reset()
	{
		Index = 0;
	}
private:
	ContainerType&	Container;
	IndexType		Index;
};

/** operator + */
template< typename ContainerType, typename IndexType >
TIndexedContainerIterator<ContainerType, IndexType> operator+(
															  INT Offset,
															  TIndexedContainerIterator<ContainerType, IndexType> RHS)
{
	return RHS + Offset;
}



/**
 * Generic const iterator which can operate on types that expose the following:
 * - A type called ElementType representing the contained type.
 * - A method IndexType Num() const that returns the number of items in the container.
 * - A method UBOOL IsValidIndex(IndexType index) which returns whether a given index is valid in the container.
 * - A method T& operator(IndexType index) const which returns a reference to a contained object by index.
 */
template< typename ContainerType, typename IndexType = INT >
class TIndexedContainerConstIterator
{
public:
	typedef typename ContainerType::ElementType ElementType;
	
	TIndexedContainerConstIterator(const ContainerType& InContainer)
	:	Container( InContainer )
	,	Index(0)
	{
	}
	
	/** Advances iterator to the next element in the container. */
	TIndexedContainerConstIterator& operator++()
	{
		++Index;
		return *this;
	}
	TIndexedContainerConstIterator operator++(int)
	{
		TIndexedContainerConstIterator Tmp(*this);
		++Index;
		return Tmp;
	}
	
	/** Moves iterator to the previous element in the container. */
	TIndexedContainerConstIterator& operator--()
	{
		--Index;
		return *this;
	}
	TIndexedContainerConstIterator operator--(int)
	{
		TIndexedContainerConstIterator Tmp(*this);
		--Index;
		return Tmp;
	}
	
	/** iterator arithmetic support */
	TIndexedContainerConstIterator& operator+=(INT Offset)
	{
		Index += Offset;
		return *this;
	}
	
	TIndexedContainerConstIterator operator+(INT Offset) const
	{
		TIndexedContainerConstIterator Tmp(*this);
		return Tmp += Offset;
	}
	
	TIndexedContainerConstIterator& operator-=(INT Offset)
	{
		return *this += -Offset;
	}
	
	TIndexedContainerConstIterator operator-(INT Offset) const
	{
		TIndexedContainerConstIterator Tmp(*this);
		return Tmp -= Offset;
	}
	
	TIndexedContainerConstIterator operator()(INT Offset) const
	{
		return *this + Offset;
	}
	
	/** @name Element access */
	//@{
	const ElementType& operator* () const
	{
		return Container( Index );
	}
	
	const ElementType* operator-> () const
	{
		return &Container( Index );
	}
	//@}
	
	/** conversion to "UBOOL" returning TRUE if the iterator has not reached the last element. */
	typedef UBOOL PrivateBooleanType;
	operator PrivateBooleanType() const
	{
		return Container.IsValidIndex(Index) ? &TIndexedContainerConstIterator::Index : NULL;
	}
	
	/** inverse of the "UBOOL" operator */
	UBOOL operator !() const
	{
		return !operator PrivateBooleanType();
	}
	
	/** Returns an index to the current element. */
	IndexType GetIndex() const
	{
		return Index;
	}
	
	/** Resets the iterator to the first element. */
	void Reset()
	{
		Index = 0;
	}
private:
	const ContainerType&	Container;
	IndexType				Index;
};

/** operator + */
template< typename ContainerType, typename IndexType >
TIndexedContainerConstIterator<ContainerType, IndexType> operator+(
																   INT Offset,
																   TIndexedContainerConstIterator<ContainerType, IndexType> RHS)
{
	return RHS + Offset;
}

/**
 * Template dynamic array
 * A dynamic sized array of typed elements. Makes the assumption that your elements are relocated-able;
 * i.e. that they can be transparently moved to new memory without a copy constructor. The main implication
 * is that pointers to elements in the TArray may be invalidated by adding or removing other elements to the array.
 * Removal of elements is O(N) and invalidates the indices of subsequent elements.
 */
template<typename InElementType,typename Allocator>
class TArray
{
public:
	typedef InElementType ElementType;
	
	TArray()
	: ArrayNum(0)
	, ArrayMax(0)
	{}
	
	TArray(ENoInit)
	: AllocatorInstance(E_NoInit)
	{}
	
	explicit TArray(INT InNum)
	: ArrayNum(InNum)
	, ArrayMax(InNum)
	{
		AllocatorInstance.ResizeAllocation(0, ArrayMax, sizeof(ElementType));
	}
	
	/**
	 * Copy constructor. Use the common routine to perform the copy.
	 * @param Other the source array to copy
	 */
	template<typename OtherAllocator>
	TArray(const TArray<ElementType, OtherAllocator>& Other)
	: ArrayNum(0)
	, ArrayMax(0)
	{
		Copy(Other);
	}
	
	TArray(const TArray<ElementType, Allocator>& Other)
	: ArrayNum(0)
	, ArrayMax(0)
	{
		Copy(Other);
	}
	
	~TArray()
	{
		check(ArrayNum >= 0);
		check(ArrayMax >= ArrayNum);
		DestructItems(0, ArrayNum);
		ArrayNum = ArrayMax = 0;
		
#if defined(_MSC_VER)
		// ensure that DebugGet gets instantiated.
		// @todo it would be nice if we had a cleaner solution for DebugGet()
		//volatile const ElementType* Dummy = &DebugGet(0);
#endif
	}
	
	/**
	 * Helper function for returning a typed pointer to the first array entry.
	 * @return Pointer to first array entry or NULL if ArrayMax == 0
	 */
	FORCEINLINE ElementType* GetTypedData()
	{
		return (ElementType*)AllocatorInstance.GetAllocation();
	}
	
	FORCEINLINE ElementType* GetData()
	{
		return (ElementType*)AllocatorInstance.GetAllocation();
	}
	
	/**
	 * Helper function for returning a typed pointer to the first array entry.
	 *
	 * @return Pointer to first array entry or NULL if ArrayMax == 0
	 */
	FORCEINLINE const ElementType* GetTypedData() const
	{
		return (ElementType*)AllocatorInstance.GetAllocation();
	}
	
	FORCEINLINE const ElementType* GetData() const
	{
		return (ElementType*)AllocatorInstance.GetAllocation();
	}
	
	/**
	 * Helper function returning the size of the inner type.
	 *
	 * @return Size in bytes of array type
	 */
	FORCEINLINE DWORD GetTypeSize() const
	{
		return sizeof(ElementType);
	}
	
	/**
	 * Helper function to return the amount of memory allocated by this container.
	 *
	 * @return Number of bytes allocated by this container
	 */
	FORCEINLINE DWORD GetAllocatedSize(void) const
	{
		return (ArrayMax * sizeof(InElementType));
	}
	
	/**
	 * @return The amount of slack in this array in elements.
	 */
	INT GetSlack() const
	{
		return ArrayMax - ArrayNum;
	}
	
	FORCEINLINE UBOOL IsValidIndex(INT i) const
	{
		return i >= 0 && i < ArrayNum;
	}
	
	FORCEINLINE INT Num() const
	{
		appCheckSlow(ArrayNum >= 0);
		appCheckSlow(ArrayMax >= ArrayNum);
		return ArrayNum;
	}
	
	FORCEINLINE ElementType& operator()(INT i)
	{
		appCheckSlow(i >= 0 && (i < ArrayNum || (i == 0 && ArrayNum == 0)));	// (i == 0 && ArrayNum == 0) is workaround for &MyArray(0) abuse
		appCheckSlow(ArrayMax >= ArrayNum);
		return GetTypedData()[i];
	}
	
	FORCEINLINE const ElementType& operator()(INT i) const
	{
		appCheckSlow(i >= 0 && (i < ArrayNum || (i == 0 && ArrayNum == 0)));	// (i == 0 && ArrayNum == 0) is workaround for &MyArray(0) abuse
		appCheckSlow(ArrayMax >= ArrayNum);
		return GetTypedData()[i];
	}
	
	ElementType Pop(void)
	{
		appCheckSlow(ArrayNum > 0);
		appCheckSlow(ArrayMax >= ArrayNum);
		ElementType Result = GetTypedData()[ArrayNum - 1];
		Remove(ArrayNum - 1);
		return Result;
	}
	
	void Push(const ElementType& Item)
	{
		AddItem(Item);
	}
	
	ElementType& Top()
	{
		return Last();
	}
	
	const ElementType& Top() const
	{
		return Last();
	}
	
	ElementType& Last(INT c = 0)	
	{
		check(AllocatorInstance.GetAllocation());
		check(c < ArrayNum);
		appCheckSlow(ArrayMax >= ArrayNum);
		return GetTypedData()[ArrayNum - c - 1];
	}
	
	const ElementType& Last(INT c = 0) const
	{
		check(GetTypedData());
		check(c < ArrayNum);
		appCheckSlow(ArrayMax >= ArrayNum);
		return GetTypedData()[ArrayNum - c - 1];
	}
	
	void Shrink()
	{
		appCheckSlow(ArrayNum >= 0);
		appCheckSlow(ArrayMax >= ArrayNum);
		if (ArrayMax != ArrayNum)
		{
			ArrayMax = ArrayNum;
			AllocatorInstance.ResizeAllocation(ArrayNum, ArrayMax, sizeof(ElementType));
		}
	}
	
	UBOOL FindItem(const ElementType& Item, INT& Index) const
	{
		const ElementType* const RESTRICT DataEnd = GetTypedData() + ArrayNum;
		for (const ElementType* const RESTRICT Data = GetTypedData();
			 Data < DataEnd;
			 Data++)
		{
			if (*Data == Item)
			{
				Index = (INT)(Data - GetTypedData());
				return TRUE;
			}
		}
		
		return FALSE;
	}
	
	INT FindItemIndex(const ElementType& Item) const
	{
		const ElementType* const RESTRICT DataEnd = GetTypedData() + ArrayNum;
		for (const ElementType* RESTRICT Data = GetTypedData();
			 Data < DataEnd;
			 Data++)
		{
			if (*Data == Item)
			{
				return (INT)(Data - GetTypedData());
			}
		}
		
		return INDEX_NONE;
	}
	
	UBOOL ContainsItem(const ElementType& Item) const
	{
		return (FindItemIndex(Item) != INDEX_NONE);
	}
	
	UBOOL operator==(const TArray& OtherArray) const
	{
		if (Num() != OtherArray.Num())
		{
			return FALSE;
		}
		
		for (INT Index = 0; Index < Num(); Index++)
		{
			if (!((*this)(Index) == OtherArray(Index)))
			{
				return FALSE;
			}
		}
		
		return TRUE;
	}
	
	UBOOL operator!=(const TArray& OtherArray) const
	{
		if (Num() != OtherArray.Num())
		{
			return TRUE;
		}
		
		for (INT Index = 0; Index < Num(); Index++)
		{
			if(!((*this)(Index) == OtherArray(Index)))
				return TRUE;
		}
		return FALSE;
	}
	
	// Add, Insert, Remove, Empty interface.
	INT Add( INT Count=1 )
	{
		check(Count>=0);
		appCheckSlow(ArrayNum>=0);
		appCheckSlow(ArrayMax>=ArrayNum);
		
		const INT OldNum = ArrayNum;
		if( (ArrayNum+=Count)>ArrayMax )
		{
			ArrayMax = AllocatorInstance.CalculateSlack( ArrayNum, ArrayMax, sizeof(ElementType) );
			AllocatorInstance.ResizeAllocation(OldNum,ArrayMax, sizeof(ElementType));
		}
		
		return OldNum;
	}
	void Insert( INT Index, INT Count=1 )
	{
		check(Count>=0);
		check(ArrayNum>=0);
		check(ArrayMax>=ArrayNum);
		check(Index>=0);
		check(Index<=ArrayNum);
		
		const INT OldNum = ArrayNum;
		if( (ArrayNum+=Count)>ArrayMax )
		{
			ArrayMax = AllocatorInstance.CalculateSlack( ArrayNum, ArrayMax, sizeof(ElementType) );
			AllocatorInstance.ResizeAllocation(OldNum,ArrayMax,sizeof(ElementType));
		}
		appMemmove
		(
		 (BYTE*)AllocatorInstance.GetAllocation() + (Index+Count )*sizeof(ElementType),
		 (BYTE*)AllocatorInstance.GetAllocation() + (Index       )*sizeof(ElementType),
		 (OldNum-Index)*sizeof(ElementType)
		 );
	}
	void InsertZeroed( INT Index, INT Count=1 )
	{
		Insert( Index, Count );
		appMemzero( (BYTE*)AllocatorInstance.GetAllocation()+Index*sizeof(ElementType), Count*sizeof(ElementType) );
	}
	INT InsertItem( const ElementType& Item, INT Index )
	{
		// construct a copy in place at Index (this new operator will insert at 
		// Index, then construct that memory with Item)
		Insert(Index,1);
		new(GetTypedData() + Index) ElementType(Item);
		return Index;
	}
	void Remove( INT Index, INT Count=1 )
	{
		appCheckSlow(Count >= 0);
		check(Index>=0);
		check(Index<=ArrayNum);
		check(Index+Count<=ArrayNum);
		
		DestructItems(Index,Count);
		
		// Skip memmove in the common case that there is nothing to move.
		INT NumToMove = ArrayNum - Index - Count;
		if( NumToMove )
		{
			appMemmove
			(
			 (BYTE*)AllocatorInstance.GetAllocation() + (Index      ) * sizeof(ElementType),
			 (BYTE*)AllocatorInstance.GetAllocation() + (Index+Count) * sizeof(ElementType),
			 NumToMove * sizeof(ElementType)
			 );
		}
		ArrayNum -= Count;
		
		const INT NewArrayMax = AllocatorInstance.CalculateSlack(ArrayNum,ArrayMax,sizeof(ElementType));
		if(NewArrayMax != ArrayMax)
		{
			ArrayMax = NewArrayMax;
			AllocatorInstance.ResizeAllocation(ArrayNum,ArrayMax,sizeof(ElementType));
		}
		appCheckSlow(ArrayNum >= 0);
		appCheckSlow(ArrayMax >= ArrayNum);
	}
	// RemoveSwap, this version is much more efficient O(Count) instead of O(ArrayNum), but does not preserve the order
	void RemoveSwap( INT Index, INT Count=1 )
	{
		check(Index>=0);
		check(Index<=ArrayNum);
		check(Index+Count<=ArrayNum);
		
		DestructItems(Index,Count);
		
		appCheckSlow(Count >= 0);
		appCheckSlow(Index >= 0); 
		appCheckSlow(Index <= ArrayNum);
		appCheckSlow(Index + Count <= ArrayNum);
		
		// Replace the elements in the hole created by the removal with elements from the end of the array, so the range of indices used by the array is contiguous.
		const INT NumElementsInHole = Count;
		const INT NumElementsAfterHole = ArrayNum - (Index + Count);
		const INT NumElementsToMoveIntoHole = Min(NumElementsInHole,NumElementsAfterHole);
		if(NumElementsToMoveIntoHole)
		{
			appMemcpy(
					  (BYTE*)AllocatorInstance.GetAllocation() + (Index                             ) * sizeof(ElementType),
					  (BYTE*)AllocatorInstance.GetAllocation() + (ArrayNum-NumElementsToMoveIntoHole) * sizeof(ElementType),
					  NumElementsToMoveIntoHole * sizeof(ElementType)
					  );
		}
		ArrayNum -= Count;
		
		const INT NewArrayMax = AllocatorInstance.CalculateSlack(ArrayNum,ArrayMax,sizeof(ElementType));
		if(NewArrayMax != ArrayMax)
		{
			ArrayMax = NewArrayMax;
			AllocatorInstance.ResizeAllocation(ArrayNum,ArrayMax,sizeof(ElementType));
		}
		appCheckSlow(ArrayNum >= 0);
		appCheckSlow(ArrayMax >= ArrayNum);
	}
	void Empty( INT Slack=0 )
	{
		DestructItems(0,ArrayNum);
		
		appCheckSlow(Slack>=0);
		ArrayNum = 0;
		// only reallocate if we need to, I don't trust realloc to the same size to work
		if (ArrayMax != Slack)
		{
			ArrayMax = Slack;
			AllocatorInstance.ResizeAllocation(0,ArrayMax,sizeof(ElementType));
		}
	}
	
	/**
	 * Appends the specified array to this array.
	 * Cannot append to self.
	 */
	FORCEINLINE void Append(const TArray& Source)
	{
		// Do nothing if the source and target match, or the source is empty.
		if ( this != &Source && Source.Num() > 0 )
		{
			// Allocate memory for the new elements.
			Reserve( ArrayNum + Source.ArrayNum );
			
			if ( TContainerTraits<ElementType>::NeedsConstructor )
			{
				// Construct each element.
				for ( INT Index = 0 ; Index < Source.ArrayNum ; ++Index )
				{
					new(GetTypedData() + ArrayNum + Index) ElementType(Source(Index));
				}
			}
			else
			{
				// Do a bulk copy.
				appMemcpy( (BYTE*)AllocatorInstance.GetAllocation() + ArrayNum * sizeof(ElementType), Source.AllocatorInstance.GetAllocation(), sizeof(ElementType) * Source.ArrayNum );
			}
			ArrayNum += Source.ArrayNum;
		}
	}
	
	/**
	 * Appends the specified array to this array.
	 * Cannot append to self.
	 */
	TArray& operator+=( const TArray& Other )
	{
		Append( Other );
		return *this;
	}
	
	/**
	 * Copies the source array into this one. Uses the common copy method
	 *
	 * @param Other the source array to copy
	 */
	template<typename OtherAllocator>
	TArray& operator=( const TArray<ElementType,OtherAllocator>& Other )
	{
		Copy(Other);
		return *this;
	}
	
	TArray& operator=( const TArray<ElementType,Allocator>& Other )
	{
		Copy(Other);
		return *this;
	}
	
	INT AddItem( const ElementType& Item )
	{
		const INT Index = Add(1);
		new(GetTypedData() + Index) ElementType(Item);
		return Index;
	}
	INT AddZeroed( INT Count=1 )
	{
		const INT Index = Add( Count );
		appMemzero( (BYTE*)AllocatorInstance.GetAllocation()+Index*sizeof(ElementType), Count*sizeof(ElementType) );
		return Index;
	}
	INT AddUniqueItem( const ElementType& Item )
	{
		for( INT Index=0; Index<ArrayNum; Index++ )
			if( (*this)(Index)==Item )
				return Index;
		return AddItem( Item );
	}
	
	/**
	 * Reserves memory such that the array can contain at least Number elements.
	 */
	void Reserve(INT Number)
	{
		if (Number > ArrayMax)
		{
			ArrayMax = Number;
			AllocatorInstance.ResizeAllocation(ArrayNum,ArrayMax,sizeof(ElementType));
		}
	}
	
	/** Removes as many instances of Item as there are in the array, maintaining order but not indices. */
	INT RemoveItem( const ElementType& Item )
	{
		// It isn't valid to specify an Item that is in the array, since removing that item will change Item's value.
		check( ((&Item) < GetTypedData()) || ((&Item) >= GetTypedData()+ArrayMax) );
		
		INT DestIndex = 0;
		for( INT Index=0; Index<ArrayNum; Index++ )
		{
			if( GetTypedData()[Index] == Item )
			{
				// Destruct items that match the specified Item.
				DestructItems(Index,1);
			}
			else
			{
				// Move items that don't match the specified Item to fill holes left by removed items.
				if(Index != DestIndex)
				{
					appMemmove(&GetTypedData()[DestIndex],&GetTypedData()[Index],sizeof(ElementType));
				}
				DestIndex++;
			}
		}
		
		// Update the array count.
		const INT OriginalNum = ArrayNum;
		ArrayNum = DestIndex;
		
		return OriginalNum - ArrayNum;
	}
	
	/** RemoveItemSwap, this version is much more efficient O(Count) instead of O(ArrayNum), but does not preserve the order */
	INT RemoveItemSwap( const ElementType& Item )
	{
		check( ((&Item) < (ElementType*)AllocatorInstance.GetAllocation()) || ((&Item) >= (ElementType*)AllocatorInstance.GetAllocation()+ArrayMax) );
		const INT OriginalNum=ArrayNum;
		for( INT Index=0; Index<ArrayNum; Index++ )
		{
			if( (*this)(Index)==Item )
			{
				RemoveSwap( Index-- );
			}
		}
		return OriginalNum - ArrayNum;
	}
	
	void Swap(INT A, INT B)
	{
		appMemswap(
				   (BYTE*)AllocatorInstance.GetAllocation()+(sizeof(ElementType)*A),
				   (BYTE*)AllocatorInstance.GetAllocation()+(sizeof(ElementType)*B),
				   sizeof(ElementType)
				   );
	}
	
	void SwapItems(INT A, INT B)
	{
		check((A >= 0) && (B >= 0));
		check((ArrayNum > A) && (ArrayNum > B));
		if (A != B)
		{
			Swap(A,B);
		}
	}
	
	/**
	 * Same as empty, but doesn't change memory allocations. It calls the
	 * destructors on held items if needed and then zeros the ArrayNum.
	 */
	void Reset(void)
	{
		DestructItems(0,ArrayNum);
		ArrayNum = 0;
	}
	
	/**
	 * Searches for the first entry of the specified type, will only work
	 * with TArray<UObject*>.  Optionally return the item's index, and can
	 * specify the start index.
	 */
	template<typename SearchType>
	UBOOL FindItemByClass(SearchType **Item = NULL, INT *ItemIndex = NULL, INT StartIndex = 0)
	{
		/**
		WClass* SearchClass = SearchType::StaticClass();
		for (INT Idx = StartIndex; Idx < ArrayNum; Idx++)
		{
			if ((*this)(Idx) != NULL && (*this)(Idx)->IsA(SearchClass))
			{
				if (Item != NULL)
				{
					*Item = (SearchType*)((*this)(Idx));
				}
				if (ItemIndex != NULL)
				{
					*ItemIndex = Idx;
				}
				return TRUE;
			}
		}
		*/
		return FALSE;
	}
	
	// Iterators
	typedef TIndexedContainerIterator< TArray<ElementType,Allocator> >  TIterator;
	typedef TIndexedContainerConstIterator< TArray<ElementType,Allocator> >  TConstIterator;
	
#if defined(_MSC_VER)
private:
	/**
	 * Helper function that can be used inside the debuggers watch window to debug TArrays. E.g. "*Class->Defaults.DebugGet(5)". 
	 *
	 * @param	i	Index
	 * @return		pointer to type T at Index i
	 */
	FORCEINLINE const ElementType& DebugGet( INT i ) const
	{
		return GetTypedData()[i];
	}
#endif
	
protected:
	
	/**
	 * Copies data from one array into this array. Uses the fast path if the
	 * data in question does not need a constructor.
	 *
	 * @param Source the source array to copy
	 */
	template<typename OtherAllocator>
	void Copy(const TArray<ElementType,OtherAllocator>& Source)
	{
		if ((void*)this != (void*)&Source)
		{
			// Just empty our array if there is nothing to copy
			if (Source.Num() > 0)
			{
				// Presize the array so there are no extra allocs/memcpys
				Empty(Source.Num());
				// Determine whether we need per element construction or bulk
				// copy is fine
				if (TContainerTraits<ElementType>::NeedsConstructor)
				{
					// Use the inplace new to copy the element to an array element
					for (INT Index = 0; Index < Source.Num(); Index++)
					{
						new(GetTypedData() + Index) ElementType(Source(Index));
					}
				}
				else
				{
					// Use the much faster path for types that allow it
					appMemcpy(AllocatorInstance.GetAllocation(),&Source(0),sizeof(ElementType) * Source.Num());
				}
				ArrayNum = Source.Num();
			}
			else
			{
				Empty(0);
			}
		}
	}
	
	/** Destructs a range of items in the array. */
	FORCEINLINE void DestructItems(INT Index,INT Count)
	{
		if( TContainerTraits<ElementType>::NeedsDestructor )
		{
			for( INT i=Index; i<Index+Count; i++ )
			{
				(&(*this)(i))->~ElementType();
			}
		}
	}
	
	enum { AllocatorNeedsElementType = Allocator::NeedsElementType };
	
	typedef typename TChooseClass<
	AllocatorNeedsElementType,
	typename Allocator::template ForElementType<ElementType>,
	typename Allocator::ForAnyElementType
	>::Result ElementAllocatorType;
	
	ElementAllocatorType AllocatorInstance;
	INT	  ArrayNum;
	INT	  ArrayMax;
};

template<typename T, typename Allocator = NDefaultAllocator>
class TArrayNoInit : public TArray<T, Allocator>
{
public:
	TArrayNoInit()
	: TArray<T>(E_NoInit)
	{}
	
	TArrayNoInit& operator=(const TArrayNoInit& Other)
	{
		TArray<T>::operator=(Other);
		return *this;
	}
	
	TArrayNoInit& operator=(const TArray<T>& Other)
	{
		TArray<T>::operator=(Other);
		return *this;
	}
};


//
// Array operator news.
//
template <typename T,typename Allocator> void* operator new( size_t Size, TArray<T,Allocator>& Array )
{
	check(Size == sizeof(T));
	const INT Index = Array.Add(1);
	return &Array(Index);
}

template <typename T,typename Allocator> void* operator new( size_t Size, TArray<T,Allocator>& Array, INT Index )
{
	check(Size == sizeof(T));
	Array.Insert(Index,1);
	return &Array(Index);
}

/** A specialization of the exchange macro that avoids reallocating when exchanging two arrays. */
template <typename T> inline void Exchange( TArray<T>& A, TArray<T>& B )
{
	appMemswap( &A, &B, sizeof(TArray<T>) );
}

/** A specialization of the exchange macro that avoids reallocating when exchanging two arrays. */
template<typename ElementType,typename Allocator>
inline void Exchange( TArray<ElementType,Allocator>& A, TArray<ElementType,Allocator>& B )
{
	appMemswap( &A, &B, sizeof(TArray<ElementType,Allocator>) );
}

#endif	// __TARRAY_H__