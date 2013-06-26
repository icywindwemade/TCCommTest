/**
 * @file	TypeTraits.h
 *
 * Declares the type traits class.
 */
#ifndef __TYPETRAITS_H__
#define __TYPETRAITS_H__

/*-----------------------------------------------------------------------------
 * Macros to abstract the presence of certain compiler intrinsic type traits 
 -----------------------------------------------------------------------------*/
#if _MSC_VER >= 1400
#define HAS_TRIVIAL_CONSTRUCTOR(T) __has_trivial_constructor(T)
#define HAS_TRIVIAL_DESTRUCTOR(T) __has_trivial_destructor(T)
#define HAS_TRIVIAL_ASSIGN(T) __has_trivial_assign(T)
#define HAS_TRIVIAL_COPY(T) __has_trivial_copy(T)
#define IS_POD(T) __is_pod(T)
#define IS_ENUM(T) __is_enum(T)
#define IS_EMPTY(T) __is_empty(T)
#else
#define HAS_TRIVIAL_CONSTRUCTOR(T) false
#define HAS_TRIVIAL_DESTRUCTOR(T) false
#define HAS_TRIVIAL_ASSIGN(T) false
#define HAS_TRIVIAL_COPY(T) false
#define IS_POD(T) false
#define IS_ENUM(T) false
#define IS_EMPTY(T) false
#endif

/*-----------------------------------------------------------------------------
 Type traits similar to TR1 (uses intrinsics supported by VC8
 Should be upated/revisited/discarded when compiler support for tr1 catches up.
 ---------------------------------------------------------------------------*/

/**
 * TIsFloatType
 */
template<typename T> struct TIsFloatType { enum { Value = false }; };

template<> struct TIsFloatType<float> { enum { Value = true }; };
template<> struct TIsFloatType<double> { enum { Value = true }; };
template<> struct TIsFloatType<long double> { enum { Value = true }; };

/**
 * TIsIntegralType
 */
template<typename T> struct TIsIntegralType { enum { Value = false }; };

template<> struct TIsIntegralType<unsigned char> { enum { Value = true }; };
template<> struct TIsIntegralType<unsigned short> { enum { Value = true }; };
template<> struct TIsIntegralType<unsigned int> { enum { Value = true }; };
template<> struct TIsIntegralType<unsigned long> { enum { Value = true }; };

template<> struct TIsIntegralType<signed char> { enum { Value = true }; };
template<> struct TIsIntegralType<signed short> { enum { Value = true }; };
template<> struct TIsIntegralType<signed int> { enum { Value = true }; };
template<> struct TIsIntegralType<signed long> { enum { Value = true }; };

template<> struct TIsIntegralType<bool> { enum { Value = true }; };
template<> struct TIsIntegralType<char> { enum { Value = true }; };

// C99, but all compilers we use support it
template<> struct TIsIntegralType<unsigned long long> { enum { Value = true }; };
template<> struct TIsIntegralType<signed long long> { enum { Value = true }; };

/**
 * TIsArithmeticType
 */
template<typename T> struct TIsArithmeticType
{
	enum { Value = TIsIntegralType<T>::Value || TIsFloatType<T>::Value };
};

/**
 * TIsPointerType
 */
template<typename T> struct TIsPointerType						{ enum { Value = false }; };
template<typename T> struct TIsPointerType<T*>					{ enum { Value = false }; };
template<typename T> struct TIsPointerType<const T*>			{ enum { Value = false }; };
template<typename T> struct TIsPointerType<const T* const>		{ enum { Value = false }; };
template<typename T> struct TIsPointerType<T* volatile>			{ enum { Value = false }; };
template<typename T> struct TIsPointerType<T* const volatile>	{ enum { Value = false }; };

/**
 * TIsVoidType
 */
template<typename T> struct TIsVoidType { enum { Value = false }; };
template<> struct TIsVoidType<void> { enum { Value = true }; };
template<> struct TIsVoidType<void const> { enum {Value = true }; };
template<> struct TIsVoidType<void volatile> { enum {Value = true }; };
template<> struct TIsVoidType<void const volatile> { enum {Value = true }; };

/**
 * TIsPODType
 * POD array and member pointer detection
 */
template<typename T> struct TIsPODType
{
	enum { Value = IS_POD(T) || IS_ENUM(T) || TIsArithmeticType<T>::Value || TIsPointerType<T>::Value };
};

/**
 * TIsFundamentalType
 */
template<typename T>
struct TIsFundamentalType
{
	enum { value = TIsArithmeticType<T>::Value || TIsVoidType<T>::Value };
};

/*-----------------------------------------------------------------------------
 Call traits - Modeled somewhat after boost's interfaces.
 -----------------------------------------------------------------------------*/
/**
 * Get traits helpers
 */
template <typename T, bool TypeIsSmall>
struct TCallTraitsParamTypeHelper
{
	typedef const T& ParamType;
	typedef const T& ConstParamType;
};

template <typename T>
struct TCallTraitsParamTypeHelper<T, true>
{
	typedef const T ParamType;
	typedef const T ConstParamType;
};

template <typename T>
struct TCallTraitsParamTypeHelper<T*, true>
{
	typedef T* ParamType;
	typedef const T* ConstParamType;
};

/**
 * TCallTraits
 *
 * Same call traits as boost, though not with as complete a solution.
 *
 * The main member to note is ParamType, which sepcifies the optimal
 * form to pass the type as a parameter to a function.
 *
 * Has a small-value optimization when a type is a POD type and as small as a pointer.
 */

/**
 * base class for call traits. Used to more easily refine portions when specializing
 */
template <typename T>
struct TCallTraitsBase
{
private:
	enum { PassByValue = TIsArithmeticType<T>::Value || TIsPointerType<T>::Value || (TIsPODType<T>::Value && sizeof(T) <= sizeof(void*)) };
	
public:
	typedef T ValueType;
	typedef T& Reference;
	typedef const T& ConstReference;
	typedef typename TCallTraitsParamTypeHelper<T, PassByValue>::ParamType ParamType;
	typedef typename TCallTraitsParamTypeHelper<T, PassByValue>::ConstParamType ConstPointerType;
};

/**
 * TCallTraits
 */
template <typename T>
struct TCallTraits : public TCallTraitsBase<T> {};

/**
 * Fix reference-to-reference problems.
 */
template <typename T>
struct TCallTraits<T&>
{
	typedef T& valueType;
	typedef T& Reference;
	typedef const T& ConstReference;
	typedef T& ParamType;
	typedef T& ConstPointerType;
};

/**
 * Array types
 */
template <typename T, size_t N>
struct TCallTraits<T [N]>
{
private:
	typedef T ArrayType[N];
	
public:
	typedef const T* ValueType;
	typedef ArrayType& Reference;
	typedef const ArrayType& ConstReference;
	typedef const T* const ParamType;
	typedef const T* const ConstPointerType;
};

/**
 * const array types
 */
template <typename T, size_t N>
struct TCallTraits<const T [N]>
{
private:
	typedef const T ArrayType[N];
	
public:
	typedef const T* ValueType;
	typedef ArrayType& Reference;
	typedef const ArrayType& ConstReference;
	typedef const T* const ParamType;
	typedef const T* const ConstPointerType;
};

/*-----------------------------------------------------------------------------
 Traits for our particular container classes
 -----------------------------------------------------------------------------*/

/**
 * Helper for array traits. Provides a common base to more easily refine a portion of the raits
 * when specializing. NeedsConstructor/NeedsDestructor is mainly used by the contiguous storage
 * containers like TArray.
 */
template<typename T>
struct TContainerTraitsBase
{
	typedef typename TCallTraits<T>::ParamType ConstInitType;
	typedef typename TCallTraits<T>::ConstPointerType ConstPointerType;
	
	enum { NeedsConstructor = !HAS_TRIVIAL_CONSTRUCTOR(T) && !TIsPODType<T>::Value };
	enum { NeedsDestructor = !HAS_TRIVIAL_CONSTRUCTOR(T) && !TIsPODType<T>::Value };
};

/**
 * Traits for container classes.
 */
template<typename T>
struct TContainerTraits : public TContainerTraitsBase<T> {};

#endif	// __TYPETRAITS_H__
