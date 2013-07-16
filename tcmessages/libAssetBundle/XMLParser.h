#ifndef _XML_PARSER_
#define _XML_PARSER_

class	TiXmlDocument;
class	TiXmlNode;

#include <list>
#include <string>

class	CXMLParser
{
public :
	CXMLParser();
	~CXMLParser();

	bool Open(const char *fname );
	bool OpenBuffer( const char * pBuf );

	void Reset();

	bool FirstChildElement(const char * item, bool stepdown=false);
	bool GoParent();
	bool NextSiblingElement(const char * item);

	int Scan(char * form, ...);
	int ScanAtt(const char *att, const char * form, ...);

	bool GetAttributeString( const char *att, char * value, int size = 0 );
	bool GetAttributeString( const char *att, std::string &value );
	bool GetAttributeParseString( const char *att, std::string value[], int num );
	bool GetAttributeInt( const char *att, int &value, int defaultvalue = 0 );
	bool GetAttributeUInt( const char *att, unsigned int &value, unsigned int defaultvalue = 0 );
	bool GetAttributeFloat( const char *att, float &value );
	bool GetAttributeParseFloat( const char *att, float value[], int num );
	bool GetAttributeBool( const char *att, bool &value, bool defaultvalue = false );
	bool GetAttributeParseBool( const char *att, bool value[], int num );
	bool GetAttributeParseInt( const char *att, int value[], int num );

	const char* GetElementText();
	
private :
	char * ReadFile(const char *fname );
	const char * GetAttribute(const char * att);
	const char * GetText();


private :
	TiXmlDocument * m_pTiDocument;
	TiXmlNode * m_pTiNode;
	std::list <TiXmlNode*> m_BaseNode;
} ;


#endif