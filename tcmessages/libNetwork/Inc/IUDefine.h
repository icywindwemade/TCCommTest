#ifndef __IU__DEFINE__
#define __IU__DEFINE__


#if WIN32
	#ifdef _DEBUG   
		#ifndef DBG_NEW      
			#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )      
			//#define new DBG_NEW   
		#endif
	#endif	// _DEBUG
#endif  // WIN32


#if ( FINAL_RELEASE && !defined(USE_PACKED_DATA) ) || ANDROID
	#define USE_PACKED_DATA		1
#endif	// FINAL_RELEASE && !defined(USE_PACKED_DATA)

#define BACKGROUND_RESUME
#define USE_RENDER_THREAD
//#define USE_FRAMECHECK

#if WIN32
	#include <Windows.h>
	#define log2f(x) (logf(x)/logf(2)) 
#elif ANDROID
	typedef unsigned long	DWORD;
	typedef unsigned int	UINT;
	typedef	int				INT;
	typedef unsigned char	BYTE;
	typedef unsigned short	WORD;
	typedef wchar_t			WCHAR;
	typedef float			FLOAT;
	#include <string>
//	typedef std::basic_string<char>	std::string;
	namespace std { 
		typedef basic_string<wchar_t> wstring; 
	} 
	#define MAX_PATH 260
	#define log2f(x) (logf(x)/logf(2)) 
	#define  _ASSERT(e)		((void)(e))
	extern char ResourcePath[2048];
	extern char DocPath[2048];
#else // iOS
//	#include <assert.h>
	typedef unsigned long	DWORD;
	typedef unsigned int	UINT;
	typedef	int				INT;
	typedef unsigned char	BYTE;
	typedef unsigned short	WORD;
	typedef wchar_t			WCHAR;
	extern char ResourcePath[2048];
	extern char DocPath[2048];
	//typedef unsigned short	WCHAR;
	#include <string>
//	typedef std::basic_string<char>	std::string;
	#define MAX_PATH 260
	#ifndef FALSE
		#define FALSE			false			
	#endif
	#define  _ASSERT		assert
#endif	// WIN32


enum LOCALIZATION
{
	eENGLISH = 1,
	eKOREAN,
	eSPAIN,
	eJAPAN,
	eCHINA,
};

enum THREAD_ID
{
	eThread_Loader = 1,
	eThread_Main,
	eThread_Render,
	eThread_Network,
};

#define _ASSERTZERO(a)	{int temp = (int)(a); _ASSERT( temp != 0 );}
#ifndef MAX
#define MAX(a,b)		((a)>(b)? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)		((a)>(b)? (b) : (a))
#endif

#ifndef ANDROID
#ifndef M_PI
#define M_PI 3.1415926535f
#endif
#endif	// ANDROID

#define M_RAD(x) (x / 180.0f * M_PI)
#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if(p) delete (p); (p) = NULL; }
#endif
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

// ui
#define SHOW_UI(x) { CUI *pUI = (CUI *)IUGameManager().GetUIManager()->GetUI( x ); if( pUI ) pUI->SetState( eUI_NORMAL ); }
#define HIDE_UI(x) { CUI *pUI = (CUI *)IUGameManager().GetUIManager()->GetUI( x ); if( pUI ) pUI->SetState( eUI_HIDDEN ); }
#define DISABLE_UI(x) { CUI *pUI = (CUI *)IUGameManager().GetUIManager()->GetUI( x ); if( pUI ) pUI->SetState( eUI_DISABLE ); }

#define SAFE_DELETE_UI(p) { if(p) { IUGameManager().GetUIManager()->RemoveUI( p ); delete (p); (p) = NULL; } }

#define SET_UI_LAYER(x, layer) { CUI *pUI = (CUI *)IUGameManager().GetUIManager()->GetUI( x ); if( pUI ) pUI->SetLayer( layer ); }
#define SET_UI_LAYER_P(p, layer) { if(p) { p->SetLayer( layer ); } }

//#define MAX_STRING 1024
//
//void TRACE(TCHAR *szInput, ...)
//{
//	TCHAR output[MAX_STRING];
//	va_list vl;
//	va_start(vl, szInput); 
//	wvsprintf(output, szInput, vl);
//
//	OutputDebugString(output);
//	va_end(vl);
//}

#if WIN32	// Windows

#define IULock			CRITICAL_SECTION
#define InitLock(x)		InitializeCriticalSection( &x )
#define DelLock(x)		DeleteCriticalSection( &x )
#define EnterLock(x)	EnterCriticalSection( &x )
#define ReleaseLock(x)	LeaveCriticalSection( &x )
#define IUSleep(x)		Sleep(x)
class CLocalText;
extern CLocalText		g_LocalText;

#elif ANDROID	// Android

#include <pthread.h>
#define IULock			int 
#define InitLock(x)		
#define DelLock(x)		
#define EnterLock(x)	
#define ReleaseLock(x)	
#define IUSleep(x)		
#define GL_CLAMP		GL_CLAMP_TO_EDGE
#define _splitpath		splitpath	
#define UNREFERENCED_PARAMETER(x)		(x)
class CLocalText;
extern CLocalText		g_LocalText;

#include "../Android/CppSource/AndroidStore.h"
#include "../Android/CppSource/JniUtil.h"

#else	// iOS

#include <pthread.h>
#include <unistd.h>
/*
#define IULock			NSLock *
#define InitLock(x)		x = [[NSLock alloc]init]
#define DelLock(x)		(void)( x )
#define EnterLock(x)	[x lock]
#define ReleaseLock(x)	[x unlock]
 */

#define IULock          pthread_mutex_t
#define InitLock(x)     pthread_mutex_init(&x, NULL)
#define DelLock(x)      pthread_mutex_destroy(&x)
#define EnterLock(x)    pthread_mutex_lock(&x)
#define ReleaseLock(x)  pthread_mutex_unlock(&x)

#define IUSleep(x)		usleep(x)
#define _splitpath		splitpath
#define UNREFERENCED_PARAMETER(x)		//(x)
//#include "IU.h"
#define GL_CLAMP		GL_CLAMP_TO_EDGE

class CLocalText;
extern CLocalText		g_LocalText;

#endif	// WIN32

const WCHAR *GetLocalTextArg2( const WCHAR *, const WCHAR*, int i );
#define IUGetLocalTextArg2(x, y, i) GetLocalTextArg2( x, y, i )

const WCHAR *GetLocalTextArg1( const WCHAR *, int i );
#define IUGetLocalTextArg1(x, i) GetLocalTextArg1(x, i)

const WCHAR *GetLocalText( const WCHAR * );
#define IUGetLocalText(x)	GetLocalText( x )
const int GetLocalization();
#define IUGetLocalization()		GetLocalization()
#define IUCRASH()				{char *temp = 0; *temp = 0; }			

extern void GetDeviceResolution( int& OutWidth, int& OutHeight );
bool EnableHD();
#define IUEnableHD() EnableHD()

#if WIN32
#include <gl/GL.h>
#include "../../External/gl/gl3.h"
#include "../../External/gl/glext.h"
#include "../../External/gl/wglext.h"
#define GL_FRAMEBUFFER_OES				GL_FRAMEBUFFER_EXT
#define GL_COLOR_ATTACHMENT0_OES		GL_COLOR_ATTACHMENT0_EXT
#define GL_RENDERBUFFER_WIDTH_OES		GL_RENDERBUFFER_WIDTH_EXT
#define GL_RENDERBUFFER_HEIGHT_OES		GL_RENDERBUFFER_HEIGHT_EXT
#define GL_RENDERBUFFER_OES				GL_RENDERBUFFER_EXT
#define GL_FRAMEBUFFER_COMPLETE_OES		GL_FRAMEBUFFER_COMPLETE_EXT
#define glOrthof						glOrtho

extern PFNGLGENFRAMEBUFFERSEXTPROC				glGenFramebuffersOES;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC			glDeleteFramebuffersOES;
extern PFNGLBINDFRAMEBUFFEREXTPROC				glBindFramebufferOES;
extern PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC	glGetRenderbufferParameterivOES;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC			glFramebufferTexture2DOES;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC		glCheckFramebufferStatusOES;

#endif	// WIN32

extern void LogPrintf( const char *pszFmt, ... );


#endif


//#define TESTUNITBALANCE
//#define TEST_CREATE_TEXT
//#define TEST_DRAW_MANYTEXT