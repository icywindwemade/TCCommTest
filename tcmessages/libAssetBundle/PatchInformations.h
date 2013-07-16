/*========================================================================
	PatchInformations.h: Package patch information declarations.
	Copyright 2012. All Rights Reserved.
========================================================================*/

#ifndef __PATCHINFORMATIONS_H__
#define __PATCHINFORMATIONS_H__

//#include <Windows.h>
#include <string>
#include <vector>

#include "Core.h"
#ifndef MAX_PATH
#define MAX_PATH 260
#endif


struct PatchVersion 
{
	int v1;
	int v2;
	int v3;
	std::string filename;
};

struct crcInfo
{
	std::string Filename;
	std::string strConvertFilename;
	unsigned long crc;
	unsigned long ConvertCrc;
};

class CXMLParser;


class CPatchInfo
{
public:
	CPatchInfo()
		: GameDataCrc( 0 )
		, ProductDataCrc( 0 )
	{}
	virtual ~CPatchInfo() {}

	virtual bool CheckFile( const std::string &Filename ) = 0;
	virtual void SetConvertFilename( const std::string &Filename, const std::string &strConvertName ) = 0;
	virtual void SaveInfo() = 0;
	virtual const char *GetDestPath( const std::string &Filename ) = 0;
	virtual void SetConvertCRC( const std::string &Filename, const std::string &strConvertName ) = 0;

	DWORD	GameDataCrc;
	DWORD	ProductDataCrc;
	DWORD	GetFileCrc( const char* szDstFilename )
	{
		char *pBuf = NULL;
		int len		= 0;
		FILE *fp	= NULL;
		fp = fopen( szDstFilename, "rb" );
		if (fp == NULL)
		{
			// Can't open file
			assert(false);
		}

		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		pBuf = new char [len+1];
		fread(pBuf, 1, len, fp);
		fclose(fp);
		(pBuf)[len] = '\0';

		return appMemCrc( pBuf, len );
	}

protected:
	std::vector<crcInfo> m_DestList;
	std::vector<crcInfo> m_PatchList;

	char m_szPatchPath[MAX_PATH];
	char m_szOriginalPath[MAX_PATH];
	int m_Version[3];
private:
};

class CPatchUpdateInfo : public CPatchInfo
{
public:
	CPatchUpdateInfo(const std::string &filename, int v1, int v2, int v3, const std::string &strDestPath);
	virtual ~CPatchUpdateInfo();

	virtual bool CheckFile( const std::string &Filename );
	virtual void SetConvertFilename( const std::string &Filename, const std::string &strConvertName );
	virtual void SaveInfo();
	virtual const char *GetDestPath( const std::string &Filename );
	virtual void SetConvertCRC( const std::string &Filename, const std::string &strConvertName ) {}

protected:
	bool IsSameWithkOriginal( const std::string &Filename, unsigned long crc  );
	std::vector<crcInfo> m_OriginalList;
	std::string m_strDestPath;

private:
};


class CFirstPatchInfo : public CPatchInfo
{
public:
	CFirstPatchInfo( std::vector<std::string> &fileList, int v1, int v2, int v3, const std::string &strDestPath );
	~CFirstPatchInfo();
	bool CheckFile( const std::string &Filename );
	void SetConvertFilename( const std::string &Filename, const std::string &strConvertName );
	void SetConvertCRC( const std::string &Filename, const std::string &strConvertName );
	void SaveInfo();

	const char *GetDestPath( const std::string &Filename );

protected:
	std::string m_strDestPath;
	bool CheckOriginal( const std::string &Filename );
	std::vector<std::string> m_OriginalList;

private:
};

#endif	// __PATCHINFORMATIONS_H__