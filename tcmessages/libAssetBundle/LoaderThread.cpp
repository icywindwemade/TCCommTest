#include "IUDefine.h"
#include "IU.h"
#include "FileManager.h"
#include "LoaderThread.h"
#include "Texture.h"
#include "PNGLoader.h"
#include "TGALoader.h"
#include "TexLoader.h"
#include "Telegram.h"
#include "AutoSync.h"

IULock g_LoaderLock;

CLoaderThread* CLoaderThread::StaticInstance = 0;

CLoaderThread* CLoaderThread::Instance()
{
	if( StaticInstance == NULL )
		StaticInstance = new CLoaderThread;

	return StaticInstance;
}

void CLoaderThread::Destroy()
{
	if ( StaticInstance )
	{
		delete StaticInstance;
		StaticInstance = NULL;
	}
}

CLoaderThread::CLoaderThread()
: m_bRunning( false )
{
	InitLock( g_LoaderLock );
}

CLoaderThread::~CLoaderThread()
{
	DelLock( g_LoaderLock );
}

void CLoaderThread::Update()
{
	sLoadData data;
	CAutoSync lock( g_LoaderLock );
	if ( m_LoadList.empty() == false )
	{
		std::vector<sLoadData>::iterator it = m_LoadList.begin();
		if ( it != m_LoadList.end() )
		{
			if( it->type == 0 )
			{

				data.pTexture = it->pTexture;
				data.name = it->name.c_str();
				m_LoadList.erase( it );

				lock.Leave();

            /* /// Don't use Texture Loading feature
                 
                int scale = 1;
				unsigned char * pBuffer = Load( data.name, data.pTexture, scale );
				if( pBuffer != NULL )
				{
#if PROFILE_TEXTURE_MANAGER
					data.pTexture->SetTextureName( data.name );
#endif	// PROFILE_TEXTURE_MANAGER

					IUTextureManager().LoadComplete( data.pTexture, pBuffer );
				}		
                 */
                
                printf("\n\n !!!!!!!!!! We are not using Loader to load textures ..... !!!!!!!\n\n");
                return;
			}
			else if( it->type == 1 )
			{
				data.id = it->id;
				data.name = it->name.c_str();
				m_LoadList.erase( it );
				lock.Leave();

            /*
				char * pBuf = 0;
				int  length = 0;
				//length = IUGameManager().GetFileManager()->LoadFile( data.name.c_str(), &pBuf );
                length = CFileManager::sharedInstance()->LoadFile( data.name.c_str(), &pBuf );
                
				if( 0 != length )
				{
					TelegramSound *newMsg = new TelegramSound( 0, 0, 0, EngineMessage::SoundLoaded, pBuf, length, data.id );
					IUGameManager().Message( newMsg );
				}
             */
                
                printf("\n\n !!!!!!!!!! We are not using Loader to load textures ..... !!!!!!!\n\n");
				return;
			}
		}
	}

	if( m_DownloadList.empty() == false )
	{
		std::vector<sDownData>::iterator it = m_DownloadList.begin();
		if ( it != m_DownloadList.end() )
		{
			std::string strPath = it->path;
			std::string strName = it->name;
			std::string strURL = it->url;
#if IOS
			int Type = it->Type;
			int FriendIndex = it->FriendIndex;
#endif	// IOS
			lock.Leave();
#if IOS
			// Check kakao profile cache
			if ( Type == EDT_KakaoProfile )
			{
				char szFileName[64], szExtName[64];
				_splitpath(	strURL.c_str(), NULL, NULL, szFileName, szExtName );
				std::string Filename = std::string(szFileName) + std::string(szExtName);

				char FullPathname[MAX_PATH];
				IUGameManager().GetFileManager()->GetFullFilename( FullPathname, Filename.c_str() );
				if ( strcmp( FullPathname, "" ) )
				{
					// Existing cache.
					CAutoSync relock( g_LoaderLock );
					std::vector<sDownData>::iterator iit = m_DownloadList.begin();
					m_DownloadList.erase( iit );
					relock.Leave();
					
					FDownloadedInfo* DownloadedInfo = new FDownloadedInfo( Type, Filename, FriendIndex );
					TelegramPointer* msg = new TelegramPointer( 0, 0, 0, EngineMessage::Downloaded, DownloadedInfo );
					IUGameManager().Message( msg );
					return;
				}
			}

			NSMutableURLRequest *urlRequest;
			NSData *urlData;
			NSHTTPURLResponse *response = nil;

			NSError *error = nil;
			NSString *address;
			if( strURL == "" )
			{
				address = [NSString stringWithFormat:@"%s%s%s", m_DownloadAddress.c_str(), strPath.c_str(), strName.c_str() ];
			}
			else
			{
				address = [NSString stringWithFormat:@"%s%s%s", strURL.c_str(), strPath.c_str(), strName.c_str() ];
			}
			
			
			NSLog( @"LOADER: Download: %@", address );
			//@"http://118.36.245.195:9001/MoD/Patch/back_title0.png"
			urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:address] cachePolicy:NSURLRequestReturnCacheDataElseLoad timeoutInterval:30];
			[urlRequest setHTTPMethod:@"GET"];
			urlData = [NSURLConnection sendSynchronousRequest:urlRequest returningResponse:&response error:&error];
			/* Return Value
			 The downloaded data for the URL request. Returns nil if a connection could not be created or if the download fails.
			 */
			NSInteger statusCode = 0;
			if ( response != nil )
			{
				statusCode = [response statusCode];
			}
			if (response == nil)
			{
				// Check for problems
				if (error != nil)
				{
					NSLog(@"%@", [error localizedDescription]);
					
					TelegramSzString *msg = new TelegramSzString( 0,0,0, EngineMessage::DownloadFail, strName.c_str() );
					IUGameManager().Message( msg );
					return;

				}
			}
			else if ( statusCode != 200 )
			{
				NSLog( @"Response Error: %d", statusCode );
				TelegramSzString *msg = new TelegramSzString( 0,0,0, EngineMessage::DownloadFail, strName.c_str() );
				IUGameManager().Message( msg );
				if ( strName == "PatchInfo.xml" )
				{
					CAutoSync relock( g_LoaderLock );
					std::vector<sDownData>::iterator iit = m_DownloadList.begin();
					m_DownloadList.erase( iit );
					relock.Leave();
				}
				return;

			}
			else if ( urlData == nil )
			{
				NSLog( @"Data not received" );
				TelegramSzString *msg = new TelegramSzString( 0,0,0, EngineMessage::DownloadFail, strName.c_str() );
				IUGameManager().Message( msg );
				return;
			}
			
			if( !urlData || statusCode != 200 )
			{
				NSLog(@"Download Failed!");
				TelegramSzString *msg = new TelegramSzString( 0,0,0, EngineMessage::DownloadFail, strName.c_str() );
				IUGameManager().Message( msg );
				return;
			}
			
			{
				CAutoSync relock( g_LoaderLock );
				std::vector<sDownData>::iterator iit = m_DownloadList.begin();
				m_DownloadList.erase( iit );
				relock.Leave();
			}
		
			NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
			NSString *documentsPath = [paths objectAtIndex:0];
			if ( Type == EDT_PatchResource )
			{
				[urlData writeToFile:[NSString stringWithFormat:@"%@/%s_%s", documentsPath, strName.c_str(), "New" ] atomically:YES];
			}
			else if ( Type == EDT_KakaoProfile )
			{
				char szFileName[64], szExtName[64];
				_splitpath(	strURL.c_str(), NULL, NULL, szFileName, szExtName );
				
				[urlData writeToFile:[NSString stringWithFormat:@"%@/%s%s", documentsPath, szFileName, szExtName ] atomically:YES];
				strName = szFileName;
				strName = strName + szExtName;
				
//				char szCachePathname[MAX_PATH];
//				IUGetDocFileName(szCachePathname, strName.c_str());
//				ConvertJpegToPng( szCachePathname );
			}
			
			FDownloadedInfo* DownloadedInfo = new FDownloadedInfo( Type, strName, FriendIndex );
			TelegramPointer* msg = new TelegramPointer( 0, 0, 0, EngineMessage::Downloaded, DownloadedInfo );
			IUGameManager().Message( msg );
			return;

#elif ANDROID
			char szAddress[256];		
			if( strURL == "" )
			{
				sprintf( szAddress, "%s%s%s", m_DownloadAddress.c_str(), strPath.c_str(), strName.c_str() );
			}
			else
			{
				sprintf( szAddress, "%s%s%s", strURL.c_str(), strPath.c_str(), strName.c_str() );
			}

			char szFilename[256];
			sprintf( szFilename, "%s/%s_%s", IUGameManager().GetFileManager()->m_DocPath.c_str(), strName.c_str(), "New" );

			LogPrintf("download by url : %s -> %s", szAddress, szFilename);

			if( JniUtil::DownloadByURL( szFilename, szAddress ) == false )
			{
				LogPrintf("download failed" );
				TelegramSzString *msg = new TelegramSzString( 0,0,0, EngineMessage::DownloadFail, strName.c_str() );
				IUGameManager().Message( msg );
				return;
			}
			//LogPrintf("download Success" );

			{
				CAutoSync relock( g_LoaderLock );
				std::vector<sDownData>::iterator iit = m_DownloadList.begin();
				m_DownloadList.erase( iit );
				relock.Leave();
			}

			TelegramSzString *msg = new TelegramSzString( 0,0,0, EngineMessage::Downloaded, strName.c_str() );
			IUGameManager().Message( msg );
			return;
#endif	// IOS
		}
	}
}

void CLoaderThread::Add( const std::string &name, CTexture *pTexture )
{
	CAutoSync lock( g_LoaderLock );
	sLoadData data;
	data.type = 0;
	data.name = name;
	data.pTexture = pTexture;

	m_LoadList.push_back(data);
}

void CLoaderThread::SetDownAddress( const std::string &address )
{
	CAutoSync lock( g_LoaderLock );
	m_DownloadAddress = address;
}

void CLoaderThread::Download( const std::string &url, const std::string &path, const std::string &name, int type, int FriendIndex )
{
	sDownData data;
	data.url = url;
	data.path = path;
	data.name = name;
	data.Type = type;
	data.FriendIndex = FriendIndex;

#if ANDROID
	LogPrintf( "LoaderThread::Download : %s", name.c_str() );
#endif	// ANDROID

	{
		CAutoSync lock( g_LoaderLock );
		m_DownloadList.push_back( data );
	}
}

void CLoaderThread::LoadSound( const std::string &name, int soundID )
{
	CAutoSync lock( g_LoaderLock );
	sLoadData data;
	data.type = 1;
	data.name = name;
	data.id = soundID;

	m_LoadList.push_back(data);
}

unsigned char* CLoaderThread::Load( const std::string &szFilename, CTexture *pTexture, int scale )
{
	CTextureLoader *loader;

	char szFileName[64], szExtName[64];
	_splitpath(	szFilename.c_str(), NULL, NULL, szFileName, szExtName );

	//if( IUGameManager().GetUIManager()->m_iScale == 2 && !strstr( szFileName, "@2") && IUEnableHD() )
    if( scale == 2 && !strstr( szFileName, "@2") && IUEnableHD() )
	{
		char szName[128];
		sprintf(szName, "%s@2%s", szFileName, szExtName);
		unsigned char *pBuffer = Load( szName, pTexture, scale );
		if( pBuffer != NULL )
		{
			pTexture->SetScale(2);
			return pBuffer;
		}
	}

#if	USE_PACKED_DATA
	if( !strcmp( szExtName, ".png" ) )
	{
		char szName[128];
		sprintf(szName, "%s.tex", szFileName);
		unsigned char *pBuffer = Load( szName, pTexture );
		if( pBuffer != NULL )
			return pBuffer;
	}
#endif	// USE_PACKED_DATA

	if( !strcmp( szExtName, ".tex" ) )
	{
		loader = new CTexLoader();
	}

#if	!USE_PACKED_DATA
	else if( !strcmp( szExtName, ".png" ) )
	{
		loader = new CPNGLoader();
	}
	else if( !strcmp( szExtName, ".tga" ) )
	{
		loader = new CTGALoader();
	}
#endif	// !USE_PACKED_DATA
	else
		return NULL;

	unsigned char *pBuffer = loader->Load( szFilename, pTexture);
	delete loader;
	return pBuffer;
}
