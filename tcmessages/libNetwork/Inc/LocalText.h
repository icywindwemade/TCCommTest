#pragma once

#include "Core.h"

//#if TEST_TMAP
#define TEST_TMAP_LOCALTEXT	0
//#endif		// TEST_TMAP


#if TEST_TMAP_LOCALTEXT
#else
#include <map>
#endif	// TEST_TMAP_LOCALTEXT
#include <string>

class CLocalText
{
public:
	CLocalText();
	~CLocalText();
	void Init( const char *szName );
	void Load( const char * szFilename );
	const WCHAR *GetText( const WCHAR* szText );

protected:
	UBOOL ParseLocalization( const char* szFilename, INT LanguageCode = 0 );

private:
	void LoadBuffer( WCHAR **pBuffer, WCHAR* pOut );

#if TEST_TMAP_LOCALTEXT
	TMap<std::wstring, std::wstring>	LocalizationStrings;
#else
	std::map<std::wstring, std::wstring> m_LocalText;
#endif	// TEST_TMAP_LOCALTEXT

	std::string	Language;
	INT			Code;
};