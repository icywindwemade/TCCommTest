#pragma once

#include "Core.h"

#if TEST_TMAP
#else
#include <map>
#endif	// FILEMANAGER_USING_TMAP

#include <string>

// Global variables
extern char GPackagePathname[];
extern char GDocumentPathname[];

/**
 * return File CRC in package zip file.
 */
extern unsigned long appGetPackageFileCrc( const char* InPackagePathname, const char* InPackageFilename );

extern INT appLoadCompressedFileFromPackage( const char* InFilename, char** OutBuffer );
extern INT appLoadCompressedFileFromDocuments( const char* InFilename, char** OutBuffer );
extern INT appLoadCompressedFile( const char* InFilename, char** OutBuffer );


#if ANDROID
struct FilenameAndPath
{
	std::string filename;
	bool archive;
};
#endif	// ANDROID

class CFileManager
{

public:

	CFileManager();
	~CFileManager();

	void Init();

	void Load();
	void AddFilePathname( const char* InFilename, const char* InPathname );

	int LoadFile( const char *filename, char** buf );
	int LoadDocFile( const char *filename, char** buf );
	int LoadBinFile( const char *filename, char** buf );
#if ANDROID
	void GetDocFilename( char *szFullFilename, const char * szFilename );
	void SetAppPath( std::string archivePath, std::string docPath )
	{ 
		m_ArchivePath = archivePath, m_DocPath = docPath;
		strcpy( DocPath, docPath.c_str() );
	}
	void GetFullFilename( char *szFullFilename, const char *szFilename, bool &bArchive );
#else	// WIN32 || IOS
	void GetFullFilename( char *szFullFilename, const char *szFilename );
#endif	// ANDROID
	void DeletePatchFiles();

public :
#if ANDROID
	std::map<std::string, FilenameAndPath> m_FileMap;
	std::string m_ArchivePath;
	std::string m_DocPath;
#else	// WIN32 || IOS
#if TEST_TMAP
	TMap<std::string, std::string>	Files;
#else
	std::map<std::string, std::string> m_FileMap;
#endif	// FILEMANAGER_USING_TMAP
#endif	// ANDROID

};