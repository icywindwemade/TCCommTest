 #include "IUDefine.h"
#include <stdlib.h>
#include "TextureManager.h"
#include "IU.h"

#include "Texture.h"
#include "PNGLoader.h"
#include "TGALoader.h"
#include "TexLoader.h"
#include "LoaderThread.h"
#include "Telegram.h"
#include "Map.h"
#include "AutoSync.h"

#include "Core.h"

IULock g_TextureLock;

CTextureManager::CTextureManager()
: m_TextureNum( 0 )
, m_iCurrentListID( 0 )
, m_iBackListID(1)
//, m_iBackListID(0)
, NumOfTextures( 0 )
{
	InitLock( g_TextureLock );
	m_Empty = "";

	//LogPrintf(" g_TextureLock : %d", g_TextureLock );
}

CTextureManager::~CTextureManager()
{
	DelLock( g_TextureLock );

#if TEST_TMAP
	for ( TMap<INT, CTexture*>::TIterator Iter(TextureList); Iter; ++Iter )
	{
		CTexture* TextureToDelete = Iter.Value();
		if ( TextureToDelete )
		{
			delete TextureToDelete;
		}
	}

	TextureList.Empty();

#else
	for ( std::map<int, CTexture *>::iterator TextureIter = m_TextureList.begin(); TextureIter != m_TextureList.end(); ++TextureIter )
	{
		CTexture* TextureToDelete = (CTexture*)TextureIter->second;
		delete TextureToDelete;
	}

	m_TextureList.clear();
#endif	// TEST_TMAP_TEXTUREMANAGER
}

#if TEST_TMAP
int CTextureManager::LoadTexture( const std::string &szFilename, bool bBackground )
{
	assert( !szFilename.empty() );

	INT NewTextureId = INDEX_NONE;
	CTexture* pTexture = NULL;

	CAutoSync sync( g_TextureLock );
	{
		INT* TextureId = TextureMap.Find( szFilename );
		if ( TextureId )
		{
			GLuint* GLTextureId = GLTextures.Find( *TextureId );
			if ( !GLTextureId )
			{
				CTexture* pTexture = new CTexture();

				TextureList.Set( *TextureId, pTexture );
				GLTextures.Set( *TextureId, -1 );
				pTexture->TextureIdx = *TextureId;

#if PROFILE_TEXTURE_MANAGER
				pTexture->SetTextureName( szFilename );
#endif // _DEBUG

				CLoaderThread::Instance()->Add( szFilename, pTexture );
			}

#if ANDROID
			IUSleep( eThread_Main );
#else	// WIN32 || IOS
			IUSleep(1);
#endif	// ANDROID
			return *TextureId;
		}

		pTexture = new CTexture();

		TextureList.Set( m_TextureNum, pTexture );
		NewTextureId = m_TextureNum;
		TextureMap.Set( szFilename, m_TextureNum );
		GLTextures.Set( m_TextureNum, -1 );
		pTexture->TextureIdx = m_TextureNum;
		m_TextureNum++;

#if PROFILE_TEXTURE_MANAGER
		pTexture->SetTextureName( szFilename );
#endif // _DEBUG
	}
	sync.Leave();

#if ANDROID
	IUSleep( eThread_Main );
#else	// WIN32 || IOS
	IUSleep(1);
#endif	// ANDROID

	CLoaderThread::Instance()->Add( szFilename, pTexture );
	IUGameManager().GetRenderManager()->SetCurrentTexture( -1 );
	return NewTextureId;
}

#else
int CTextureManager::LoadTexture( const std::string &szFilename, bool bBackground )
{
	int iId;
	CTexture *pTexture;
	CAutoSync sync( g_TextureLock );
	{
		std::map<std::string, int >::iterator it = m_TextureMap.find( szFilename );
		if( it != m_TextureMap.end() )
		{
			if( m_GLTexture.find( it->second ) == m_GLTexture.end() )
			{
				std::map< std::string, int>::iterator iit = m_TextureMap.begin();
				std::string  szFilename;
				while( iit != m_TextureMap.end() )
				{
					if( iit->second == it->second )
					{
						szFilename = iit->first;
						break;
					}

					iit++;
				}

				CTexture *pTexture = new CTexture();

				m_TextureList[it->second] = pTexture;
				m_GLTexture[it->second] = -1;
				pTexture->TextureIdx = it->second;

#if PROFILE_TEXTURE_MANAGER
				pTexture->SetTextureName( szFilename );
#endif // _DEBUG

				CLoaderThread::Instance()->Add( szFilename, pTexture );
			}
#if ANDROID
			IUSleep( eThread_Main );
#else	// WIN32 || IOS
			IUSleep(1);
#endif	// ANDROID
			return it->second;
		}

		pTexture = new CTexture();

		m_TextureList[m_TextureNum] = pTexture;
		iId = m_TextureNum;
		m_TextureMap[szFilename] = m_TextureNum;
		m_GLTexture[m_TextureNum] = -1;
		pTexture->TextureIdx = m_TextureNum;
		m_TextureNum++;

#if PROFILE_TEXTURE_MANAGER
		pTexture->SetTextureName( szFilename );
#endif // _DEBUG
	}
	sync.Leave();

#if ANDROID
	IUSleep( eThread_Main );
#else	// WIN32 || IOS
	IUSleep(1);
#endif	// ANDROID

	{
		CLoaderThread::Instance()->Add( szFilename, pTexture );
		///IUGameManager().GetRenderManager()->SetCurrentTexture( -1 );
		return iId;
	}

}
#endif	// TEST_TMAP_TEXTUREMANAGER


void CTextureManager::UpdateSubTexture( int iid, unsigned char * pBuffer, int pos[2], int size[2] )
{
	FGLTextureMessage data;
	CAutoSync sync( g_TextureLock );
#if TEST_TMAP
	data.pTexture = TextureList.FindRef( iid );
#else
	data.pTexture = m_TextureList[iid];
#endif	// TEST_TMAP_TEXTUREMANAGER
	data.MessageType = EGLMessage_TextureSub;
	data.iPos[0] = pos[0];		data.iPos[1] = pos[1];
	data.iSize[0] = size[0];	data.iSize[1] = size[1];

	int iMemSize = size[ 0 ] * size[ 1 ] * 2;
	if( iMemSize <= 0 )
	{
		return;
	}
	
	data.pBuffer = new unsigned char[ iMemSize ];
	memcpy( data.pBuffer, pBuffer, sizeof( unsigned char) * iMemSize );
	m_LoadCompleteList[m_iCurrentListID].push_back( data );
}

int CTextureManager::CreateEmptyTexture( int size[2], int format, int type )
{
	int iid;
	CTexture *pTexture;
	pTexture = new CTexture();
	pTexture->SizeX = size[0];
	pTexture->SizeY = size[1];
	pTexture->Format = format;
	pTexture->Type = type;
#if PROFILE_TEXTURE_MANAGER
	pTexture->SetTextureName( "EMPTY TEXTURE" );
#endif	// PROFILE_TEXTURE_MANAGER

	{
		CAutoSync sync( g_TextureLock );
#if TEST_TMAP
		TextureList.Set( m_TextureNum, pTexture );
		GLTextures.Set( m_TextureNum, -1 );
#else
		m_TextureList[m_TextureNum] = pTexture;
		m_GLTexture[m_TextureNum] = -1;
#endif	// TEST_TMAP_TEXTUREMANAGER
		pTexture->TextureIdx = m_TextureNum;
		iid = m_TextureNum;
		m_TextureNum++;
	}

#if ANDROID
	IUSleep( eThread_Main );
#else	// WIN32 || IOS
	IUSleep(1);
#endif	// ANDROID

	LoadComplete( pTexture, NULL );
	return iid;
}

#if TEST_TMAP 
void CTextureManager::RemoveTexture( int iid )
{
	CAutoSync sync( g_TextureLock );
	if ( !TextureList.HasKey( iid ) )
	{
		return;
	}

	CTexture *pTexture = TextureList.FindRef( iid );
	if( pTexture != NULL )
	{
		std::string TextureName;
		int iid = pTexture->TextureIdx;

		if ( TextureList.HasKey( iid ) )
		{
#if PROFILE_TEXTURE_MANAGER
			TextureName = pTexture->TextureName;
#endif	// _DEBUG
			delete pTexture;
			TextureList.Remove( iid );
		}

		if ( GLTextures.HasKey( iid ) )
		{
			FGLTextureMessage data;
			data.MessageType = EGLMessage_TextureRemove;
			data.GLTextureId = GLTextures.FindRef( iid );
#if PROFILE_TEXTURE_MANAGER
			data.SetTextureName( TextureName );
#endif	// _DEBUG

			m_LoadCompleteList[m_iCurrentListID].push_back( data );

			GLTextures.Remove( iid );
			NumOfTextures--;
		}
	}
	else
	{
		if ( TextureList.HasKey( iid ) )
		{
			TextureList.Remove( iid );
		}
	}
}

#else
void CTextureManager::RemoveTexture( int iid )
{
	CAutoSync sync( g_TextureLock );
	CTexture *pTexture = m_TextureList[iid];

	if( pTexture != NULL )
	{
		std::string TextureName;
		int iid = pTexture->TextureIdx;
		std::map< int, CTexture*>::iterator iit = m_TextureList.find( iid );
		if( iit != m_TextureList.end() )
		{
#if PROFILE_TEXTURE_MANAGER
			TextureName = iit->second->TextureName;
#endif	// _DEBUG
			delete iit->second;
			m_TextureList.erase( iit );
		}

		std::map< int,GLuint>::iterator iiit = m_GLTexture.find( iid );
		if( iiit != m_GLTexture.end() )
		{
			FGLTextureMessage data;
			data.MessageType = EGLMessage_TextureRemove;
			data.GLTextureId = iiit->second;
#if PROFILE_TEXTURE_MANAGER
			data.SetTextureName( TextureName );
#endif	// _DEBUG

			m_LoadCompleteList[m_iCurrentListID].push_back( data );

			m_GLTexture.erase( iiit );
			NumOfTextures--;
		}
	}
	else
	{
		std::map<int, CTexture *>::iterator it = m_TextureList.find( iid );
		if( it != this->m_TextureList.end() )
		{
			m_TextureList.erase( it );
		}
	}
}
#endif	// TEST_TMAP_TEXTUREMANAGER

void CTextureManager::PushMessage( int iMessage )
{
	FGLTextureMessage data;
	data.MessageType = iMessage;

	CAutoSync sync( g_TextureLock );
	m_LoadCompleteList[m_iCurrentListID].push_back( data );
}

#if TEST_TMAP
GLuint CTextureManager::GetGLTexture( int iid )	
{
	CAutoSync sync( g_TextureLock );
	{
		if ( GLTextures.HasKey( iid ) )
		{
			return GLTextures.FindRef( iid );
		}

		const std::string* szFilename = TextureMap.FindKey( iid );
		if ( szFilename )
		{	
			CTexture *pTexture = new CTexture();

			TextureList.Set( iid, pTexture );
			GLTextures.Set( iid, -1 );
			pTexture->TextureIdx = iid;


#if PROFILE_TEXTURE_MANAGER
			pTexture->SetTextureName( *szFilename );
#endif	// PROFILE_TEXTURE_MANAGER
			CLoaderThread::Instance()->Add( *szFilename, pTexture );

			return -1;
		}
	}

	return 0;
}
#else
GLuint CTextureManager::GetGLTexture( int iid )	
{
	CAutoSync sync( g_TextureLock );
	{
		std::map<int, GLuint>::iterator it = m_GLTexture.find( iid );
		if( it != m_GLTexture.end() )
			return it->second;

		std::map< std::string, int>::iterator iit = m_TextureMap.begin();
		std::string szFilename;
		while( iit != m_TextureMap.end() )
		{
			if( iit->second == iid )
			{
				szFilename = iit->first;
				break;
			}

			iit++;
		}

		CTexture *pTexture = new CTexture();

		m_TextureList[iid] = pTexture;
		m_GLTexture[iid] = -1;
		pTexture->TextureIdx = iid;

#if PROFILE_TEXTURE_MANAGER
		pTexture->SetTextureName( szFilename );
#endif	// PROFILE_TEXTURE_MANAGER
		CLoaderThread::Instance()->Add( szFilename, pTexture );
	}
	return -1;
}
#endif	// TEST_TMAP_TEXTUREMANAGER

#if TEST_TMAP
const std::string& CTextureManager::GetTextureName( int iid )
{
	const std::string* TextureFilename = TextureMap.FindKey( iid );
	if ( TextureFilename )
	{
		return *TextureFilename;
	}

	return m_Empty;
}
#else
const std::string& CTextureManager::GetTextureName( int iid )
{
	std::map< std::string, int>::iterator iit = m_TextureMap.begin();
	while( iit != m_TextureMap.end() )
	{
		if( iit->second == iid )
		{
			return iit->first;
			break;
		}
		iit++;
	}
	return m_Empty;
}
#endif	// TEST_TMAP_TEXTUREMANAGER

#if TEST_TMAP
const CTexture *CTextureManager::GetTexture( int iid )
{
	CAutoSync sync( g_TextureLock );
	if ( TextureList.HasKey( iid ) )
	{
		return TextureList.FindRef( iid );
	}

	const std::string* TextureFilename = TextureMap.FindKey( iid );
	if ( TextureFilename )
	{
		CTexture *pTexture = new CTexture();

		TextureList.Set( iid, pTexture );
		GLTextures.Set( iid, -1 );
		pTexture->TextureIdx = iid;
#if PROFILE_TEXTURE_MANAGER
		pTexture->SetTextureName( *TextureFilename );
#endif	// PROFILE_TEXTURE_MANAGER
		CLoaderThread::Instance()->Add( *TextureFilename, pTexture );

		return pTexture;
	}

	return NULL;
}
#else
const CTexture *CTextureManager::GetTexture( int iid )
{
	CAutoSync sync( g_TextureLock );
	std::map<int, CTexture *>::iterator it = m_TextureList.find( iid );
	if( it != m_TextureList.end() )
		return it->second;

	std::map< std::string, int>::iterator iit = m_TextureMap.begin();
	std::string  szFilename;
	while( iit != m_TextureMap.end() )
	{
		if( iit->second == iid )
		{
			szFilename = iit->first;
			break;
		}

		iit++;
	}

	CTexture *pTexture = new CTexture();

	m_TextureList[iid] = pTexture;
	m_GLTexture[iid] = -1;
	pTexture->TextureIdx = iid;
#if PROFILE_TEXTURE_MANAGER
	pTexture->SetTextureName( szFilename );
#endif	// PROFILE_TEXTURE_MANAGER
	CLoaderThread::Instance()->Add( szFilename, pTexture );
	return pTexture;
}
#endif	// TEST_TMAP_TEXTUREMANAGER

void CTextureManager::LoadComplete( CTexture *pTexture, unsigned char * pbuffer )
{
	FGLTextureMessage data;
	data.pTexture = pTexture;
	data.pBuffer = pbuffer;
#if PROFILE_TEXTURE_MANAGER
	data.SetTextureName( pTexture->TextureName );
#endif	// _DEBUG

	if( pbuffer == NULL )
	{	
		data.MessageType = EGLMessage_TextureEmpty;
	}
	else
	{
		data.MessageType = EGLMessage_TextureLoad;
	}

	{
		CAutoSync sync( g_TextureLock );
		m_LoadCompleteList[m_iCurrentListID].push_back( data );
	}
}

void CTextureManager::UpdateTexture( CTexture *pTexture, unsigned char * pBuffer )
{
	GLuint glID;
	glGenTextures(1, &glID);

#if PROFILE_TEXTURE_MANAGER
	char DebugString[1024];
	sprintf( DebugString, "glGenTextures: %d %s\n", glID, pTexture->TextureName.c_str() );
#if WIN32
	OutputDebugString( DebugString );
#else
    printf( "%s", DebugString );
    
#if PROFILE_MEMORY
    print_free_memory();
#endif  // 1

#endif  // WIN32
    
#endif	// PROFILE_TEXTURE_MANAGER

	NumOfTextures++;
	glBindTexture(GL_TEXTURE_2D, glID);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	if( pTexture->ColorBit == 32 )
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pTexture->SizeX, pTexture->SizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);
#if		PROFILE_TEXTURE_MANAGER
		OutputDebugString("32Bits Image: ");
#endif	// PROFILE_TEXTURE_MANAGER
	}
	else if( pTexture->ColorBit == 16 )
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pTexture->SizeX, pTexture->SizeY, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, pBuffer);
#if		PROFILE_TEXTURE_MANAGER
		OutputDebugString("4444 16Bits Image: ");
#endif	// PROFILE_TEXTURE_MANAGER
	}
	else // if( pTexture->ColorBit == ( 0x80 & 16) )
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pTexture->SizeX, pTexture->SizeY, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pBuffer);
#if		PROFILE_TEXTURE_MANAGER
		OutputDebugString("565 16Bits Image: ");
#endif	// PROFILE_TEXTURE_MANAGER
	}

#if		PROFILE_TEXTURE_MANAGER
	OutputDebugString(pTexture->TextureName.c_str());
	OutputDebugString("\n");
#endif	// PROFILE_TEXTURE_MANAGER

#if TEST_TMAP
	GLTextures.Set( pTexture->TextureIdx, glID );
#else
	m_GLTexture[pTexture->TextureIdx] = glID;
#endif	// TEST_TMAP_TEXTUREMANAGER
	delete [] pBuffer;
}

void CTextureManager::Update()
{
	{
		CAutoSync sync( g_TextureLock );
		if( m_LoadCompleteList[m_iCurrentListID].size() == 0 )
			return;
		m_iCurrentListID = m_iBackListID;
		m_iBackListID = m_iBackListID == 1 ? 0 : 1;  
		m_LoadCompleteList[m_iCurrentListID].clear();
	}

#if ANDROID
	IUSleep( eThread_Render );
#else	// WIN32 || IOS
	IUSleep(1);
#endif	// ANDROID

	std::vector< FGLTextureMessage>::iterator it = m_LoadCompleteList[m_iBackListID].begin();
	for(; it != m_LoadCompleteList[m_iBackListID].end(); it++ )
	{
		if( it->MessageType == EGLMessage_TextureEmpty )
		{
			int size[2] = { it->pTexture->SizeX, it->pTexture->SizeY };
			GLuint glid = it->pTexture->GenerateEmptyTexture( size, it->pTexture->Format, it->pTexture->Type );
			CAutoSync sync( g_TextureLock );
#if TEST_TMAP
			GLTextures.Set( it->pTexture->TextureIdx, glid );
#else
			m_GLTexture[it->pTexture->TextureIdx] = glid;
#endif	// TEST_TMAP_TEXTUREMANAGER

        }
		else if( it->MessageType == EGLMessage_TextureLoad )
		{
			UpdateTexture( it->pTexture, it->pBuffer );
			TelegramSzString *msg = new TelegramSzString( 0,0,0, EngineMessage::Loaded, GetTextureName( it->pTexture->TextureIdx ).c_str() );
			///IUGameManager().ThreadMessage( msg );
		}
		else if( it->MessageType == EGLMessage_TextureSub )
		{
            if ( it->pTexture )
			{
                glBindTexture(GL_TEXTURE_2D, it->pTexture->GetTextureID() );
                glTexSubImage2D( GL_TEXTURE_2D, 0, it->iPos[0], it->iPos[1], it->iSize[0], it->iSize[1], it->pTexture->Format, it->pTexture->Type, it->pBuffer );
				SAFE_DELETE_ARRAY( it->pBuffer );
            }
		}
		else if( it->MessageType == EGLMessage_TextureRemove)
		{
			glDeleteTextures( 1, &(it->GLTextureId) );
#if !ANDROID
#if PROFILE_TEXTURE_MANAGER
			char DebugString[1024];
			sprintf( DebugString, "glDeleteTextures: %d %s\n", it->GLTextureId, it->TextureName.c_str() );
            
#if WIN32
            OutputDebugString( DebugString );
#else	// IOS
            printf( "%s", DebugString );
#endif // WIN32
#if PROFILE_MEMORY
            print_free_memory();
#endif  // 1

#endif  // WIN32
            
#endif	// !ANDROID


		}
		else if( it->MessageType == EGLMessage_MapLoaded )
		{
			CMap * pMap = IUGameManager().GetMap();
			if( pMap )
				pMap->BuildMap();

			Telegram *msg = new Telegram( 0, 0, 0, EngineMessage::MapLoaded );
			IUGameManager().ThreadMessage( msg );
		}
		else if( it->MessageType == EGLMessage_ShowShadow )
		{
			IUGetRenderManager()->ShowFog( true );
		}
		else if( it->MessageType == EGLMessage_HideShadow )
		{
			IUGetRenderManager()->ShowFog( false );
		}
	}
	m_LoadCompleteList[m_iBackListID].clear();
	IUGameManager().GetRenderManager()->SetCurrentTexture( -1 );
}

#if TEST_TMAP
void CTextureManager::Empty()
{
	CAutoSync sync( g_TextureLock );

	TArray<INT>	PendingRemoveTextures;

	for ( TMap<INT, CTexture*>::TIterator Iter(TextureList); Iter; ++Iter )
	{
		CTexture* pTexture = Iter.Value();
		if( pTexture != NULL )
		{
			INT TextureId = pTexture->TextureIdx;

			GLuint* GLTextureId = GLTextures.Find( TextureId );
			if ( GLTextureId )
			{
				FGLTextureMessage data;
				data.MessageType = EGLMessage_TextureRemove;
				data.GLTextureId = *GLTextureId;

#if PROFILE_TEXTURE_MANAGER
				data.SetTextureName( pTexture->TextureName );
#endif	// _DEBUG

				m_LoadCompleteList[m_iCurrentListID].push_back( data );
				GLTextures.Remove( TextureId );
			}

			delete pTexture;
		}
	}

	//TextureList.Empty();
	//GLTextures.Empty();
}

#else

void CTextureManager::Empty()
{
    CAutoSync sync( g_TextureLock );
    for ( int i = 0; i < (int)m_TextureList.size(); i++ )
    {
        CTexture *pTexture = m_TextureList[i];
        if( pTexture != NULL )
        {
#if PROFILE_TEXTURE_MANAGER
			std::string TextureName;
#endif	// PROFILE_TEXTURE_MANAGER
            int iid = pTexture->TextureIdx;
            std::map< int, CTexture*>::iterator iit = m_TextureList.find( iid );
            if( iit != m_TextureList.end() )
            {
#if PROFILE_TEXTURE_MANAGER
				//TextureName = iit->second->TextureName;
#endif	// PROFILE_TEXTURE_MANAGER
                delete iit->second;
                m_TextureList.erase( iit );
            }

            std::map< int,GLuint>::iterator iiit = m_GLTexture.find( iid );
            if( iiit != m_GLTexture.end() )
            {
                FGLTextureMessage data;
                data.MessageType = EGLMessage_TextureRemove;
                data.GLTextureId = iiit->second;

#if PROFILE_TEXTURE_MANAGER
				data.SetTextureName( TextureName );
#endif	// _DEBUG

                m_LoadCompleteList[m_iCurrentListID].push_back( data );
                m_GLTexture.erase( iiit );
            }
        }
    }
}
#endif	// TEST_TMAP_TEXTUREMANAGER