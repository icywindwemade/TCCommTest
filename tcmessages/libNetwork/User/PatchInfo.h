#ifndef __PATCHINFO_H__
#define __PATCHINFO_H__

#include "Core.h"
#include "IUDefine.h"
#include "Telegram.h"
//#include "GameObject.h"

enum EPatchResultCode
{
	EPRC_Success		= 0	,
	EPRC_SuccessSkip		,
	EPRC_DownloadFailed		,
	EPRC_InfoFailed			,
	EPRC_InvalidFiles		,
};

enum EPatchFileState
{
	PFS_None		= 0,
	PFS_Modified	= 1,
	PFS_Deleted		= 2,
	PFS_Added		= 3,
};

typedef	void (* PatchCallbackCompleted) ( int );

struct NPatchFileInfo
{
	std::string		Filename;
	EPatchFileState	State;
	std::string		HashStr;
	int 			Hash;
	int				Ver;
	
	NPatchFileInfo()
		: State( PFS_None )
	{
	}
};

class NPatchInfo
{
public:
	static NPatchInfo* Instance();

public:
	NPatchInfo();
	virtual ~NPatchInfo();

	void ResetAll();

	/*
     bool Message( Telegram* InMsg );

	UBOOL ParseOldPatchInfo( std::string FilePathname );
	UBOOL ParseNewPatchInfo( std::string FilePathname );
	
*/
    
	UBOOL IsEnabledPatch()
	{
		return ( OldBuildVersion < NewBuildVersion );
	}
	
	UBOOL IsNeedAppUpdate()
	{
		return bIsNeedAppUpdate;
	}

	UBOOL IsNeedFirstPatch()
	{
		return bIsNeedFirstPatch;
	}

	UBOOL IsPatching()
	{
		return bPatching;
	}
/*
	INT	GetNewBuildVersion()
	{
		return NewBuildVersion;
	}

	INT GetPatchListNum()
	{
		return PatchListNum;
	}

	INT GetPatchListTotalNum()
	{
		return PatchListTotalNum;
	}
	
	UBOOL FinishPatch();
	UBOOL FindPatchFileInfoByName( const char* InFilename, NPatchFileInfo& OutPatchFileInfo );
	
	void RemoveExistingFiles();

	// Added
	void ResetFirstPatch();
	void StartPatch( const char* InURL, PatchCallbackCompleted InCallback );
	void SkipPatch( PatchCallbackCompleted InCallback );
	bool OnDownloadFail( Telegram* InMessage );
	bool OnDownloaded( Telegram* InMessage );

	void OnProcessPatchInfo();
	UBOOL ParsePatchInfo();
	UBOOL ParsePatchInfo( std::string FilePathname );
	void OnProcessPatchFiles( const char* InDownloadedFilename );

	void OnPatchCompleted( EPatchResultCode InCode );
	
protected:
	INT ParsePatchInfo( std::string FilePathname, TArray<NPatchFileInfo>& OutPatchFiles ); */
	
private:
	INT		OldBuildVersion;
	INT		NewBuildVersion;
	UBOOL	bIsNeedAppUpdate;
	UBOOL	bIsNeedFirstPatch;
	UBOOL	bPatching;

	int		PatchListNum;
	int		PatchListTotalNum;
    
    char    DownloadURL[256];
	
	std::string OldVersionPathname;
	std::string NewVersionPathname;

	PatchCallbackCompleted OnCompletedCallback;
	
public:
	TArray<NPatchFileInfo>	OldPatchFiles;
	TArray<NPatchFileInfo>	NewPatchFiles;

	// for android
	int m_Major;
	int m_Minor;
	int m_Revision;
};

extern NPatchInfo* GPatchInfo;

#endif	// __PATCHINFO_H__
