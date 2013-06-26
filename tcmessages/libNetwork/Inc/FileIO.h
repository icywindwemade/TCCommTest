#pragma once

#include <string>
#include "IUDefine.h"

class CFileIO
{
public:
	CFileIO();
	virtual ~CFileIO();

	virtual bool Open( const std::string &filename );
	virtual bool OpenCurrentFile( const std::string &filename );
	virtual int Read( char *ptr, int len );
	virtual int ReadInt();
	virtual char ReadChar();
	virtual short ReadShort();
	virtual float ReadFloat();
	virtual int ReadString( char *szBuf, int len );
	virtual int GetLen()	{ return m_iLen; }
	virtual unsigned long GetCRC();
	virtual char *GetBuf() { return m_Buf; }

protected:
	char *m_Buf;
	int m_iLen;
	int m_iOff;
};

class CFileWriter
{
public:
	CFileWriter();
	virtual ~CFileWriter();

	virtual bool Open( const std::string &filename );
	virtual bool Write( const void *ptr, int len );
	virtual bool Write( int i );
	virtual bool Write( float f );
	virtual bool Write( const char *ptr );

protected:
	FILE *m_fp;
	int m_iLen;
};

class CZipWriter : public CFileWriter
{
public:
	CZipWriter();
	virtual ~CZipWriter();

	bool Open( const std::string &filename );
	bool Write( const void *ptr, int len );
	bool Write( int i );
	bool Write( float f );
	bool Write( const char *ptr );

protected:
	void checkBuffer( int len );
	int m_iMaxLen;
	char *m_pBuffer;
};

class CZipLoader : public CFileIO
{
public:
	CZipLoader();
	virtual ~CZipLoader();

	bool Open( const std::string &filename );
	int Read( char *ptr, int len );
	bool ReadInt(int &iValue);
	bool ReadUInt(UINT &iValue);
	char ReadChar();
	short ReadShort();
	float ReadFloat();
	bool ReadFloat( float &fValue);
	int ReadString( char *szBuf, int len );
	int GetLen() { return m_iLen; }
	char * GetBuf() { return m_Buf; }
};

class CZipBufLoader : public CZipLoader
{
private :
	CZipBufLoader();

public:
	CZipBufLoader( char * buf, int len );
	virtual ~CZipBufLoader() { SAFE_DELETE_ARRAY( m_Buf ); }
};

class CBufLoader : public CFileIO
{
private :
	CBufLoader();
public :
	CBufLoader( char * buf, int len ){ m_Buf = buf; m_iLen = len; m_iOff = 0; }	
	virtual ~CBufLoader(){ SAFE_DELETE_ARRAY( m_Buf ); }
};