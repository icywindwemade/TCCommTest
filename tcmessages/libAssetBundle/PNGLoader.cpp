

#include "IUDefine.h"
#include "PNGLoader.h"

#include "Texture.h"
#include <stdio.h>
#include "IU.h"
#include <math.h>
#include "png.h"
#include "FileIO.h"

#include "stdlib.h"

#if WIN32
#include <gl/GL.h>
#include "../../External/gl/gl3.h"
#include "../../External/gl/glext.h"
#elif ANDROID
#include <gles/gl.h>
#include <gles/glext.h>
#include <iostream>
#else	// IOS
#include <OpenGLES/ES1/gl.h>
//#import <Foundation/Foundation.h>
//#import <UIKit/UIKit.h>
#include <QuartzCore/QuartzCore.h>
#endif	// WIN32

#if ANDROID
#include <stdlib.h>
#include "FileIO.h"

void png_zip_read( png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) 
{	
	if ( png_get_io_ptr( png_ptr ) == NULL)
		return;

	CBufLoader *pLoader = (CBufLoader*)png_get_io_ptr( png_ptr );

	const size_t bytesRead = pLoader->Read( (char*)outBytes, (size_t)byteCountToRead);

	if((png_size_t)bytesRead != byteCountToRead)
		return;
}

#endif	// ANDROID

const int kMaxTextureSize = 2048;



CPNGLoader::CPNGLoader()
{
}

CPNGLoader::~CPNGLoader()
{
}

unsigned char* CPNGLoader::Load( const std::string &lpFilename, CTexture * pNewTexture )
{
#if WIN32	
	char buf[MAX_PATH];
	IUGetFullFileName( buf, lpFilename.c_str() );

	FILE *fp = fopen( buf, "rb" );

	if( !fp )
		return 0;	

	png_size_t number = 8;
	png_byte header[8];	// 8 is the maximum size that can be checked
	fread( header, 1, number, fp );

	int is_png = !png_sig_cmp( header, 0, number );
	if( !is_png )
		return 0;

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
		return 0;

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return 0;
	}

	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return 0;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return 0;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, number);

	png_read_info(png_ptr, info_ptr);
	
	png_uint_32 width;
	png_uint_32 height;
	int bit_depth;
	int color_type;

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

	if ( color_type == PNG_COLOR_TYPE_PALETTE )
		png_set_palette_to_rgb(png_ptr);

	if( color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8 )
		png_set_expand_gray_1_2_4_to_8( png_ptr );

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	if (bit_depth == 16)
		png_set_strip_16(png_ptr);

	if( bit_depth < 8 )
		png_set_packing( png_ptr );

	png_color_8p sig_bit;

	if (png_get_sBIT(png_ptr, info_ptr, &sig_bit))
		png_set_shift(png_ptr, sig_bit);

	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	if (bit_depth == 16)
		png_set_swap(png_ptr);

	if (bit_depth < 8)
		png_set_packswap(png_ptr);

	png_read_update_info( png_ptr, info_ptr );

	png_bytep *row_pointers;
	row_pointers = new png_bytep[height];

	UINT heightBackup = height;
	width = (UINT)(powf( 2, (float)((int)log2f( (float)width-1 )+1) ));
	height = (UINT)(powf( 2, (float)((int)log2f( (float)height -1 )+1) )); 

	unsigned char* pImageData = new unsigned char[height*width*4];
	memset(pImageData, 0, sizeof( unsigned char)* height*width*4);
	unsigned char* pImagePoint = pImageData;

	for (png_uint_32 i=0; i<heightBackup; ++i)
	{
		row_pointers[i]=pImagePoint;
		pImagePoint += width*4;
	}

	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, end_info);
	png_destroy_read_struct(&png_ptr,&info_ptr, &end_info);
	fclose(fp);
	delete [] row_pointers;

	pNewTexture->SizeX = width;
	pNewTexture->SizeY = height;
	pNewTexture->ColorBit = 32;	

	return pImageData;


#elif ANDROID
	char szName[1024];

	const char *pos;
	pos = strrchr( lpFilename.c_str(), '/' );
	if( pos == NULL )
	{
		pos = strrchr( lpFilename.c_str(), '\\' );
		if( pos == NULL )
			strcpy( szName, lpFilename.c_str() );
		else
			strcpy( szName, pos+1 );
	}
	else
		strcpy( szName, pos+1 );

	char *pBuffer;
	int size = IUGameManager().GetFileManager()->LoadFile( szName, &pBuffer );
	if( pBuffer == NULL )
		return 0;	

	CBufLoader *loader = new CBufLoader( pBuffer, size );
	
	do
	{
		png_size_t number = 8;
		png_byte header[8];	// 8 is the maximum size that can be checked
		loader->Read( (char*)header, number );

		int is_png = !png_sig_cmp( header, 0, number );
		if( !is_png )
			break;
	
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr)
			break;

		png_structp pStart = png_ptr;

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
			break;
		}

		png_infop end_info = png_create_info_struct(png_ptr);
		if (!end_info)
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			break;
		}
	
		png_ptr = pStart;	
		png_set_read_fn( png_ptr, loader, png_zip_read );		
		png_set_sig_bytes(png_ptr, 8);	
		png_read_info(png_ptr, info_ptr);


		png_uint_32 width = 0;
		png_uint_32 height = 0;
		int bitDepth = 0;
		int colorType = -1;
		png_uint_32 retval = png_get_IHDR(png_ptr, info_ptr,
										   &width,
										   &height,
										   &bitDepth,
										   &colorType,
										   NULL, NULL, NULL);
		
		if(retval != 1)
			break;
		
		if ( colorType == PNG_COLOR_TYPE_PALETTE )
			png_set_palette_to_rgb(png_ptr);
		
		if( colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8 )
			png_set_expand_gray_1_2_4_to_8( png_ptr );

		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			png_set_tRNS_to_alpha(png_ptr);

		if (bitDepth == 16)
			png_set_strip_16(png_ptr);

		if( bitDepth < 8 )
			png_set_packing( png_ptr );

		png_color_8p sig_bit;

		if (png_get_sBIT(png_ptr, info_ptr, &sig_bit))
			png_set_shift(png_ptr, sig_bit);

		if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(png_ptr);

		if (bitDepth == 16)
			png_set_swap(png_ptr);

		if (bitDepth < 8)
			png_set_packswap(png_ptr);

		png_read_update_info( png_ptr, info_ptr );


		png_bytep *row_pointers;
		row_pointers = new png_bytep[height];

		UINT heightBackup = height;
		width = (UINT)(powf( 2, (float)((int)log2f( (float)width-1 )+1) ));
		height = (UINT)(powf( 2, (float)((int)log2f( (float)height -1 )+1) )); 

		unsigned char* pImageData = new unsigned char[height*width*4];
		memset(pImageData, 0, sizeof( unsigned char)* height*width*4);
		unsigned char* pImagePoint = pImageData;

		for (png_uint_32 i=0; i<heightBackup; ++i)
		{
			row_pointers[i]=pImagePoint;
			pImagePoint += width*4;
		}
		

		png_read_image(png_ptr, row_pointers);
		png_read_end(png_ptr, end_info);
		png_destroy_read_struct( &png_ptr,&info_ptr, &end_info );
	
		//pNewTexture->ColorBit = bitDepth;
		pNewTexture->ColorBit = 32;
		pNewTexture->SizeX = width;
		pNewTexture->SizeY = height;

		if( loader != NULL )
			delete loader;
		delete[] row_pointers;

		return pImageData;


	} while( 0 );

	
	if( loader != NULL )
		delete loader;

	return 0;
#else	// IOS
    /*
	char buf[MAX_PATH];
	IUGetFullFileName( buf, lpFilename.c_str() );

    NSString* strFilename = [[NSString alloc] initWithCString:buf encoding:NSUTF8StringEncoding]; 
	UIImage* image = [[[UIImage alloc] initWithContentsOfFile:strFilename] autorelease];
	[strFilename release];
	
	if( image == nil )
		return NULL;

	CGAffineTransform	transform = CGAffineTransformIdentity;
	int width, height;

	CGImageRef				CGImage;	

	CGImage = image.CGImage;


	//glGenTextures(1, &glID);
	//glBindTexture(GL_TEXTURE_2D, glID);

	////	configure the image to use linear interpolation when increasing or decreasing
	////	the texture in size to fit on a polygon.
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//	get the width and height of the image
	int w,h;
	w = width = CGImageGetWidth(CGImage);
	h = height = CGImageGetHeight(CGImage);

	//	adjust the images width and height to be powers of 2
	if( (width != 1) && (width & (width - 1)) )
		width = (int)(powf( 2, (float)((int)log2f( (float)width-1 )+1) ));


	if( (height != 1) && (height & (height - 1)) ) 
		height = (int)(powf( 2, (float)((int)log2f( (float)height -1 )+1) )); 


	//	scale down an image greater than the max texture size
	while((width > kMaxTextureSize) || (height > kMaxTextureSize)) 
	{
		width /= 2;
		height /= 2;
		transform = CGAffineTransformScale(transform, 0.5, 0.5);
	}

	//	create a device dependant color space
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();

	//	get the bitmap data into RGBA format with the help of Core Graphics
	if (height * width * 4 <= 0 )
	{
		return NULL;
	}
	
	unsigned char *imageData = (unsigned char *)malloc( height * width * 4 );

	CGContextRef context = CGBitmapContextCreate( imageData, width, height, 8, 4 * width, colorSpace, 
		kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );

	CGColorSpaceRelease( colorSpace );
	CGContextClearRect( context, CGRectMake( 0, 0, width, height ) );
	CGContextTranslateCTM( context, 0, height - height );
	CGContextDrawImage( context, CGRectMake( 0, 0, width, height ), CGImage );
	CGContextRelease(context);	
		
	pNewTexture->ColorBit = 32;

	pNewTexture->SizeX = w;
	pNewTexture->SizeY = h;
     */
    unsigned char * imageData = NULL;
    printf("\n\nERROR: !!!!!!!!!!!!! PNGLoader.Load() IOS is not implemented !!!!!!!!!!!!!!!!! \n\n");
	return imageData;
#endif	// WIN32
}


bool CPNGLoader::Convert( const std::string &lpFilename, const std::string &outFilename, int& OutSizeX, int& OutSizeY, int colorBit )
{
	char buf[MAX_PATH];
	IUGetFullFileName( buf, lpFilename.c_str() );

	FILE* fp = fopen( buf, "rb" );
	if( !fp )
		return false;
	
	png_size_t number = 8;
	png_byte header[8];	// 8 is the maximum size that can be checked
	fread( header, 1, number, fp );

	int is_png = !png_sig_cmp( header, 0, number );
	if( !is_png )
		return false;

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
		return false;

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return false;
	}

	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return false;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return false;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, number);

	png_read_info(png_ptr, info_ptr);

	png_uint_32 width;
	png_uint_32 height;
	int bit_depth;
	int color_type;

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

	if ( color_type == PNG_COLOR_TYPE_PALETTE )
		png_set_palette_to_rgb(png_ptr);

	if( color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8 )
		png_set_expand_gray_1_2_4_to_8( png_ptr );

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	if (bit_depth == 16)
		png_set_strip_16(png_ptr);

	if( bit_depth < 8 )
		png_set_packing( png_ptr );

	png_color_8p sig_bit;

	if (png_get_sBIT(png_ptr, info_ptr, &sig_bit))
		png_set_shift(png_ptr, sig_bit);

	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	if (bit_depth == 16)
		png_set_swap(png_ptr);

	if (bit_depth < 8)
		png_set_packswap(png_ptr);

	png_read_update_info( png_ptr, info_ptr );

	png_bytep *row_pointers;
	row_pointers = new png_bytep[height];

	UINT heightBackup = height;
	width = (UINT)(powf( 2, (float)((int)log2f( (float)width-1 )+1) ));
	height = (UINT)(powf( 2, (float)((int)log2f( (float)height -1 )+1) )); 

	unsigned char* pImageData = new unsigned char[height*width*4];
	memset(pImageData, 0, sizeof( unsigned char)* height*width*4);
	unsigned char* pImagePoint = pImageData;

	int colorbit;
	if( color_type == PNG_COLOR_TYPE_RGB )
		colorbit = 3;
	else
		colorbit = 4;

	for (png_uint_32 i=0; i<heightBackup; ++i)
	{
		row_pointers[i]=pImagePoint;
		pImagePoint += width*colorbit;
	}

	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, end_info);
	png_destroy_read_struct(&png_ptr,&info_ptr, &end_info );
	fclose(fp);

	CZipWriter fw;
	fw.Open( outFilename.c_str() );

	if( colorBit == 16 )
	{
		unsigned char *image16bit = new unsigned char[height*width*2];
		for( UINT i = 0 ; i < height * width; i ++ )
		{
			image16bit[i*2] = (pImageData[i*4+2] & 0xf0) | ((pImageData[i*4+3] >> 4)&0x0f);
			image16bit[i*2+1] = (pImageData[i*4] & 0xf0) | ((pImageData[i*4+1] >> 4)&0x0f);
		}
		delete [] pImageData;
		pImageData = image16bit;
	}
	else if( colorBit == (0x80 | 16) )
	{
		unsigned char *image16bit = new unsigned char[height*width*2];
		for( UINT i = 0 ; i < height * width; i ++ )
		{
			// GL_RGBA   R8G8B8A8
			// GL_UNSIGNED_SHORT_5_6_5 B5(0x1f00) G6(0xe007) R5(0x00f8)
			image16bit[i*2] = ((pImageData[i*4+2] >>3) & 0x1f) | ((pImageData[i*4+1] << 3)&0xe0);
			image16bit[i*2+1] = ((pImageData[i*4+1] >> 5)&0x07) | ((pImageData[i*4] )&0xf8);
		}
		delete [] pImageData;
		pImageData = image16bit;
	}

	if( color_type == PNG_COLOR_TYPE_RGB )
	{
		int r = 0, g = 0, b = 0;
		unsigned char *image32bit = new unsigned char[height*width*4];
		for( UINT i = 0 ; i < height * width; i++ )
		{
			image32bit[i*4] = pImageData[i*3];
			image32bit[i*4+1] = pImageData[i*3+1];
			image32bit[i*4+2] = pImageData[i*3+2];
			image32bit[i*4+3] = 0xff;

			r = MAX( r, pImageData[i*3]);
			g = MAX( g, pImageData[i*3+1]);
			b = MAX( b, pImageData[i*3+2]);
		}

		delete [] pImageData;
		pImageData = image32bit;

		if( r < 32 && g < 64 && b < 32 )
		{
			for( UINT i = 0 ; i < height * width; i++ )
			{
				pImageData[i*4] = pImageData[i*4]<<3;
				pImageData[i*4+1] = pImageData[i*4+1]<<2;
				pImageData[i*4+2] = pImageData[i*4+2]<<3;
			}
		}
	}

	fw.Write( 9006 );
	fw.Write( (int)width );
	fw.Write( (int)height );
	fw.Write( (int)colorBit );
	fw.Write( pImageData, width * height * (colorBit&0x7f) / 8 );
    
    OutSizeX = (int)width;
    OutSizeY = (int)height;

	delete [] pImageData;
	return true;
}

UBOOL ConvertJpegToPng( const char* JpegFilePathname, const char* PngFilePathname )
{
	if ( !JpegFilePathname )
	{
		return FALSE;
	}
	
	if ( !PngFilePathname )
	{
		PngFilePathname = JpegFilePathname;
	}
	
#if IOS
	FILE* FileHandle = fopen( JpegFilePathname, "rb" );
	if( !FileHandle )
	{
		return FALSE;
	}
	
	fseek(FileHandle, 0, SEEK_END);
	long Length = ftell(FileHandle);
	fseek(FileHandle, 0, SEEK_SET);
	
	if ( Length <= 0 )
	{
		return FALSE;
	}
	
	char* Buffer = new char [Length];
	fread(Buffer, 1, Length, FileHandle);
	fclose(FileHandle);
	
	NSData* data = [NSData dataWithBytes:Buffer length:Length];
	UIImage* image = [UIImage imageWithData:data];
	NSString* filename = [NSString stringWithUTF8String:PngFilePathname];
	[UIImagePNGRepresentation(image) writeToFile:filename atomically:YES];
	
	delete [] Buffer;
	
	return TRUE;
#endif	// IOS
	
	return FALSE;

}