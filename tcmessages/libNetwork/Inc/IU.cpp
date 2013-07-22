
#include "Core.h"
#include "IUDefine.h"
#include <stdlib.h>
#include "IU.h"
#include "LocalText.h"

#if WIN32
#include <tchar.h>
#elif IOS// IOS
#include <wchar.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#endif	// WIN32

IU *gIU = NULL;
char ResourcePath[2048] = {0,};
char DocPath[2048] = {0,};

#ifdef USE_FRAMECHECK
FrameCheck g_UpdateTime;
FrameCheck g_RenderTime;
#endif

//
//	Global Variables
//
INT GIsCooking = 0;


IU::IU()
: m_fFPS( 0.f )
, m_CurTime( 0 )
#if !FINAL_RELEASE
, m_fSpeed( 1.0f)
#endif	// !FINAL_RELEASE
, FrameCount( 0 )
{
	bDestroyEnemyCastle = false;
	bDestroyMyCastle = false;
	bSpeedUp = false;
	bSpeedDown = false;
    
}

IU::~IU()
{
	if ( GRandom )
	{
		delete GRandom;
		GRandom = NULL;
	}
}

bool IU::Init( int iWidth, int iHeight, int iScale, int dir )
{
#if TEST_TMAP
	FName::StaticInit();
#endif	// TEST_TMAP

	GRandom = new WELL512();
	assert( GRandom );
	GRandom->seed( 100 );

	m_iWidth =iWidth;
	m_iHeight = iHeight;
	
	m_CurTime = GetAccurateTick();
#if WIN32
	m_Rand.SetSeed( (UINT)m_CurTime );
#elif ANDROID
	m_Rand.SetSeed( (UINT)m_CurTime );
#else	// IOS
	UINT seed = fmod( m_CurTime, 0xffffffff);
	m_Rand.SetSeed( seed );
#endif	// WIN32

	return true;
}
/*
int IU::LoadTexture( const std::string &lpFilename )
{
	return m_TextureMgr.LoadTexture( lpFilename );
}


int IU::UpdateTime()
{
#if WIN32
	DWORD curtime = timeGetTime();
#elif ANDROID
	struct timeval tv;
	gettimeofday( &tv, NULL );
	DWORD curtime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
#else	// IOS
	double curtime = CFAbsoluteTimeGetCurrent()* 1000.0;
#endif	// WIN32
	int delta =	(int)( curtime - m_CurTime );
	int num = delta / DeltaMilisec;
	if( num == 0 )
	{
		m_iDelta = 0;
		return 0;
	}
	else 
	{
		m_iDelta = num * DeltaMilisec;
		m_CurTime += num*DeltaMilisec;
	}

#if !FINAL_RELEASE
	m_iDelta = (int)(DeltaMilisec * m_fSpeed);
	UpdateForFPS( m_iDelta );
#endif	// !FINAL_RELEASE

	return m_iDelta;
}

int IU::GetUpdateNum()
{
#if WIN32
	DWORD curtime = timeGetTime();
#elif ANDROID
	struct timeval tv;
	gettimeofday( &tv, NULL );
	DWORD curtime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
#else	// IOS
	double curtime = CFAbsoluteTimeGetCurrent()* 1000.0;
#endif	// WIN32
	int delta =	(int)( curtime - m_CurTime );

#if !FINAL_RELEASE
	delta = (int)(delta * m_fSpeed);
#endif	// !FINAL_RELEASE

	int num = delta / DeltaMilisec;
#if !FINAL_RELEASE
	if( num > 0 )
		UpdateForFPS( DeltaMilisec * num );
#endif	// !FINAL_RELEASE
	return num;
}
*/
int IU::UpdateDelta()
{
#if !FINAL_RELEASE
	m_CurTime += (DWORD)(DeltaMilisec / m_fSpeed);
#else	// FINAL_RELEASE
	m_CurTime += DeltaMilisec;
#endif	// !FINAL_RELEASE

	return DeltaMilisec;
}

void IU::UpdateForFPS( int delta )
{
	static int tick[64], tickn = 0;
	tick[tickn++&63] = delta;
	if( tickn > 64 )
	{
		int total = 0;
		int i;
		for( i = 0; i < 64; i++ )
			total += tick[i];
		m_fFPS = 1000.f / total *i;
	}
}
/*
#if WIN32
DWORD IU::GetAccurateTick()
{
	return  timeGetTime();
}
DWORD IU::GetCurTick()
{
	return m_CurTime;
}
#elif ANDROID
DWORD IU::GetAccurateTick()
{
	struct timeval tv;
	gettimeofday( &tv, NULL );
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
DWORD IU::GetCurTick()
{
	return m_CurTime;
}
#else	// IOS
double IU::GetAccurateTick()
{
	return CFAbsoluteTimeGetCurrent()* 1000.0;
}
double IU::GetCurTick()
{
	return m_CurTime;
}
#endif	// WIN32

*/
int ParseString( char **szString, char *szOut )
{
	for(; **szString && strchr("\n\r\t \0", **szString); (*szString)++) ;
	int i;
	for( i=0; **szString && !strchr("\n\r\t \0", **szString); i++, (*szString)++)
		szOut[i] = **szString;

	szOut[i] = L'\0';
	return i;
}

void IUPrintf( WCHAR *wszOut, const WCHAR *fmt, ... )
{
	va_list va_arg;
	WCHAR wszNumber[10];
	WCHAR *pBuf = wszOut;

	va_start( va_arg, fmt );

	while( *fmt != 0 )
	{
		if( *fmt == L'%' )
		{
			fmt++;
			switch( *fmt )
			{
			case L'%' :
				{
					*pBuf = *fmt;
					pBuf++;
				}
				break;
			case L'd' :
				{
					int i = 0;
					int num = va_arg( va_arg, int );

					do
					{
						wszNumber[i] = (num %10) + L'0';
						num = num / 10;
						i++;
					}
					while( num != 0 );

					while( i > 0)
					{
						i--;
						*pBuf = wszNumber[i];
						pBuf++;
					}
				}
				break;
			case L's' :
				{
					WCHAR * chr = va_arg( va_arg, WCHAR * );
					while( chr && *chr != 0 )
					{
						*pBuf = *chr;
						pBuf++;
						chr++;
					}

				}
				break;
			}
			fmt++;
		}
		else
		{
			*pBuf = *fmt;
			pBuf++;
			fmt++;
		}
	}

	*pBuf = NULL;

	va_end( va_arg );

}

void IUPrintf2( WCHAR *wszOut, int size, const WCHAR *fmt, ... )
{
	memset( wszOut, 0, sizeof(WCHAR)* size);
	
	va_list va_arg;
	WCHAR wszNumber[60];
	WCHAR *pBuf = wszOut;

	va_start( va_arg, fmt );

	int iSize = 0;

	while( *fmt != 0 )
	{
		if( *fmt == L'%' )
		{
			fmt++;
			switch( *fmt )
			{
			case L'd' :
				{
					int i = 0;
					int num = va_arg( va_arg, int );

					do
					{
						wszNumber[i] = (num %10) + L'0';
						num = num / 10;
						i++;
					}
					while( num != 0 );


					while( i > 0)
					{
						i--;
						*pBuf = wszNumber[i];
						pBuf++;
						if( ++iSize > size )
							goto IUPRINTF_END;
					}
				}
				break;
			case L's' :
				{
					WCHAR * chr = va_arg( va_arg, WCHAR * );
#if ANDROID
					while( *chr != 0 )
					{
						*pBuf = *chr;
						pBuf++;
						chr++;
						if( ++iSize > size )
							goto IUPRINTF_END;
					}
#else	// WIN32 || IOS
					while( chr && *chr != 0 )
					{
						*pBuf = *chr;
						pBuf++;
						chr++;
					}
#endif	// ANDROID

				}
				break;
			}
			fmt++;
		}
		else
		{
			*pBuf = *fmt;
			pBuf++;
			fmt++;
			if( ++iSize > size )
				goto IUPRINTF_END;
		}
	}

IUPRINTF_END :

	*pBuf = NULL;

	va_end( va_arg );
}

/*

int ConvertUTF8( const char* szSrc, char* szOut, int maxLen )
{
#if WIN32

	if( IsUTF8String(szSrc) )
	{
		strncpy( szOut, szSrc, maxLen );
	}
	else
	{
		int nLength, nLength2;
		BSTR     bstrCode;
		char    *pszUTFCode = NULL;
		nLength = MultiByteToWideChar(CP_ACP, 0, szSrc, lstrlen(szSrc), NULL, NULL);
		bstrCode = SysAllocStringLen(NULL, nLength);
		MultiByteToWideChar(CP_ACP, 0, szSrc, lstrlen(szSrc), bstrCode, nLength);
		nLength2 = WideCharToMultiByte(CP_UTF8, 0, bstrCode, -1, pszUTFCode, 0, NULL, NULL);
		WideCharToMultiByte(CP_UTF8, 0, bstrCode, -1, szOut, nLength2+1, NULL, NULL);
	}

	return strlen( szOut );

#elif ANDROID

	if( !IsUTF8String(szSrc) )
	{
		return JniUtil::ConvertMultiToUTF8( szSrc, szOut, maxLen );
	}
	else
	{
		strncpy( szOut, szSrc, maxLen );

		return strlen( szOut );
	}
#else	// IOS
	if( !IsUTF8String(szSrc) )
	{
		NSString* str = [NSString stringWithUTF8String: szSrc];    
		strncpy( szOut, [str cStringUsingEncoding:NSUTF8StringEncoding], maxLen );

		return strlen( szOut );
	}
	else
	{
		strncpy( szOut, szSrc, maxLen );

		return strlen( szOut );
	}
#endif	// WIN32
}
 */

void IU::Log( const char* pFormat, ... )
{
	char szBuf[1024] = {0,};
	va_list marker;
	va_start(marker, pFormat );
	vsprintf( szBuf, pFormat, marker);
	va_end( marker );
#if WIN32
	
	int pos = strlen(szBuf);
	if( szBuf[pos -1] != '\n' )
	{
		szBuf[pos] = '\n';
		szBuf[pos+1] = 0;

	}
	OutputDebugString( szBuf );
#elif ANDROID
	//LogPrintf( szBuf );
#else	// IOS
    printf( "%s", szBuf );
	//NSString *szLog = [NSString stringWithUTF8String:szBuf];
	// NSLog( @"%@",szLog );
#endif	// WIN32
}

int Utf8::GetUTF8( const char * unicode, char *buf )
{
//	// OS X uses 32-bit char
//	const int bytes = wcslen(unicode) * sizeof(char_t);
//	// comp_bLittleEndian is in the lib I use in order to detect PowerPC/Intel
//	CFStringEncoding encoding = kCFStringEncodingUTF32LE;
//	CFStringRef str = CFStringCreateWithBytesNoCopy(NULL, 
//													(const UInt8*)unicode, bytes, 
//													encoding, false, 
//													kCFAllocatorNull
//													);
//	
//	const int bytesUtf8 = CFStringGetMaximumSizeOfFileSystemRepresentation(str);
//	CFStringGetFileSystemRepresentation(str, buf, bytesUtf8);
//	CFRelease(str);
//	return strlen( buf );
	return 0;
}

int Utf8::GetUnicode( const char * utf8, char *buf )
{
//	// OS X uses 32-bit wchar
//	const int bytes = strlen(utf8) * sizeof(char);
//	// comp_bLittleEndian is in the lib I use in order to detect PowerPC/Intel
//	CFStringEncoding encoding = kCFStringEncodingUTF32LE;
//	const NSStringEncording kEndording = CFStringConvertEncordingToNSStringEncording( kCFStringEncodingUTF32LE );
//
//	
//	CFStringRef str = CFStringCreateWithBytesNoCopy(NULL, 
//													(const UInt8*)utf8, bytes, 
//													encoding, false, 
//													kCFAllocatorNull
//													);
//	
//	const int bytesUtf8 = CFStringGetMaximumSizeOfFileSystemRepresentation(str);
//	CFStringGetFileSystemRepresentation(str, buf, bytesUtf8);
//	CFRelease(str);
//	return strlen( buf );
	return 0;
}


Utf8::Utf8(const wchar_t* wsz)
: m_utf8(NULL)
{
    /*
#if IOS
	// 한글 테스트...
	// OS X uses 32-bit wchar
	const int bytes = wcslen(wsz) * sizeof(wchar_t);
	// comp_bLittleEndian is in the lib I use in order to detect PowerPC/Intel
	CFStringEncoding encoding = kCFStringEncodingUTF32LE;
	CFStringRef str = CFStringCreateWithBytesNoCopy(NULL, 
													(const UInt8*)wsz, bytes, 
													encoding, false, 
													kCFAllocatorNull
													);
	
	const int bytesUtf8 = CFStringGetMaximumSizeOfFileSystemRepresentation(str);
	m_utf8 = new char[bytesUtf8];
	CFStringGetFileSystemRepresentation(str, m_utf8, bytesUtf8);
	CFRelease(str);
#endif	// IOS   */
}   

/*
int IUGetFullFileName( char *outBuf, const char * szFilename, bool bBinaryPath  )
{
#if ANDROID
	bool bArchive;
#endif	// ANDROID

	if ( GIsCooking )
	{
		strcpy(outBuf,ResourcePath);
		const char * pos = szFilename;
#if !WIN32
		pos = strrchr(pos, '/');
		if (pos == NULL )
		{
			strcat(outBuf, "/" );
			pos = szFilename;
		}
#endif	// !WIN32
		strcat(outBuf,pos);
	}
	else
	{
		if( bBinaryPath )
		{
			strcpy(outBuf,ResourcePath);
			const char * pos = szFilename;
#if !WIN32
			pos = strrchr(pos, '/');
			if (pos == NULL )
			{
				strcat(outBuf, "/" );
				pos = szFilename;
			}
#endif	// !WIN32
			strcat(outBuf,pos);
		}
		else
		{
			const char * pos = strrchr(szFilename, '/');
			if (pos == NULL )
				pos = szFilename;
			else
				pos++;

#if ANDROID
			IUGameManager().GetFileManager()->GetFullFilename( outBuf, pos, bArchive );
#else	// WIN32 || IOS
			IUGameManager().GetFileManager()->GetFullFilename( outBuf, pos );
#endif	// ANDROID
			if( outBuf[0] == 0 )
			{
				int i = 0;
				i++;
			}
		}
	}

	return strlen( outBuf );
}

int IUGetFullFileNameExt( char *outBuf, const char * szFilename, const char * szExt, bool bBinaryPath )
{
#if ANDROID
	bool bArchive;
#endif	// ANDROID

	if ( GIsCooking )
	{
		strcpy(outBuf,ResourcePath);
		const char * pos = szFilename;
#if !WIN32
		pos = strrchr(pos, '/');
		if (pos == NULL )
		{
			strcat(outBuf, "/" );
			pos = szFilename;
		}
#endif	// !WIN32
		strcat( outBuf,pos);
		strcat( outBuf, szExt );
	}
	else
	{
		if( bBinaryPath )
		{
			strcpy(outBuf,ResourcePath);
			const char * pos = szFilename;
#if !WIN32
			pos = strrchr(pos, '/');
			if (pos == NULL )
			{
				strcat(outBuf, "/" );
				pos = szFilename;
			}
#endif	// !WIN32
			strcat( outBuf,pos);
			strcat( outBuf, szExt );
		}
		else
		{
			const char * pos = strrchr(szFilename, '/');
			char filename[MAX_PATH];

			if (pos == NULL )
				pos = szFilename;
			else
				pos++;

			strcpy( filename, pos );
			strcat( filename, szExt );
#if ANDROID
			IUGameManager().GetFileManager()->GetFullFilename( outBuf, filename, bArchive );
#else	// WIN32 || IOS
			IUGameManager().GetFileManager()->GetFullFilename( outBuf, filename );
#endif	// ANDROID
		}
	}

	return strlen( outBuf );
}
*/
int IUGetDocFileName( char *outBuf, const char *filename )
{
#if !ANDROID
	strcpy(outBuf,DocPath);
	const char * pos = filename;
#if !WIN32
	pos = strrchr(pos, '/');
	if (pos == NULL )
	{
		strcat(outBuf, "/" );
		pos = filename;
	}
#endif	// !WIN32
	strcat(outBuf, pos);
	return strlen( outBuf );
#else	// ANDROID
	strcpy(outBuf,DocPath);
	const char * pos = filename;

	pos = strrchr(pos, '/');
	if (pos == NULL )
	{
		strcat(outBuf, "/" );
		pos = filename;
	}
	strcat(outBuf, pos);
	return strlen( outBuf );

#endif	// !ANDROID
}

int IUGetBinFileName( char *outBuf, const char *filename )
{
	strcpy(outBuf,ResourcePath);
	const char * pos = filename;
#if !WIN32
	pos = strrchr(pos, '/');
	if (pos == NULL )
	{
		strcat(outBuf, "/" );
		pos = filename;
	}
#endif	// !WIN32
	strcat(outBuf, pos);
	return strlen( outBuf );
}

#if !WIN32
void splitpath( const char*szFullPath, char* szDrive, char* szDir, char* szText, char * szExt )
{
	if( szFullPath[0] == '\0' )
	{
		if( szDrive != NULL )
			szDrive[0] = '\0';
		if( szDir != NULL )
			szDir[0] = '\0';
		if( szText != NULL )
			szText[0] = '\0';
		if( szExt != NULL )
			szExt[0] = '\0';
		return;
	}
	
	if( szDrive != NULL )
		szDrive[0] = '\0';

	const char *fname = strrchr( szFullPath, '\\');
	if( fname == NULL )
		fname = strrchr( szFullPath, '/');

	if( fname != NULL )
	{
		if( szDir != NULL )
		{
			strncpy( szDir, szFullPath, fname - szFullPath );
			szDir[fname - szFullPath] = '\0';
		}
		fname++;
	}
	else
		fname = szFullPath;

	const char *extension = strrchr( fname, '.');
	if( szText != NULL && fname < extension )
	{
		strncpy( szText, fname, extension - fname );
		szText[extension - fname] = '\0';
	}
	//extension++;

	if( szExt != NULL )
	{
		if( fname < extension )
			strcpy( szExt, extension );
	}
}
#endif	// !WIN32

void GetWideString( const char *szText, WCHAR *szWBuf, int len )
{
#if WIN32
	//MultiByteToWideChar( CP_ACP, MB_COMPOSITE, szText, -1, szWBuf, len );

	if( IsUTF8String(szText) )
	{
		MultiByteToWideChar( CP_UTF8, 0, szText, -1, szWBuf, len );
	}
	else
	{
		MultiByteToWideChar( CP_ACP, MB_COMPOSITE, szText, -1, szWBuf, len );
	}
	
#elif ANDROID

	if( !IsUTF8String(szText) )
	{
		//LogPrintf( "************* IsUTF8String NOT : %s *************", szText );

		char szTemp[256];
		int utfLen = JniUtil::ConvertMultiToUTF8( szText, szTemp, 256 );
		{
			int p = 0;
			int q = 0;
			WCHAR d;
			while( szTemp[p] != 0 && q < len - 1 )
			{
				if( (szTemp[p]&0xe0)== 0xe0 )
				{
					d = ((szTemp[p]& 0x0f)<<12)| ((szTemp[p+1] & 0x3f) <<6 ) | (szTemp[p+2]& 0x3f );
					p+=3;
				}
				else if( (szTemp[p]&0xc0) == 0xc0 )
				{
					d = ((szTemp[p] & 0x1f) << 6 ) | ((szTemp[p+1]&0x3f));
					p+=2;
				}
				else
				{
					d = szTemp[p] & 0x7f;
					p++;
				}
				szWBuf[q] = d;
				q++;
			}
			szWBuf[q] = NULL;

		}
	}
	else
	{
		int p = 0;
		int q = 0;
		WCHAR d;
		while( szText[p] != 0 && q < len - 1 )
		{
			if( (szText[p]&0xe0)== 0xe0 )
			{
				d = ((szText[p]& 0x0f)<<12)| ((szText[p+1] & 0x3f) <<6 ) | (szText[p+2]& 0x3f );
				p+=3;
			}
			else if( (szText[p]&0xc0) == 0xc0 )
			{
				d = ((szText[p] & 0x1f) << 6 ) | ((szText[p+1]&0x3f));
				p+=2;
			}
			else
			{
				d = szText[p] & 0x7f;
				p++;
			}
			szWBuf[q] = d;
			q++;
		}
		szWBuf[q] = NULL;
	}
#else	// IOS
    printf("===========> GetWideString is not implemented.\n");
	/*
    if( !IsUTF8String(szText) )
	{
		NSString *szWText = [NSString stringWithUTF8String: szText];
		//	if( szWText == nil )
		//	{
		szWText = [NSString stringWithCString: szText encoding:0x80000940];
		//	}

		if( szWText == nil )
		 {
			 szWText = [NSString stringWithUTF8String: " " ];
		 }
		 
		 wcsncpy( szWBuf, (WCHAR*)[szWText cStringUsingEncoding:NSUTF32StringEncoding], len );
//		 wprintf( L"%ls\n", szWBuf );
	}
	else
	{
		NSString *szWText = [NSString stringWithUTF8String: szText];
		if( szWText == nil )
		{
			szWText = [NSString stringWithCString: szText encoding:0x80000940];
		}

		if( szWText == nil )
		{
			szWText = [NSString stringWithUTF8String: " " ];
		}
		 
		wcsncpy( szWBuf, (WCHAR*)[szWText cStringUsingEncoding:NSUTF32StringEncoding], len );
//		wprintf( L"%ls\n", szWBuf );
	}
        */
    
#endif	// WIN32
}

bool IsUTF8String( const char *szText )
{
	if(!szText) return 0; 
	const unsigned char * bytes = (const unsigned char *)szText; 

	int TotalCount = 0;

	while(*bytes)
	{ 
		// ASCII 
		if( (bytes[0] == 0x09 || bytes[0] == 0x0A || bytes[0] == 0x0D || (0x20 <= bytes[0] && bytes[0] <= 0x7E) ) )  
		{ 
			bytes += 1;

			TotalCount += 1;
			continue; 
		} 

		// non-overlong 2-byte
//		if( ( (0xC2 <= bytes[0] && bytes[0] <= 0xDF) && (0x80 <= bytes[1] && bytes[1] <= 0xBF) ) ) 
//		{ 
//			bytes += 2;
//
//			TotalCount += 2;
//			continue; 
//		} 
		
		// excluding overlongs 
		if( (bytes[0] == 0xE0 && (0xA0 <= bytes[1] && bytes[1] <= 0xBF) && (0x80 <= bytes[2] && bytes[2] <= 0xBF) ) || 
			( ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) || bytes[0] == 0xEE || bytes[0] == 0xEF) && (0x80 <= bytes[1] && bytes[1] <= 0xBF) && (0x80 <= bytes[2] && bytes[2] <= 0xBF) ) || // straight 3-byte
			( bytes[0] == 0xED && (0x80 <= bytes[1] && bytes[1] <= 0x9F) && (0x80 <= bytes[2] && bytes[2] <= 0xBF) ) ) // excluding surrogates
		{ 
			bytes += 3;

			TotalCount += 3;
			continue; 
		} 

		// planes  1-3
		if( ( bytes[0] == 0xF0 && (0x90 <= bytes[1] && bytes[1] <= 0xBF) && (0x80 <= bytes[2] && bytes[2] <= 0xBF) && (0x80 <= bytes[3] && bytes[3] <= 0xBF) ) || 
			( (0xF1 <= bytes[0] && bytes[0] <= 0xF3) && (0x80 <= bytes[1] && bytes[1] <= 0xBF) && (0x80 <= bytes[2] && bytes[2] <= 0xBF) && (0x80 <= bytes[3] && bytes[3] <= 0xBF) ) || // planes 4-15
			( bytes[0] == 0xF4 && (0x80 <= bytes[1] && bytes[1] <= 0x8F) && (0x80 <= bytes[2] && bytes[2] <= 0xBF) && (0x80 <= bytes[3] && bytes[3] <= 0xBF) ) ) // plane 16
		{ 
			bytes += 4;

			TotalCount += 4;
			continue; 
		} 

		return false; 
	} 

	return true; 
}

int GetStringCount( const char *szText )
{
	int nTextCnt = 0;

	if( !szText ) return nTextCnt;
	const unsigned char * bytes = (const unsigned char *)szText; 

	while(*bytes)
	{
		nTextCnt++;

		if( (bytes[0] == 0xE0 && (0xA0 <= bytes[1] && bytes[1] <= 0xBF) && (0x80 <= bytes[2] && bytes[2] <= 0xBF) ) || 
			( ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) || bytes[0] == 0xEE || bytes[0] == 0xEF) && (0x80 <= bytes[1] && bytes[1] <= 0xBF) && (0x80 <= bytes[2] && bytes[2] <= 0xBF) ) || // straight 3-byte
			( bytes[0] == 0xED && (0x80 <= bytes[1] && bytes[1] <= 0x9F) && (0x80 <= bytes[2] && bytes[2] <= 0xBF) ) ) // excluding surrogates
		{ 
			bytes += 3; 
		} 
		else
		{
			bytes += 1;
		}
	}

	return nTextCnt;
}

bool IsUTF8ValidString( const char *szText )
{
	if( !szText ) return false;
	const unsigned char * bytes = (const unsigned char *)szText; 

	while(*bytes)
	{
	
		// num, alpha lower & upper
		if( (0x30 <= bytes[0] && 0x39 >= bytes[0]) ||
			(0x41 <= bytes[0] && 0x5A >= bytes[0]) ||
			(0x61 <= bytes[0] && 0x7A >= bytes[0]) )
		{
			bytes += 1;
			continue;
		}

		if( ( (0xEA == bytes[0]) &&
			(0xB0 <= bytes[1] && 0xBF >= bytes[1]) &&
			(0x80 <= bytes[2] && 0xBF >= bytes[2]) ) ||
			( (0xEB <= bytes[0] && 0xEC >= bytes[0]) &&
			(0x80 <= bytes[1] && 0xBF >= bytes[1]) &&
			(0x80 <= bytes[2] && 0xBF >= bytes[2]) ) ||
			( (0xED == bytes[0]) &&
			(0x80 <= bytes[1] && 0x9D >= bytes[1]) &&
			(0x80 <= bytes[2] && 0xBF >= bytes[2]) ) ||
			( (0xED == bytes[0]) &&
			(0x9E == bytes[1]) &&
			(0x80 <= bytes[2] && 0xA3 >= bytes[2]) ))
		{
			bytes += 3;
			continue;
		}

		return false;
	}

	return true;
}

static int g_Localization = 0;

const int GetLocalization()
{
	if( g_Localization != 0 )
	{
		return g_Localization;
	}

	g_Localization = eKOREAN;
	//g_Localization = eENGLISH;

	//const WCHAR *pLanguage = GetLocalText(L"Language");
	//if( !wcscmp( pLanguage, L"English" ) )
	//	g_Localization = eENGLISH;
	//else if( !wcscmp( pLanguage, L"Korea" ) )
	//	g_Localization = eKOREAN;
	//else if( !wcscmp( pLanguage, L"Spain" ) )
	//	g_Localization = eSPAIN;
	//else if( !wcscmp( pLanguage, L"Japan" ) )
	//	g_Localization = eJAPAN;
	//else if( !wcscmp( pLanguage, L"China" ) )
	//	g_Localization = eCHINA;

	return g_Localization;
}
std::wstring replaceAll(const std::wstring &str, const std::wstring &pattern, const std::wstring &replace)
{
	std::wstring result = str;
	std::wstring::size_type pos = 0;
	std::wstring::size_type offset = 0;

	while((pos = result.find(pattern, offset)) != std::wstring::npos)
	{
		result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
		offset = pos + replace.size();
	}

	return result;
}

const WCHAR *GetLocalTextArg2( const WCHAR * szKey, const WCHAR* szString, int i )
{
#if WIN32 || IOS
	static WCHAR ConvertedString[1024];
	std::wstring orgString = g_LocalText.GetText( szKey );
	std::string::size_type findD,findS = 0;
	
	findD = orgString.find(L"%d");
	findS = orgString.find(L"%s");

	std::wstring Temp = replaceAll( g_LocalText.GetText( szKey ), L"%@", L"%ls");

#if IOS
	if ( findD >= findS )
		IUPrintf2( ConvertedString, 1024, g_LocalText.GetText( szKey ), szString, i );
	else
		IUPrintf2( ConvertedString, 1024, g_LocalText.GetText( szKey ), i, szString );

#else	// WIN32
	if ( findD >= findS )
	swprintf( ConvertedString, 1024, Temp.c_str(), szString, i );
	else
		swprintf( ConvertedString, 1024, Temp.c_str(), i, szString );
#endif	// IOS
	
	return ConvertedString;
	
#elif defined(ANDROID)
	static WCHAR ConvertedString[1024];

	WCHAR wszLocaltext[1024];
	wcscpy( wszLocaltext, g_LocalText.GetText( szKey ) );
	int len = wcslen(wszLocaltext);
	for( int ii = 1; ii < len; ii++ )
	{
		if( wszLocaltext[ii-1] == L'%' && wszLocaltext[ii] == L'@' )
			wszLocaltext[ii] = L's';
	}

	IUPrintf2( ConvertedString, 1024, wszLocaltext, szString, i );
	//swprintf( ConvertedString, 1024, Temp.c_str(), szString, i );
	return ConvertedString;
#else
    int len = wcslen( szKey ) * sizeof( WCHAR );
	NSString *strKey = [[NSString alloc] initWithBytes:szKey length: len encoding:NSUTF32LittleEndianStringEncoding];
    
    NSString *strText = NSLocalizedString( strKey , "" );
    
    len = wcslen( szString ) * sizeof( WCHAR );
    NSString *strString = [[NSString alloc] initWithBytes:szString length: len encoding:NSUTF32LittleEndianStringEncoding];
    
    strText = [ strText stringByReplacingOccurrencesOfString:@"%s" withString:@"%@" ];
	std::wstring orgString = (wchar_t*)[strText cStringUsingEncoding: NSUTF32StringEncoding];
	std::string::size_type findD,findS = 0;
	
	findD = orgString.find(L"%d");
	findS = orgString.find(L"%@"); 

	NSString* strTemp = Nil;
	if ( findD >= findS )
	{
		strTemp = [NSString stringWithFormat:strText, strString, i];
	}
	else
	{
		strTemp = [NSString stringWithFormat:strText, i, strString];
	}
    
	strText = strTemp;
	[strKey release];
    [strString release];
    
	return (wchar_t*)[strText cStringUsingEncoding: NSUTF32StringEncoding];
#endif	// WIN32 || IOS
}

const WCHAR *GetLocalTextArg1( const WCHAR * szKey, int i )
{
#if WIN32 || IOS
	static WCHAR ConvertedString[1024];

#if IOS
	IUPrintf2( ConvertedString, 1024, g_LocalText.GetText( szKey ), i );
#elif WIN32
	swprintf( ConvertedString, 1024, g_LocalText.GetText( szKey ), i );
#endif	// IOS

	return ConvertedString;
	
#elif ANDROID
	static WCHAR ConvertedString[1024];
	IUPrintf2( ConvertedString, 1024, g_LocalText.GetText( szKey ), i );
	//swprintf( ConvertedString, 1024, g_LocalText.GetText( szKey ), i );
	return ConvertedString;
#else	// IOS
    int len = wcslen( szKey ) * sizeof( WCHAR );
	NSString *strKey = [[NSString alloc] initWithBytes:szKey length: len encoding:NSUTF32LittleEndianStringEncoding];
    NSString *strText = NSLocalizedString( strKey , "" );
    
    NSString *strTemp = [NSString stringWithFormat:strText, i];
    strText = strTemp;
	[strKey release];
//    [strTemp release];
    
	return (wchar_t*)[strText cStringUsingEncoding: NSUTF32StringEncoding];
#endif	// WIN32 || IOS
}

const WCHAR * GetLocalText( const WCHAR *szKey )
{
#if WIN32 || IOS
	return g_LocalText.GetText( szKey );
#elif ANDROID
	return g_LocalText.GetText( szKey );
#else
	int len = wcslen( szKey ) * sizeof( WCHAR );
	NSString *strKey = [[NSString alloc] initWithBytes:szKey length: len encoding:NSUTF32LittleEndianStringEncoding];
    NSString *strText = NSLocalizedString( strKey , "" );
	[strKey release];
	//return (wchar_t*)[strText cStringUsingEncoding: NSUTF16StringEncoding];
	return (wchar_t*)[strText cStringUsingEncoding: NSUTF32StringEncoding];
#endif	// WIN32 || IOS
}
/*
void GetDeviceResolution( int& OutWidth, int& OutHeight )
{
	IUGameManager().GetUIManager()->GetResolution( OutWidth, OutHeight );
}*/

bool EnableHD()
{
#if WIN32
	return false;

	INT Width;
	INT Height;
	GetDeviceResolution( Width, Height );

	if ( Width >= 960 )
	{
		return true;
	}

	return false;
#elif ANDROID
	return false;
#else	// IOS
    size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    
    char *machine = (char*)malloc(size);
    sysctlbyname("hw.machine", machine, &size, NULL, 0);

    if (strstr( machine, "iPhone1,1" ) )    // iPhone 1G
    {
        return false;
    }
    if (strstr( machine, "iPhone1,2" ) )    // iPhone 3G
    {
        return false;
    }
    if (strstr( machine, "iPhone2,1" ) )    // iPhone 3GS
    {
        return false;
    }
    if (strstr( machine, "iPhone3,1" ) )    // iPhone 4
    {
        return false;
    }
    if (strstr( machine, "iPhone3,3" ) )    // Verizon iPhone 4
    {
        return false;
    }
    if (strstr( machine, "iPhone4,1" ) )    // iPhone 4S
    {
        return true;
    }
    if (strstr( machine, "iPod1,1" ) )      // iPod Touch 1G
    {
        return false;
    }
    if (strstr( machine, "iPod2,1" ) )      // iPod Touch 2G
    {
        return false;
    }
    if (strstr( machine, "iPod3,1" ) )      // iPod Touch 3G
    {
        return false;
    }
    if (strstr( machine, "iPod4,1" ) )      // iPod Touch 4G
    {
        return true;
    }
    if (strstr( machine, "iPad1,1" ) )      // iPad
    {
        return false;
    }
    if (strstr( machine, "iPad2,1" ) )      // iPad 2 (WiFi)
    {
        return true;
    }
    if (strstr( machine, "iPad2,2" ) )      // iPad 2 (GSM)
    {
        return true;
    }
    if (strstr( machine, "iPad2,3" ) )      // iPad 2 (CDMA)
    {
        return true;
    }
    if (strstr( machine, "i386" ) )         // Simulator
    {
        return true;
    }
    if (strstr( machine, "x86_64" ) )       // Simulator
    {
        return true;
    }
    
    return false;
#endif  // WIN32
}

FrameCheck::FrameCheck()
: m_iPos( 0 )
{
}

/*
 void FrameCheck::Start()
{
#if WIN32
	m_Curtime = timeGetTime();
#elif ANDROID
	struct timeval tv;
	gettimeofday( &tv, NULL );
	m_Curtime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
#else	// IOS
	m_Curtime = CFAbsoluteTimeGetCurrent()* 1000.0;
#endif	// WIN32
	m_iStartTime[ m_iPos&63] = m_Curtime;
}

void FrameCheck::End()
{
#if WIN32
	DWORD time = timeGetTime();
#elif ANDROID
	struct timeval tv;
	gettimeofday( &tv, NULL );
	DWORD time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
#else	// IOS
	double time = CFAbsoluteTimeGetCurrent()* 1000.0;
#endif	// WIN32
	m_iTime[m_iPos++&15] = time - m_Curtime;
}
 */

int FrameCheck::GetTime()
{
	int total = 0;
	if( m_iPos > 16 )
	{
		int i;
		for( i = 0; i < 16; i++ )
			total += m_iTime[i];
	}
	return total / 16;
}

float FrameCheck::GetFps()
{
	return 1000.0f / (m_iStartTime[(m_iPos-1)&63] - m_iStartTime[(m_iPos+1)&63]) * 63;
}


#if WIN32
PFNGLGENFRAMEBUFFERSEXTPROC				glGenFramebuffersOES;
PFNGLDELETEFRAMEBUFFERSEXTPROC			glDeleteFramebuffersOES;
PFNGLBINDFRAMEBUFFEREXTPROC				glBindFramebufferOES;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC	glGetRenderbufferParameterivOES;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC		glFramebufferTexture2DOES;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC		glCheckFramebufferStatusOES;
#endif	// WIN32

CLocalText		g_LocalText;


void LogPrintf( const char *pszFmt, ... )
{
#if !FINAL_RELEASE

#if ANDROID
	char szBuf[1024];
	va_list argList;
	va_start( argList, pszFmt);
	vsnprintf( szBuf, 1024, pszFmt, argList );
	va_end( argList );

	JniUtil::LOGPrintf( szBuf );

#elif IOS
	char szBuf[1024];
	va_list argList;
	va_start( argList, pszFmt);
	vsnprintf( szBuf, 1024, pszFmt, argList );
	va_end( argList );

	/// NSString *szLog = [NSString stringWithUTF8String:szBuf];
	/// NSLog( @"%@",szLog );
    printf( "%s", szBuf);

#else // WIN32
	char szBuf[1024];
	va_list argList;
	va_start( argList, pszFmt);
	vsnprintf( szBuf, 1024, pszFmt, argList );
	va_end( argList );

	int pos = strlen(szBuf);
	if( szBuf[pos -1] != '\n' )
	{
		szBuf[pos] = '\n';
		szBuf[pos+1] = 0;

	}
	OutputDebugString( szBuf );
#endif	// ANDROID

#endif // !FINAL_RELEASE
}
