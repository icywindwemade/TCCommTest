#pragma once

#include "PNGLoader.h"

class CFileIO;

class CTGALoader : public CTextureLoader
{
public:
	CTGALoader();
	virtual ~CTGALoader();

	virtual unsigned char * Load( const std::string &lpFilename, CTexture * pNewTexture );
	GLuint Convert( const std::string &lpFilename, const std::string &outFilename, CTexture * pNewTexture );

private:
	void Load16( unsigned char *pBuff, int w, int h, CFileIO &f );
	void Load32( unsigned char *pBuff, int w, int h, CFileIO &f );
};