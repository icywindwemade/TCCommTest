#ifndef _RESOURCE_TEXTURE__
#define _RESOURCE_TEXTURE__

#if WIN32
#include <gl/GL.h>
#include "../../External/gl/gl3.h"
#include "../../External/gl/glext.h"
#elif ANDROID
#include <GLES/gl.h>
#else	// IOS
#include <OpenGLES/ES2/gl.h>
#endif	// WIN32

#include <string>

class CTexture
{
public:
	CTexture() { m_iScale = 1; };
	~CTexture() {};

	GLuint GenerateEmptyTexture( int size[2], int format, int type );

	void GetSize( int &x, int &y ) const { x = SizeX; y = SizeY; }
	void GetSize( int size[2]) const { size[0] = SizeX; size[1] = SizeY; }
	int	 GetScale() const  { return m_iScale; };
	void SetScale(int iScale )	{ m_iScale = iScale; }

	int SizeX;
	int SizeY;
	int ColorBit;
	int Format;
	int Type;
	GLuint GetTextureID() const;
	int TextureIdx;
	int m_iScale;

	std::string	TextureName;

#if PROFILE_TEXTURE_MANAGER
	void SetTextureName( const std::string& InTextureName )
	{
		TextureName = InTextureName;
	}
#endif	// _DEBUG

};




#endif