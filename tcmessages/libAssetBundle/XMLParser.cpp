
#include "xmlparser.h"
#include "IUDefine.h"
#include "tinyxml.h"
#if WIN32
#include <crtdbg.h>
#endif	// WIN32
#include <stdio.h>
#include <stdarg.h>
#include "IU.h"
#include "FileIO.h"
#include "FileManager.h"


CXMLParser::CXMLParser()
{
	m_pTiDocument = NULL;
	m_pTiNode = NULL;
}


CXMLParser::~CXMLParser()
{
	if (m_pTiDocument != NULL)
		delete m_pTiDocument;
}


bool CXMLParser::FirstChildElement(const char * item, bool stepdown)
{
	char szCode[256];
#if WIN32
	strcpy(szCode, item);
	//WideCharToMultiByte(CP_ACP, 0, item, -1, szCode, 256, NULL, NULL);
#else
	strcpy(szCode, item);
	//Utf8::GetUTF8(item, szCode );	
#endif	// WIN32
	TiXmlNode * node;

	if (m_BaseNode.size() == 0)
		node = m_pTiDocument->FirstChild(szCode);
	else
		node = (*m_BaseNode.begin())->ToElement()->FirstChildElement(szCode);

	if (node != NULL)
	{
		if (stepdown == true)
			m_BaseNode.push_front(node);

		m_pTiNode = node;
		return true;
	}

	return false;
}


bool CXMLParser::GoParent()
{
	if (m_BaseNode.size() > 0)
	{
		m_BaseNode.erase(m_BaseNode.begin());

		if (m_BaseNode.size() > 0)
			m_pTiNode = (*m_BaseNode.begin());
		else
			m_pTiNode = NULL;
		return true;
	}
	return false;
}


void CXMLParser::Reset()
{
	m_BaseNode.clear();
}


bool CXMLParser::NextSiblingElement(const char * item)
{
	char szCode[256];
#if WIN32
	//WideCharToMultiByte(CP_ACP, 0, item, -1, szCode, 256, NULL, NULL);
	strcpy( szCode, item );	
#else
	strcpy( szCode, item );	
#endif	// WIN32

	if (m_pTiNode != NULL)
	{
		TiXmlNode * parent = (*m_BaseNode.begin());
		TiXmlNode * pOld;
		TiXmlNode * node;

		pOld = !strcmp(parent->Value(), szCode) ? parent : m_pTiNode;
		node = pOld->NextSiblingElement(szCode);

		if (node != NULL)
		{
			if (parent == pOld)
			{
				m_BaseNode.erase(m_BaseNode.begin());
				m_BaseNode.push_front(node);
			}
			m_pTiNode = node;
			return true;
		}

		return false;
	}
	return false;
}


const char* CXMLParser::GetAttribute(const char * att)
{
	if( m_pTiNode == NULL )
		return NULL;

	char szCode[256];
#if WIN32
	//WideCharToMultiByte(CP_ACP, 0, att, -1, szCode, 256, NULL, NULL);
	strcpy(szCode, att );
#else
	strcpy(szCode, att );
#endif	// WIN32

	return m_pTiNode->ToElement()->Attribute(szCode);
}


const char* CXMLParser::GetText()
{
	return m_pTiNode != NULL ? m_pTiNode->ToElement()->GetText() : NULL;
}


int CXMLParser::Scan(char * fmt, ...)
{
	va_list arg;
	int i, cnt=0, j;
	char token[256];

	if( m_pTiNode == NULL )
		return 0;

	const char * ptr = m_pTiNode->ToElement()->GetText();
	va_start(arg, fmt);

	for(i=0; fmt[i]; i++)
	{
		if (fmt[i] == '%')
		{
			for(; *ptr && strchr("\n\r\t ", *ptr); ptr++) ;

			for(j=0; *ptr && !strchr("\n\r\t ", *ptr); j++, ptr++)
				token[j] = *ptr;

			if (j == 0)
				return cnt;

			token[j] = '\0';

			switch(fmt[i+1])
			{
			case 'x' : case 'X' :
				scanf( token, "%x", va_arg(arg, int*));
				break;
			case 'f' : case 'F' :
				scanf( token, "%f", va_arg(arg, float*));
				break;
			case 'd' : case 'u' :
				scanf( token, "%d", va_arg(arg, int*));
				break;
			case 's' :
				{
					char szTokenW[256];
#if WIN32
					//MultiByteToWideChar( CP_UTF8, 0, (char*)token, -1, szTokenW, -1 );
					strcpy( (char*)szTokenW, (char*)token);
#else
					strcpy((char*)szTokenW, (char*)token);
#endif	// WIN32
					strcpy( va_arg(arg, char*), (char*)szTokenW);
				}
				break;
			}
			cnt++;
		}
	}

	va_end(arg);
	return cnt;
}

int CXMLParser::ScanAtt(const char *att, const char* fmt, ...)
{
	const char* ptr;
	va_list arg;
	int i, cnt=0, j;
	char token[256];

	ptr = GetAttribute(att);

	if (ptr == NULL)
		return 0;

	va_start(arg, fmt);

	for(i=0; fmt[i]; i++)
	{
		if (fmt[i] == '%')
		{
			for(; *ptr && strchr("\n\r\t ", *ptr); ptr++) ;

			for(j=0; *ptr && !strchr("\n\r\t ", *ptr); j++, ptr++)
				token[j] = *ptr;

			if (j == 0)
				return cnt;

			token[j] = '\0';

			switch(fmt[i+1])
			{
			case 'x' : case 'X' :
				sscanf(token, "%x", va_arg(arg, int*));
				break;
			case 'f' : case 'F' :
				sscanf(token, "%f", va_arg(arg, float*));
				break;
			case 'd' : case 'u' :
				sscanf(token, "%d", va_arg(arg, int*));
				break;
			case 'l' :
				sscanf(token, "%ld", va_arg(arg, long*));
				break;
			case 's' :
				{
					char szTokenW[256];
#if WIN32
					//MultiByteToWideChar( CP_UTF8, 0, (char*)token, -1, szTokenW, 256 );
					strcpy( szTokenW, token);
#else
					strcpy(szTokenW, token);
#endif	// WIN32
					strcpy( va_arg(arg, char*), (char*)szTokenW);
				}
				break;
			}
			cnt++;
		}
	}

	va_end(arg);
	return cnt;
}

bool CXMLParser::OpenBuffer( const char * pBuf )
{
	if( pBuf )
	{
		if (m_pTiDocument != NULL)
			delete m_pTiDocument;

		m_pTiDocument = new TiXmlDocument;
		m_pTiDocument->Parse(pBuf);

		if (m_pTiDocument->Error() != 0)
		{
#ifdef _DEBUG
			printf( "Error in %s: %s\n", m_pTiDocument->Value(), m_pTiDocument->ErrorDesc() );
#endif
			return false;
		}
		return true;
	}
	return false;

	return true;
}

bool CXMLParser::Open(const char* f )
{
	//LogPrintf( "CXMLParser::Open %s", f );

	if (f == NULL)
		return false;

#if USE_PACKED_DATA

#if ANDROID
	char szFileName[MAX_PATH];
	_splitpath(	f, NULL, NULL, szFileName, NULL );
	char szFile[128];
	sprintf(szFile, "%s.dat", szFileName);

	char *pBuf = NULL;
	int len = IUGameManager().GetFileManager()->LoadFile( szFile, &pBuf );

	CZipBufLoader *pLoader = new CZipBufLoader( pBuf, len );
	char *ptr = pLoader->GetBuf();

#else // IOS || WIN32

	char szFileName[MAX_PATH];
	_splitpath(	f, NULL, NULL, szFileName, NULL );
	char szFile[MAX_PATH];
	IUGetFullFileNameExt(szFile, szFileName, ".dat" );

	CZipLoader fz;
	fz.Open( szFile );
	char *ptr = fz.GetBuf();

#endif // ANDROID


#else	// USE_PACKED_DATA
	char * ptr = ReadFile(f);
#endif	// USE_PACKED_DATA

	if( ptr == NULL )
		return false;

	bool bResult = OpenBuffer( ptr );

#if	USE_PACKED_DATA

#if ANDROID
	delete pLoader;

#else	// WIN32 || IOS
	ptr = NULL;

#endif // ANDROID

#else
	if ( ptr )
	{
		delete [] ptr;
		ptr = NULL;
	}
#endif	// USE_PACKED_DATA

	return bResult;
}


char* CXMLParser::ReadFile(const char *fname )
{
	int len;

	char *pBuf = NULL;
	len =  CFileManager::sharedInstance()->LoadFile( fname, &pBuf );
	return pBuf;
}


bool CXMLParser::GetAttributeString( const char *att, char * value, int size )
{
	const char* ptr;
	ptr = GetAttribute(att);
	if( ptr == NULL )
		return false;
#if WIN32
	//MultiByteToWideChar( CP_UTF8, 0, ptr, -1, value, size );
	strcpy( value, ptr );
#else
	strcpy( value, ptr );
#endif	// WIN32
	return true;
}

bool CXMLParser::GetAttributeString( const char *att, std::string &value )
{
	const char* ptr;
	ptr = GetAttribute(att);
	if( ptr == NULL )
		return false;

	value = ptr;
	return true;
}

bool CXMLParser::GetAttributeParseInt( const char *att, int value[], int num )
{
	const char* ptr;
	ptr = GetAttribute(att);
	if( ptr == NULL )
		return false;

	for( int i = 0; i < num; i++ )
	{
		const char *found = strchr( ptr, ' '  );
		char val[16];
		if( found != NULL )
		{
			strncpy( val, ptr, found - ptr );
			val[ found - ptr] = NULL;

			if( val[0] == 0 )
				strcpy( val, "-1" );
		}
		else if( ptr[0] == 0 )
			strcpy( val, "-1" );
		else
			strcpy( val, ptr );
		value[i] = atoi( val );

		if ( found == NULL )
		{
			break;
		}
		ptr = found+1;
	}

	return true;
}

bool CXMLParser::GetAttributeParseString( const char *att, std::string value[], int num )
{
	const char* ptr;
	ptr = GetAttribute(att);
	if( ptr == NULL )
		return false;

	for( int i = 0; i < num; i++ )
	{
		const char *found = strchr( ptr, ' '  );
		char val[256];
		if( found != NULL )
		{
			strncpy( val, ptr, found - ptr );
			val[ found - ptr] = NULL;

			if( val[0] == 0 )
				strcpy( val, "" );
			ptr = found+1;
		}
		else if( ptr[0] == 0 )
		{
			strcpy( val, "" );
			ptr = NULL;
		}
		else
		{
			strcpy( val, ptr );
			ptr = NULL;
		}
		value[i] = val;

		if (ptr == NULL)
		{
			break;
		}
	}
	return true;
}

bool CXMLParser::GetAttributeInt( const char *att, int &value, int defaultvalue )
{
	const char* ptr;
	ptr = GetAttribute(att);
	if( ptr == NULL )
	{
		value = defaultvalue;
		return false;
	}
	value = atoi( ptr );
	return true;
}

bool CXMLParser::GetAttributeUInt( const char *att, unsigned int &value, unsigned int defaultvalue )
{
	const char* ptr;
	ptr = GetAttribute(att);
	if( ptr == NULL )
	{
		value = defaultvalue;
		return false;
	}
	value = strtoul( ptr, NULL, 10 );
	return true;
}

bool CXMLParser::GetAttributeFloat( const char *att, float &value )
{
	const char* ptr;
	ptr = GetAttribute(att);
	if( ptr == NULL )
		return false;
	sscanf(ptr, "%f", &value);
	return true;
}

bool CXMLParser::GetAttributeParseFloat( const char *att, float value[], int num )
{
	const char* ptr;
	ptr = GetAttribute(att);
	if( ptr == NULL )
		return false;

	for( int i = 0; i < num; i++ )
	{
		const char *found = strchr( ptr, ' '  );
		char val[16];
		if( found != NULL )
		{
			strncpy( val, ptr, found - ptr );
			val[ found - ptr] = NULL;

			if( val[0] == 0 )
				strcpy( val, "0" );
		}
		else if( ptr[0] == 0 )
			strcpy( val, "0" );
		else
			strcpy( val, ptr );
		sscanf(val, "%f", &value[i]);

		if (found == NULL)
		{
			break;
		}

		ptr = found+1;
	}

	return true;
}

bool CXMLParser::GetAttributeBool( const char *att, bool &value, bool defaultvalue )
{
	const char* ptr;
	ptr = GetAttribute(att);
	if( ptr == NULL )
	{
		value = defaultvalue;
		return false;
	}
	value = atoi( ptr ) == 1 ? true : false;
	return true;
}

bool CXMLParser::GetAttributeParseBool( const char *att, bool value[], int num )
{
	const char* ptr;
	ptr = GetAttribute(att);
	if( ptr == NULL )
		return false;

	for( int i = 0; i < num; i++ )
	{
		const char *found = strchr( ptr, ' '  );
		char val[16];
		if( found != NULL )
		{
			strncpy( val, ptr, found - ptr );
			val[ found - ptr] = NULL;

			if( val[0] == 0 )
				strcpy( val, "0" );
		}
		else if( ptr[0] == 0 )
			strcpy( val, "0" );
		else
			strcpy( val, ptr );
		value[i] = atoi( val ) == 1 ? true : false;

		if ( found == NULL)
		{
			break;
		}
		ptr = found+1;
	}

	return true;
}

const char* CXMLParser::GetElementText()
{
	return GetText();
}