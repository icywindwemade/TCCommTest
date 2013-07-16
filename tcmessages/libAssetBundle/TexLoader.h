#pragma once

#include <string>
#include "PNGLoader.h"

class CTexLoader : public CTextureLoader
{
public:
	CTexLoader();
	virtual ~CTexLoader();

	virtual unsigned char * Load( const std::string &lpFilename, CTexture * pNewTexture );
};

bool TexWrite( const char * szFilename, int w, int h, int colorbit, const BYTE *pBuf );