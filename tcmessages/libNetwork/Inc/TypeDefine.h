#pragma once

#ifndef CHAR
typedef char CHAR;
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef SHORT
typedef short SHORT;
#endif

#ifndef WORD
typedef unsigned short WORD;
#endif

#ifndef LONG
typedef long LONG;
#endif

#ifndef DWORD
typedef unsigned long DWORD;
#endif

#ifndef INT
typedef int INT;
#endif

#ifndef UINT
typedef unsigned int UINT;
#endif

#ifdef _WIN32

#ifndef LONGLONG
typedef __int64 LONGLONG;
#endif

#else

#ifndef LONGLONG
typedef long long LONGLONG;
#endif

#endif //  _WIN32

#ifdef _WIN32

#ifndef INT64
typedef __int64 INT64;
#endif

#else

#ifndef INT64
typedef long long INT64;
#endif

#endif //  _WIN32