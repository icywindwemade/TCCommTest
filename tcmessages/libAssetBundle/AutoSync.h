#pragma once

#include "IUDefine.h"

#if ANDROID
class CAutoSync
{
public:
	CAutoSync( IULock pLock) { m_pLock = pLock; EnterLock( pLock ); }
	~CAutoSync(void)	{ Leave(); }

	void Leave() { if( m_pLock != -1 ) ReleaseLock( m_pLock ); m_pLock = -1; }

private :
	IULock m_pLock;
};

#else // IOS || WIN32

class CAutoSync
{
public:
	CAutoSync( IULock &pLock) { m_pLock = &pLock; EnterLock( pLock ); }
	~CAutoSync(void)	{ Leave(); }

	void Leave() { if( m_pLock != NULL ) ReleaseLock( *m_pLock ); m_pLock = NULL; }

private :
	IULock *m_pLock;
};

#endif	// ANDROID
