/*========================================================================
	LowLevelStrings.cpp: Low level string utilities.
	Copyright 2012. All Rights Reserved.
========================================================================*/

#include "Core.h"


char appToUpper( char c )
{
	// compiler generates incorrect code if we try to use TEXT('char') instead of the numeric values directly
	//@hack - ideally, this would be data driven or use some sort of lookup table
	// some special cases
	switch ( (wchar_t)c )
	{
		// these special chars are not 32 apart
	case 255: return (char)159; // diaeresis y
	case 156: return (char)140; // digraph ae

		// characters within the 192 - 255 range which have no uppercase/lowercase equivalents
	case 240:
	case 208:
	case 223:
	case 247:
		return c;
	}

	if ( (c >= TEXT('a') && c <= TEXT('z')) || (c > 223 && c < 255) )
	{
		return c - UPPER_LOWER_DIFF;
	}

	// no uppercase equivalent
	return c;
}

wchar_t appToUpper( wchar_t c )
{
	// compiler generates incorrect code if we try to use TEXT('char') instead of the numeric values directly
	//@hack - ideally, this would be data driven or use some sort of lookup table
	// some special cases
	switch ( (wchar_t)c )
	{
		// these special chars are not 32 apart
	case 255: return (char)159; // diaeresis y
	case 156: return (char)140; // digraph ae

		// characters within the 192 - 255 range which have no uppercase/lowercase equivalents
	case 240:
	case 208:
	case 223:
	case 247:
		return c;
	}

	if ( (c >= TEXT('a') && c <= TEXT('z')) || (c > 223 && c < 255) )
	{
		return c - UPPER_LOWER_DIFF;
	}

	// no uppercase equivalent
	return c;
}

DWORD appStrihash( const char* Data )
{
	DWORD Hash=0;
	while( *Data )
	{
		char Ch = appToUpper(*Data++);
		WORD  B  = Ch;
		Hash     = ((Hash >> 8) & 0x00FFFFFF) ^ GCRCTable[(Hash ^ B) & 0x000000FF];
	}
	return Hash;
}

DWORD appStrihash( const wchar_t* Data )
{
	DWORD Hash=0;
	while( *Data )
	{
		wchar_t Ch = appToUpper(*Data++);
		WORD  B  = Ch;
		Hash     = ((Hash >> 8) & 0x00FFFFFF) ^ GCRCTable[(Hash ^ B) & 0x000000FF];
		B		 = Ch >> 8;
		Hash     = ((Hash >> 8) & 0x00FFFFFF) ^ GCRCTable[(Hash ^ B) & 0x000000FF];
	}
	return Hash;
}

long long appAtoi64( const char* InString )
{
#if WIN32
	return _atoi64( InString );
#else
	return strtoll( InString, NULL, 0 );
#endif // WIN32
}
