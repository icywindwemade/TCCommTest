#include "IUDefine.h"

#include "Texture.h"
#include "IU.h"

#include "Core.h"
#include "TextureManager.h"


GLuint CTexture::GetTextureID() const
{
	return IUTextureManager().GetGLTexture( TextureIdx );
}


// Render Thread
GLuint CTexture::GenerateEmptyTexture( int size[2], int format, int type )
{
	SizeX = size[0];
	SizeY = size[1];

	GLuint TextureID;

	glGenTextures(1, &TextureID);

	glBindTexture(GL_TEXTURE_2D, TextureID);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	glTexImage2D(GL_TEXTURE_2D, 0, format, size[0], size[1], 0, format , type, NULL );
#if		PROFILE_TEXTURE_MANAGER
	char OutputString[1024];
	sprintf( OutputString, "Create Empty Texture: format %d type %d\n", format, type);
	OutputDebugString( OutputString );
#endif	// PROFILE_TEXTURE_MANAGER
	return TextureID;
}
