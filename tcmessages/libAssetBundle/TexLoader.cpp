
#include "IUDefine.h"
#include "TexLoader.h"

#include "Texture.h"
#include <stdio.h>
#include "IU.h"
#include <math.h>
#include "png.h"
#include "FileIO.h"

const int kMaxTextureSize = 1024;

CTexLoader::CTexLoader()
{
}

CTexLoader::~CTexLoader()
{
}

#if ANDROID
unsigned char* CTexLoader::Load( const std::string &lpFilename, CTexture * pNewTexture )
{

	char *pBuf = NULL;
	int len = IUGameManager().GetFileManager()->LoadFile( lpFilename.c_str(), &pBuf );

	CZipBufLoader *pLoader = new CZipBufLoader( pBuf, len );

	int width,height,colorbit;
	pLoader->ReadInt( width );
	pLoader->ReadInt( width );
	pLoader->ReadInt( height );
	pLoader->ReadInt( colorbit );
	char* pImageData = new char[height*width*(colorbit&0x7f)/8];
	pLoader->Read( pImageData, height*width*(colorbit&0x7f)/8 );

	pNewTexture->SizeX = width;
	pNewTexture->SizeY = height;
	pNewTexture->ColorBit = colorbit;

	delete pLoader;

	return (unsigned char * )pImageData;
}

#else	// WIN32 || IOS

unsigned char * CTexLoader::Load( const std::string &lpFilename, CTexture * pNewTexture )
{
	char buf[MAX_PATH];
	IUGetFullFileName( buf, lpFilename.c_str() );

	CZipLoader loader;
	loader.Open( buf );
	int width,height,colorbit;
	loader.ReadInt( width );
	loader.ReadInt( width );
	loader.ReadInt( height );
	loader.ReadInt( colorbit );
	char* pImageData = new char[height*width*(colorbit&0x7f)/8];
	loader.Read( pImageData, height*width*(colorbit&0x7f)/8 );

	pNewTexture->SizeX = width;
	pNewTexture->SizeY = height;
	pNewTexture->ColorBit = colorbit;

	return (unsigned char * )pImageData;
}

#endif	// ANDROID



bool TexWrite( const char * szFilename, int w, int h, int colorbit, const BYTE *pBuf )
{
	char szDocFilename[MAX_PATH];
	IUGetDocFileName( szDocFilename, szFilename );

	CZipWriter fw;
	fw.Open( szDocFilename );

	fw.Write( 9006 );
	fw.Write( w );
	fw.Write( h );
	fw.Write( colorbit );
	fw.Write( pBuf, w * h * (colorbit&0x7f) / 8 );

	return true;
}