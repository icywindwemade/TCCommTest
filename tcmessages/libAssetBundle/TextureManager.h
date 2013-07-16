#ifndef __TEXTURE_MANAGER__
#define __TEXTURE_MANAGER__

#define PROFILE_TEXTURE_MANAGER		0

#include "Core.h"

#if TEST_TMAP
#else
#include <map>
#endif	// TEST_TMAP

#include <string>
#include <vector>

#if WIN32
#include <gl/GL.h>
#include "../../External/gl/gl3.h"
#include "../../External/gl/glext.h"
#elif ANDROID
#include <GLES/gl.h>
#else	// IOS
#include <OpenGLES/ES1/gl.h>
#endif	// WIN32

class CTexture;

enum EGLMessageType
{
	EGLMessage_TextureLoad,
	EGLMessage_TextureEmpty,
	EGLMessage_TextureSub,
	EGLMessage_TextureRemove,
	EGLMessage_MapLoaded,
	EGLMessage_ShowShadow,
	EGLMessage_HideShadow,
};

struct FGLTextureMessage
{
	CTexture *pTexture;
	unsigned char * pBuffer;
	GLuint GLTextureId;
	int MessageType;
	int iPos[2];
	int iSize[2];
	std::string	TextureName;
	
#if PROFILE_TEXTURE_MANAGER
	void SetTextureName( const std::string& InTextureName )
	{
		TextureName = InTextureName;
	}
#endif	// _DEBUG
};

class CTextureManager
{
public:
	CTextureManager();
	~CTextureManager();
	int LoadTexture( const std::string &lpFilename, bool bBackground = true );
	int GetTextureNum()
	{ 
#if TEST_TMAP
		return GLTextures.Num();
#else
		return m_GLTexture.size();
#endif	// TEST_TMAP_TEXTUREMANAGER
	}
	int CreateEmptyTexture(int size[2], int format = GL_RGBA, int type = GL_UNSIGNED_BYTE );
	void UpdateSubTexture( int iid, unsigned char * pBuffer, int pos[], int size[] );
	const CTexture *GetTexture( int iid );
	const std::string &GetTextureName( int iid );
	GLuint GetGLTexture( int iid );
	void RemoveTexture( int iid );
	void PushMessage( int iMessage );

	void Update();
	void LoadComplete( CTexture *pTexture, unsigned char *pBuffer );

	void Empty();

private:
	void UpdateTexture( CTexture *pTexture, unsigned char * pBuffer );

#if TEST_TMAP
	TMap<std::string, INT>	TextureMap;
	TMap<INT, CTexture*>	TextureList;
	TMap<INT, GLuint>		GLTextures;
#else
	std::map<std::string, int > m_TextureMap;
	std::map<int, CTexture *> m_TextureList;
	std::map< int,GLuint>	 m_GLTexture;
#endif		// TEST_TMAP_TEXTUREMANAGER

	int m_TextureNum;

	std::vector<FGLTextureMessage> m_LoadCompleteList[2];
	int m_iCurrentListID;
	int m_iBackListID;
	std::string m_Empty;

	UINT		NumOfTextures;
};

#endif