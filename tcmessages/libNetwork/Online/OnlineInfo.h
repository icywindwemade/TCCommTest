//##############################################################################################
// OnlineInfo.h
//##############################################################################################
#ifndef _ONLINE_INFO_H_
#define _ONLINE_INFO_H_

#include "IUDefine.h"
#include "UnionDefine.h"
#include "OnlineDefine.h"

//##############################################################################################
// NServerChannel
//##############################################################################################
struct NServerChannel
{
	unsigned short	ID;
	unsigned short	Users;
	unsigned short	MaxUsers;
	unsigned short 	Port;
	char			IP[16];		// UD_IP_LEN

	NServerChannel()
		: ID( 0 )
		, Users( 0 )
		, MaxUsers( 0 )
		, Port( 0 )
	{
	}

	bool IsFull()
	{
		return ( MaxUsers > 0 && Users >= MaxUsers);
	}
	
};

//##############################################################################################
// NNotification
//##############################################################################################
struct NNotification
{
	int		State;
	int		Type;
	int		Time;
	int		Link;
	char	Title[64];
	char	Desc[256];
	char	URL[128];

	NNotification()
		: State( 0 )
		, Type( 0 )
		, Time( 0 )
		, Link( 0 )
	{
	}
};

//##############################################################################################
// NOnlineInfo
//##############################################################################################
class NOnlineInfo
{
public:
	static NOnlineInfo* Instance();

public:
	NOnlineInfo();
	~NOnlineInfo();

	void ResetAll();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Account
	void ResetAccount();

	void DidLogin( int InConnID, long long InUserKey, long long InAuthID, int InAuthToken );
	void DidLogout();
	bool IsLogin();
	bool CanRelogin();

	void DidKickin();
	void DidKickout();
	bool IsKickout();

	void DidLoaded( bool InSuccess=true );
	bool IsLoaded();

	void UseGuest( bool InEnable=true );
	bool IsGuest();

	void AllowGuest( int InOption );
	int GetGuestOption();
    bool IsLimitGuest();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Device
	void ResetDevice();
	void Rooting();
	bool IsRooting();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Network
	void ResetOnline();

	void UseOffline( bool InEnable=true );
	bool IsOffline();

	void UseNetworkWWAN( bool InEnable=true );
	bool CanNetworkWWAN();

	void SetNetworkModel( int InState );
	int GetNetworkModel();
	bool CanNetwork();

	// Ping
	void StartPingTime( int InTime );
	void RenewPingTime( int InTime, int InLastTime );
	int	GetPingTime();
	int	GetPingGrade();
	
	// Server Time( World Time )
	void SetOnlineTime( long long InTime );
	long long GetOnlineTime();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// ServerChannel ( Game Servers )
	void ResetChannels( int InChannelCount=UD_MAX_CHANNEL_COUNT );
	NServerChannel* FindChannel( int InChannelID );
	NServerChannel* SelectChannel();
	NServerChannel* GetSelectedChannel();
	NServerChannel* GetChannel( int InIndex );
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Channel( Game Server ) Migration
	void ResetChannelMigration();
	void MigrateChannel( int InChannelID );
	bool IsChannelMigrating();
	bool IsMigratingChannelID( int InChannelID );

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Scene
	void ResetSceneMode();
	void ChangeSceneMode( int InBattleMode, int InMatchMode, int InMatchTarget );
	int GetSceneBattleMode();
	bool IsSceneMultiMode();

	void SetScenePosition( int InPosition );
	int GetScenePosition();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Match
	void ResetMatch();

	void BeganMatchMigrating();
	void EndedMatchMigrating();
	bool IsMatchMigrating();

	void BeganMatchInviting( int InBattleMode, int InMatchMode, int InMatchTarget=0, long long InTargetKey=0, const char* InTargetName=0 );
	void EndedMatchInviting();
	bool IsMatchInviting();

	void BeganMatching( int InBattleMode=-1, int InMatchMode=-1, int InMatchTarget=-1, long long InMatchTargetKey=0 );
	void EndedMatching( int InBattleMode=-1, int InMatchMode=-1 );
	bool IsMatching();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Patch
	void ResetURLs();
	void SetEventURL( const char* InEventURL );
	void SetPatchURL( const char* InPatchURL );
	void SetSocialURL( const char* InSocialURL );
	const char* GetEventURL();
	const char* GetPatchURL();
	const char* GetSocialURL();

public:
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Account
	int		 				ConnectionID;						// CND Connection ID
	long long 				UserKey;							// CND User Key
	char 					UserNick[UD_NICK_LEN];              // CND User Nick		( Use for CND )

	long long               AuthID;								// Only use for WemadeSocial
	int 					AuthToken;							// Only use for WemadeSocial

	int						PlatformCode;						// Platform Code (CND, WemadeSocial, KakaoTalk, Auto...)
	long long 				PlatformUserKey;					// Platform User Key or ID	( Use for CND )
	char 					PlatformUserID[UD_ID_LEN];			// Platform User ID			( Use for CND )
	char 					PlatformUserPW[UD_PWD_LEN];			// Platform User Password	( Use for CND )
	char 					PlatformUserNick[UD_NICK_LEN];		// Platform User Nick		( Use for CND )
	char 					PlatformUserEMail[UD_EMAIL_LEN];	// Platform User Email		( Use for CND )
	char 					PlatformUserToken[UD_TOKEN_LEN];	// Platform User Token		( Use for CND )
	char 					PlatformUserName[UD_NICK_LEN];		// Platform User Name   	( Use for CND ) Only Check CND User Nick
	char 					PlatformUserImgURL[256];			// Platform User Image URL (Profile)
	char 					PlatformUserImgPath[256];			// Platform User Image URL (Profile)
	bool					PlatformUserMessageBlocked;			// Platform user message blocked

	bool					bLogin;								// Login state
	bool					bKickout;							// Disconnected by Server
	bool					bLoaded;							// Loaded User Data? for One login logic

	bool					bNicknameRegisterFailed;
    bool                    bNoticeEvent;
	bool					bAllowMessage;
	
	bool					bGuest;								// Guest
	int						GuestOption;						// Server allowed ( 0:Not, 1:Min 2:Full )

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Device
	char 					UUID[UD_UUID_LEN];					// Application Unique ID
	char 					UDID[UD_MAC_ADDRESS_LEN];			// Device Unique ID
	char 					RNDT[UD_RNTK_LEN];					// ReomoteNotificationDeviceToken
	char					DeviceModel[UD_DEVICE_MODEL_LEN];	// Device( iPhone3GS, iPhone 4G, iPhone4GS, GalaxyS2....
	char					DeviceStore[UD_DEVICE_STORE_LEN];	// iTunes, T-Store... 
	char					DeviceVersion[UD_DEVICE_SYSTEM_VERSION_LEN];	// ex) 4.3.1 ( iOS )
	int						DeviceOS;	// iOS, Android, Windows...
	int						DeviceOSVer; // ex) 4.3.1 -> 4
	int						DeviceState;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Network
	bool					bStartup;
	bool					bOffline;
	bool					bPurchasing;
	bool					bNetworkWWAN; // 3G Usable
	int						NetworkModel;
	int						PingSendTime;
	int						PingRecvTime;
	int						PingTime;
	int						PingGrade;
	int						OfflineTime;
	int						ClientTime;
	long long				ServerTime;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// ServerChannel ( Game Servers )
	NServerChannel			ServerChannels[UD_MAX_CHANNEL_COUNT];
	int						ServerChannelCount;
	NServerChannel*			SelectedChannel;
	int						MigratingChannelID;
	bool					bChannelMigrating;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Scene
	int						ScenePosition;
	int						SceneBattleMode;
	int						SceneMatchMode;
	int						SceneMatchTarget;
	bool					bSceneMultiMode;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Match
	bool					bMatchMigrating;
	bool					bMatchInviting;
	bool					bMatching;
	
	int						BattleMode;
	int						MatchMode;
	int						MatchTarget;
	long long 				MatchTargetKey;
	char					MatchTargetNick[UD_NICK_LEN];

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Notification & Event
	NNotification			Notification;
	NNotification			AutoNotification;
	int						EventID;
	int						EventID2;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// URL
	char					EventURL[UD_DOMAIN_LEN];	// Event promotion URL address
	char					PatchURL[UD_DOMAIN_LEN];	// Patch URL address
	char					SocialURL[UD_DOMAIN_LEN];	// Wemade social URL address

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Purchase
	long long				PermissionKey;
	int						ProductKey;
	WCHAR					ErrorMsg[256];
	
};

extern NOnlineInfo* GOnlineInfo;

#endif // _ONLINE_INFO_H_
