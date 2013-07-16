#ifndef _PNG__LOADER___
#define _PNG__LOADER___

#include "Core.h"

#if WIN32
#include <gl/GL.h>
#include "../../External/gl/gl3.h"
#include "../../External/gl/glext.h"
#elif ANDROID
#include <gles/gl.h>
#include <gles/glext.h>
#else	// IOS
#include <OpenGLES/ES2/gl.h>
#endif	// WIN32

#include <string>

extern UBOOL ConvertJpegToPng( const char* JpegFilePathname, const char* PngFilePathname = NULL );

class CTexture;

class CTextureLoader
{
public :
	virtual ~CTextureLoader()
	{
	}
	virtual unsigned char * Load( const std::string &lpFilename, CTexture * pNewTexture ) = 0;

};

class CPNGLoader : public CTextureLoader
{
public:
	CPNGLoader();
	virtual ~CPNGLoader();

	virtual unsigned char * Load( const std::string &lpFilename, CTexture * pNewTexture );

	static bool Convert( const std::string &lpFilename, const std::string &outFilename, int& OutSizeX, int& OutSizeY, int colorBit = 32);
};
#endif