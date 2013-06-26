/**
 * @file	MemoryBase.h
 *
 * Declares the memory base class.
 */
#ifndef __MEMORYBASE_H__
#define __MEMORYBASE_H__

#pragma warning(disable: 4100)

/** Memory allocator */
enum ECacheBehaviour
{
	CACHE_Normal		= 0,
	CACHE_WriteCombine	= 1,
	CACHE_None			= 2,
	CACHE_Virtual		= 3,
	CACHE_MAX	// needs to be last entry
};

//
// C style memory allocation stubs that fall back to C runtime
//
#ifndef appSystemMalloc
#define appSystemMalloc		malloc
#endif
#ifndef appSystemFree
#define appSystemFree		free
#endif
#ifndef appSystemRealloc
#define appSystemRealloc	realloc
#endif

/**
 * Inherit from NUseSystemMallocForNew if you want your objects to be placed in memory
 * allocated by the system malloc routines, bypassing GMalloc. This is e.g. used by NMalloc
 * itself.
 */
class NUseSystemMallocForNew
{
public:
	/**
	 * Overloaded new operator using the system allocator.
	 *
	 * @param Size Amount of memory to allocate (in bytes)
	 * @return A pointer to a block of memory with size Size or NULL
	 */
	void* operator new(size_t Size)
	{
		return appSystemMalloc(Size);
	}
	
	/**
	 * Overloaded delete operator using the system allocator
	 *
	 * @param Ptr Pointer to delete
	 */
	void operator delete(void* Ptr)
	{
		appSystemFree(Ptr);
	}
	
	/**
	 * Overloaded array new operator using the system allocator.
	 *
	 * @param Size Amount of memory allocate (in bytes)
	 * @return A pointer to a block of memory with size Size or NULL
	 */
	void* operator new[](size_t Size)
	{
		return appSystemMalloc(Size);
	}
	
	/**
	 * Overloaded array delete operator using the system allocator
	 *
	 * @param Ptr Pointer to delete 
	 */
	void operator delete[](void* Ptr)
	{
		appSystemFree(Ptr);
	}
};

/** The global memory allocator's interface */
class NMalloc : public NUseSystemMallocForNew
{
public:
	virtual ~NMalloc() {}
	
	virtual void* Malloc(DWORD Count, DWORD Alignment = DEFAULT_ALIGNMENT) = 0;
	virtual void* Realloc(void* Original, DWORD Count, DWORD Alignment = DEFAULT_ALIGNMENT) = 0;
	virtual void Free(void* Original) = 0;
	virtual void* PhysicalAlloc(DWORD Count, ECacheBehaviour CacheBehaviour = CACHE_WriteCombine) { return NULL; }
	virtual void PhysicalFree(void* Original) {}
	
	/** Called every game thread tick */
	virtual void Tick(FLOAT DeltaTime) {}
	
	/**
	 * Returns if the allocator is guaranteed to be thread-safe and therefore
	 * doesn't need a unnecessary thread-safety wrapper around it.
	 */
	virtual UBOOL IsInternallyThreadSafe() const { return FALSE; }
	
	/**
	 * Gather memory allocations for both virtual and physical allocations.
	 *
	 * @param Virtual [out] size of virtual allocations
	 * @param Physical [out] size of physical allocations
	 */
	virtual void GetAllocationInfo(SIZE_T& Virtual, SIZE_T& Physical) { Virtual = Physical = 0; }
	
	/**
	 * Keeps trying to allocate memory until we fail
	 *
	 * @param Ar Device to send output to
	 */
	//virtual void CheckMemoryFragmentationLevel(class NOutputDevice& Ar) { Ar.Log(TEXT("CheckMemoryFragmentationLevel not implemented")); }
	
	/**
	 * If possible give memory back to the os from unused segments
	 *
	 * @param ReservePad	Amount of space to reserve when trimming
	 * @param bShowStats	Log stats about how much memory was actually trimmed. Disable this for performance
	 * @return	TRUE if succeeded
	 */
	virtual bool TrimMemory(SIZE_T /*ReservePad*/, UBOOL bShowStats = FALSE) { return FALSE; }
	
	/** Total number of calls Malloc, if implemented by derived class. */
	static QWORD	TotalMallocCalls;
	/** Total number of calls Free, if implemented by derived class. */
	static QWORD	TotalFreeCalls;
	/** Total number of calls Realloc, if implemented by derived class. */
	static QWORD	TotalReallocCalls;
	/** Total number of calls to PhysicalAlloc, if implemented by derived class. */
	static QWORD	TotalPhysicalAllocCalls;
	/** Total number of calls to PhysicalFree, if implemented by derived class. */
	static QWORD	TotalPhysicalFreeCalls;
};

/** The global memory allocator */
extern NMalloc* GMalloc;

/**
 * Memory functions
 */
void*	appMemmove(void* Dest, const void* Src, INT Count);
INT		appMemcmp(const void* Buf1, const void* Buf2, INT Count);
UBOOL	appMemIsZero(const void* Buffer, INT Count);
DWORD	appMemCrc(const void* Data, INT Length, DWORD CRC = 0);
void	appMemswap(void* Ptr1, void* Ptr2, DWORD Size);

/**
 * Sets the first Count chars of Dest to the character C.
 */
#define	appMemset(Dest, C, Count)		memset(Dest, C, Count)

#ifndef DEFINED_appMemcpy
#define appMemcpy(Dest, Src, Count)		memcpy(Dest, Src, Count)
#endif

#ifndef DEFINED_appMemzero
#define appMemzero(Dest, Count)			memset(Dest, 0, Count)
#endif

/** Helper function called on first allocation to create and initialize GMalloc. */
extern void CreateMalloc();

/** Helper function called on last allocation to destroy and finalize GMalloc. */
extern void DestroyMalloc();

/**
 * C style memory allocation stubs.
 */
inline void* appMalloc(DWORD Count, DWORD Alignment = DEFAULT_ALIGNMENT)
{
	if (!GMalloc)
	{
		CreateMalloc();
	}
	
	if ( GMalloc )
	{
		return GMalloc->Malloc(Count, Alignment);
	}

	return NULL;
}

inline void* appRealloc(void* Original, DWORD Count, DWORD Alignment = DEFAULT_ALIGNMENT)
{
	if (!GMalloc)
	{
		CreateMalloc();
	}
	
	if ( GMalloc )
	{
		return GMalloc->Realloc(Original, Count, Alignment);
	}

	return NULL;
}

inline void appFree(void* Original)
{	
	if (!GMalloc)
	{
		CreateMalloc();
	}
	
	if ( GMalloc )
	{
		return GMalloc->Free(Original);
	}
}

inline void* appPhysicalAlloc(DWORD Count, ECacheBehaviour CacheBehaviour = CACHE_WriteCombine)
{
	if (!GMalloc)
	{
		CreateMalloc();
	}
	
	if ( GMalloc )
	{
		return GMalloc->PhysicalAlloc(Count, CacheBehaviour);
	}

	return NULL;
}

inline void appPhysicalFree(void* Original)
{
	if (!GMalloc)
	{
		CreateMalloc();
	}
	
	if ( GMalloc )
	{
		return GMalloc->PhysicalFree(Original);
	}
}

/*
DWORD appMemCrc(const void* Data, INT Length, DWORD CRC = 0);


extern DWORD appStrCrc( const TCHAR* Data );
extern DWORD appStrCrcCaps( const TCHAR* Data );
extern DWORD appAnsiStrCrc( const char* Data );
extern DWORD appAnsiStrCrcCaps( const char* Data );
*/
#endif	// __MEMORYBASE_H__