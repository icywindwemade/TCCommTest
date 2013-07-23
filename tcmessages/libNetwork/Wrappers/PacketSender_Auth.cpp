//##############################################################################################
// PacketSender_Auth.cpp
//##############################################################################################
#include "PacketSender.h"
#include "GamePacket.h"

//#include "Client.h"
#include "OnlineHandler.h"

//#include "MessageHandler.h"
//#include "PlatformHandler.h"
//#include "UserInfo.h"
#include "BuildDefine.h"
//#include "GlobalInclude.h"


#if IOS
#include <SystemConfiguration/SCNetworkReachability.h>
#endif


void Mock_OnlineInfo();

//##############################################################################################
// Authentication
//##############################################################################################
int ClientNetworkStatus( const char* InHost/*=0*/ )
{
#if ANDROID
    
	if( JniUtil::IsAvailableWIFI() == true ) return eUDNM_WiFi;
	if( JniUtil::IsAvailableNetwork() == true ) return eUDNM_WWAN;
    
	return eUDNM_None;
    
#elif IOS
	SCNetworkConnectionFlags ReachFlags;
	SCNetworkReachabilityRef ReachRef;
    
	if( InHost )	ReachRef = SCNetworkReachabilityCreateWithName( 0, InHost );
	else			ReachRef = SCNetworkReachabilityCreateWithName( 0, "google.com" );
    
	bool bFlags = SCNetworkReachabilityGetFlags( ReachRef, &ReachFlags );
	CFRelease( ReachRef );
    
	if( !bFlags ) return eUDNM_None;
    
	bool bReachable = ReachFlags & kSCNetworkFlagsReachable;
	bool bNeedsConnection = ReachFlags & kSCNetworkFlagsConnectionRequired;
	bool bNonWiFi = ReachFlags & kSCNetworkReachabilityFlagsTransientConnection;
    
	if( bNonWiFi ) return eUDNM_WWAN; // 3G
    
	if( bReachable && !bNeedsConnection ) return eUDNM_WiFi; // WiFi
    
	return eUDNM_None;
    
#else // WIN32
    
	return eUDNM_Wire;
    
#endif	// ANDROID
}

void OLAuthenticateLogin()
{
	FnLog("NET: LOGIC, Login start");

	// Already Login
	if( GOnlineInfo->IsLogin() ) return;

	// Display progress spin
	/// ShowProgressMessage();

	// Check Network
	GOnlineInfo->SetNetworkModel( ClientNetworkStatus( ONLINE_NETWORK_CHECK_URL ) );
	if( !GOnlineInfo->CanNetwork() ) 
	{
        /*
		ShowAlertMessage( EMBA_OK, EMCT_Exit, L"%s||%s||%s", IUGetLocalText( L"info_connect_error" ), IUGetLocalText( L"info_check_network" ), IUGetLocalText( L"info_endgame" ) ); */
        FnLog("LOGIC: Can not connect to network!");
		return;
	}

	// Will call OLCallbackGateConnected()
	OLConnectGateServer();
}

void OLRequestAuthGateServer()
{
	// Send ProtocolVersion , Will receive PTCL_ANS_GATE_AUTHENTICATION
	FnLog("NET: SEND, REQ_GATE_AUTHENTICATION to GateServer\n");

	REQ_GATE_AUTHENTICATION NewPacket;

	INT MajorVersion = 20, MinorVersion = 1, BuildRevision = 0;
    /// ClientVersion( MajorVersion, MinorVersion, BuildRevision );
    /// HARDCODING!!!!!
	NewPacket.MajorVersion = MajorVersion;
	NewPacket.MinorVersion = MinorVersion;
	NewPacket.StoreType = 0; /// ClientStoreType();

FnLog("NET: SEND, REQ_GATE_AUTHENTICATION %d %d %d\n", MajorVersion, MinorVersion, BuildRevision );
    
	OLSendGatePacket( &NewPacket, sizeof(NewPacket) );
}

void OLRequestServerChannels()
{
	// Send Request , Will receive PTCL_ANS_GAME_INFO
	FnLog("NET: SEND, REQ_GAME_INFO to GateServer");

	REQ_GAME_INFO NewPacket;

	OLSendGatePacket( &NewPacket, sizeof(NewPacket) );
}

void OLRequestGameLogin()
{
	// Send login info , Will receive PTCL_ANS_GAME_LOGIN
	FnLog("NET: SEND, REQ_GAME_LOGIN to GameServer");

	NServerChannel* TargetChannel = GOnlineInfo->GetSelectedChannel();
	if( !TargetChannel )
	{
		FnLog("NET: SEND, Server channel info is null point");

		OLDisconnectGameServer();

//		ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s||%s||%s", IUGetLocalText( L"info_login_fail_full" ), IUGetLocalText( L"info_error_restart" ) );
//
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if !defined(BUILD_WITH_PLATFORM_TARGET) || (BUILD_WITH_PLATFORM_TARGET == BUILD_PLATFORM_NONE)
	GOnlineInfo->PlatformCode = eUDPC_CND;
#endif // BUILD_WITH_PLATFORM_TARGET == BUILD_PLATFORM_NONE
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
	REQ_GAME_LOGIN NewPacket;

	// Client Version HARDCODING!!!!!
	INT MajorVersion = 20, MinorVersion = 1, BuildRevision = 0;
	/// ClientVersion( MajorVersion, MinorVersion, BuildRevision );
	NewPacket.MajorVersion = MajorVersion;
	NewPacket.MinorVersion = MinorVersion;
	/// NewPacket.AndroidCRC = ClientPackageCrc("classes.dex");

	// Channel
	NewPacket.ChannelID		= TargetChannel->ID;

	// Device
	OLRefreshDeviceInfo(); // Refresh
    
    ////////////////////////////////////////////////////////////////////////////
    /// TODO: REPLACE THIS The TEMPORARY HARDCODED INFO
    Mock_OnlineInfo();
    ////////////////////////////////////////////////////////////////////////////
    
    
	NewPacket.NetworkModel	= (unsigned char)GOnlineInfo->NetworkModel;
	NewPacket.OS			= (unsigned char)GOnlineInfo->DeviceOS;
	NewPacket.OSTopVersion	= (unsigned char)(GOnlineInfo->IsRooting() ? GOnlineInfo->DeviceOSVer+10000 : GOnlineInfo->DeviceOSVer);
	memcpy( NewPacket.RNTK, GOnlineInfo->RNDT, UD_RNTK_LEN ); // Remote Notification Token Key by System Callback 
	memcpy( NewPacket.UUID, GOnlineInfo->UUID, UD_UUID_LEN );
	memcpy( NewPacket.MACAddress, GOnlineInfo->UDID, UD_MAC_ADDRESS_LEN );
	memcpy( NewPacket.DeviceModel, GOnlineInfo->DeviceModel, UD_DEVICE_MODEL_LEN );
	memcpy( NewPacket.DeviceStore, GOnlineInfo->DeviceStore, UD_DEVICE_STORE_LEN );
	memcpy( NewPacket.DeviceSystemVersion, GOnlineInfo->DeviceVersion, UD_DEVICE_SYSTEM_VERSION_LEN );

	if( GOnlineInfo->IsGuest()) // && !PFIsSupportedGuestAccount() )
	{
		GOnlineInfo->PlatformCode = eUDPC_Guest;
		memset( GOnlineInfo->PlatformUserID, 0, UD_ID_LEN );
		memset( GOnlineInfo->PlatformUserPW, 0, UD_PWD_LEN );
	}

    // ToDo: delete ?
    /// PFRefreshAccessToken( GOnlineInfo->PlatformUserToken, UD_TOKEN_LEN );
    
   	// Account
//	NewPacket.Platform.PlatformCode = eUDPC_CND;
	NewPacket.Platform.PlatformCode = (unsigned char)GOnlineInfo->PlatformCode;
	memcpy( NewPacket.Platform.ID, GOnlineInfo->PlatformUserID, UD_ID_LEN );
	memcpy( NewPacket.Platform.Pwd, GOnlineInfo->PlatformUserPW, UD_PWD_LEN );
	memcpy( NewPacket.Platform.Name, GOnlineInfo->PlatformUserName, UD_NICK_LEN );
	memcpy( NewPacket.Platform.Token, GOnlineInfo->PlatformUserToken, UD_TOKEN_LEN );

	FnLog( "NET: SEND, LOGIN Start" );
	FnLog( "VERSION: Major, %d", MajorVersion );
	FnLog( "VERSION: Minor, %d", MinorVersion );
    FnLog( "VERSION: Build, %d", BuildRevision);
    
	FnLog( "CHANNEL: ID, %d", TargetChannel->ID );
    
	FnLog( "DEVICE: NET, %d", GOnlineInfo->NetworkModel );
	FnLog( "DEVICE: OS, %d", GOnlineInfo->DeviceOS );
	FnLog( "DEVICE: OSVer, %d", GOnlineInfo->DeviceOSVer );
	FnLog( "DEVICE: UDID, %s", GOnlineInfo->UDID );
	FnLog( "DEVICE: UUID, %s", GOnlineInfo->UUID );
	FnLog( "DEVICE: RNTK, %s", GOnlineInfo->RNDT );
	FnLog( "DEVICE: Model, %s %d %d", GOnlineInfo->DeviceModel, GOnlineInfo->DeviceOS, GOnlineInfo->DeviceOSVer );
	FnLog( "DEVICE: Store, %s", GOnlineInfo->DeviceStore );
	FnLog( "DEVICE: Ver, %s", GOnlineInfo->DeviceVersion );
	FnLog( "PLATFORM: CODE, %d", GOnlineInfo->PlatformCode );
	FnLog( "PLATFORM: ID, %s", GOnlineInfo->PlatformUserID );
	FnLog( "PLATFORM: PW, %s", GOnlineInfo->PlatformUserPW );
	FnLog( "PLATFORM: Nick, %s", GOnlineInfo->PlatformUserNick );
	FnLog( "PLATFORM: Name, %s", GOnlineInfo->PlatformUserName );
	FnLog( "PLATFORM: Token, %s", GOnlineInfo->PlatformUserToken );
	FnLog( "NET: SEND, LOGIN END" );

	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );    
}

void OLRequestGameLogout()
{
	// Send login info , Will receive PTCL_ANS_GAME_LOGOUT
	FnLog("NET: SEND, REQ_GAME_LOGOUT to GameServer");

	REQ_GAME_LOGOUT NewPacket;

	OLSendGamePacket( &NewPacket, sizeof(NewPacket) );    
}

void OLRequestDeleteMember()
{
	// Send login info , Will receive PTCL_ANS_DELETE_MEMBER
	FnLog("NET: SEND, REQ_DELETE_MEMBER to GameServer");

	REQ_DELETE_MEMBER NewPacket;

	OLSendGamePacket( &NewPacket, sizeof(NewPacket) );    
}

void OLLoadDataAll()
{
	// Request All Data to GameServer
	FnLog("NET: LOGIC, Load All Began...");

	//ShowProgressMessage();

	/// GUserInfo->ResetAll();

	// Inventory
	OLRequestInventoryCastles();
	OLRequestInventoryUnits();

	// Deck
	OLRequestLoadDeck();

	// Mission
	OLRequestMissionHistory();

	// Profile
	OLRequestProfileBaseInfo( GOnlineInfo->UserKey );
	OLRequestProfilePrivateInfo();

#if ANDROID
	OLRequestTStoreInfo();
#endif	// ANDROID

	// Event
	OLRequestEventRecord( 1486 );

	OLRequestEventRecord( 1501 );

	OLRequestEventRecord( 1502 );

	// If you not receive server time, you can't progress next( can't close progress box )
	OLLoadServerTime(); 	// Request server time
}

void OLLoadServerTime()
{
	// Send Request , Will receive PTCL_ANS_CURRENT_TIME
	FnLog("NET: SEND, REQ_CURRENT_TIME to GameServer");

	REQ_CURRENT_TIME NewPacket;
	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
}

void OLRequestMissionHistory()
{
	// Send Request , Will receive PTCL_ANS_MISSION_HISTORY

	FnLog("NET: SEND, REQ_MISSION_HISTORY to GameServer"); 

	REQ_MISSION_HISTORY NewPacket;

	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
}

void OLRequestEventRecord( int InEventID, bool InSave/*=false*/ )
{
	REQ_EVENT_RECORDING NewPacket;

	NewPacket.EventID = InEventID;
	NewPacket.Purpose = InSave ? REQ_EVENT_RECORDING::eSave : REQ_EVENT_RECORDING::eCheck;

	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
}


//##############################################################################################
void Mock_OnlineInfo()
{
    GOnlineInfo->NetworkModel = 1;
    GOnlineInfo->DeviceOS = 10;
    GOnlineInfo->DeviceOSVer = 6;
    strcpy( GOnlineInfo->UDID, "10DDB1B25A18");
    strcpy( GOnlineInfo->UUID, "AB0A7D2D-1A5F-4F83-AF42-2812DF4A859A");
    //GOnlineInfo->RNDT =;
    strcpy( GOnlineInfo->DeviceModel, "x86_64");
    GOnlineInfo->DeviceOS = 10;
    GOnlineInfo->DeviceOSVer =6;
    strcpy( GOnlineInfo->DeviceStore ,"iTunes");
    strcpy( GOnlineInfo->DeviceVersion, "6.1");
    GOnlineInfo->PlatformCode = 7;
    strcpy( GOnlineInfo->PlatformUserID , "89695557306372640");
    //GOnlineInfo->PlatformUserPW =;
    strcpy( GOnlineInfo->PlatformUserNick, "WEME");
    //GOnlineInfo->PlatformUserName =;
    strcpy( GOnlineInfo->PlatformUserToken, "HStRkqyHm22mmxhtRmp2eUuLTO3G_MAlznxxx3P5sRuSq4Nq5YHuKcHXWsMpVYspEKvMM48yXrDFeL9v5A1DuA");
}