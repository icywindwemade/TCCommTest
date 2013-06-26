/**
 * @file	Core.h
 *
 * Declares the core class.
 */
#ifndef __CORE_H__
#define __CORE_H__

#if defined(_WIN32) || defined(_WIN64)
#include "VCWin32.h"
#elif __APPLE__
#include "TargetConditionals.h"

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
#include "iOS.h"
#elif TARGET_OS_MAC
#endif	// TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#elif __ANDROID__
#include "Android.h"
#else
#error Unsupported platform
#endif	// _WIN32 || _WIN64

#pragma pack( push )
#pragma warning( disable:4127 )		/* conditional expression is constant */
#pragma warning( disable:4512 )		/*  operator=() couldn't be generated */
#pragma pack( pop )


enum ENoInit		{ E_NoInit		=  0		};
enum EIndexNone     { INDEX_NONE    = -1        };

//
// Global constants.
//
#undef MININT
#undef MAXBYTE
#undef MAXWORD
#undef MAXDWORD
#undef MAXINT

enum { MINBYTE      = 0x00              };
enum { MINWORD      = 0x0000U           };
enum { MINDWORD     = 0x00000000U       };
enum { MINSBYTE     = 0x80              };
enum { MINSWORD     = 0x8000            };
enum { MININT       = 0x80000000        };

enum { MAXBYTE      = 0xff              };
enum { MAXWORD      = 0xffffU           };
enum { MAXDWORD     = 0xffffffffU       };
enum { MAXSBYTE     = 0x7f              };
enum { MAXSWORD     = 0x7fff            };
enum { MAXINT       = 0X7fffffff        };

//
// Unicode or single byte character set mappings.
// 
#ifdef UNICODE

#ifndef _TCHAR_DEFINED
typedef UNICHAR		TCHAR;
#endif	// _TCHAR_DEFINED
typedef UNICHARU	TCHARU;

#ifndef _TEXT_DEFINED
#undef	TEXT
#define TEXT(s)	L##s
#endif	// _TEXT_DEFINED

#ifndef _US_DEFINED
#undef US
#define US NString(L"")
#endif	// _US_DEFINED


inline TCHAR	FromAnsi	( ANSICHAR In )	{ return (BYTE)In;						  }
inline TCHAR	FromUnicode	( UNICHAR  In )	{ return In;							  }
inline ANSICHAR	ToAnsi		( TCHAR	   In ) { return (WORD)In < 0x100 ? In : MAXBYTE; }
inline UNICHAR	ToUnicode	( TCHAR	   In ) { return In;							  }

#else	// UNICODE

#undef TEXT
#define TEXT(s)	s

#undef US
#define US	NString("")

inline TCHAR    FromAnsi	( ANSICHAR In )	{ return In;										}
inline TCHAR    FromUnicode	( UNICHAR  In )	{ return (TCHAR)( (WORD)In < 0x100 ? In : MAXBYTE );}
inline ANSICHAR ToAnsi		( TCHAR	   In ) { return In;										}
inline UNICHAR  ToUnicode	( TCHAR	   In ) { return (BYTE)In;									}

#endif	// UNICODE

#include "MemoryBase.h"

#include "LowLevelStrings.h"
#include "TypeTraits.h"
#include "TemplateBase.h"
#include "MallocAnsi.h"

#include "ContainerAllocationPolicies.h"

//
// Container forward declarations.
//
template<typename ElementType,typename Allocator = NDefaultAllocator>
class TBaseArray;

template<typename T,typename Allocator = NDefaultAllocator>
class TArray;

template<typename T>
class TTransArray;

template<typename KeyType,typename ValueType,typename SetAllocator = NDefaultSetAllocator>
class TMap;

template<typename KeyType,typename ValueType,typename SetAllocator = NDefaultSetAllocator>
class TMultiMap;

#include "TArray.h"								// Dynamic array definitions.
#include "TBitArray.h"							// Bit array definitions.
#include "TSet.h"								// Set definitions.
#include "TSparseArray.h"						// Sparse array definitions.
#include "TMap.h"								// Dynamic map definitions.
#include "List.h"								// Dynamic list definitions.

#include "PredefinedNames.h"
#include "Name.h"
#include "MathBase.h"
#include "TSorting.h"

#include "Random.h"

extern WELL512* GRandom;
extern INT GIsCooking;

#define TEST_TMAP	0

#endif	// __CORE_H__