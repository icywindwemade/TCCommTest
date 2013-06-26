/**
 * @file	MemoryBase.cpp
 *
 * Implements the memory base class.
 */
#include "Core.h"

// Memory allocator
NMalloc*	GMalloc		= NULL;


/** Helper function called on first allocation to create and initialize GMalloc. */
void CreateMalloc()
{
	GMalloc = new NMallocAnsi();
}

/** Helper function called on last allocation to destroy and finalize GMalloc. */
void DestroyMalloc()
{
	if ( GMalloc )
	{
		delete GMalloc;
	}
}

void* appMemmove( void* Dest, const void* Src, INT Count )
{
	return memmove( Dest, Src, Count );
}

//
// CRC functions.
//

/** CRC32 computer based on CRC32_POLY. */
DWORD appMemCrc(const void* InData, INT Length, DWORD CRC)
{
	BYTE* Data = (BYTE*)InData;
	CRC = ~CRC;
	for (INT i = 0; i < Length; i++)
	{
		CRC = (CRC << 8) ^ GCRCTable[(CRC >> 24) ^ Data[i]];
	}

	return ~CRC;
}

/*
//
// String CRC.
//
DWORD appStrCrc( const TCHAR* Data )
{
	INT Length = appStrlen( Data );
	DWORD CRC = 0xFFFFFFFF;
	for( INT i=0; i<Length; i++ )
	{
		TCHAR C   = Data[i];
		INT   CL  = (C&255);
		CRC       = (CRC << 8) ^ GCRCTable[(CRC >> 24) ^ CL];;
		INT   CH  = (C>>8)&255;
		CRC       = (CRC << 8) ^ GCRCTable[(CRC >> 24) ^ CH];;
	}
	return ~CRC;
}

//
// String CRC, case insensitive.
//
DWORD appStrCrcCaps( const TCHAR* Data )
{
	INT Length = appStrlen( Data );
	DWORD CRC = 0xFFFFFFFF;
	for( INT i=0; i<Length; i++ )
	{
		TCHAR C   = appToUpper(Data[i]);
		INT   CL  = (C&255);
		CRC       = (CRC << 8) ^ GCRCTable[(CRC >> 24) ^ CL];
		INT   CH  = (C>>8)&255;
		CRC       = (CRC << 8) ^ GCRCTable[(CRC >> 24) ^ CH];
	}
	return ~CRC;
}

// Ansi String CRC.
//
DWORD appAnsiStrCrc( const char* Data )
{
	INT Length = strlen( Data );
	DWORD CRC = 0xFFFFFFFF;
	for( INT i=0; i<Length; i++ )
	{
		char C   = Data[i];
		INT   CL  = (C&255);
		CRC       = (CRC << 8) ^ GCRCTable[(CRC >> 24) ^ CL];;
		INT   CH  = (C>>8)&255;
		CRC       = (CRC << 8) ^ GCRCTable[(CRC >> 24) ^ CH];;
	}
	return ~CRC;
}

//
// Ansi String CRC, case insensitive.
//
DWORD appAnsiStrCrcCaps( const char* Data )
{
	INT Length = strlen( Data );
	DWORD CRC = 0xFFFFFFFF;
	for( INT i=0; i<Length; i++ )
	{
		char C   = toupper(Data[i]);
		INT   CL  = (C&255);
		CRC       = (CRC << 8) ^ GCRCTable[(CRC >> 24) ^ CL];
		INT   CH  = (C>>8)&255;
		CRC       = (CRC << 8) ^ GCRCTable[(CRC >> 24) ^ CH];
	}
	return ~CRC;
}
*/

//
// Memory functions.
//
void appMemswap(void* Ptr1, void* Ptr2, DWORD Size)
{
	void* Temp = appAlloca(Size);
	appMemcpy(Temp, Ptr1, Size);
	appMemcpy(Ptr1, Ptr2, Size);
	appMemcpy(Ptr2, Temp, Size);
}
