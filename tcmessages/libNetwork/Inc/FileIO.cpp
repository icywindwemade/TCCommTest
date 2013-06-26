#include "IUDefine.h"
#include "IU.h"
#include "assert.h"
#include "FileIO.h"
#include "zlib.h"

CFileIO::CFileIO()
: m_Buf( NULL )
{
}

CFileIO::~CFileIO()
{
	SAFE_DELETE_ARRAY( m_Buf );
}

bool CFileIO::Open( const std::string &filename )
{
	SAFE_DELETE_ARRAY( m_Buf );

	FILE *fp;
	char name[MAX_PATH];
	IUGetFullFileName( name, filename.c_str() );

	fp = fopen(name, "rb");
	if (fp == NULL)
		return false;

	fseek(fp, 0, SEEK_END);
	m_iLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	m_Buf = new char [m_iLen+1];
	fread(m_Buf, 1, m_iLen, fp);
	fclose(fp);
	m_Buf[m_iLen] = '\0';

	m_iOff = 0;
	return true;
}

bool CFileIO::OpenCurrentFile( const std::string &filename )
{
	SAFE_DELETE_ARRAY( m_Buf );

	FILE *fp;
	fp = fopen(filename.c_str(), "rb");
	if (fp == NULL)
		return false;

	fseek(fp, 0, SEEK_END);
	m_iLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	m_Buf = new char [m_iLen+1];
	fread(m_Buf, 1, m_iLen, fp);
	fclose(fp);
	m_Buf[m_iLen] = '\0';

	m_iOff = 0;
	return true;
}

int CFileIO::Read( char *ptr, int len )
{
	if( m_iOff + len > m_iLen )
		len = m_iLen - m_iOff;
	memcpy( ptr, &m_Buf[m_iOff], len );
	m_iOff += len;
	return len;
}

int CFileIO::ReadInt()
{
	int i;
	Read( (char*)&i, sizeof( int ) );
	return i;
}

char CFileIO::ReadChar()
{
	char c;
	Read( &c, sizeof( char ) );
	return c;
}
short CFileIO::ReadShort()
{
	short s;
	Read( (char*)&s, sizeof( short) );
	return s;
}

int CFileIO::ReadString( char *szBuf, int len )
{
	int Length = ReadInt();

	_ASSERT( len >  Length );
	if( Length == 0 )
	{
		szBuf[0] = 0;
		return 0;
	}

	Length = Read( szBuf, Length );
	szBuf[Length] = NULL;

	return Length;
}

float CFileIO::ReadFloat()
{
	float f;
	Read( (char*)&f, sizeof( float ) );
	return f;
}

unsigned long CFileIO::GetCRC()
{
	unsigned long crc = crc32(0L, Z_NULL, 0);
	crc = crc32(crc, (const Bytef *)GetBuf(), GetLen());
	return crc; 
}


CFileWriter::CFileWriter()
: m_fp( NULL )
{
}

CFileWriter::~CFileWriter()
{
	if( m_fp != NULL )
	{
		fflush( m_fp );
		fclose( m_fp );
	}
}

bool CFileWriter::Open( const std::string &filename  )
{
	m_fp = fopen(filename.c_str(), "wb");
	if (m_fp == NULL)
		return false;
	m_iLen = 0;
	return true;
}

bool CFileWriter::Write( const void *ptr, int len )
{
	m_iLen += fwrite( ptr, sizeof(char), len, m_fp );
	return true;
}

bool CFileWriter::Write( int i )
{
	Write( (void*)&i, sizeof( int ) );
	return true;
}

bool CFileWriter::Write( float f )
{
	Write( (void*)&f, sizeof( float ) );
	return true;
}

bool CFileWriter::Write( const char *ptr)
{
	Write( int( strlen( ptr )) );
	Write( (const void*)(ptr), strlen( ptr ));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CZipWriter::CZipWriter()
: m_iMaxLen( 0 )
, m_pBuffer( NULL )
{
	m_fp = NULL;
	m_iLen = 0;

}

CZipWriter::~CZipWriter()
{
	if( m_fp != NULL )
	{
		char *pBuffer = new char[m_iLen+ 12 ];
		memset( pBuffer, 0, sizeof(char) * (m_iLen + 12 ) );
		uLong len = m_iLen+12;
		compress( (Bytef *)pBuffer, &len, (Bytef *)m_pBuffer, m_iLen );

		fwrite( &m_iLen, sizeof(int), 1, m_fp );
		fwrite( pBuffer, sizeof(char), len, m_fp );

		if( m_fp != NULL )
		{
			fflush( m_fp );
			fclose( m_fp );
		}

		SAFE_DELETE_ARRAY( pBuffer );
	}

	SAFE_DELETE_ARRAY( m_pBuffer );
}

bool CZipWriter::Open( const std::string &filename  )
{
	m_fp = fopen(filename.c_str(), "wb");
	if (m_fp == NULL)
		return false;
	m_iLen = 0;
	return true;
}

void CZipWriter::checkBuffer( int len )
{
	if( m_iLen + len < m_iMaxLen )
		return;
	m_iMaxLen += len + len /2;
	char *pBuffer = new char[m_iMaxLen];
	if( m_pBuffer != NULL )
	{
		memcpy( pBuffer, m_pBuffer, m_iLen );
		delete [] m_pBuffer;
	}
	m_pBuffer = pBuffer;
}

bool CZipWriter::Write( const void *ptr, int len )
{
	checkBuffer( len );
	memcpy( &m_pBuffer[m_iLen], ptr, len );
	m_iLen += len;
	return true;
}

bool CZipWriter::Write( int i )
{
	Write( (void*)&i, sizeof( int ) );
	return true;
}

bool CZipWriter::Write( float f )
{
	Write( (void*)&f, sizeof( float ) );
	return true;
}

bool CZipWriter::Write( const char *ptr)
{
	Write( int( strlen( ptr )) );
	Write( (const void*)(ptr), strlen( ptr ));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CZipBufLoader::CZipBufLoader( char * buf, int len )
{
	int size = *(int*)buf;
	char * pBuff = buf +4;

	m_Buf = new char[size];

	uLong uLen = len - sizeof(int);
	uLong uSize = size;
	uncompress( (Bytef *)m_Buf, &uSize ,(Bytef *)pBuff, uLen);
	delete[] buf;

	m_iOff = 0;
	m_iLen = size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CZipLoader::CZipLoader()
{
	m_Buf = NULL;
}

CZipLoader::~CZipLoader()
{
	SAFE_DELETE_ARRAY( m_Buf );
}

bool CZipLoader::Open( const std::string &filename )
{
	SAFE_DELETE_ARRAY( m_Buf );

	FILE *fp;
	fp = fopen(filename.c_str(), "rb");
	if (fp == NULL)
		return false;

	fseek(fp, 0, SEEK_END);
	int len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	int size;
	fread( &size, sizeof( int ), 1, fp ); 
	char *pBuffer = new char [len - sizeof(int)];
	fread(pBuffer, len - sizeof(int), 1, fp);
	fclose(fp);

	m_Buf = new char[size];

	uLong uLen = len - sizeof(int);
	uLong uSize = size;
	uncompress( (Bytef *)m_Buf, &uSize ,(Bytef *)pBuffer, uLen);
	delete[] pBuffer;

	m_iOff = 0;
	m_iLen = size;
	return true;
}

int CZipLoader::Read( char *ptr, int len )
{
	if( m_iOff + len > m_iLen )
		len = m_iLen - m_iOff;

	memcpy( ptr, &m_Buf[m_iOff], len );
	m_iOff += len;
	return len;
}

bool CZipLoader::ReadInt(int &iValue)
{
	return ( Read( (char*)&iValue, sizeof( int ) ) != 0 );
}

bool CZipLoader::ReadUInt(UINT&iValue)
{
	return ( Read( (char*)&iValue, sizeof( int ) ) != 0 );
}

char CZipLoader::ReadChar()
{
	char c;
	Read( &c, sizeof( char ) );
	return c;
}
short CZipLoader::ReadShort()
{
	short s;
	Read( (char*)&s, sizeof( short) );
	return s;
}

int CZipLoader::ReadString( char *szBuf, int len )
{
	int Length;
	if( !ReadInt( Length) )
		return 0;

	_ASSERT( len >  Length);
	if( Length == 0 )
	{
		szBuf[0] = 0;
		return 0;
	}

	Length = Read( szBuf, Length );
	szBuf[Length] = NULL;

	return Length;
}

float CZipLoader::ReadFloat()
{
	float f;
	Read( (char*)&f, sizeof( float ) );
	return f;
}

bool CZipLoader::ReadFloat( float &fValue)
{
	return ( Read( (char*)&fValue, sizeof( float ) ) != 0 );
}

