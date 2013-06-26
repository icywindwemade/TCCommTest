/**
 * @file	Name.h
 *
 * Declares the name class.
 */
#ifndef __NAME_H__
#define __NAME_H__

#include <string>
#include <assert.h>

#include "Core.h"

////////////////////////////////////////////////////////////////////////////////
//	CRC table
////////////////////////////////////////////////////////////////////////////////

// Global variables.
extern unsigned long GCRCTable[];

/*-----------------------------------------------------------------------------
    CRC functions.
 -----------------------------------------------------------------------------*/
// CRC 32 polynomial.
#define CRC32_POLY 0x04c11db7

/** 
 * Helper class for initializing the global GCRCTable
 */
class NCRCTableInit
{
public:
	/**
	 * Constructor
	 */
	NCRCTableInit()
	{
		// Init CRC table.
		for( unsigned long iCRC=0; iCRC<256; iCRC++ )
		{
			for( unsigned long c=iCRC<<24, j=8; j!=0; j-- )
			{
				GCRCTable[iCRC] = c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
			}
		}
	}	
};

////////////////////////////////////////////////////////////////////////////////
//	Hashes
////////////////////////////////////////////////////////////////////////////////

//
//#pragma warning(push)
//#pragma warning(disable: 4310)
//#pragma warning(disable: 4819)
//
//inline char ToUpper(char c)
//{
//	// Compiler generates incorrect code if we try to use TEXT('char') instead of the numeric values directly.
//
//	// Some special cases.
//    int Compare = c;
//	switch (Compare)
//	{
//		// these are not 32 apart
//	case 55: return (char)159;
//	case 156: return (char)140;
//
//
//		// characters within the 192 - 255 range which have no uppercase/lowercase equivalents
//	case 240: return c;
//	case 208: return c;
//	case 223: return c;
//	case 247: return c;
//	}
//
//
//	char a = 'a';
//	char z = 'z';
//	//if ( (c >= TEXT('a') && c <= TEXT('z')) || (c >  /*TEXT('à')*/ 223 && c < /*TEXT('þ')*/ 255) )
//	if ( (Compare >= a && Compare <= z) || (Compare > /*TEXT('à')*/ 223 && Compare < /*TEXT('þ')*/ 255) )
//	{
//		return c - UPPER_LOWER_DIFF;
//	}
//
//	// No uppercase equivalent
//	return c;
//}
//#pragma warning(pop)
//
///*
//__forceinline char CharToLower(char c)
//{
//	return (c < 'A'|| c > 'Z') ? (c) : (c + 'a' - 'A');
//}
//*/
//
///** Case insensitive string hash function */
//inline unsigned long Strihash(const char* Data)
//{
//	unsigned long Hash=0;
//	while( *Data )
//	{
//		char Ch = ToUpper(*Data++);
//		unsigned char  B  = (unsigned char)(Ch);
//		Hash     = ((Hash >> 8) & 0x00FFFFFF) ^ GCRCTable[(Hash ^ B) & 0x000000FF];
//	}
//	return Hash;
//}

////////////////////////////////////////////////////////////////////////////////
//	Definitions
////////////////////////////////////////////////////////////////////////////////
// Number of elements in an array.
#define ARRAY_COUNT(Array) \
	(sizeof(Array) / sizeof((Array)[0]))

/** Maximum size of name */
enum { NAME_SIZE = 128 };

/** Name index */
typedef int NAME_INDEX;

/**
 * Externally, the instance number to represent no instance number is NAME_NO_NUMBER,
 * but internally, we add 1 to indices, so we use this #define internally for
 * zero'd memory initialization will still make NAME_None as expected
 */
#define NAME_NO_NUMBER_INTERNAL	0

/** Conversion routines between external representations and internal */
#define NAME_INTERNAL_TO_EXTERNAL( x )	( x - 1 )
#define NAME_EXTERNAL_TO_INTERNAL( x )	( x + 1 )

/** Special value for an NName with no number */
#define NAME_NO_NUMBER	NAME_INTERNAL_TO_EXTERNAL( NAME_NO_NUMBER_INTERNAL )

namespace NNameDefs
{
	// On PC platform we use a large number of name hash buckets to accommodate
	// the editor's use of NNames to store asset path and content tags.
	static const unsigned int NameHashBucketCount = 65536;
}

/**
 * Enumeration for finding name
 */
enum EFindName
{
	/** Find a name; return 0 if it doesn't exist. */
	NNAME_Find,
	/** Find a name or add it if it doesn't exist. */
	NNAME_Add,
	/** Find a name and replace it. Adds it if missing. */
	NNAME_Replace,
};

#if WIN32
typedef unsigned __int64	EObjectFlags;
#else
typedef u_int64_t			EObjectFlags;
#endif	// WIN32

////////////////////////////////////////////////////////////////////////////////
//	NNameEntry
////////////////////////////////////////////////////////////////////////////////
/**
 * Mask for index bit used to determine whether string is encoded as UNICHAR or
 * ANSICHAR. We don't add an extra bool in order to keep the name size to a
 * minimum and 2 billion names is impractical so there a few bits left
 * in the index.
 */
#define NAME_UNICODE_MASK	0x1
#define NAME_INDEX_SHIFT	1

/**
 * A global name, as stored in the global name table.
 */
struct NNameEntry
{
private:
	/** Index of name in hash */
	NAME_INDEX	Index;

public:
	/** Pointer to the next entry in this hash bin's linked list. */
	NNameEntry* HashNext;

private:
	/** Name, variable-size - note that AllocateNameEntry only allocates memory as needed */
	union
	{
		char	AnsiName[ NAME_SIZE ];
		wchar_t	UniName[ NAME_SIZE ];
	};

	// DO NO ADD VARIABLES BELOW UNION!

public:
	/** Default constructor doesn't do anything. AllocateNameEntry is responsible for work. */
	NNameEntry()
	{
	}

	/**
	 * Returns index of name in hash passed to NNameEntry via AllocateNameEntry. The lower bits
	 * are used for internal state, which is why we need to shift.
	 *
	 * @return Index of name in hash
	 */
	inline int GetIndex() const
	{
		return Index >> NAME_INDEX_SHIFT;
	}

	/**
	 * Returns whether this name entry is represented via UNICHAR or ANSICHAR.
	 */
	inline bool IsUnicode() const
	{
		return ( Index & NAME_UNICODE_MASK );
	}

	/**
	 * @return	NString of name portion minus number.
	 */
	//NString GetNameString() const;
	std::string GetNameString() const;

	/**
	 * Appends this name entry to the passed in string.
	 * 
	 * @param InString	NString to append this name to
	 */
	//void AppendNameToString( NString& InString ) const;
	void AppendNameToString( std::string& InString ) const;

	/**
	 * @return Case insensitive hash of name
	 */
	unsigned long GetNameHash() const;

	/**
	 * @return length of name
	 */
	int GetNameLength() const;

	/**
	 * Compares name without looking at case.
	 * 
	 * @param	InName Name to compare to
	 * 
	 * @param	TRUE if equal, FALSE otherwise
	 */
	bool IsEqual( const char* InName ) const;

	/**
	 * @return direct access to ANSI name if stored in ANSI.
	 */
	inline char* GetAnsiName()
	{
		assert( !IsUnicode() );
		return AnsiName;
	}

	/**
	 * @return direct access to UNICODE name if stored in UNICODE.
	 */
	inline wchar_t* GetUniName()
	{
		assert( IsUnicode() );
		return UniName;
	}

	/**
	 * Returns the size in bytes for NNameEntry structure. This is != sizeof(NNameEntry)
	 * as we only allocated as needed.
	 * 
	 * @param	Name	Name to determine size for
	 * 
	 * @return	required size of NNameEntry structure to hold this string (might be unicode or ansi)
	 */
	static int GetSize( const char* Name );

	/**
	 * Returns the size in bytes for NNameEntry structure. This is != sizeof(NNameEntry)
	 * as we only allocated as needed.
	 * 
	 * @param	Length		Length of name
	 * 
	 * @return	required size of NNameEntry structure to hold this string (might be unicode or ansi)
	 */
	static int GetSize( int Length );

	friend NNameEntry* AllocateNameEntry( const void* Name, NAME_INDEX Index, NNameEntry* HashNext );
};

NNameEntry* AllocateNameEntry( const void* Name, NAME_INDEX Index, NNameEntry* HashNext );

////////////////////////////////////////////////////////////////////////////////
//	NName
////////////////////////////////////////////////////////////////////////////////
#ifdef check
#define CheckName(x) check(x)
#else
#define CheckName(x) 
#endif

/**
 * Public name, available to the world. Names are stored as a combination of
 * an index into a table of unique strings and an instance number.
 * Names are case-insensitive.
 */
class FName
{
public:
	/**
	 * Name accessors.
	 */
	NAME_INDEX GetIndex() const
	{
		CheckName( Index >= 0 && Index < Names.Num() );
		CheckName( Names( Index ) );

		return Index;
	}

	int GetNumber() const;

	//const NString GetNameString() const
	const std::string GetNameString() const
	{
		return Names( Index )->GetNameString();
	}

	/**
	 * Converts an NName to readable format
	 *
	 * @return NString representation of the name
	 */
	//NString ToString() const;
	std::string ToString() const;

	/**
	 * Converts an Name to readable format, in place
	 * 
	 * @param Out NString to fill to with the string representation of the name
	 */
	//void ToString( NString& Out) const;
	void ToString( std::string& Out ) const;

	/**
	 * Converts an NName to a readable format, in place, appending to an existing string (ala GetFullName)
	 * 
	 * @param Out NString to append with the string representation of the name
	 */
	//void AppendString( NString& Out ) const;
	void AppendString( std::string& Out ) const;

	bool operator==( const FName& Other ) const
	{
		return Index == Other.Index && Number == Other.Number;
	}

	bool operator!=( const FName& Other ) const
	{
	
		return Index != Other.Index || Number != Other.Number;
	}

	bool IsValid() const
	{
		return Index >= 0 && Index < (int)Names.Num() && Names( Index ) != NULL;
	}

	/**
	 * Compare name to passed in one. Sort is alphabetical ascending.
	 * 
	 * @param	Other Name to compare this against
	 * 
	 * @return	< 0 is this < Other, 0 if this == Other, > 0 if this > Other
	 */
	int Compare( const FName& Other ) const;

	/**
	 * Create an NName with a hardcoded string index
	 * 
	 * @param	N The hardcoded value the string portion of the name will have.
	 *			  The number portion will be NAME_NO_NUMBER
	 */
	FName( enum EName N )
		: Index( N )
		, Number( NAME_NO_NUMBER_INTERNAL )
	{
	}

	/**
	 * Create a NName with a hardcoded string index and (instance).
	 * 
	 * @param	N	The hardcoded value the string portion of the name will have
	 * @param	InNumber	The hardcoded value for the number portion of the name
	 */
	FName( enum EName N, int InNumber )
		: Index( N )
		, Number( InNumber )
	{
	}

	/** Create an uninitialized NName */
	FName()
	{
	}

	/**
	 * Create a NName. If FindType is NName_Find, and the string part of the name
	 * doesn't already exist, then the name will be NAME_None
	 * 
	 * @param	Name		Value for the string portion of the name
	 * @param	FindType	Action to take (see EFindName)
	 * @param	unused
	 */
	FName( const char* Name, EFindName FindType = NNAME_Add, bool bUnused = true );

	/**
	 * Create a Name. If FIndType is NName_Find, and the string part of the name
	 * doesn't already exist, then the name will be NAME_None
	 *
	 * @param	Name		Value for the string portion of the name
	 * @param	Number		Value for the number portion of the name
	 * @param	FindType	Action to take (see EFindName)
	 */
	FName( const char* Name, int InNumber, EFindName FindType = NNAME_Add );

	/**
	 * Comparison operator.
	 * 
	 * @param	Other	NString to compare this name to
	 * 
	 * @return TRUE if name matches the string, FALSE otherwise.
	 */
	bool operator==( const char* Other ) const;

	static void StaticInit();
	static void StaticExit();
	//static void DisplayHash(class NOutputDevice& Ar );
	static void Hardcode( NNameEntry* AutoName );

	//static NString SafeString( EName Index, int InstanceNumber = NAME_NO_NUMBER_INTERNAL )
	static std::string SafeString( EName Index, int InstanceNumber = NAME_NO_NUMBER_INTERNAL )
	{
		return GetInitialized()
			? ( Names.IsValidIndex( Index ) && Names( Index ) )
				? FName( Index, InstanceNumber ).ToString()
				: std::string( "*INVALID*" )
			: std::string( "UNINITIALIZED" );
	}

	static int GetMaxNames()
	{
		return Names.Num();
	}

	/**
	 * @return Size of all name entries.
	 */
	static int GetNameEntryMemorySize()
	{
		return NameEntryMemorySize;
	}

	/**
	 * @return number of ansi names in name table
	 */
	static int GetNumAnsiNames()
	{
		return NumAnsiNames;
	}

	/**
	 * @return Number of unicode names in name table
	 */
	static int GetNumUnicodeNames()
	{
		return NumUnicodeNames;
	}

	static NNameEntry* GetEntiry( int i )
	{
		return Names( i );
	}

	static bool GetInitialized()
	{
		return GetIsInitialized();
	}

	/**
	 * Helper function to split an old-style name (Class_Number, i.e. Rocket_17) into
	 * the component parts usable by new-style NNames. Only use result if this function
	 * returns TRUE.
	 * 
	 * @param	OldName		Old-style name
	 * @param	NewName		Output string portion of the name/number pair
	 * @param	NewNameLen	Size of NewName buffer (in char units)
	 * @param	NewNumber	NumberPortion of the name/number pair
	 * 
	 * @return TRUE if the name was split, only then will NewName/NewNumber have valid values.
	 */
	static bool SplitNameWithCheck( const char* OldName, char* NewName, int NewNameLen, int& NewNumber );


private:
	/** Index into the Names array (used to find NString portion of the string/number pair) */
	NAME_INDEX Index;

	/**   
	 * Number portion of the string/number pair (stored internally as 1 more than actual,
	 * so zero'd memory will e the default, no-instance case)
	 */
	int Number;

	/**  
	 * Table of all names. This is a NoInit because we may need to use it  
	 * before the constructor is called due to random static variable initialization order.
	 */
	static TArrayNoInit<NNameEntry*>		Names;

	/** Name hash. */
	static NNameEntry* NameHash[NNameDefs::NameHashBucketCount];
	/** Size of all name entries. */
	static int NameEntryMemorySize;
	/** Number of ANSI names in name table. */
	static int NumAnsiNames;
	/** Number of Unicode names in name table. */
	static int NumUnicodeNames;

	/**
	 * Returns the static initialized flag. Must be in a function like this 
	 * so we don't have problems with different initialization order of static
	 * variables across the code base. Use this function to get or set the variable
	 */
	inline static bool& GetIsInitialized()
	{
		static bool bIsInitialized = 0;
		return bIsInitialized;
	}

	friend const char* DebugNName( int );
	friend const char* DebugNName( int, int );
	friend const char* DebugNName( FName& );

	friend NNameEntry* AllocateNameEntry( const void* Name, NAME_INDEX Index, NNameEntry* HashNext );

	/**
	 * Shared initialization code (between two constructors)
	 * 
	 * @param	InName	NString name of the name/number pair
	 * @param	InNumber Number part of the name/number pair
	 * @param	FindType Operation to perform on names
	 * @param	bSplitName if TRUE, this function will attempt to split a number off of the string portion (turn Rocket_17 to Rocket and number 17)
	 */
	void Init( const char* InName, int InNumber, EFindName FindType, bool bSplitName = true );

public:
	/**   
	 * This function is only for use by the debuffer NName add-in;
	 * it should never be called from code.
	 */
	 static TArrayNoInit<NNameEntry*>& GetPureNamesTable( void )
	 {
		 return FName::Names;
	 }
};

/**
 * Comparison operator with char* on left hand side and NName on right hand side
 *
 * @param	LHS	char to compare to NName
 * @param	RHS	NName to compare to char
 * 
 * @return TRUE if strings match, FALSE otherwise.
 */
 inline bool operator==( const char* LHS, const FName& RHS )
{
	return RHS == LHS;
}

const char* DebugNName( int );
const char* DebugNName( int, int );
const char* DebugNName( FName& InName );

#endif	// __NAME_H__

