#include "IUDefine.h"
#include "IU.h"
#include "LocalText.h"
#include "FileIO.h"

//#include "rapidxml.hpp"
//#include "rapidxml_utils.hpp"

#if ANDROID
#include "../Android/CppSource/JniUtil.h"
#endif	// ANDROID

CLocalText::CLocalText()
{
}

CLocalText::~CLocalText()
{
}

void CLocalText::Init( const char *szName )
{
	char szFilename[MAX_PATH];

	sprintf( szFilename, "%s.txt", szName );
	/*
	0 = Korea
	1 = English
	2 = Japan
	*/
	ParseLocalization("Localizable.xml", 0 );
}

void CLocalText::LoadBuffer( WCHAR ** pBuf, WCHAR * szBuf )
{
	if( **pBuf != L'\"' )
		return;
	(*pBuf)++;

	do
	{
		if( **pBuf == L'\\' )
		{
			*szBuf = **pBuf;
			szBuf++;
			(*pBuf)++;
		}
		*szBuf = **pBuf;
		szBuf++;
		(*pBuf)++;
	}
	while( **pBuf != L'\"' );
	*szBuf = L'\0';
	(*pBuf)++;
}

void CLocalText::Load( const char * szFilename )
{
#if WIN32
	char *pBuffer;
	int size = IUGameManager().GetFileManager()->LoadFile( szFilename, &pBuffer );
	if( pBuffer == NULL )
		return;	

	WCHAR *pBuf = (WCHAR*)pBuffer;
	WCHAR *pEnd = pBuf + size;

	if( pBuf[0] != 0xfeff )
		return;
	pBuf++;


	pBuf = wcschr( pBuf, L'\"' );
	if( pBuf == NULL )
		return;

	while( pBuf < pEnd )
	{
		if( pBuf[0] == L'\r' && pBuf[1] == L'\n' )
		{
			pBuf += 2;
			continue;
		}

		WCHAR szKey[256];
		LoadBuffer( &pBuf, szKey );
		pBuf = wcschr( pBuf, L'\"' );
		if( pBuf == NULL )
			break;

		WCHAR szText[256];
		LoadBuffer( &pBuf, szText );
		pBuf = wcschr( pBuf, L'\"' );

#if TEST_TMAP_LOCALTEXT
		LocalizationStrings.Set( std::wstring(szKey), std::wstring(szText) );
#else
		m_LocalText.insert( std::pair< std::wstring, std::wstring>( szKey, szText ) );
#endif	// TEST_TMAP_LCOALTEXT
		if( pBuf == NULL )
			break;
	}

	delete []pBuffer;
#elif ANDROID
	char *pBuffer;
	int size = IUGameManager().GetFileManager()->LoadFile( szFilename, &pBuffer );
	if( pBuffer == NULL )
		return;	

	int len = size/2;
	WCHAR *pBuf = new WCHAR[len+ 1];
	memset( pBuf, 0, sizeof(WCHAR) * (len + 1));

	for( int i = 0; i < len; i++ )
	{
		pBuf[i] = pBuffer[i*2] | (int)(pBuffer[i*2+1]) << 8;
	}
	pBuf[len] = 0;

	delete []pBuffer;

	WCHAR *pEnd = &pBuf[len];

	if( pBuf[0] != 0xfeff )
		return;
	pBuf++;

	pBuf = wcschr( pBuf, L'\"' );
	if( pBuf == NULL )
		return;

	while( pBuf < pEnd )
	{
		if( pBuf[0] == L'\r' && pBuf[1] == L'\n' )
		{
			pBuf += 2;
			continue;
		}

		WCHAR szKey[256];
		LoadBuffer( &pBuf, szKey );
		pBuf = wcschr( pBuf, L'\"' );
		if( pBuf == NULL )
			break;

		WCHAR szText[256];
		if( pBuf[1] != L'\"')
			LoadBuffer( &pBuf, szText );
		else
		{
			szText[0] = NULL;
			pBuf++;
			pBuf++;
		}
		pBuf = wcschr( pBuf, L'\"' );

		m_LocalText.insert( std::pair< std::wstring, std::wstring>( szKey, szText ) );
		if( pBuf == NULL )
			break;
	};
	delete [] pBuf;
#endif	// WIN32
}

const WCHAR* CLocalText::GetText( const WCHAR* szText )
{
#if TEST_TMAP_LOCALTEXT
	
	if ( LocalizationStrings.HasKey( std::wstring(szText) ) )
	{
		std::wstring Localized = LocalizationStrings.FindRef( std::wstring(szText) );
		const WCHAR *pwText = Localized.c_str();
		return pwText;
	}

#else
	std::map<std::wstring, std::wstring>::iterator it = m_LocalText.find( szText );
	if( it != m_LocalText.end() )
		return it->second.c_str();
#endif	// TEST_TMAP_LOCALTEXT

	return L"Invalid String";
}

/*
UBOOL CLocalText::ParseLocalization( const char* InFilename, INT InLanguageCode )
{
	UBOOL bResult = FALSE;
	if ( !InFilename )
	{
		return FALSE;
	}

#if USE_PACKED_DATA
#if ANDROID
	char* pBuffer = NULL;
	INT LoadedSize = appLoadCompressedFileFromPackage( "Localizable.dat", &pBuffer );
	if ( LoadedSize <= 0 )
	{
		LogPrintf( "Error Loading Version" );
		return FALSE;
	}

#else	// WIN32 || IOS
	char szFileName[MAX_PATH];
	_splitpath(	InFilename, NULL, NULL, szFileName, NULL );
	char szFile[MAX_PATH];
	IUGetFullFileNameExt(szFile, szFileName, ".dat" );

	CZipLoader* Loader = new CZipLoader();
	if ( !Loader->Open( szFile ) )
	{
		delete Loader;
		return FALSE;
	}

	int Length = Loader->GetLen();
	char* pBuffer = new char[ Length + 1 ];
	Loader->Read( pBuffer, Length );
	pBuffer[Length] = 0;

	delete Loader;
#endif	// ANDROID
#else
	
	char* pBuffer = NULL;
	int Length = IUGameManager().GetFileManager()->LoadFile( InFilename, &pBuffer );
	if( pBuffer == NULL || Length <= 0 )
	{
		return FALSE;
	}
#endif	//  USE_PACKED_DATA

	// XML Parsing
	rapidxml::xml_document<char> xmlDoc;
	xmlDoc.parse<0>( pBuffer );
	rapidxml::xml_attribute<char>* Attribute = NULL;
	rapidxml::xml_node<char>* LanguageNode = NULL;
	rapidxml::xml_node<char>* StringNode = NULL;

	// Check header
	rapidxml::xml_node<char>* root = xmlDoc.first_node("Localization");
	if ( !root )
	{
		goto END_PARSE;
	}

	for ( LanguageNode = root->first_node("Language"); LanguageNode; LanguageNode = LanguageNode->next_sibling("Language") )
	{
		Attribute = LanguageNode->first_attribute("Language");
		if ( !Attribute )
		{
			goto END_PARSE;
		}
		std::string ReadLanguage = Attribute->value();
		
		Attribute = Attribute->next_attribute("Code");
		if ( !Attribute )
		{
			goto END_PARSE;
		}
		
		INT LanguageCode = atoi( Attribute->value() );
		if ( LanguageCode == InLanguageCode )
		{
			Language = ReadLanguage;
			Code = LanguageCode;
			break;
		}
	}

	if ( !LanguageNode )
	{
		// Can't find language code
		goto END_PARSE;
	}


	for ( StringNode = LanguageNode->first_node("String"); StringNode; StringNode = StringNode->next_sibling("String") )
	{
		// Search attributes in node
		std::string LocalizationKey;
		std::string LocalizationValue;

		rapidxml::xml_attribute<char>* KeyAttribute = NULL;
		rapidxml::xml_attribute<char>* ValueAttribute = NULL;
		KeyAttribute = StringNode->first_attribute("Key");
		ValueAttribute = KeyAttribute->next_attribute("Value");
		if( !KeyAttribute || !ValueAttribute )
		{
			// Invalid xml format.
			goto END_PARSE;
		}

		wchar_t UniKey[1024];
		GetWideString( KeyAttribute->value(), UniKey, 1024 );
		wchar_t UniText[1024];
		GetWideString( ValueAttribute->value(), UniText, 1024 );
#if TEST_TMAP_LOCALTEXT
		LocalizationStrings.Set( std::wstring(UniKey), std::wstring(UniText) );
#else
		m_LocalText.insert( std::pair< std::wstring, std::wstring>( std::wstring(UniKey), std::wstring(UniText) ) );
#endif	// TEST_TMAP_LOCALTEXT
	}

	bResult = TRUE;

END_PARSE:
	delete [] pBuffer;

	return bResult;
}
 */
