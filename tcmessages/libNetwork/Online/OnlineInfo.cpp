//##############################################################################################
// OnlineInfo.cpp
//##############################################################################################
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "OnlineInfo.h"

//##############################################################################################
// GOnlineInfo
//##############################################################################################
NOnlineInfo* GOnlineInfo=0;

//##############################################################################################
// NOnlineInfo
//##############################################################################################
NOnlineInfo* NOnlineInfo::Instance() 
{ 
	static NOnlineInfo StaticInstance;
	return &StaticInstance; 
}

NOnlineInfo::NOnlineInfo()
{
	ResetAll();
}

NOnlineInfo::~NOnlineInfo()
{
	ResetAll();
}

void NOnlineInfo::ResetAll()
{
	ResetAccount();
	ResetDevice();
	ResetChannels();
	ResetChannelMigration();
	ResetOnline();
	ResetSceneMode();
	ResetMatch();
	ResetURLs();
}

void NOnlineInfo::ResetAccount()
{
	// Account
	
	ConnectionID = -1;
	UserKey = 0;
	memset( UserNick, 0, UD_NICK_LEN );    

	AuthID = 0;
	AuthToken = 0;
	
	PlatformCode = 0;
	PlatformUserKey = 0;
	memset( PlatformUserID, 0, UD_ID_LEN );
	memset( PlatformUserPW, 0, UD_PWD_LEN );
	memset( PlatformUserNick, 0, UD_NICK_LEN );
	memset( PlatformUserEMail, 0, UD_EMAIL_LEN );
	memset( PlatformUserToken, 0, UD_TOKEN_LEN );
	memset( PlatformUserName, 0, UD_NICK_LEN );
	memset( PlatformUserImgURL, 0, 256 );
   	memset( PlatformUserImgPath, 0, 256 );
	PlatformUserMessageBlocked = false;
    
	bLogin = false;
	bKickout = false;
	bLoaded = false;
	

	bNicknameRegisterFailed = false;
    bNoticeEvent = false;
	bAllowMessage = false;

	bGuest = false;
	GuestOption = 0;

	EventID = 0;
	EventID2 = 0;
}

void NOnlineInfo::DidLogin( int InConnID, long long InUserKey, long long InAuthID, int InAuthToken )
{
	bLogin = true;
	bPurchasing = false;
	bOffline = false;

	ConnectionID = InConnID;
	UserKey	= InUserKey;
    AuthID = InAuthID;
	AuthToken = InAuthToken;

	DidKickin();

	OfflineTime = 0;

	bNicknameRegisterFailed = false;
}

void NOnlineInfo::DidLogout()
{
	bLogin = false;
}

bool NOnlineInfo::IsLogin()
{
	return bLogin;
}

bool NOnlineInfo::CanRelogin()
{
	return ( UserKey > 0 );
}

void NOnlineInfo::DidKickin()
{
	bKickout = false;
}

void NOnlineInfo::DidKickout()
{
	bKickout = true;
}

bool NOnlineInfo::IsKickout()
{
	return bKickout;
}

void NOnlineInfo::DidLoaded( bool InSuccess/*=true*/ )
{
	bLoaded = InSuccess;
}

bool NOnlineInfo::IsLoaded()
{
	return bLoaded;
}

void NOnlineInfo::UseGuest( bool InEnable/*=true*/ )
{
	bGuest = InEnable;
}

bool NOnlineInfo::IsGuest()
{
	return bGuest;
}

void NOnlineInfo::AllowGuest( int InOption )
{
	GuestOption = InOption;
}

int NOnlineInfo::GetGuestOption()
{
	return GuestOption;
}

bool NOnlineInfo::IsLimitGuest()
{
    //Server allowed Guest ( GuestOption == 0:Not Supported, 1:Limit Contents 2:Full Contents )
    
    // Limit Contents
    if( bGuest && GuestOption == 1 ) return true;
    
    return false;
}

void NOnlineInfo::ResetDevice()
{
	// Device
	memset( UUID, 0, UD_UUID_LEN );
	memset( UDID, 0, UD_MAC_ADDRESS_LEN );
	memset( RNDT, 0, UD_RNTK_LEN );
	memset( DeviceModel, 0, UD_DEVICE_MODEL_LEN );
	memset( DeviceStore, 0, UD_DEVICE_STORE_LEN );
	memset( DeviceVersion, 0, UD_DEVICE_SYSTEM_VERSION_LEN );
	DeviceOS = 0;
	DeviceOSVer = 0;
	DeviceState = 0;
}

void NOnlineInfo::Rooting()
{
	DeviceState = 1;
}

bool NOnlineInfo::IsRooting()
{
	return ( DeviceState == 1 );
}

void NOnlineInfo::ResetOnline()
{
	bStartup = false;
	bOffline = false;
	bPurchasing = false;
	bNetworkWWAN = false;

	NetworkModel = eUDNM_None;

	OfflineTime = 0;
	ClientTime = 0;
	ServerTime = 0;
}

void NOnlineInfo::UseOffline( bool InEnable/*=true*/ )
{
	bOffline = InEnable;
}

bool NOnlineInfo::IsOffline()
{
	return bOffline;
}

void NOnlineInfo::UseNetworkWWAN( bool InEnable/*=true*/ )
{
	bNetworkWWAN = InEnable;
}

bool NOnlineInfo::CanNetworkWWAN()
{
	return bNetworkWWAN;
}

void NOnlineInfo::SetNetworkModel( int InState )
{
	NetworkModel = InState;
}

int NOnlineInfo::GetNetworkModel()
{
	return NetworkModel;
}

bool NOnlineInfo::CanNetwork()
{
	return ( NetworkModel != eUDNM_None );
}

void NOnlineInfo::StartPingTime( int InTime )
{
	PingSendTime = InTime;
	PingRecvTime = InTime;
}

void NOnlineInfo::RenewPingTime( int InTime, int InLastTime )
{
	PingRecvTime = InTime;

	int NewPingTime = PingRecvTime - InLastTime;

	PingSendTime = PingRecvTime;

	static int PingTimes[ONLINE_PING_CHECK_COUNT];
	static int PingCount = 0;

	PingTimes[PingCount] = NewPingTime;
	PingCount++;

	if( PingCount >= ONLINE_PING_CHECK_COUNT ) PingCount = 0;

	PingTime = 0;
	for( int i = 0; i < ONLINE_PING_CHECK_COUNT; i++ )
	{
		PingTime += PingTimes[i];
	}

	PingTime /= ONLINE_PING_CHECK_COUNT;

	if( PingTime < ONLINE_PING_GRADE_TOP )			PingGrade = 4;
	else if( PingTime < ONLINE_PING_GRADE_MID )		PingGrade = 3;
	else if( PingTime < ONLINE_PING_GRADE_LOW )		PingGrade = 2;
	else /*if( PingTime < ONLINE_PING_GRADE_BAD )*/	PingGrade = 1;
}

int	NOnlineInfo::GetPingTime()
{
	return PingTime;
}

int	NOnlineInfo::GetPingGrade()
{
	return PingGrade;
}

void NOnlineInfo::SetOnlineTime( long long InTime )
{
	ClientTime = 0;
	ServerTime = InTime;
}

long long NOnlineInfo::GetOnlineTime()
{
	if( ClientTime <= 0 ) return ServerTime;

	long long TempTime = ClientTime/1000;

	//// YYYY/MM/DD/HH/mm/ss

	long long Sec = TempTime%60;
	long long Min = (TempTime/60)%60;
	long long Hour = (TempTime/(60*60))%24;
	long long Day = (TempTime/(60*60*24));

	TempTime = (Day*1000000) + (Hour*10000) + (Min*100) + Sec;

	return (ServerTime + TempTime);
}

void NOnlineInfo::ResetChannels( int InChannelCount/*=UD_MAX_CHANNEL_COUNT*/)
{
	memset( ServerChannels, 0, sizeof(struct NServerChannel)*UD_MAX_CHANNEL_COUNT );
	ServerChannelCount = InChannelCount;

	SelectedChannel = 0;
}

NServerChannel* NOnlineInfo::FindChannel( int InChannelID )
{
	for( int i = 0; i < ServerChannelCount; i++ )
	{
		NServerChannel* TargetChannel = &ServerChannels[i];
		if( TargetChannel && TargetChannel->ID == InChannelID )
		{
			return TargetChannel;
		}
	}

	return 0;
}

NServerChannel* NOnlineInfo::SelectChannel()
{
	int ChannelUserCount = -1;
	NServerChannel* SelectingChannel = 0;
	NServerChannel* MigratingChannel = 0;

	for( int i = 0; i < ServerChannelCount; i++ )
	{
		NServerChannel* TargetChannel = &ServerChannels[i];
		if( TargetChannel )
		{
			// If Migrating....
			if( bChannelMigrating && MigratingChannelID == TargetChannel->ID )
			{
				MigratingChannel = TargetChannel;
				break;
			}

			// Find more users channel ( Default Search )
			if( !TargetChannel->IsFull() && TargetChannel->Users > ChannelUserCount )
			{
				SelectingChannel = TargetChannel;
				ChannelUserCount = TargetChannel->Users;
			}
		}
	}

	SelectedChannel = 0;

	if( bChannelMigrating && MigratingChannel )
	{
		SelectedChannel = MigratingChannel;

		ResetChannelMigration();
	}
	else
	{
		SelectedChannel = SelectingChannel;
	}

	return SelectedChannel;
}

NServerChannel* NOnlineInfo::GetSelectedChannel()
{
	return SelectedChannel;
}

NServerChannel* NOnlineInfo::GetChannel( int InIndex )
{
	if( InIndex < 0 || InIndex >= ServerChannelCount ) return 0;

	return &ServerChannels[InIndex];
}

void NOnlineInfo::ResetChannelMigration()
{
	bChannelMigrating = false;
	MigratingChannelID = -1;
}

void NOnlineInfo::MigrateChannel( int InChannelID )
	{
	bChannelMigrating = true;
	MigratingChannelID = InChannelID;
}

bool NOnlineInfo::IsChannelMigrating()
		{
	return bChannelMigrating;
}

bool NOnlineInfo::IsMigratingChannelID( int InChannelID )
			{
	return ( MigratingChannelID >= 0 && MigratingChannelID == InChannelID );
	}


void NOnlineInfo::ResetSceneMode()
{
	ScenePosition = 0;
	SceneBattleMode = eUDBM_None;
	SceneMatchMode = eUDMM_Max;
	SceneMatchTarget = eUDMT_None;
	bSceneMultiMode = false;

	MatchTarget = 0;
	MatchTargetKey = 0;
}

void NOnlineInfo::ChangeSceneMode( int InBattleMode, int InMatchMode, int InMatchTarget )
{
	SceneBattleMode = InBattleMode;
	SceneMatchMode = InMatchMode;
	SceneMatchTarget = InMatchTarget;
	bSceneMultiMode = (SceneMatchTarget < eUDMT_Single);
}

int NOnlineInfo::GetSceneBattleMode()
{
	return SceneBattleMode;
}

bool NOnlineInfo::IsSceneMultiMode()
{
	return bSceneMultiMode;
}

void NOnlineInfo::SetScenePosition( int InPosition )
{
	ScenePosition = InPosition;
}

int NOnlineInfo::GetScenePosition()
{
	return ScenePosition;
}

void NOnlineInfo::ResetMatch()
{
	bMatchMigrating = false;
	bMatchInviting = false;
	bMatching = false;

	BattleMode = 0;
	MatchMode = 0;
	MatchTarget = 0;
	MatchTargetKey = 0;

	memset( MatchTargetNick, 0, UD_NICK_LEN );
}

void NOnlineInfo::BeganMatchMigrating()
{
	bMatchMigrating = true;
}
void NOnlineInfo::EndedMatchMigrating()
{
	bMatchMigrating = false;
}
bool NOnlineInfo::IsMatchMigrating()
{
	return bMatchMigrating;
}

void NOnlineInfo::BeganMatchInviting( int InBattleMode, int InMatchMode, int InMatchTarget, long long InTargetKey, const char* InTargetName/*=0*/ )
{
	bMatchInviting = true;

	BattleMode = InBattleMode;
	MatchMode = InMatchMode;

	if( InTargetName && strlen( InTargetName ) > 0 )
	{
		memset( MatchTargetNick, 0, UD_NICK_LEN );
		strncpy( MatchTargetNick, InTargetName, UD_NICK_LEN-1 );
	}

	if( InMatchTarget == 0 && InTargetKey == 0 ) return;

	MatchTarget = InMatchTarget;
	MatchTargetKey = InTargetKey;
}

void NOnlineInfo::EndedMatchInviting()
{
	bMatchInviting = false;
	//bMatching = false;
	//MatchTarget = 0;
}

bool NOnlineInfo::IsMatchInviting()
{
	// Inviting match with target
	return bMatchInviting;
}

void NOnlineInfo::BeganMatching( int InBattleMode/*=-1*/, int InMatchMode/*=-1*/, int InMatchTarget/*=-1*/, long long InMatchTargetKey/*=0*/ )
{
	bMatchInviting = false;
	bMatching = true;

	if( InBattleMode >= 0 ) BattleMode = InBattleMode;
	if( InMatchMode >= 0 ) MatchMode = InMatchMode;

	if( InMatchTarget >= 0 ) MatchTarget = InMatchTarget;
	if( InMatchTargetKey > 0 ) MatchTargetKey = InMatchTargetKey;
}

void NOnlineInfo::EndedMatching( int InBattleMode/*=-1*/, int InMatchMode/*=-1*/ )
{
	bMatchInviting = false;
	bMatching = false;

	if( InBattleMode < 0 || InMatchMode < 0 ) return;

	BattleMode = InBattleMode;
	MatchMode = InMatchMode;
}

bool NOnlineInfo::IsMatching()
{
	// Progressing match with target
	return bMatching;
}

void NOnlineInfo::ResetURLs()
{
	memset( EventURL, 0, sizeof(char) * UD_DOMAIN_LEN );
	memset( PatchURL, 0, sizeof(char) * UD_DOMAIN_LEN );
	memset( SocialURL, 0, sizeof(char) * UD_DOMAIN_LEN );
}

void NOnlineInfo::SetEventURL( const char* InEventURL )
{
	if ( !InEventURL )
	{
		return;
	}

	strcpy( EventURL, InEventURL );
}

void NOnlineInfo::SetPatchURL( const char* InPatchURL )
{
	if ( !InPatchURL )
	{
		return;
	}

	strcpy( PatchURL, InPatchURL );
}

void NOnlineInfo::SetSocialURL( const char* InSocialURL )
{
	if ( !InSocialURL )
	{
		return;
	}

	strcpy( SocialURL, InSocialURL );
}

const char* NOnlineInfo::GetEventURL()
{
	return EventURL;
}

const char* NOnlineInfo::GetPatchURL()
{
	return PatchURL;
}

const char* NOnlineInfo::GetSocialURL()
{
	return SocialURL;
}

