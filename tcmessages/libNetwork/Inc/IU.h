#ifndef _MY_LIFE_FOR_IU__
#define _MY_LIFE_FOR_IU__

#include <string.h>

#include "IURand.h"
//#include "TextureManager.h"

const int DeltaMilisec =33;

class IU
{
public:
	IU();
	~IU();

        bool Init( int iWidth, int iHeight, int iScale, int dir );

	int UpdateTime();
	int GetDeltaTime() { return m_iDelta; }

	int GetUpdateNum();
	int UpdateDelta();

	void SetStartingTime( int delta = 0 )		{ m_StartTime = m_CurTime - delta; }
	int GetRunningFrame()						{ return (DWORD)(m_CurTime - m_StartTime)/DeltaMilisec; }
	DWORD GetRunningTime()						{ return m_CurTime - m_StartTime; }

#if WIN32 || ANDROID
	DWORD GetAccurateTick();
	DWORD GetCurTick();
#else	// IOS
	double GetAccurateTick();
	double GetCurTick();
#endif	// WIN32 || ANDROID

	void UpdateForFPS( int delta );
	float GetFPS() { return m_fFPS; }
	UINT Rand()		{ return m_Rand.Rand(); }
    
    /// CTextureManager &GetTextureManager() { return m_TextureMgr; }
    /// int LoadTexture( const std::string &lpFilename );

	void SetUID( int uID )	{ m_UID = uID; }
	int GetUID()	{ return m_UID++; }

	void Log( const char* pFormat,... );

	//ForTest
	DWORD m_MaxPing;
	DWORD m_MinPing;
	DWORD FrameCount;

#if !FINAL_RELEASE
	float m_fSpeed;
#endif	// !FINAL_RELEASE

protected:
private:
    /// CTextureManager		m_TextureMgr;

	int m_iWidth;
	int m_iHeight;
#if WIN32 || ANDROID
	DWORD m_CurTime;
	DWORD m_StartTime;
#else	// IOS
	double m_CurTime; 
	double m_StartTime; 
#endif	// WIN32 || ANDROID
	int m_iDelta;
	int m_UID;
	float m_fFPS;
	CIURandom	m_Rand;

public:
	bool	bSpeedUp;
	bool	bSpeedDown;
	bool	bDestroyMyCastle;
	bool	bDestroyEnemyCastle;
};


class Utf8
{
public:
    Utf8(const wchar_t* wsz);
	~Utf8() 
    { 
        if( m_utf8 )
        {
			delete[] m_utf8;
        }
    }
	
	static int  GetUTF8( const char * unicode, char *buf );
	static int  GetUnicode( const char *utf8, char * buf );
	
public:
    operator const char*() const { return m_utf8; }
	
private:
    char* m_utf8;
};

class FrameCheck
{
public :
	FrameCheck();
	~FrameCheck() {}
	void Start();
	void End();
	int GetTime();
	float GetFps();

private :
#if WIN32 || ANDROID
	DWORD m_Curtime;
	int m_iTime[64];
	DWORD m_iStartTime[64];
#else	// IOS
	double m_Curtime;
	int m_iTime[64];
	double m_iStartTime[64];
#endif	// WIN32 || ANDROID
	int m_iPos;
};

#ifdef USE_FRAMECHECK
extern FrameCheck g_UpdateTime;
extern FrameCheck g_RenderTime;
#endif
extern IU *gIU;

int IUGetFullFileName( char *outBuf, const char * szFilename, bool bBinaryPath = false );
int IUGetFullFileNameExt(char *outBuf, const char *szFilename, const char * szExt, bool bBinaryPath = false );
int IUGetDocFileName( char *outBuf, const char *filename );
int IUGetBinFileName( char *outBuf, const char *filename );
void GetWideString( const char *szText, WCHAR *szWBuf, int len );
bool IsUTF8String( const char *szText );
bool IsUTF8ValidString( const char *szText );
int ParseString( char **szString, char *szOut );
void IUPrintf( WCHAR *out, const WCHAR *fmt, ... );
void IUPrintf2( WCHAR *wszOut, int size, const WCHAR *fmt, ... );

int ConvertUTF8( const char* szSrc, char* szOut, int maxLen );

int GetStringCount( const char *szText );

#if !WIN32
void splitpath( const char*szFullPath, char* szDrive, char* szDir, char* szName, char * szExt );
#endif	// !WIN32

#define IURand()				gIU->Rand()
#define IUTextureManager()		gIU->GetTextureManager()

#if defined( _DEBUG) || defined( DEBUG )
#define IULOG	gIU->Log
#else
#if WIN32
#define IULOG	(void)
#else	
#define IULOG   (void)
#endif	// WIN32
#endif // _DEBUG

extern void LogPrintf( const char *pszFmt, ... );


#endif

