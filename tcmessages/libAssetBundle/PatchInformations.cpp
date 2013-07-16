#include "PatchInformations.h"

#include "zlib.h"

#include "XMLParser.h"
#include "FileIO.h"



CPatchUpdateInfo::CPatchUpdateInfo(const std::string &filename, int v1, int v2, int v3, const std::string &strDestPath )
{
	CXMLParser parser;
	if( parser.Open( filename.c_str() ) )
	{
		if( parser.FirstChildElement( "addedfile", true ) )
		{
			if( parser.FirstChildElement( "added", true ) )
			{
				do 
				{
					crcInfo fileInfo;
					char szFilename[256];		parser.GetAttributeString( "filename", szFilename, 256 );
					char szCRC[256];			parser.GetAttributeString( "crc", szCRC, 256 );
					fileInfo.Filename = szFilename;
					sscanf( szCRC, "%lu", &fileInfo.crc );
					m_OriginalList.push_back( fileInfo );
				} while( parser.NextSiblingElement( "added" ) );
				parser.GoParent();
			}
			parser.GoParent();
		}
	}

	m_Version[0] = v1;
	m_Version[1] = v2;
	m_Version[2] = v3;

	m_strDestPath = strDestPath;
	char path[MAX_PATH];
	sprintf( path, "%s", m_strDestPath.c_str() );			CreateDirectory( path, NULL );
	sprintf( path, "%s/Pack", m_strDestPath.c_str() );		CreateDirectory( path, NULL );					strcpy( m_szOriginalPath, path );
	sprintf( path, "%s/%d.%d.%d", m_strDestPath.c_str(), v1, v2, v3 );	CreateDirectory( path, NULL );		strcpy( m_szPatchPath, path );
}

CPatchUpdateInfo::~CPatchUpdateInfo()
{
}

bool CPatchUpdateInfo::CheckFile( const std::string &Filename )
{
	CFileIO f;
	if( !f.Open( Filename.c_str() ) )
		return false;
	uLong crc = crc32(0L, Z_NULL, 0);
	crc = crc32(crc, (const Bytef *)f.GetBuf(), f.GetLen());

	char filename[MAX_PATH];
	char ext[MAX_PATH];
	_splitpath(	Filename.c_str(), NULL, NULL, filename, ext );
	strcat( filename, ext );

	crcInfo info;
	info.crc = crc;
	info.Filename = filename;

	m_DestList.push_back( info );

	if(!IsSameWithkOriginal( Filename, crc ) )
	{
		m_PatchList.push_back( info );
		return true;
	}
	return false;
}
void CPatchUpdateInfo::SetConvertFilename( const std::string &Filename, const std::string &strConvertName )
{
	char filename[MAX_PATH];
	char ext[MAX_PATH];
	_splitpath(	Filename.c_str(), NULL, NULL, filename, ext );
	strcat( filename, ext );

	std::vector<crcInfo>::iterator it = m_PatchList.begin();
	while( it != m_PatchList.end() )
	{
		if( it->Filename == filename )
		{
			it->strConvertFilename = strConvertName;
			return;
		}
		it++;
	}
}

bool CPatchUpdateInfo::IsSameWithkOriginal( const std::string &Filename, unsigned long crc )
{
	char filename[MAX_PATH];
	char ext[MAX_PATH];
	_splitpath(	Filename.c_str(), NULL, NULL, filename, ext );
	strcat( filename, ext );

	std::vector<crcInfo>::iterator it = m_OriginalList.begin();
	while( it != m_OriginalList.end() )
	{
		if( (*it).Filename == filename )
		{
			if( (*it).crc == crc )
				return true;
			else
				return false;
		}
		it++;
	}
	return false;
}

const char* CPatchUpdateInfo::GetDestPath( const std::string &Filename )
{
	return m_szPatchPath;
}


void CPatchUpdateInfo::SaveInfo()
{

	{
		{
			CZipWriter fw;
			fw.Open( "../Patch/PatchInfo.dat" );

			char buffer[MAX_PATH];
			sprintf( buffer, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" );		fw.Write( buffer, strlen( buffer ) );
			sprintf( buffer, "\t<version curversion=\"%d %d %d\"/>\n", m_Version[0],m_Version[1],m_Version[2] );	fw.Write( buffer, strlen( buffer ) );
			sprintf( buffer, "<patchlist>\n" );										fw.Write( buffer, strlen( buffer ) );
			std::vector<crcInfo>::iterator it = m_PatchList.begin();
			while( it != m_PatchList.end() )
			{
				sprintf( buffer, "\t<added filename=\"%s\" crc=\"%lu\"/>\n",(*it).strConvertFilename.c_str(), (*it).crc );	
				fw.Write( buffer, strlen( buffer ) );
				it++;
			}
			sprintf( buffer, "</patchlist>\n" );									fw.Write( buffer, strlen( buffer ) );
		}

		{
			CFileWriter fw;
			fw.Open( "../Patch/PatchInfo.xml" );

			char buffer[MAX_PATH];
			sprintf( buffer, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" );		fw.Write( buffer, strlen( buffer ) );

			sprintf( buffer, "\t<version curversion=\"%d %d %d\"/>\n", m_Version[0],m_Version[1],m_Version[2] );	fw.Write( buffer, strlen( buffer ) );
			sprintf( buffer, "<patchlist>\n" );										fw.Write( buffer, strlen( buffer ) );
			std::vector<crcInfo>::iterator it = m_PatchList.begin();
			while( it != m_PatchList.end() )
			{
				sprintf( buffer, "\t<added filename=\"%s\" crc=\"%lu\"/>\n",(*it).strConvertFilename.c_str(), (*it).crc );	
				fw.Write( buffer, strlen( buffer ) );
				it++;
			}
			sprintf( buffer, "</patchlist>\n" );									fw.Write( buffer, strlen( buffer ) );
		}

		char szSrcFilename[MAX_PATH];
		char szDstFilename[MAX_PATH];
		sprintf( szDstFilename, "%s/%d.%d.%d/PatchInfo.xml", m_strDestPath.c_str(), m_Version[0], m_Version[1], m_Version[2] );
		CopyFile( "../Patch/PatchInfo.xml", szDstFilename, false );

		sprintf( szDstFilename, "%s/Pack%d.%d.%d/PatchInfo.dat", m_strDestPath.c_str(), m_Version[0], m_Version[1], m_Version[2] );
		CopyFile( "../Patch/PatchInfo.dat", szDstFilename, false );

		sprintf( szSrcFilename, "%s/%d.%d.%d/product.dat", m_strDestPath.c_str(), m_Version[0], m_Version[1], m_Version[2] );
		sprintf( szDstFilename, "%s/product.dat", m_strDestPath.c_str() );
		CopyFile( szSrcFilename, szDstFilename, false );

		sprintf( szSrcFilename, "%s/%d.%d.%d/gameData.dat", m_strDestPath.c_str(), m_Version[0], m_Version[1], m_Version[2] );
		sprintf( szDstFilename, "%s/gameData.dat", m_strDestPath.c_str() );
		CopyFile( szSrcFilename, szDstFilename, false );
	}
}


CFirstPatchInfo::CFirstPatchInfo(std::vector<std::string> &fileList, int v1, int v2, int v3, const std::string &strDestPath)
{
	std::vector<std::string>::iterator it = fileList.begin();
	while( it != fileList.end() )
	{
		m_OriginalList.push_back( *it );
		it++;
	}

	m_Version[0] = v1;
	m_Version[1] = v2;
	m_Version[2] = v3;

	m_strDestPath = strDestPath;
	char path[MAX_PATH];
	sprintf( path, "%s", m_strDestPath.c_str() );							CreateDirectory( path, NULL );
	sprintf( path, "%s/Pack%d.%d.%d", m_strDestPath.c_str(), v1, v2, v3 );	CreateDirectory( path, NULL );		strcpy( m_szOriginalPath, path );
	sprintf( path, "%s/%d.%d.%d", m_strDestPath.c_str(), v1, v2, v3 );		CreateDirectory( path, NULL );		strcpy( m_szPatchPath, path );
}

CFirstPatchInfo::~CFirstPatchInfo()
{
}

bool CFirstPatchInfo::CheckFile( const std::string &Filename )
{
	CFileIO f;
	if( !f.Open( Filename.c_str() ) )
		return false;
	uLong crc = crc32(0L, Z_NULL, 0);
	crc = crc32(crc, (const Bytef *)f.GetBuf(), f.GetLen());

	char filename[MAX_PATH];
	char ext[MAX_PATH];
	_splitpath(	Filename.c_str(), NULL, NULL, filename, ext );
	strcat( filename, ext );

	crcInfo info;
	info.crc = crc;
	info.Filename = filename;
	info.ConvertCrc = 0;

	m_DestList.push_back( info );

	return true;
}
void CFirstPatchInfo::SetConvertFilename( const std::string &Filename, const std::string &strConvertName )
{
	char filename[MAX_PATH];
	char ext[MAX_PATH];
	_splitpath(	Filename.c_str(), NULL, NULL, filename, ext );
	strcat( filename, ext );

	std::vector<crcInfo>::iterator it = m_PatchList.begin();
	while( it != m_PatchList.end() )
	{
		if( it->Filename == filename )
		{
			it->strConvertFilename = strConvertName;
			return;
		}
		it++;
	}
}

void CFirstPatchInfo::SetConvertCRC( const std::string &Filename, const std::string &strConvertName )
{
	char filename[MAX_PATH];
	char ext[MAX_PATH];
	_splitpath(	Filename.c_str(), NULL, NULL, filename, ext );
	strcat( filename, ext );

	std::vector<crcInfo>::iterator it = m_PatchList.begin();
	while( it != m_PatchList.end() )
	{
		if( it->Filename == filename )
		{
			CFileIO f;
			if( !f.Open( strConvertName.c_str() ) )
				return;
			uLong crc = crc32(0L, Z_NULL, 0);
			crc = crc32(crc, (const Bytef *)f.GetBuf(), f.GetLen());
			it->ConvertCrc = crc;
			return;
		}
		it++;
	}
}

bool CFirstPatchInfo::CheckOriginal( const std::string &Filename )
{
	return true;
	//char filename[MAX_PATH];
	//char ext[MAX_PATH];
	//_splitpath(	Filename.c_str(), NULL, NULL, filename, ext );
	//strcat( filename, ext );

	//std::vector<std::string>::iterator it = m_OriginalList.begin();
	//while( it != m_OriginalList.end() )
	//{
	//	if( *it == filename )
	//		return true;
	//	it++;
	//}
	//return false;
}

const char* CFirstPatchInfo::GetDestPath( const std::string &Filename )
{
	if( CheckOriginal( Filename ) )
		return m_szOriginalPath;
	return m_szPatchPath;
}


void CFirstPatchInfo::SaveInfo()
{
	char szFilename[MAX_PATH];
	{
		{
			CZipWriter fw;
			GetCurrentDirectory(MAX_PATH, szFilename);
			sprintf( szFilename, "%s/data.dat", m_szOriginalPath );
			fw.Open( szFilename );

			char buffer[MAX_PATH];
			sprintf( buffer, "VERSION" );									fw.Write( buffer );
			sprintf( buffer, "%d %d 0", m_Version[0], m_Version[1] );		fw.Write( buffer );
		}


		{
			CZipWriter fw;
			fw.Open( "../Patch/PatchInfo.dat" );

			char buffer[MAX_PATH];
			sprintf( buffer, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" );		fw.Write( buffer, strlen( buffer ) );
			sprintf( buffer, "\t<version curversion=\"%d %d %d\"/>\n", m_Version[0],m_Version[1],m_Version[2] );	fw.Write( buffer, strlen( buffer ) );
			sprintf( buffer, "<patchlist>\n" );										fw.Write( buffer, strlen( buffer ) );
			std::vector<crcInfo>::iterator it = m_PatchList.begin();
			while( it != m_PatchList.end() )
			{
				sprintf( buffer, "\t<added filename=\"%s\" crc=\"%lu\"/>\n",(*it).strConvertFilename.c_str(), (*it).ConvertCrc );	
				fw.Write( buffer, strlen( buffer ) );
				it++;
			}
			sprintf( buffer, "</patchlist>\n" );									fw.Write( buffer, strlen( buffer ) );
		}

		{
			CFileWriter fw;
			fw.Open( "../Patch/PatchInfo.xml" );

			char buffer[MAX_PATH];
			sprintf( buffer, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" );		fw.Write( buffer, strlen( buffer ) );

			sprintf( buffer, "\t<version curversion=\"%d %d %d\"/>\n", m_Version[0],m_Version[1],m_Version[2] );	fw.Write( buffer, strlen( buffer ) );
			sprintf( buffer, "<patchlist>\n" );										fw.Write( buffer, strlen( buffer ) );
			std::vector<crcInfo>::iterator it = m_PatchList.begin();
			while( it != m_PatchList.end() )
			{
				sprintf( buffer, "\t<added filename=\"%s\" crc=\"%lu\" convertcrc=\"%lu\"/>\n",(*it).strConvertFilename.c_str(), (*it).crc, (*it).ConvertCrc );	
				fw.Write( buffer, strlen( buffer ) );
				it++;
			}
			sprintf( buffer, "</patchlist>\n" );									fw.Write( buffer, strlen( buffer ) );
		}

		char szSrcFilename[MAX_PATH];
		char szDstFilename[MAX_PATH];
		sprintf( szDstFilename, "%s/%d.%d.%d/PatchInfo.xml", m_strDestPath.c_str(), m_Version[0], m_Version[1], m_Version[2] );
		CopyFile( "../Patch/PatchInfo.xml", szDstFilename, false );

		sprintf( szDstFilename, "%s/Pack%d.%d.%d/PatchInfo.dat", m_strDestPath.c_str(), m_Version[0], m_Version[1], m_Version[2] );
		CopyFile( "../Patch/PatchInfo.dat", szDstFilename, false );

		sprintf( szSrcFilename, "%s/Pack%d.%d.%d/product.dat", m_strDestPath.c_str(), m_Version[0], m_Version[1], m_Version[2] );
		sprintf( szDstFilename, "%s/product.dat", m_strDestPath.c_str() );
		CopyFile( szSrcFilename, szDstFilename, false );

		ProductDataCrc = GetFileCrc( szDstFilename );

		sprintf( szSrcFilename, "%s/Pack%d.%d.%d/gameData.dat", m_strDestPath.c_str(), m_Version[0], m_Version[1], m_Version[2] );
		sprintf( szDstFilename, "%s/gameData.dat", m_strDestPath.c_str() );
		CopyFile( szSrcFilename, szDstFilename, false );

		GameDataCrc = GetFileCrc( szDstFilename );
	}
}

