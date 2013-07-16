
#include "IUDefine.h"
#include "TGALoader.h"

#include "Texture.h"
#include <stdio.h>
#include "IU.h"
#include <math.h>
#include "png.h"
#include "FileIO.h"

#if WIN32
#include <gl/GL.h>
#include "../../External/gl/gl3.h"
#include "../../External/gl/glext.h"
#elif ANDROID
#include <gles/gl.h>
#include <gles/glext.h>
#else	// IOS
#include <OpenGLES/ES1/gl.h>
//#import <Foundation/Foundation.h>
//#import <UIKit/UIKit.h>
#include <QuartzCore/QuartzCore.h>
#endif	// WIN32

#if WIN32
#define GL_UNSIGNED_BYTE_3_3_2            0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4         0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1         0x8034
#define GL_UNSIGNED_INT_8_8_8_8           0x8035
#define GL_UNSIGNED_INT_10_10_10_2        0x8036
#define GL_RESCALE_NORMAL                 0x803A
#define GL_UNSIGNED_BYTE_2_3_3_REV        0x8362
#define GL_UNSIGNED_SHORT_5_6_5           0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV       0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4_REV     0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV     0x8366
#define GL_UNSIGNED_INT_8_8_8_8_REV       0x8367
#endif	// WIN32
const int kMaxTextureSize = 1024;


CTGALoader::CTGALoader()
{
}

CTGALoader::~CTGALoader()
{
}

void CTGALoader::Load16( unsigned char *pBuff, int w, int h, CFileIO &f )
{
	for( int i = 0; i < h; i++ )
	{
		short* pImage = (short *)(&pBuff[ h*w*2 - i*w*2-w*2] );
		for( int j = 0; j < w; j++ )
		{
			short color = (short)f.ReadShort();
			*pImage = color<<1;
			pImage++;
		}
	}
}

void CTGALoader::Load32( unsigned char *pBuff, int w, int h, CFileIO &f )
{
	for( int i = 0; i < h; i++ )
	{
		char* pImage = (char *)(&pBuff[ h*w*4 - i*w*4-w*4] );
		for( int j = 0; j < w; j++ )
		{
			pImage[2] = f.ReadChar();
			pImage[1] = f.ReadChar();
			pImage[0] = f.ReadChar();
			pImage[3] = f.ReadChar();
			pImage +=4;
		}
	}
}


unsigned char* CTGALoader::Load( const std::string &lpFilename, CTexture * pNewTexture )
{
	//GLuint glID = -1;
	//CFileIO f;
	//if( !f.Open( lpFilename ) )
	//	return -1;

	//int len = f.ReadChar();
	//int colorMapType = f.ReadChar();
	//int rle = f.ReadChar();
	//int MapStart = f.ReadShort();
	//int MapLenghth = f.ReadShort();
	//int MapDepth = f.ReadChar();
	//int xOffset = f.ReadShort();
	//int yOffset = f.ReadShort();
	//
	//int width, height;
	//width = f.ReadShort();
	//height = f.ReadShort();
	//char colorbit = f.ReadChar();
	//f.ReadChar();

	//unsigned char* pImageData = NULL;

	//if( colorbit == 16 )
	//	pImageData = new unsigned char[height*width*2];
	//else if( colorbit == 32 )
	//	pImageData = new unsigned char[height*width*4];

	//if( rle == 0x02 )
	//{
	//	if( colorbit == 16 )
	//		Load16( pImageData, width, height, f );
	//	else if( colorbit == 32 )
	//		Load32( pImageData, width, height, f );
	//}
	//else if( rle == 0x0a )
	//{
	//	// ±ÍÂú¾Æ¼­ ¾ÈÇÔ.
	//	SAFE_DELETE( pImageData );
	//	return -1;
	//}
	//else
	//{
	//	SAFE_DELETE( pImageData );
	//	return -1;
	//}


	//*pNewTexture = new CTexture();
	//(*pNewTexture)->SizeX = width;
	//(*pNewTexture)->SizeY = height;

	//glGenTextures(1, &glID);

	//glBindTexture(GL_TEXTURE_2D, glID);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//int error = glGetError();
	//switch(colorbit)
	//{
	//case 16:
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, pImageData);
	//	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, pImageData);
	//	break;
	//case 32 :
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImageData);
	//	break;
	//}

	//error = glGetError();
	//delete [] pImageData;

	//return glID;
	return NULL;
}


GLuint CTGALoader::Convert( const std::string &lpFilename, const std::string &outFilename, CTexture * pNewTexture )
{
	GLuint glID = -1;
	return glID;
}