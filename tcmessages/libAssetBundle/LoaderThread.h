#pragma once

#include <vector>
#include <string>
#include "Texture.h"

struct sLoadData
{
	int type;
	int id;
	std::string name;
	CTexture *pTexture;
};

enum EDownloadType
{
	EDT_PatchResource	= 0,
	EDT_KakaoProfile	= 1, 
};


struct sDownData
{
	std::string url;
	std::string path;
	std::string name;
	int	Type;
	int FriendIndex;
	int crc;

	sDownData()
		: Type ( EDT_PatchResource )
		, crc( -1 )
		, FriendIndex( -1 )
	{
	}
};

struct FDownloadedInfo
{
	int Type;
	std::string Filename;
	int FriendIndex;
	
	FDownloadedInfo( INT InType, const std::string& InFilename, INT InFriendIndex = -1 )
		: Type( InType )
		, Filename( InFilename )
		, FriendIndex( InFriendIndex )
	{
	}
};

class CLoaderThread
{
public:
	static CLoaderThread* Instance();
	static void Destroy();

	CLoaderThread();
	~CLoaderThread();

	void Update();
	void Add( const std::string &name, CTexture *pTexture );
	void SetDownAddress( const std::string &address );
	void Download( const std::string &url, const std::string &path, const std::string &name, int Type = EDT_PatchResource, int FriendIndex = INDEX_NONE );
	void LoadSound( const std::string &name, int soundID );
	static unsigned char * Load( const std::string &name, CTexture *pTexture, int scale );

private:
	bool					m_bRunning;
	static CLoaderThread*	StaticInstance;

	std::vector< sLoadData > m_LoadList;

	std::string  m_DownloadAddress;
	std::vector< sDownData > m_DownloadList;
};

//extern IULock g_LoaderLock;