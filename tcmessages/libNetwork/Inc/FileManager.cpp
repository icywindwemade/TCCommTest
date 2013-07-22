#include "Core.h"
#include "IUDefine.h"
#include "IU.h"

#include "FileManager.h"
#include <vector>
#include "AutoSync.h"
#include "FileIO.h"

#if WIN32 || ANDROID
#include "contrib/minizip/unzip.h"
#elif IOS
#include "unzip.h"
#endif	// WIN32 || ANDROID

//	Global variables.
char GPackagePathname[2048]	= {0,};
char GDocumentPathname[2048] = {0,};


unsigned long appGetPackageFileCrc( const char* InPackagePathname, const char* InPackageFilename )
{
#if !ANDROID
	return 0;
#endif	// !ANDROID

	unzFile ZipFileHandle = NULL;
	ZipFileHandle = cocos2d::unzOpen( InPackagePathname );
	if( ZipFileHandle == NULL )
	{
		return 0;
	}

	if( cocos2d::unzLocateFile(ZipFileHandle, InPackageFilename, 1) != UNZ_OK )
	{
		cocos2d::unzCloseCurrentFile(ZipFileHandle);
		return 0;
	}

	char szFilePathA[260];
	cocos2d::unz_file_info FileInfo;
	if ( unzGetCurrentFileInfo(ZipFileHandle, &FileInfo, szFilePathA, sizeof(szFilePathA), NULL, 0, NULL, 0) != UNZ_OK )
	{
		return 0;
	}

	if( ZipFileHandle != NULL )
	{
		cocos2d::unzCloseCurrentFile(ZipFileHandle);
	}

	return FileInfo.crc;
}

INT appLoadCompressedFileFromPackage( const char* InFilename, char** OutBuffer )
{
	if ( !InFilename )
	{
		return -1;
	}

	char FilePathnameInZip[MAX_PATH];
	sprintf( FilePathnameInZip, "assets/%s", InFilename );
	
	uLong uLen = 0;
	uLong uSize = 0;
	char* pBuff = NULL;
	int size = 0;
	char* ReadBuffer = NULL;

	INT Result = 0;
	int ZipErrorCode = UNZ_OK;
	unzFile ZipFileHandle = cocos2d::unzOpen( GPackagePathname );
	if ( !ZipFileHandle )
	{
		LogPrintf( "Can't open package.");
		Result = -1;

		goto HANDLE_ERROR;
	}

	if ( ( ZipErrorCode = cocos2d::unzLocateFile( ZipFileHandle, FilePathnameInZip, 1 ) ) != UNZ_OK )
	{
		LogPrintf( "Do not existing file in package: %s", FilePathnameInZip );
		Result = 0;

		goto HANDLE_ERROR;
	}

	char ZipFilePathname[MAX_PATH];
	cocos2d::unz_file_info FileInfo;
	if( ( ZipErrorCode = unzGetCurrentFileInfo( ZipFileHandle, &FileInfo, ZipFilePathname, MAX_PATH, NULL, 0, NULL, 0) ) != UNZ_OK )
	{
		LogPrintf( "Can not load file information: %s", FilePathnameInZip );
		Result = -3;

		goto HANDLE_ERROR;
	}

	if ( ( ZipErrorCode = cocos2d::unzOpenCurrentFile( ZipFileHandle ) ) != UNZ_OK )
	{
		LogPrintf( "Can not open file: %s", FilePathnameInZip );
		Result = -4;

		goto HANDLE_ERROR;
	}

	ReadBuffer = new char[ FileInfo.uncompressed_size ];
	if ( ( ( ZipErrorCode = cocos2d::unzReadCurrentFile( ZipFileHandle, ReadBuffer, FileInfo.uncompressed_size ) ) < 0 ) || ( ZipErrorCode != FileInfo.uncompressed_size ) )
	{
		LogPrintf( "Can not read file: %s %d", FilePathnameInZip, FileInfo.uncompressed_size );
		Result = -5;

		goto HANDLE_ERROR;
	}

	ZipErrorCode = UNZ_OK;
	Result = FileInfo.uncompressed_size;

	size = *(int*)ReadBuffer;
	pBuff = ReadBuffer + 4;

	*OutBuffer = new char[size];

	uLen = Result - sizeof(int);
	uSize = size;
	uncompress( (Bytef *)*OutBuffer, &uSize ,(Bytef *)pBuff, uLen);
	delete[] ReadBuffer;


HANDLE_ERROR:
	if ( ZipErrorCode != UNZ_OK )
	{
		LogPrintf( "Uncompressed Error Code: %d", ZipErrorCode );
	}

	if ( ZipFileHandle )
	{
		cocos2d::unzCloseCurrentFile( ZipFileHandle );
	}

	return Result;
}

INT appLoadCompressedFileFromDocuments( const char* InFilename, char** OutBuffer )
{
	char FilePathname[MAX_PATH];

	sprintf( FilePathname, "%s/%s", GDocumentPathname, InFilename );

	INT Result = 0;
	FILE* FileHandle = fopen( FilePathname, "rb" );
	if ( !FileHandle )
	{
		LogPrintf( "Can't open package: %s", FilePathname );
		return Result;
	}

//	INT Start = ftell( FileHandle );
	fseek( FileHandle, 0, SEEK_END );
	INT FileSize = ftell( FileHandle );
	if ( FileSize <= 0 )
	{
		LogPrintf( "Can not load file information: %s", FilePathname );
		fclose( FileHandle );
		return -3;
	}
	fseek( FileHandle, 0, SEEK_SET );

	char* ReadBuffer = new char[FileSize];
	size_t ReadSize = fread( ReadBuffer, 1, FileSize, FileHandle );
	if ( ReadSize != FileSize )
	{
		LogPrintf( "Can not read file: %s %d", FilePathname, ReadSize );
		fclose( FileHandle );
		return -5;
	}

	fclose( FileHandle );

	uLong uLen = 0;
	uLong uSize = 0;
	char* pBuff = NULL;
	int size = 0;

	size = *(int*)ReadBuffer;
	pBuff = ReadBuffer + 4;

	*OutBuffer = new char[size];

	uLen = Result - sizeof(int);
	uSize = size;
	uncompress( (Bytef *)*OutBuffer, &uSize ,(Bytef *)pBuff, uLen);
	delete[] ReadBuffer;

	return FileSize;
}

INT appLoadCompressedFile( const char* InFilename, char** OutBuffer )
{
	if ( !InFilename )
	{
		return -1;
	}

	INT Result = appLoadCompressedFileFromDocuments( InFilename, OutBuffer );
	if ( Result > 0 && *OutBuffer != NULL )
	{
		return Result;
	}

	return appLoadCompressedFileFromPackage( InFilename, OutBuffer );
}

//	CFileManager
IULock g_FileListLock;

CFileManager::CFileManager()
{
	InitLock( g_FileListLock );
}


CFileManager::~CFileManager()
{
    DelLock( g_FileListLock );

#if TEST_TMAP
	Files.Empty();
#else
	m_FileMap.clear();
#endif	// FILEMANAGER_USING_TMAP
}

void CFileManager::Init()
{
	Load();
}

CFileManager * gFileMgr = NULL;
CFileManager * CFileManager::sharedInstance() {
    if (gFileMgr == NULL) {
        gFileMgr = new CFileManager();
    }
    
    return gFileMgr;
}

void CFileManager::deleteInstance() {
    if( gFileMgr ) {
        delete gFileMgr;
        gFileMgr = NULL;
    }
}

void CFileManager::Load()
{
    CAutoSync lockVar( g_FileListLock );

#if TEST_TMAP
	Files.Empty();
#else
	m_FileMap.clear();
#endif	// FILEMANAGER_USING_TMAP

#if WIN32
	std::vector<std::string> PathList;
	std::vector<std::string> CurrentList;
	PathList.push_back( "." );
	CurrentList.push_back( "." );
	WIN32_FIND_DATA fd;
	HANDLE h;

	while( CurrentList.size() != 0 )
	{
		std::vector<std::string> NextList;

		std::vector<std::string>::iterator it = CurrentList.begin();
		while( it != CurrentList.end() )
		{
			char path[MAX_PATH];
			sprintf( path, "%s/*.*", (*it).c_str() );

			h = FindFirstFile( path, &fd);
			if (h != INVALID_HANDLE_VALUE)
			{
				do {
					if ( fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
					{
						if( fd.cFileName[0] != '.' )
						{
							if( !strcmp( (*it).c_str(), "." ))
							{
								PathList.push_back( fd.cFileName );
								NextList.push_back( fd.cFileName );
							}
							else
							{
								char FullPath[MAX_PATH];
								sprintf( FullPath, "%s/%s", (*it).c_str(), fd.cFileName );

								PathList.push_back( FullPath );
								NextList.push_back( FullPath );
							}
						}
					}

				}	while(FindNextFile(h, &fd) == TRUE);
				FindClose(h);
			}
			it++;
		}
		CurrentList.clear();
		CurrentList = NextList;
		NextList.clear();
	}

	std::vector<std::string>::iterator it = PathList.begin();
	while( it != PathList.end() )
	{
		char path[MAX_PATH];
		sprintf( path, "%s/*.*", (*it).c_str() );
		h = FindFirstFile( path, &fd);
		if (h != INVALID_HANDLE_VALUE)
		{
			do 
			{
				if( fd.cFileName[0] == '.' )
					continue;
				
				char filename[MAX_PATH];
				char ext[MAX_PATH];
				_splitpath(	fd.cFileName, NULL, NULL, filename, ext );

				char name[MAX_PATH];
				sprintf( name, "%s/%s", (*it).c_str(), fd.cFileName );

				const char *pos = strrchr(fd.cFileName, '/');
				if( pos == NULL )
					pos = fd.cFileName;

				if ( std::string(pos) == "ko.txt" || std::string(pos) == "UI_Title_Korea.xml")
				{
					int i = 0;
					i++;
				}

#if TEST_TMAP
				Files.Set( std::string( pos ), std::string( name ) );
#else
				m_FileMap[pos] = name;
#endif	// FILEMANAGER_USING_TMAP


			} while(FindNextFile(h, &fd) == TRUE);
			FindClose(h);
		}
		it++;
	}
	PathList.clear();
#elif ANDROID	
	unsigned char * pBuffer = NULL;
	unzFile pFile = NULL;
	pFile = unzOpen(m_ArchivePath.c_str());

	do
	{
		if( pFile == NULL )
			break;

		if( unzGoToFirstFile(pFile) != UNZ_OK )
			break;

		char szFilePath[260];
		do 
		{
			unz_file_info FileInfo;
			if( unzGetCurrentFileInfo(pFile, &FileInfo, szFilePath, sizeof(szFilePath), NULL, 0, NULL, 0) != UNZ_OK )
				break;

			if( strstr(szFilePath, "assets") == 0 )
				continue;

			const char *pos = strrchr(szFilePath, '/');
			if( pos != NULL )
				pos++;
			else
			{
				pos = strrchr(szFilePath, '\\');
				if( pos != NULL )
					pos++;
				else 
					pos = szFilePath;
			}
			if( pos[0] == 0 )
				continue;

			FilenameAndPath nameandpath;
			nameandpath.filename = szFilePath;
			nameandpath.archive = true;
			
			m_FileMap[pos] = nameandpath;
		} while ( unzGoToNextFile(pFile) == UNZ_OK );
	}while(0);

	if( pFile != NULL )
		unzCloseCurrentFile( pFile );


	JniUtil::CalculateFilePathListFromExternalDirectory( this->m_DocPath.c_str() );
	int iFileCount = JniUtil::GetCurCalculatedFilePathCount();

	char szFilePath[ MAX_PATH ];
	for( int i = 0;		i < iFileCount;		++i )
	{
		::memset( szFilePath, 0, sizeof( char ) * MAX_PATH );
		JniUtil::GetCurCalculatedFilePath( szFilePath, i );

		const char *pos = strrchr(szFilePath, '/');
		if( pos != NULL )
			pos++;
		else
		{
			pos = strrchr(szFilePath, '\\');
			if( pos != NULL )
				pos++;
			else 
				pos = szFilePath;
		}
		if( pos[0] == 0 )
			continue;

		FilenameAndPath nameandpath;
		nameandpath.filename = szFilePath;
		nameandpath.archive = false;

		m_FileMap[pos] = nameandpath;

		LogPrintf("=========FileMap %s ##Full:%s", pos, szFilePath );

	}
	
	JniUtil::ClearCurFilePathList();





#else	// iOS
    /*
    NSString *fullpath = [[NSBundle mainBundle] bundlePath];
    NSFileManager *fm;
    NSString *path;
    NSDirectoryEnumerator *dirEnum;
    fm = [NSFileManager defaultManager];
    [fm changeCurrentDirectoryPath:fullpath];
    path = [fm currentDirectoryPath];
    dirEnum = [fm enumeratorAtPath: path];

	while(( path = [dirEnum nextObject] ) != nil )
    {
        NSString * Fullname = [fullpath stringByAppendingPathComponent:path];
        NSString * Filename = [path lastPathComponent];
        m_FileMap[[Filename cStringUsingEncoding:NSUTF8StringEncoding ] ] = [Fullname cStringUsingEncoding:NSUTF8StringEncoding ];
    }

    {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsPath = [paths objectAtIndex:0];
        [fm changeCurrentDirectoryPath:documentsPath];
        path = [fm currentDirectoryPath];
        dirEnum = [fm enumeratorAtPath: path];
        
        while(( path = [dirEnum nextObject] ) != nil )
        {
            NSString * Fullname = [documentsPath stringByAppendingPathComponent:path];
            NSString * Filename = [path lastPathComponent];
            m_FileMap[[Filename cStringUsingEncoding:NSUTF8StringEncoding ] ] = [Fullname cStringUsingEncoding:NSUTF8StringEncoding ];
        }
    }
     */
    
#endif	// WIN32
}

#if ANDROID

void CFileManager::GetDocFilename( char *szFullFilename, const char * szFilename )
{
	sprintf( szFullFilename, "%s/%s",m_DocPath.c_str(), szFilename );
}

int CFileManager::LoadFile( const char *filename, char** pBuf )
{
	char fullFilename[MAX_PATH];
	bool bArchive;

    CAutoSync lockVar( g_FileListLock );
    EnterLock(g_FileListLock);
    
	std::map<std::string, FilenameAndPath>::iterator it = m_FileMap.find( filename );
	if( it != m_FileMap.end() )
	{
		strcpy( fullFilename, it->second.filename.c_str() );
		bArchive = it->second.archive;
	}
	else
	{
		return 0;
	}
	lock.Leave();

	int size = 0;

	if( bArchive )
	{
		unzFile pFile = NULL;
		do
		{
			pFile = unzOpen( m_ArchivePath.c_str() );

			if( pFile == NULL )
				break;

			if( unzLocateFile(pFile, fullFilename, 1) != UNZ_OK )
				break;

			char szFilePathA[260];
			unz_file_info FileInfo;
			if( unzGetCurrentFileInfo(pFile, &FileInfo, szFilePathA, sizeof(szFilePathA), NULL, 0, NULL, 0) != UNZ_OK )
				break;

			if( unzOpenCurrentFile(pFile) != UNZ_OK )
				break;

			*pBuf = new char[FileInfo.uncompressed_size];
			unzReadCurrentFile(pFile, *pBuf, FileInfo.uncompressed_size);
			unzCloseCurrentFile(pFile);
			unzClose( pFile );
			return FileInfo.uncompressed_size;
		} while( 0 );

		if( pFile != NULL )
		{
			unzCloseCurrentFile(pFile);
			unzClose( pFile );
		}
		return 0;
	}
	else
	{
		CFileIO f;

		f.Open( fullFilename );
		*pBuf = new char[f.GetLen() + 1];
		f.Read( *pBuf, f.GetLen() );
		(*pBuf)[f.GetLen()] = NULL;
		return f.GetLen();
	}
	return 0;
}

int CFileManager::LoadDocFile( const char *filename, char** pBuf )
{
	char fullFilename[MAX_PATH];

	sprintf( fullFilename, "%s/%s",m_DocPath.c_str(), filename );

	CFileIO f;

	if( f.OpenCurrentFile( fullFilename ) == false )
	{
		return 0;
	}

	*pBuf = new char[f.GetLen() + 1];
	f.Read( *pBuf, f.GetLen() );
	(*pBuf)[f.GetLen()] = NULL;

	return f.GetLen();
}

int CFileManager::LoadBinFile( const char *filename, char** pBuf )
{
	char fullFilename[MAX_PATH];
	char pureFilename[MAX_PATH];
	char szFileName[64];
	char szExtName[64];
	
	strcpy( szFileName, filename );

	unzFile pFile = NULL;
	do
	{
		pFile = unzOpen( m_ArchivePath.c_str() );

		if( pFile == NULL )
		{
			LogPrintf(" LoadBinFile error: %s", m_ArchivePath.c_str());
			break;
		}

		if( unzLocateFile(pFile, szFileName, 1) != UNZ_OK )
		{
			LogPrintf(" LoadBinFile locate file error: %s", szFileName);
			break;
		}

		char szFilePathA[260];
		unz_file_info FileInfo;
		if( unzGetCurrentFileInfo(pFile, &FileInfo, szFilePathA, sizeof(szFilePathA), NULL, 0, NULL, 0) != UNZ_OK )
			break;

		if( unzOpenCurrentFile(pFile) != UNZ_OK )
			break;

		*pBuf = new char[FileInfo.uncompressed_size];
		unzReadCurrentFile(pFile, *pBuf, FileInfo.uncompressed_size);
		unzCloseCurrentFile(pFile);
		return FileInfo.uncompressed_size;
	} while( 0 );

	if( pFile != NULL )
		unzCloseCurrentFile(pFile);
	return 0;
}


void CFileManager::GetFullFilename( char *szFullFilename, const char *szFilename, bool &bArchive )
{
    CAutoSync lockVar( g_FileListLock );
    EnterLock( g_FileListLock );
	std::map<std::string, FilenameAndPath>::iterator it = m_FileMap.find( szFilename );
	if( it != m_FileMap.end() )
	{
		strcpy( szFullFilename, it->second.filename.c_str() );
		bArchive = it->second.archive;
	}
	else
		szFullFilename[0] = NULL;
}
#else	// WIN32 || IOS


int CFileManager::LoadFile( const char *filename, char** pBuf  )
{
	if ( !GIsCooking )
	{
		char fullFilename[MAX_PATH];

#if TEST_TMAP
		
		std::string* FullPathname = Files.Find( std::string(filename) );
		if ( !FullPathname )
		{
			OutputDebugString("\t -- LoadFile: ");
			OutputDebugString(filename);
			OutputDebugString("\n");
			return NULL;
		}

		strcpy( fullFilename, FullPathname->c_str() );

#else
		std::map<std::string, std::string>::iterator it = m_FileMap.find( filename );
		if( it != m_FileMap.end() )
		{
			strcpy( fullFilename, it->second.c_str() );
		}
		else
			return NULL;
#endif	// FILEMANAGER_USING_TMAP

		int len;
		FILE *fp;
		fp = fopen(fullFilename, "rb");
		if (fp == NULL)
			return NULL;
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		*pBuf = new char [len+1];
		fread(*pBuf, 1, len, fp);
		fclose(fp);
		(*pBuf)[len] = '\0';
		return len;
	}
	else
	{
		int len;
		FILE *fp;
		fp = fopen(filename, "rb");
		if (fp == NULL)
			return NULL;
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		*pBuf = new char [len+1];
		fread(*pBuf, 1, len, fp);
		fclose(fp);
		(*pBuf)[len] = '\0';
		return len;
	}
}

void CFileManager::GetFullFilename( char *szFullFilename, const char *szFilename )
{
#if TEST_TMAP
	szFullFilename[0] = NULL;

	std::string* FullPathname = Files.Find( std::string(szFilename) );
	if ( FullPathname )
	{
		strcpy( szFullFilename, FullPathname->c_str() );
	}

#else
	std::map<std::string, std::string>::iterator it = m_FileMap.find( szFilename );
	if( it != m_FileMap.end() )
	{
		strcpy( szFullFilename, it->second.c_str() );
	}
	else
		szFullFilename[0] = NULL;
#endif	// FILEMANAGER_USING_TMAP
}

#endif	// ANDROID

void CFileManager::DeletePatchFiles()
{
#if ANDROID

	JniUtil::RemoveExistingFiles( IUGameManager().GetFileManager()->m_DocPath.c_str() );

#elif IOS 
    /*
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsPath = [paths objectAtIndex:0];

	NSFileManager *fm;
	NSString *path;
	NSDirectoryEnumerator *dirEnum;
	fm = [NSFileManager defaultManager];
	[fm changeCurrentDirectoryPath:documentsPath];
	path = [fm currentDirectoryPath];
	dirEnum = [fm enumeratorAtPath: path];

	while(( path = [dirEnum nextObject] ) != nil )
	{
		NSString * Fullname = [documentsPath stringByAppendingPathComponent:path];
		NSString * Filename = [path lastPathComponent];

		if ( ![Filename isEqualToString:@"data.dat" ] &&
			![Filename isEqualToString:@"UserData.dat"] )
		{
			[fm removeItemAtPath:Fullname error:NULL];
		}
	} */
#endif	// ANDROID
}

void CFileManager::AddFilePathname( const char* InFilename, const char* InPathname )
{
	if ( !InFilename || !InPathname )
	{
		return;
	}

#if ANDROID
	FilenameAndPath NewFilename;
	NewFilename.filename = InPathname;
	NewFilename.archive = false;

	m_FileMap[InFilename] = NewFilename;
#else	// WIN32 || IOS
	m_FileMap[InFilename] = InPathname;
#endif	// ANDROID
}
