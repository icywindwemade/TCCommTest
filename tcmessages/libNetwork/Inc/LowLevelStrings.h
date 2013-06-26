/*========================================================================
	LowLevelStrings.h: Low level string utilities
	Copyright 2012. All Rights Reserved.
========================================================================*/

#ifndef __LOWLEVELSTRINGS_H__
#define __LOWLEVELSTRINGS_H__

#define UPPER_LOWER_DIFF	32

char appToUpper( char c );
wchar_t appToUpper( wchar_t c );

/** Case insensitive string hash function. */
DWORD appStrihash( const char* Data );

DWORD appStrihash( const wchar_t* Data );

long long appAtoi64( const char* InString );

#endif	// __LOWLEVELSTRINGS_H__