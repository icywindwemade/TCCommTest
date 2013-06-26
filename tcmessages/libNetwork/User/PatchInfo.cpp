#include "PatchInfo.h"
//#include "IU.h"
//#include "Client.h"
#include "UnionDefine.h"
#include "zlib.h"
//#include "FileIO.h"
//#include "LoaderThread.h"
//#include "MessageHandler.h"
//#include "PlatformHandler.h"

//#if IOS
//#include "rapidxml.hpp"
//#include "rapidxml_utils.hpp"
//    #import <Foundation/Foundation.h>
//#endif

#define DEBUG_PATCH	0

NPatchInfo* GPatchInfo=0;

NPatchInfo* NPatchInfo::Instance() 
{ 
	static NPatchInfo StaticInstance;
	return &StaticInstance; 
}

NPatchInfo::NPatchInfo()
	: OldBuildVersion( -1 )
	, NewBuildVersion( -1 )
	, bIsNeedAppUpdate( FALSE )
	, bIsNeedFirstPatch( TRUE )
	, bPatching( FALSE )
	, PatchListNum( 0 )
	, PatchListTotalNum( 0 )
{
    memset( DownloadURL, 0, 256 );
	OnCompletedCallback = 0;
}

NPatchInfo::~NPatchInfo()
{
	ResetAll();
}

void NPatchInfo::ResetAll()
{
	OldPatchFiles.Empty();
	NewPatchFiles.Empty();
}

/*
 bool NPatchInfo::Message( Telegram* InMsg )
{
	switch( InMsg->Msg )
	{
	case EngineMessage::DownloadFail:
		{
			return OnDownloadFail( InMsg );
		}
		break;

	case EngineMessage::Downloaded :
		{
			return OnDownloaded( InMsg );
		}
		break;
	}

	return false;
}

UBOOL NPatchInfo::ParseOldPatchInfo( std::string FilePathname )
{
	if ( FilePathname.empty() )
	{
		return FALSE;
	}
	
	OldVersionPathname = FilePathname;
	OldBuildVersion = ParsePatchInfo( FilePathname, OldPatchFiles );
	if ( OldBuildVersion > 0 )
	{
		return TRUE;
	}
	
	return FALSE;
}

UBOOL NPatchInfo::ParseNewPatchInfo( std::string FilePathname )
{
	if ( FilePathname.empty() )
	{
		return FALSE;
	}
	
	NewVersionPathname = FilePathname;
	NewBuildVersion = ParsePatchInfo( FilePathname, NewPatchFiles );
	if ( NewBuildVersion == 0 )
	{
		bIsNeedAppUpdate = TRUE;
	}
	else if ( NewBuildVersion > 0 )
	{
		return TRUE;
	}
	
	return FALSE;
}

INT NPatchInfo::ParsePatchInfo( std::string FilePathname, TArray<NPatchFileInfo>& OutPatchFiles )
{
	OutPatchFiles.Empty();
	INT BuildVersion = -1;

#if IOS
	if ( FilePathname.empty() )
	{
		return -1;
	}
	
	// Store character buffer.
	std::vector< char > xmlData;
	
	// Load XML file
	std::basic_ifstream<char> xmlFile( FilePathname.c_str()  );
	if ( !xmlFile.is_open() )
	{
		return -1;
	}
	xmlFile.seekg(0, std::ios::end);
	size_t size = (size_t)xmlFile.tellg();
	xmlFile.seekg(0); 
	
	xmlData.clear();
	xmlData.resize( size + 1);
	xmlData[size] = 0;
	
	xmlFile.read( &xmlData.front(), (std::streamsize)size );
	
	// XML Parsing
	rapidxml::xml_document< char > xmlDoc;
	xmlDoc.parse<0>( &xmlData.front() );
	
	char* Name;
	char* Value = 0;
	rapidxml::xml_node< char >* Item;
	rapidxml::xml_node< char >* SubItem;
	rapidxml::xml_attribute< char >* Attr;
	
	
	// PatchInfo
	rapidxml::xml_node< char >* root = xmlDoc.first_node();
	Name = root->name();
	if ( strcmp(Name, "PatchInfo") )
	{
		return -1;
	}
	
	// BuildVersion
	for ( Attr = root->first_attribute(); Attr; 
		 Attr = Attr->next_attribute() )
	{
		Name  = Attr->name();	// Attribute 의 이름
		Value = Attr->value();	// Attribute 의 값
	}
	
	if ( strcmp( Name, "BuildVersion" ) )
	{
		return -1;
	}
	
	if ( Value )
	{	
		BuildVersion = atoi(Value);
		if ( BuildVersion == 0 )
		{
			return 0;
		}
	}
	if ( BuildVersion < 0 )
	{
		return -1;
	}
	
	// Search sub nodes from root
	for ( Item = root->first_node(); Item; Item = Item->next_sibling())
	{
		// Node name
		
		Name = Item->name();
		if ( strcmp( Name, "Files" ) )
		{
			return -1;
		}
		
		// Search attributes in node
		for ( Attr = Item->first_attribute(); Attr; Attr = Attr->next_attribute() )
		{
			Name  = Attr->name();	// Attribute name
			Value = Attr->value();	// Attribute value
		}
		
		// Search child nodes from node
		for ( SubItem = Item->first_node(); SubItem; SubItem = SubItem->next_sibling() )
		{
			// File
			Name  = SubItem->name();	// Sub node name
			Value = SubItem->value();	// Sub node value
			if ( !strcmp( Name, "File" ) )
			{
				NPatchFileInfo FileInfo;
				for ( rapidxml::xml_attribute< char >* ChildAttr = SubItem->first_attribute();
					 ChildAttr; ChildAttr = ChildAttr->next_attribute() )
				{
					Name = ChildAttr->name();
					Value = ChildAttr->value();
					
					if ( !strcmp( Name, "State" ) )
					{
						if ( !strcmp( Value, "Modified" ) )
						{
							FileInfo.State = PFS_Modified;
						}
						else if ( !strcmp( Value, "Added" ) )
						{
							FileInfo.State = PFS_Added;	
						}
						else if ( !strcmp( Value, "Deleted") )
						{
							FileInfo.State = PFS_Deleted;								
						}
						else
						{
							// Invalid file state
							return -1;
						}
					}
					else if ( !strcmp( Name, "Filename" ) )
					{
						FileInfo.Filename = Value;
						if ( FileInfo.Filename.empty() )
						{
							// Invalid file name
							return -1;
						}
					}
					else if ( !strcmp( Name, "FileHash" ) )
					{
						FileInfo.HashStr = Value;
					}
					else
					{
						// Invalid xml
						return -1;
					}
				}
				if ( FileInfo.State != PFS_Deleted && FileInfo.State != PFS_None )
				{
					OutPatchFiles.AddItem( FileInfo );	
				}
			}
		}
	}
	
#if DEBUG_PATCH
	printf("\nBuildVersion: %d File Num: %d", BuildVersion, OutPatchFiles.Num() );
	for ( INT Index = 0; Index < OutPatchFiles.Num(); Index++ )
	{
		printf( "\nFile Information: \n\tState: %d\n\tFilename: %s\n\tHash: %s", OutPatchFiles(Index).State, OutPatchFiles(Index).Filename.c_str(), OutPatchFiles(Index).HashStr.c_str() );
	}
#endif	// DEBUG_PATCH_FILES

#endif // IOS
	
	return BuildVersion;
}

UBOOL NPatchInfo::FinishPatch()
{
#if IOS

	if ( NewPatchFiles.Num() > 0 )
	{
		NSString *fullpath = [[NSBundle mainBundle] bundlePath];
		NSFileManager *fm;
		NSString *path;
		NSDirectoryEnumerator *dirEnum;
		fm = [NSFileManager defaultManager];
		[fm changeCurrentDirectoryPath:fullpath];
		path = [fm currentDirectoryPath];
		dirEnum = [fm enumeratorAtPath: path];
		
		{
			NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
			NSString *documentsPath = [paths objectAtIndex:0];
			[fm changeCurrentDirectoryPath:documentsPath];
			path = [fm currentDirectoryPath];
			dirEnum = [fm enumeratorAtPath: path];
			
			// Remove old files.
			NSString* PatchVersionPathname = nil;
			NSString* OriginalVersionPatchPathname = nil;
			while(( path = [dirEnum nextObject] ) != nil )
			{
				NSString * Fullname = [documentsPath stringByAppendingPathComponent:path];
				NSString * Filename = [path lastPathComponent];
				
				NSString* OriginalFilename = [Filename stringByReplacingOccurrencesOfString:@"_New" withString:@""];
				NPatchFileInfo FoundFileInfo;
				UBOOL bIsPatchFile = FindPatchFileInfoByName( [OriginalFilename UTF8String], FoundFileInfo );
				if ( bIsPatchFile && [OriginalFilename isEqualToString:@"data.dat"] == NO )
				{
					NSString* OriginalFullname = [Fullname stringByReplacingOccurrencesOfString:@"_New" withString:@""];
					[fm moveItemAtPath:Fullname toPath:OriginalFullname error:NULL]; 
				}
				else if ( [ Filename isEqualToString:@"PatchInfo.xml_New" ] )
				{
					PatchVersionPathname = Fullname;
					NSString* OriginalFullname = [Fullname stringByReplacingOccurrencesOfString:@"_New" withString:@""];
					OriginalVersionPatchPathname = OriginalFullname;
				}
				
//				CFileIO  f;
//				f.Open( [OriginalFilename cStringUsingEncoding:NSUTF8StringEncoding ] );
//				uLong crc = crc32( 0L, Z_NULL, 0 );
//				crc = crc32( crc, (const Bytef * )f.GetBuf(), f.GetLen() );
//				
//				if( it->second == crc )
//					m_PatchFileList.erase( it );

			}
			if ( PatchVersionPathname != nil && OriginalVersionPatchPathname != nil )
			{
				[fm moveItemAtPath:PatchVersionPathname toPath:OriginalVersionPatchPathname error:nil];
			}
			else
			{
				return FALSE;
			}
		}
	}

#elif ANDROID


	LogPrintf( "StartPatch" );

	if( NewPatchFiles.Num() > 0 )
	{
		JniUtil::CalculateFilePathListFromExternalDirectory( IUGameManager().GetFileManager()->m_DocPath.c_str() );
		int iFileCount = JniUtil::GetCurCalculatedFilePathCount();

		char szFilePath[ MAX_PATH ];

		std::string PatchVersion = "";
		std::string OriginalVersion = "";


		for( int i = 0; i < iFileCount; ++i )
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

			char szOriginalFilename[MAX_PATH];

			strcpy( szOriginalFilename, pos );
			char *newPos = NULL;
			newPos = strstr( szOriginalFilename, "_New");
			if( newPos != NULL )
				*newPos = '\0';
			else
				continue;

			char szOriginalFullname[MAX_PATH];

			strcpy( szOriginalFullname, szFilePath );
			newPos = NULL;
			newPos = strstr( szOriginalFullname, "_New");
			if( newPos != NULL )
				*newPos = '\0';
			if( !strcmp( szOriginalFilename, "Version.dat" ) )
			{
				PatchVersion = szFilePath;
				OriginalVersion = szOriginalFullname;
			}
			else
			{
				JniUtil::RenameFile( szFilePath, szOriginalFullname );
				LogPrintf( "RenameVersion : %s, %s", szFilePath, szOriginalFullname );
			}
		}

		if( PatchVersion != "" && OriginalVersion != "" )
			JniUtil::RenameFile( PatchVersion.c_str(), OriginalVersion.c_str() );
		else 
			return FALSE;

		JniUtil::ClearCurFilePathList();
	}

	LogPrintf( "EndPatch" );

	return TRUE;

#endif	// IOS

	return TRUE;
}

UBOOL NPatchInfo::FindPatchFileInfoByName( const char* InFilename, NPatchFileInfo& OutPatchFileInfo )
{
	if ( !InFilename )
	{
		return FALSE;
	}
	
	for ( INT i = 0; i < NewPatchFiles.Num(); i++ )
	{
		if ( NewPatchFiles(i).Filename == InFilename )
		{
			OutPatchFileInfo = NewPatchFiles(i);
			return TRUE;
		}
	}
	
	return FALSE;
}

void NPatchInfo::RemoveExistingFiles()
{
#if IOS 

	NSString *fullpath = [[NSBundle mainBundle] bundlePath];
	NSFileManager *fm;
	NSString *path;
	NSDirectoryEnumerator *dirEnum;
	fm = [NSFileManager defaultManager];
	[fm changeCurrentDirectoryPath:fullpath];
	path = [fm currentDirectoryPath];
	dirEnum = [fm enumeratorAtPath: path];
	
	{
		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		NSString *documentsPath = [paths objectAtIndex:0];
		[fm changeCurrentDirectoryPath:documentsPath];
		path = [fm currentDirectoryPath];
		dirEnum = [fm enumeratorAtPath: path];
		
		// Remove old files.
		while(( path = [dirEnum nextObject] ) != nil )
		{
			NSString * Fullname = [documentsPath stringByAppendingPathComponent:path];
			NSString * Filename = [path lastPathComponent];
			if ( ![Filename isEqualToString:@"data.dat" ] )
			{
				[fm removeItemAtPath:Fullname error:NULL];
			}
		}
	}

#endif // IOS 
}

void NPatchInfo::ResetFirstPatch()
{
	bIsNeedFirstPatch = TRUE;
}

void NPatchInfo::StartPatch(  const char* InURL, PatchCallbackCompleted InCallback )
{
	OnCompletedCallback = InCallback;
	bPatching = TRUE;
    
    strcpy( DownloadURL, InURL );

#if ANDROID

	LogPrintf( "StartPatch : %s", DownloadURL );
	
	int iMarketType = ClientStoreType();
	if( ( iMarketType == (int)eUDGooglePlay ) ||
		( iMarketType == (int)eUDTStore ) )
	{	
		char SubDirectory[256];
		int MajorVer = -1;
		int MinorVer = -1;
		int BuildNum = -1;
		ClientVersion( MajorVer, MinorVer, BuildNum );
		sprintf( SubDirectory, "Patches/Google/%d/", MajorVer );

		CLoaderThread::Instance()->Download( InURL, SubDirectory, "PatchInfo.dat" );

		LogPrintf( "StartGooglePatch\0" );
	}
#else	// WIN32 || IOS

	CLoaderThread::Instance()->Download( InURL, "", "PatchInfo.xml" );

#endif	// ANDROID
}

void NPatchInfo::SkipPatch( PatchCallbackCompleted InCallback )
{
	bIsNeedFirstPatch = FALSE;
	OnCompletedCallback = InCallback;
	
	//OnPatchCompleted( EPRC_SuccessSkip );
	OnPatchCompleted( EPRC_Success );
}

bool NPatchInfo::OnDownloadFail( Telegram* InMessage )
{
	if ( !InMessage || !InMessage->GetExtraData() )
	{
		return false;
	}
	
#if IOS || WIN32
	FDownloadedInfo* DownloadedInfo = (FDownloadedInfo*)InMessage->GetExtraData();
	if ( DownloadedInfo->Type != EDT_PatchResource )
	{
		return false;
	}
		
	const char* FailedFilename = DownloadedInfo->Filename.c_str();
#else
	const char* FailedFilename = (char*)InMessage->GetExtraData();
#endif	// IOS

	if ( !strcmp( FailedFilename, "PatchInfo.xml" ) )
	{
		OnPatchCompleted( EPRC_InfoFailed );

		return TRUE;
	}

	OnPatchCompleted( EPRC_DownloadFailed );

	return TRUE;
}

bool NPatchInfo::OnDownloaded( Telegram* InMessage )
{
	if ( !InMessage || !InMessage->GetExtraData() )
	{
		return false;
	}

#if IOS || WIN32
	FDownloadedInfo* DownloadedInfo = (FDownloadedInfo*)InMessage->GetExtraData();
	if ( DownloadedInfo->Type != EDT_PatchResource )
	{
		return false;
	}

	const char* DownloadedFilename = DownloadedInfo->Filename.c_str();

#else

	const char* DownloadedFilename = (char*)InMessage->GetExtraData();

#endif	// IOS

	char PatchListFileName[64] = {0,};

#if ANDROID
	strcpy( PatchListFileName, "PatchInfo.dat" );
#else	// WIN32 || ANDROID
	strcpy( PatchListFileName, "PatchInfo.xml" );
#endif	// ANDROID

	if ( !strcmp( DownloadedFilename, PatchListFileName ) )
	{
		OnProcessPatchInfo();
	}
	else
	{
		OnProcessPatchFiles( DownloadedFilename );
	}

	return true;
}

void NPatchInfo::OnProcessPatchInfo()
{

#if ANDROID
	JniUtil::LOG( "Start NPatchInfo::OnProcessPatchInfo()\0" );
#endif	// ANDROID

	UBOOL bProcessed = ParsePatchInfo();
	if ( !bProcessed )
	{

#if ANDROID
		//IUGameManager().GetFileManager()->Load();
#endif	// ANDROID

		RemoveExistingFiles();

		OnPatchCompleted( EPRC_InvalidFiles );

		return;
	}
}

UBOOL NPatchInfo::ParsePatchInfo()
{
#if IOS
	// Parsing new patch info.
	char NewPathcInfoFilename[MAX_PATH];
	IUGetDocFileName( NewPathcInfoFilename, "PatchInfo.xml_New" );

	UBOOL bParsed = ParseNewPatchInfo( NewPathcInfoFilename );
	if ( !bParsed )
	{
		return FALSE;
	}

	char OldPathcInfoFilename[MAX_PATH];
	IUGetFullFileName( OldPathcInfoFilename, "PatchInfo.xml" );

	ParseOldPatchInfo( OldPathcInfoFilename );
	if ( IsNeedAppUpdate() )
	{
		ShowAlertMessage( EMBA_OK, EMCT_Update, IUGetLocalText( L"string_message_need_appupdate" ) );
		return FALSE;
	}

	if ( !IsEnabledPatch() )
	{
		return FALSE;
	}

	PatchListNum = 0;   
	if ( NewPatchFiles.Num() > 0 )
	{
		RemoveExistingFiles();

		PatchListTotalNum = PatchListNum = NewPatchFiles.Num();
		for ( INT i = 0; i < NewPatchFiles.Num(); i++ )
		{
			char SubDirectory[256];
			sprintf( SubDirectory, "Patches/%d/", GetNewBuildVersion() );
			CLoaderThread::Instance()->Download( DownloadURL, SubDirectory, NewPatchFiles(i).Filename );
		}
	}

#elif ANDROID

	
	LogPrintf( "Start NPatchInfo::ParsePatchInfo()\0" );

	char NewPathcInfoFilename[MAX_PATH];

	//	
	//IUGetDocFileName( NewPathcInfoFilename, "PatchInfo.dat_New" );
	strcpy( NewPathcInfoFilename, "PatchInfo.dat_New" );

	ParsePatchInfo( NewPathcInfoFilename );

	INT iMajor = 0, iMinor = 0, iRevision = 0;
	ClientVersion( iMajor, iMinor, iRevision );
	if( iMajor != m_Major )
	{
		RemoveExistingFiles();
	}

	

	LogPrintf( "CompareVersion %d and %d", this->m_Major, iMajor );


	int iStoreType = ClientStoreType();

	if( NewPatchFiles.Num() > 0 )
	{

		LogPrintf( "PatchFileCount : %d", NewPatchFiles.Num() );

		PatchListTotalNum = PatchListNum = NewPatchFiles.Num();
		for ( int i = 0; i < NewPatchFiles.Num(); i++ )
		{
			if( NewPatchFiles(i).Ver > iMinor )
			{
				char SubDirectory[256];				

				if( ( iStoreType == (int)eUDGooglePlay ) ||
					( iStoreType == (int)eUDTStore ) )
				{
					sprintf( SubDirectory, "Patches/Google/%d/%d/", iMajor, NewPatchFiles(i).Ver );
					//sprintf( SubDirectory, "Patches/Google/%d/%d/", iMajor, 1 );
				}
				else
				{
					sprintf( SubDirectory, "Patches/SK/%d/%d/", iMajor, NewPatchFiles(i).Ver );
				}
				CLoaderThread::Instance()->Download( DownloadURL, SubDirectory, NewPatchFiles(i).Filename );
			}
		}
	}

	LogPrintf( "End NPatchInfo::ParsePatchInfo()\0" );

#endif	// IOS

	return TRUE;
}

UBOOL NPatchInfo::ParsePatchInfo( std::string FilePathname )
{
#if ANDROID

	LogPrintf( "Start ParsePatchInfo : %s", FilePathname.c_str() );

	if ( FilePathname.empty() )
		return FALSE;

	char *pBuf = NULL;
	int size = IUGameManager().GetFileManager()->LoadDocFile( FilePathname.c_str(), &pBuf );

	LogPrintf( "LoadDocFilePath : %s", FilePathname.c_str() );
	LogPrintf( "Size : %d", size );

	CZipBufLoader *pLoader = new CZipBufLoader( pBuf, size );
	char *ptr = pLoader->GetBuf();

	CXMLParser parser;
	parser.OpenBuffer( ptr );

	LogPrintf( "OpenBuffer()\0" );

	if( this->NewPatchFiles.Num() > 0 )
	{
		this->NewPatchFiles.Empty();
	}

	if( parser.FirstChildElement( "PatchFiles", true ) )
	{

		LogPrintf( "Start First\0" );

		if( parser.FirstChildElement( "Version", true ) )
		{
			parser.GetAttributeInt( "major", m_Major);
			parser.GetAttributeInt( "minor", m_Minor );
			parser.GetAttributeInt( "revision", m_Revision );
			parser.GoParent();
		}

		LogPrintf( "Start Second\0" );

		if( parser.FirstChildElement( "File", true ) )
		{
			INT iMajor = 0, iMinor = 0, iRevision = 0;
			ClientVersion( iMajor, iMinor, iRevision );

			do {
				NPatchFileInfo info;
				char szFilename[256];
				parser.GetAttributeString( "filename", szFilename, 256 );	info.Filename = szFilename;
				parser.GetAttributeInt( "hash", info.Hash, 0 );
				parser.GetAttributeInt( "version", info.Ver, 0 );
								

				
				if( info.Ver > iMinor )
					NewPatchFiles.AddItem(info);
				else
					OldPatchFiles.AddItem(info);
				

			} 
			while( parser.NextSiblingElement( "File" ) );
			parser.GoParent();
		}
		parser.GoParent();
	}

	delete pLoader;

	LogPrintf( "End ParsePatchInfo()\0" );

#endif	// ANDROID

	return TRUE;
}

void NPatchInfo::OnProcessPatchFiles( const char* InDownloadedFilename )
{
#if IOS
	PatchListNum--;
	//	printf( "Donwloaded File: %s\n", InDownloadedFilename );
	//	IULOG( "PatchListNum : %d", PatchListNum );
	if( PatchListNum == 0 )
	{
		FinishPatch();

		IUGameManager().GetFileManager()->Load();
		IUGameManager().LoadVersion();

		bIsNeedFirstPatch = FALSE;

		OnPatchCompleted( EPRC_Success );
	}

#elif ANDROID
	PatchListNum--;
	if( PatchListNum == 0 )
	{
		FinishPatch();

		IUGameManager().GetFileManager()->Load();
		IUGameManager().LoadVersion();

		bIsNeedFirstPatch = FALSE;

		OnPatchCompleted( EPRC_Success );
	}

#endif	// IOS
}

void NPatchInfo::OnPatchCompleted( EPatchResultCode InCode )
{
	bPatching = FALSE;
	if( OnCompletedCallback ) OnCompletedCallback( InCode );
}
*/