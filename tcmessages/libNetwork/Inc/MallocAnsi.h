/**
 * @file	MallocAnsi.h
 *
 * Declares the malloc ansi class.
 */
#ifndef __MALLOCANSI_H__
#define __MALLOCANSI_H__


#ifdef _MSC_VER
#define	USE_ALIGNED_MALLOC	1
#else
#define	USE_ALIGNED_MALLOC	0
#endif

//
//	Ansi C memory allocator
//
class NMallocAnsi : public NMalloc
{
	// Alignment.
	enum { ALLOCATION_ALIGNMENT = 16 };
	
public:
	/** Constructor enabling low fragmentation heap on platforms supporting it. */
	NMallocAnsi()
	{
#ifdef _MSC_VER
		// Enable low fragmentation heap - http://msdn2.microsoft.com/en-US/library/aa366750.aspx
		intptr_t	CrtHeapHandle	= _get_heap_handle();
		ULONG		EnableLFH		= 2;
		HeapSetInformation((PVOID)CrtHeapHandle, HeapCompatibilityInformation, &EnableLFH, sizeof(EnableLFH));
#endif
	}

	// NMalloc interface.
	virtual void* Malloc(DWORD Size, DWORD Alignment)
	{
#if USE_ALIGNED_MALLOC
		void* Ptr = _aligned_malloc(Size, ALLOCATION_ALIGNMENT);
		return Ptr;
#else
		void* Ptr = malloc(Size + ALLOCATION_ALIGNMENT + sizeof(void*) + sizeof(DWORD));
		void* AlignedPtr = Align((BYTE*)Ptr + sizeof(void*) + sizeof(DWORD), ALLOCATION_ALIGNMENT);
		*((void**)((BYTE*)AlignedPtr - sizeof(void*)					)) = Ptr;
		*((DWORD*)((BYTE*)AlignedPtr - sizeof(void*) - sizeof(DWORD)	)) = Size;
		return AlignedPtr;
#endif
	}
	
	virtual void* Realloc( void* Ptr, DWORD NewSize, DWORD Alignment )
	{
		//check(Alignment == DEFAULT_ALIGNMENT && "Alignment currently unsupported in Ansi Malloc");
		void* Result;
#if USE_ALIGNED_MALLOC
		if( Ptr && NewSize )
		{
			Result = _aligned_realloc( Ptr, NewSize, ALLOCATION_ALIGNMENT );
		}
		else if( Ptr == NULL )
		{
			Result = _aligned_malloc( NewSize, ALLOCATION_ALIGNMENT );
		}
		else
		{
			_aligned_free( Ptr );
			Result = NULL;
		}
#else
		if( Ptr && NewSize )
		{
			// Can't use realloc as it might screw with alignment.
			Result = Malloc( NewSize, DEFAULT_ALIGNMENT );
			appMemcpy( Result, Ptr, Min(NewSize, *((DWORD*)( (BYTE*)Ptr - sizeof(void*) - sizeof(DWORD))) ) );
			Free( Ptr );
		}
		else if( Ptr == NULL )
		{
			Result = Malloc( NewSize, DEFAULT_ALIGNMENT );
		}
		else
		{
			free( *((void**)((BYTE*)Ptr-sizeof(void*))) );
			Result = NULL;
		}
#endif
		return Result;
	}
	
	virtual void Free( void* Ptr )
	{
#if USE_ALIGNED_MALLOC
		_aligned_free( Ptr );
#else
		if( Ptr )
		{
			free( *((void**)((BYTE*)Ptr-sizeof(void*))) );
		}
#endif
	}
	
	/**
	 * Returns if the allocator is guaranteed to be thread-safe and therefore
	 * doesn't need a unnecessary thread-safety wrapper around it.
	 *
	 * @return TRUE as we're using system allocator
	 */
	virtual UBOOL IsInternallyThreadSafe() const
	{
		return TRUE;
	}

	
//	virtual UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar ) 
//	{ 
//#if defined(_MSC_VER) && !SHIPPING_PC_GAME
//		if( ParseCommand(&Cmd,TEXT("HEAPCHECK")) )
//		{
//			INT Result = _heapchk();
//			check(Result!=_HEAPBADBEGIN);
//			check(Result!=_HEAPBADNODE);
//			check(Result!=_HEAPBADPTR);
//			check(Result!=_HEAPEMPTY);
//			check(Result==_HEAPOK);
//			return TRUE;
//		}
//#endif
//		return FALSE; 
//	}
};

#endif	// __MALLOCANSI_H__