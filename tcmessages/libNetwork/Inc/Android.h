/**
 * @file	Android.h
 *
 * Declares the iOS interface.
 */
#ifndef __ANDROID_H__
#define __ANDROID_H__

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include <assert.h>

// Baisc typedefs
#undef UINT
#undef PTRINT
#undef UPTRINT

// Unsigned base types.
typedef unsigned int    UINT;       // 32-bit unsigned.

// Other base types.
typedef UINT            UBOOL;
typedef intptr_t        PTRINT;     // Integer large enough to hold pointer.
typedef uintptr_t       UPTRINT;    // Unsigned integer large enough to hold a pointer.

#ifndef DWORD
typedef unsigned long DWORD;
#endif

#ifndef FLOAT
typedef float FLOAT;
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

typedef u_int16_t   WORD;
typedef u_int64_t   QWORD;
typedef intptr_t    PTRINT;

// Signed base types.
typedef __int8_t    SBYTE;          // 8-bit signed.
typedef __int16_t   SWORD;          // 16-bit signed.
typedef __int32_t   INT;            // 32-bit signed.
typedef long        LONG;           // Windows type
typedef LONG*       LPLONG;         // Windows type

typedef __int64_t   SQWORD;         // 64-bit signed.

// Character types.
typedef char            ANSICHAR;   // An ANSI character. normally a signed type.
typedef unsigned char   ANSICHARU;
typedef wchar_t         UNICHAR;    // A unicode character. normally a signed type.
typedef wchar_t         WCHAR;      // Windows type
typedef wchar_t         UNICHARU;   // excatly reasearch

#ifndef UNICODE
#define TCHAR   ANSICHAR
#define TCHAR_CALL_OS(funcW, funcA) (funcA)
#define TCHAR_TO_ANSI(str) str
#define ANSI_TO_TCHAR(str) str
#else
typedef wchar_t TCHAR;
#define TCHAR_IS_4_BYTES    1
#define _TCHAR_DEFINED

#define CP_OEMCP    1
#define CP_ACP      1

#include <wchar.h>
#include <wctype.h>
#endif  // UNICODE


#define __forceinline inline
#define FORCEINLINE inline
#define SIZE_T      size_t

#undef FALSE
#undef TRUE
#define FALSE   0
#define TRUE    1

#define VARARGS     __cdecl
#define STDCALL     __stdcall

#ifdef _MSC_VER
#define FORCENOINLINE __declspec(noinline)
#else
#define FORCENOINLINE
#endif  // _MSC_VER

// Alignment
#define GCC_PACK(n)
#define GCC_ALIGN(n)    __attribute__ ((aligned(n)))
#define GCC_BITFIELD_MAGIC
#define MS_ALIGN(n)

#define RESTRICT    __restrict

// this function used only Windows
//extern "C" void* __cdecl _alloca(size_t);
#define appAlloca(Size) ((Size==0) ? 0 : alloca((Size+7)&~7))


//
// Sizes.
//
// Default boundary to align memory allocations on.
enum EMemoryAlignment { DEFAULT_ALIGNMENT = 8 };

/**
 * http://www.innovative-dsp.com/support/onlinehelp/Malibu/Innovative__Ones@unsigned_int.html
 *
 * @return     Returns the number of non-zero bits within the specified, 32-bit parameter,
 *                     using a variable-precision SWAR algorithm to perform a tree reduction adding
 *                     the bits in a 32-bit value.
 */
inline unsigned int Ones(unsigned int x)
{
    // 32-bit recursive reduction using SWAR...  but first step is mapping 2-bit values
    // into sum of 2 1-bit values in sneaky way
    x -= ((x >> 1) & 0x55555555);
    x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
    x = (((x >> 4) + x) & 0x0f0f0f0f);
    x += (x >> 8);
    x += (x >> 16);
    
    return(x & 0x0000003f);
}

/**
 * http://www.innovative-dsp.com/support/onlinehelp/Malibu/Innovative__FloorLog2@register_unsigned_int.html
 *
 * @return        the closest integer value less than or equal to the base 2 log of input parameter
 */
inline unsigned int FloorLog2(register unsigned int x)
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    
#ifdef  LOG0UNDEFINED
    return(Ones(x) - 1);
#else
    return(Ones(x >> 1));
#endif
}
/**
 * Computes the base 2 logarithm for an integer value that is greater than 0.
 * The result is rounded down to the nearest integer.
 *
 * @param      Value the value to compute the log of
 */
inline DWORD appFloorLog2(DWORD Value)
{
    // Use BSR to return the log2 of the integer
    DWORD Log2 = FloorLog2(Value);
    return Log2;
}
/**
 * Counts the number of leading zeros in the bit representation of the value
 *
 * @param      Value   the value to determine the number of leading zeros for
 *
 * @return     the number of zeros before the first "on" bit
 */
inline DWORD appCountLeadingZeros(DWORD Value)
{
    if (Value == 0)
    {
        return 32;
    }
    
    return 31 - appFloorLog2(Value);
}

/**
 * Returns smallest N such that (1 << N) >= Arg.
 * Note: AppCeilLogTwo(0) = 0 because (1 << 0 ) = 1 >= 0.
 */
FORCEINLINE DWORD appCeilLogTwo( DWORD Arg )
{
    INT Bitmask = ((INT)(appCountLeadingZeros(Arg) << 26)) >> 31;
    return (32 - appCountLeadingZeros(Arg - 1)) & (~Bitmask);
}

/** @return Rounds the given number up to the next highest power of two. */
FORCEINLINE UINT appRoundUpToPowerOfTwo(UINT Arg)
{
    return 1 << appCeilLogTwo(Arg);
}

FORCEINLINE FLOAT appSqrt( FLOAT Value )
{
    return sqrtf( Value );
}

FORCEINLINE FLOAT appInvSqrt(FLOAT F)
{
	return 1.0f / sqrtf(F);
}

/**
 * Converts a float to the nearest integer. Rounds up when the fraction is .5
 * @param	F	Floating point value to convert
 *
 * @return	The nearest integer to 'F'.
 */
FORCEINLINE INT appRound(FLOAT F)
{
    return ( F > 0.0f ) ? INT(floor( F + 0.5f )) : INT(ceil( F - 0.5f ));
}

#endif	// __ANDROID_H__