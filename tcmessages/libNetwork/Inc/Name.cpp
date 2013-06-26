/**
 * @file	Name.cpp
 *
 * Implements the name class.
 */
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////
//	CRC table.
////////////////////////////////////////////////////////////////////////////////
unsigned long GCRCTable[256];

////////////////////////////////////////////////////////////////////////////////
//	NName helpers.
////////////////////////////////////////////////////////////////////////////////
/**
 * Helper function that can be used inside the debuggers watch window. E.g. "DebugNName( Class->Name.Index )"
 * 
 * @param	Index Name index to look up string for
 *
 * @return	Associated name
 */
const char* DebugNName( int Index )
{
	// Hardcoded static array. This function is only used inside the debugger so it should be find to return it.
	static char TempName[256];
	strcpy( TempName, FName::SafeString( (EName) Index ).c_str() );
	return TempName;
}


/**
 * Helper function that can be used inside the debuggers watch window. E.g. "DebugNName(Class->Name.Index, Class->Name.Number)".
 *
 * @param Number Internal instance number of the NName to print (which is 1 more than the prineted number)
 *
 * @return Associated name
 */
const char* DebugNName(int Index, int Number)
{
	// Hardcoded static array. This function is only used inside the debugger so it should be fine to return it.
	static char TempName[256];
	strcpy( TempName, FName::SafeString( (EName)Index, Number ).c_str() );
	return TempName;
}

/**
 * Helper function that can be used inside the debuggers watch window. E.g. "DebugFName(Class->Name)". 
 *
 * @param	Name	Name to look up string for
 * @return			Associated name
 */
const char* DebugNName(FName& Name)
{
	// Hardcoded static array. This function is only used inside the debugger so it should be fine to return it.
	static char TempName[256];
	strcpy(TempName, FName::SafeString((EName)Name.Index, Name.Number).c_str() );
	return TempName;
}

#define TEXT( s )	s

/** Converts an integer to a string */
void ItoaAppend(int InNumber, std::string& NumberString)
{
#if WIN32
	__int64	Num					= InNumber; // This avoids having to deal with negating -MAXINT-1
#else
	int64_t	Num					= InNumber; // This avoids having to deal with negating -MAXINT-1
#endif	// WIN32
	const char*	NumberChar[11]		= { TEXT("0"), TEXT("1"), TEXT("2"), TEXT("3"), TEXT("4"), TEXT("5"), TEXT("6"), TEXT("7"), TEXT("8"), TEXT("9"), TEXT("-") };
	bool	bIsNumberNegative	= false;
	char	TempNum[16];		// 16 is big enough
	int		TempAt				= 16; // fill the temp string from the top down.

	// Correctly handle negative numbers and convert to positive integer.
	if( Num < 0 )
	{
		bIsNumberNegative = true;
		Num = -Num;
	}

	TempNum[--TempAt] = 0; // NULL terminator

	// Convert to string assuming base ten and a positive integer.
	do 
	{
		TempNum[--TempAt] = *NumberChar[Num % 10];
		Num /= 10;
	} while( Num );

	// Append sign as we're going to reverse string afterwards.
	if( bIsNumberNegative )
	{
		TempNum[--TempAt] = *NumberChar[10];
	}

	NumberString += TempNum + TempAt;

}


////////////////////////////////////////////////////////////////////////////////
//	NNameEntry
////////////////////////////////////////////////////////////////////////////////
/**
 * @return NString of name portion minus number
 */
std::string NNameEntry::GetNameString() const
{
	if ( IsUnicode() )
	{
		return std::string( "InvalidName" );
	}
	else
	{
		return std::string( AnsiName );
	}
}

/**
 * Append this name entry to the passed in string.
 * 
 * @param	String	String to append this name to
 */
void NNameEntry::AppendNameToString( std::string& String ) const
{
	if ( IsUnicode() )
	{
	}
	else
	{
		String += AnsiName;
	}
}

/**
 * @return	Case insensitive hash of name
 */
unsigned long NNameEntry::GetNameHash() const
{
	return appStrihash( AnsiName );
}

/**
 * @return	Length of name
 */
int NNameEntry::GetNameLength() const
{
	if ( IsUnicode() )
	{
		return wcslen( UniName );
	}
	else
	{
		return strlen( AnsiName );
	}
}

/**
 * Compares name without looking at case.
 * 
 * @param	InName	Name to compare to
 * 
 * @return	TRUE if equal, FALSE otherwise.
 */
bool NNameEntry::IsEqual( const char* InName ) const
{
	if ( IsUnicode() )
	{
		// matching unicodeness means strings are not equal.
		return false;
	}
	else
	{
#if WIN32
		return stricmp( AnsiName, InName ) == 0;
#else
        return strcasecmp( AnsiName, InName ) == 0;
#endif	// WIN32
	}
}

/**
 * Returns the size in bytes for NNameEntry structure. This is != sizeof(NNameEntry)
 * as we only allocated as needed
 * 
 * @param	Length		Length of name
 * @param	bIsPureAnsi	Whether name is pure ANSI or not
 * 
 * @return	Required size of NNameEntry structure to hold this string (might be unicode or ansi)
 */
int NNameEntry::GetSize( int Length )
{
	// Calculate base size without union array.
	int Size = sizeof(NNameEntry) - NAME_SIZE * sizeof( wchar_t );
	// Add size rquired for string
	Size += ( Length + 1 ) * sizeof ( char );

	return Size;
}

////////////////////////////////////////////////////////////////////////////////
//	NName statics.
////////////////////////////////////////////////////////////////////////////////
// Static variables
NNameEntry*					FName::NameHash[ NNameDefs::NameHashBucketCount ];
TArrayNoInit<NNameEntry*>	FName::Names;
int							FName::NameEntryMemorySize;
/** Number of ANSI names in name table */
int							FName::NumAnsiNames;
/** Number of UNICODE names in name table */
int							FName::NumUnicodeNames;



/**
 * Pooled allocator for NNameEntry structures. Doesn't have to worry about freeing memory as those
 * never go away. It simply uses 64K chunks and allocates new ones as space runs out. This reduces
 * allocation overhead significantly (only minor waste on 64k boundaries) and also greatly helps
 * with fragmentation as 50-100k allocations turn into tens of allocations.
 */
class NNameEntryPoolAllocator
{
public:
	/** Initializes all member variables. */
	NNameEntryPoolAllocator()
	{
		TotalAllocatedPages	= 0;
		CurrentPoolStart	= NULL;
		CurrentPoolEnd		= NULL;
	}
	
	/**
	 * Allocates the requested amount of bytes and casts them to a NNameEntry pointer.
	 *
	 * @param   Size  Size in bytes to allocate
	 * @return  Allocation of passed in size cast to a NNameEntry pointer.
	 */
	NNameEntry* Allocate( int Size )
	{
		// Allocate a new pool if current one is exhausted. We don't worry about a little bit
		// of waste at the end given the relative size of pool to average and max allocation.
		if( CurrentPoolEnd - CurrentPoolStart < Size )
		{
			AllocateNewPool();
		}
		//check( CurrentPoolEnd - CurrentPoolStart >= Size );
		// Return current pool start as allocation and increment by size.
		NNameEntry* NameEntry = (NNameEntry*) CurrentPoolStart;
		CurrentPoolStart += Size;
		return NameEntry;
	}
	
	/**
	 * Returns the amount of memory to allocate for each page pool.
	 *
	 * @return  Page pool size.
	 */
	inline int PoolSize()
	{
		// Allocate in 64k chunks as it's ideal for page size.
		return 64 * 1024;
	}
	
	/**
	 * Returns the number of pages that have been allocated so far for names.
	 *
	 * @return  The number of pages allocated.
	 */
	inline int PageCount()
	{
		return TotalAllocatedPages;
	}

	FORCEINLINE void Exit()
	{
		TotalAllocatedPages		= 0;
		CurrentPoolStart		= NULL;
		CurrentPoolEnd			= NULL;

		for ( INT i = 0; i < TotalAllocatedPages; i++ )
		{
			if ( AllocatedPages( i ) )
			{
				appFree( AllocatedPages( i ) );
			}
		}
	}
	
private:
	/** Allocates a new pool. */
	void AllocateNewPool()
	{
		TotalAllocatedPages++;
		CurrentPoolStart = (unsigned char*) malloc(PoolSize());
		CurrentPoolEnd = CurrentPoolStart + PoolSize();

		AllocatedPages.AddItem(CurrentPoolStart);
	}
	
	/** Beginning of pool. Allocated by AllocateNewPool, incremented by Allocate.	*/
	unsigned char* CurrentPoolStart;
	/** End of current pool. Set by AllocateNewPool and Checked by Allocate.		*/
	unsigned char* CurrentPoolEnd;
	/** Total number of pages that have been allocated.								*/
	int TotalAllocatedPages;

	TArray<BYTE*>	AllocatedPages;
};

/** Global allocator for name entries. */
NNameEntryPoolAllocator GNameEntryPoolAllocator;

NNameEntry* AllocateNameEntry( const void* Name, NAME_INDEX Index, NNameEntry* HashNext )
{
	const size_t NameLen = strlen( (char*)Name );
	int NameEntrySize = NNameEntry::GetSize( NameLen );
	NNameEntry* NameEntry = GNameEntryPoolAllocator.Allocate( NameEntrySize );
	FName::NameEntryMemorySize += NameEntrySize;

	NameEntry->Index	= ( Index << NAME_INDEX_SHIFT ) | 0;
	NameEntry->HashNext	= HashNext;

	// Can't rely on the template override for static arrays since the safe crt version of strcpy will fill in
	// the remainder of the array of NAME_SIZE with 0xfd. So, we have to pass in the length of the dynamically allocated array instead.
	strcpy( NameEntry->GetAnsiName(), (char*)Name );
	FName::NumAnsiNames++;

	return NameEntry;
}

////////////////////////////////////////////////////////////////////////////////
//	NName
////////////////////////////////////////////////////////////////////////////////
/**
 * Hardcode a name
 */
void FName::Hardcode( NNameEntry* AutoName )
{
	// Add name to name hash
	int iHash			= AutoName->GetNameHash() & ( ARRAY_COUNT( NameHash ) - 1 );
	AutoName->HashNext	= NameHash[ iHash ];
	NameHash[ iHash ]	= AutoName;
	
	// Expnad the table if needed.
	for ( int i =  Names.Num(); i <= AutoName->GetIndex(); i++ )
	{
		Names.AddItem(NULL);
	}

	// Add name to table.
	if ( Names( AutoName->GetIndex() ) )
	{
		// error!!!
		// Hardcoded name ... at index ... was duplicated. Existing entry is ...
	}

	Names( AutoName->GetIndex() ) = AutoName;
}

/**
 * Create a NName. If FindType is NNAME_Find, and the string part of the name
 * doesn't already exist, then the name will be NAME_None.
 * 
 * @param	Name		Value for the string portion of the name
 * @param	FindType	Action to take (see EFindName)
 */
FName::FName( const char* Name, EFindName FindType /* = NNAME_Add */, bool bUnused /* = true */ )
{
	Init( Name, NAME_NO_NUMBER_INTERNAL, FindType );
}

FName::FName( const char* Name, int InNumber, EFindName FindType /* = NNAME_Add */ )
{
	Init( Name, InNumber, FindType);
}

bool FName::operator==( const char* Other ) const
{
	// Find name entry associated with this NName
	assert( Other );
	assert( Index < (int)Names.Num() );
	NNameEntry* Entry = Names( Index );
	assert( Entry );

	// Temporary buffer to hold split name in case passed in name is of Name_Number format.
	char TempBuffer[NAME_SIZE];
	int InNumber = NAME_NO_NUMBER_INTERNAL;
	int TempNumber = NAME_NO_NUMBER_INTERNAL;

	// check whether we need to split the passed in string into name and number portion.
	if ( SplitNameWithCheck( Other, TempBuffer, ARRAY_COUNT( TempBuffer ), TempNumber ) )
	{
		Other = TempBuffer;
		InNumber = NAME_EXTERNAL_TO_INTERNAL( TempNumber );
	}

	// Report a match if both the number and string portion match
	bool bAreNamesMatching = false;
#if WIN32
	if ( InNumber == Number && 0 == stricmp( Other, Entry->GetAnsiName() ) )
#else
    if ( InNumber == Number && 0 == strcasecmp( Other, Entry->GetAnsiName() ) )
#endif	// WIN32
	{
		bAreNamesMatching = true;
	}

	return bAreNamesMatching;
}

int FName::Compare( const FName& Other ) const
{
	// Names match, check whether numbers match.
	if ( GetIndex() == Other.GetIndex() )
	{
		return GetNumber() - Other.GetNumber();
	}
	// Names don't match. This means we don't even need to check numbers.
	else
	{
		NNameEntry* ThisEntry = Names( GetIndex() );
		NNameEntry* OtherEntry = Names( Other.GetIndex() );

#if WIN32
		return stricmp( ThisEntry->GetAnsiName(), OtherEntry->GetAnsiName() );
#else
		return strcasecmp( ThisEntry->GetAnsiName(), OtherEntry->GetAnsiName() );
#endif	// WIN32
	}
}

void FName::Init( const char* InName, int InNumber, EFindName FindType, bool bSplitName /* = true */ )
{
	assert( strlen( InName ) <= NAME_SIZE );

	// initialize the name subsystem if necessary
	if ( !GetIsInitialized() )
	{
		StaticInit();
	}

	char TempBuffer[ NAME_SIZE ];
	memset( TempBuffer, 0, sizeof(char) * NAME_SIZE);
	int TempNumber = -1;

	// if we were passed in a number, we can't split again,
	// otherwise a_1_2_3_5 would change everytime it was loaded in.
	if ( InNumber == NAME_NO_NUMBER_INTERNAL && bSplitName == true)
	{
		if ( SplitNameWithCheck( InName, TempBuffer, ARRAY_COUNT( TempBuffer ), TempNumber ) )
		{
			InName = TempBuffer;
			InNumber = NAME_EXTERNAL_TO_INTERNAL( TempNumber );
		}
	}

	assert( InName );

	// If empty or invalid name was specified, return NAME_None.
	if ( !InName[0] )
	{
		Index = NAME_None;
		Number = NAME_NO_NUMBER_INTERNAL;
		return;
	}

	// set the number
	Number = InNumber;

	// Hash value of string. Depends on whether the name is going to be ansi or unicode.
	int iHash = -1;

	// Figure out whether we have a pure ansi name or not
	char AnsiName[ NAME_SIZE ];
	memset( AnsiName, 0, sizeof(char) * NAME_SIZE);
	strncpy( AnsiName, InName, ARRAY_COUNT( AnsiName ) );
	iHash = appStrihash( AnsiName ) & ( ARRAY_COUNT( NameHash ) - 1);

	// Try to find the name in the hash
	for ( NNameEntry* Hash = NameHash[ iHash ]; Hash; Hash = Hash->HashNext )
	{
		// Compare the passed in string, either ANSI or UNICODE
		if ( Hash->IsEqual( AnsiName ) )
		{
			// Found it in the hash.
			Index = Hash->GetIndex();

			// Check to see if the caller wants to replace the contents of the
			// NName with the specified value. This is useful for compiling
			// script classes whether the file name is lower case but the class
			// was intended to be uppercase.
			if ( FindType == NNAME_Replace )
			{
				// This should be impossible due to the compare above
				// This *must* be true, or we'll overwrite memory when the
				// copy happens if it is longer
				// check( appStrnel( InName ) == Hash->GetNameLength();
				// Can't rely on the template override for static arrays since
				// the safe crt version of strcpy will fill in
				// the remainder of the array of NAME_SIZE with 0xfd.
				// So, we have to pass in the length of the dynamically allocated
				// array instead.
				strcpy( Hash->GetAnsiName(), AnsiName );
			}

			return;
		}
	}

	// Didn't find name.
	if ( FindType == NNAME_Find )
	{
		// Not found
		Index = NAME_None;
		Number = NAME_NO_NUMBER_INTERNAL;
		return;
	}

	// Add new entry
	// Add new entry.
	Index = Names.Add();
	
	// Allocate and set the name.
	const void* NewName = NULL;
	//if( bIsPureAnsi )
	{
		NewName = AnsiName;
	}
	//else
	//{
	//	NewName = InName;
	//}
	Names(Index) = NameHash[iHash] = AllocateNameEntry( NewName, Index, NameHash[iHash] );//, bIsPureAnsi );
}

std::string FName::ToString() const
{
	std::string Out;
	ToString( Out );
	return Out;
}

void FName::ToString( std::string& Out ) const
{
	assert( Index < (int)Names.Num() );
	assert( Names( Index ) );

    Out.clear();
	NNameEntry* NameEntry = Names( Index );
	if ( NameEntry )
    {
        AppendString( Out );
    }
}

void FName::AppendString( std::string& Out ) const
{
	assert( Index < (int)Names.Num() );
	assert( Names( Index ) );

	NNameEntry* NameEntry = Names( Index );
	NameEntry->AppendNameToString( Out );
	if ( Number != NAME_NO_NUMBER_INTERNAL)
	{
		Out += "_";
		ItoaAppend( NAME_INTERNAL_TO_EXTERNAL( Number ), Out );
	}
}

int FName::GetNumber() const
{
	return Number;
}

////////////////////////////////////////////////////////////////////////////////
//	NName subsystem
////////////////////////////////////////////////////////////////////////////////
//
// Initialize the name subsystem.
//
void FName::StaticInit()
{
	// Global instance used to initialize the GCRCTable. It used to be initialized in appInit().
	static NCRCTableInit	GCRCTableInit;

	assert( GetIsInitialized() == false );
	assert( ( ARRAY_COUNT( NameHash ) & ( ARRAY_COUNT( NameHash ) - 1 ) ) == 0 );
	GetIsInitialized() = 1;

	// Initialize the TArrayNoInit() on first use, Not when the constructor is
	// called (as it could happen AFTER this function)
	appMemzero( &FName::Names, sizeof( FName::Names ) );
	

	// Init the name hash
	for ( int HashIndex = 0; HashIndex < ARRAY_COUNT( FName::NameHash ); HashIndex++ )
	{
		NameHash[ HashIndex ] = NULL;
	}

	// Register all hardcoded names
#define REGISTER_NAME( num, namestr ) Hardcode( AllocateNameEntry( #namestr, num, NULL ) );	// default using ansichar , true ) );
#include "PreDefinedNames.h"
}


bool FName::SplitNameWithCheck( const char* OldName, char* NewName, int NewNameLen, int& NewNumber )
{
	bool bSucceeded = false;
	const int OldNameLength = strlen( OldName );

	if ( OldNameLength > 0 )
	{
		// get string length
		const char* LastChar = OldName + ( OldNameLength - 1 );

		// if the last char is a number, then we will try to split
		const char* Ch = LastChar;
		if ( *Ch >= '0' && *Ch <= '9' )
		{
			// go backwards, looking an underscore or the start of the string
			// (we don't look at first char because '_9' won't split well
			while ( *Ch >= '0' && *Ch <= '9'&& Ch > OldName )
			{
				Ch--;
			}

			// if the first non-number was an underscore (as opposed to a letter),
			// we can split
			if ( *Ch == '_' )
			{
				// check for the case where there are multiple digits after the _ and the first one
				// is a 0 ("Rocket_04"). Can't split this case. (So, we check if the first char
				// is not 0 or the length of the number is 1 ( since Rocket_0 is valid )
				if ( Ch[1] != '0' || LastChar - Ch == 1 )
				{
					// attempt to convert what's following it to a number
					NewNumber = atoi( Ch + 1 );

					// copy the name portion into the buffer
					strncpy( NewName, OldName, Min<int>( Ch - OldName + 1, NewNameLen ) );

					// mark successful
					bSucceeded = true;
				}
			}
		}
	}

	return bSucceeded;
}

void FName::StaticExit()
{
	if ( !GetIsInitialized() )
	{
		return;
	}

	Names.Empty();

	GNameEntryPoolAllocator.Exit();

	GetIsInitialized() = 0;
}
