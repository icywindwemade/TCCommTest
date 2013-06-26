/**
 * @file	TemplateBase.h
 *
 * Declares the template base class.
 */
#ifndef __TEMPLATEBASE_H__
#define __TEMPLATEBASE_H__

#include <string>
#include "LowLevelStrings.h"

//
// Hash functions for common types.
//
inline DWORD GetTypeHash( const BYTE A )
{
	return A;
}

inline DWORD GetTypeHash( const SBYTE A )
{
	return A;
}
inline DWORD GetTypeHash( const WORD A )
{
	return A;
}
inline DWORD GetTypeHash( const SWORD A )
{
	return A;
}
inline DWORD GetTypeHash( const INT A )
{
	return A;
}
inline DWORD GetTypeHash( const DWORD A )
{
	return A;
}
inline DWORD GetTypeHash( const QWORD A )
{
	return (DWORD)A+((DWORD)(A>>32) * 23);
}
inline DWORD GetTypeHash( const SQWORD A )
{
	return (DWORD)A+((DWORD)(A>>32) * 23);
}
//inline DWORD GetTypeHash( const TCHAR* S )
//{
//	return appStrihash(S);
//}

inline DWORD GetTypeHash( const void* A )
{
	return (DWORD)(PTRINT)A;
}

inline DWORD GetTypeHash( void* A )
{
	return (DWORD)(PTRINT)A;
}

inline DWORD GetTypeHash( const std::basic_string<char>& A )
{
	return (DWORD)appStrihash(A.c_str());
}

inline DWORD GetTypeHash( const std::basic_string<wchar_t>& A )
{
	return (DWORD)appStrihash(A.c_str());
}

//
//	Type information.
//
/** Base type information for atomic types which pass by value. */
template<typename T>
class TTypeInfoAtomicBase
{
public:
	typedef T ConstInitType;
	enum { NeedsConstructor = 0	};
	enum { NeedsDestructor  = 0 };
};

/** Base type information for constructed types which pass by reference. */
template<typename T>
class TTypeInfoConstructedBase
{
public:
	typedef const T& ConstInitType;
	enum { NeedsConstructor = 1 };
	enum { NeedsDestructor = 1  };
};

/** The default behaviour is for types to behave as constructed types. */
template <typename T>
class TTypeInfo : public TTypeInfoConstructedBase<T>{};

/** C-Style pointers require no construction. */
template<typename T>
class TTypeInfo<T*> : public TTypeInfoAtomicBase<T*> {};

/** Choose between two different classes based on a boolean. */
template<bool Predicate,typename TrueClass,typename FalseClass>
class TChooseClass {};

template<typename TrueClass,typename FalseClass>
class TChooseClass<true,TrueClass,FalseClass>
{
public:
	typedef TrueClass Result;
};

template<typename TrueClass,typename FalseClass>
class TChooseClass<false,TrueClass,FalseClass>
{
public:
	typedef FalseClass Result;
};

//
// Standard templates
//
template<class T>
inline T Abs(const T A)
{
	return (A >= (T)0) ? A : -A;
}


template<class T>
inline T Max(const T A, const T B)
{
	return (A >= B) ? A : B;
}

template<class T>
inline T Min(const T A, const T B)
{
	return (A <= B) ? A : B;
}

template< class T > inline T Max3( const T A, const T B, const T C )
{
	return Max ( Max( A, B ), C );
}

template< class T > inline T Min3( const T A, const T B, const T C )
{
	return Min ( Min( A, B ), C );
}


template<class T>
inline T Clamp(const T X, const T Min, const T Max)
{
	return X < Min ? Min : X < Max ? X : Max;
}

template<class T>
inline T Align(const T Ptr, INT Alignment)
{
	return (T)(((PTRINT)Ptr + Alignment - 1) & ~(Alignment - 1));
}

template<class T> inline void Swap(T& A, T& B)
{
	const T Temp = A;
	A = B;
	B = Temp;
}

template<class T> inline void Exchange(T& A, T& B)
{
	Swap(A, B);
}

/**
 * Chooses between the two parameters based on whether the first is NULL or not.
 * @return If the first parameter provided is non-NULL, it is returned; otherwise the second parameter is returned.
 */
template<typename ReferencedType>
ReferencedType* IfAThenAElseB(ReferencedType* A, ReferencedType* B)
{
	const PTRINT IntA = reinterpret_cast<PTRINT>(A);
	const PTRINT IntB = reinterpret_cast<PTRINT>(B);
	
	// Compute a mask which has all bits set if IntA is zero, and no bits set if it's non-zero.
	const PTRINT MaskB = -(!IntA);
	
	return reinterpret_cast<ReferencedType*>(IntA | (MaskB & IntB));
}


/** This is used to provide type specific behavior for a move which may change the value of B. */
template<typename T>
void Move(T& A,typename TContainerTraits<T>::ConstInitType B)
{
	// Destruct the previous value of A.
	A.~T();
	
	// Use placement new and a copy constructor so types with const members will work.
	new(&A) T(B);
}

#endif	// __TEMPLATEBASE_H__