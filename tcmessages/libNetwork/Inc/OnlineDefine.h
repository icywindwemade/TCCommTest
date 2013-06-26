#pragma once

//##############################################################################################
// 
//##############################################################################################
// Packet
#define ONLINE_MAX_PACKET_SIZE			8192

// Frame
#define ONLINE_FRAME_TIME				40 // 33
#define ONLINE_LATENCY_FRAME			25 // 35

// Sync
#define ONLINE_SYNC_LATENCY_TIME		1000 // 1 sec
#define ONLINE_SYNC_INTERVAL_TIME		200
#define ONLINE_SYNC_DELAY_TIME			2000 // 2 sec
#define ONLINE_SYNC_MIN_PACKET			128

// Check Connection
#define ONLINE_CONNECT_TIMEOUT			10000 // 10 sec
#define ONLINE_PING_CHECK_TIME			1000 // 5 sec
#define ONLINE_PING_CHECK_COUNT			4
#define ONLINE_PING_GRADE_TOP			100		
#define ONLINE_PING_GRADE_MID			250
#define ONLINE_PING_GRADE_LOW			500
#define ONLINE_PING_GRADE_BAD			750

#define ONLINE_MESSAGE_TIMEOUT			60000	// 30 sec
#define ONLINE_DEFAULT_TIMEOUT			10000	// 10 sec
#define ONLINE_RESTART_TIMEOUT			300000	// 5 Min
#define ONLINE_BUY_TIMEOUT				300000

#define ONLINE_MATCH_SEARCH_TIMEOUT		10000	// 10 sec
#define ONLINE_MATCH_EXPAND_TIMEOUT		30000	// 30 sec
#define ONLINE_MATCH_COUNTDOWN_TIME		5000	// 5 sec

#define	ONLINE_DISPLAY_NAME_LENGTH		12

#define ONLINE_NETWORK_CHECK_URL		"google.com"

enum EOnlineConnectionTarget
{
	EOCT_GateServer	= 0	,
	EOCT_GameServer		,
	EOCT_Max
};

enum EOnlineMessageCode
{
	EOMC_Success        = 0	,
    EOMC_Failed             ,
    EOMC_LoggedIn           ,
    EOMC_LoggedOut          ,
    EOMC_PatchBegin         ,
    EOMC_PatchEnded         ,
    EOMC_InputNick          ,
    EOMC_GroupRanks         ,
    EOMC_Achievements       ,
    EOMC_BattleRecommends   ,
    EOMC_BattleHistory      ,

	EOMC_UserDetail         ,
	EOMC_FriendDetail       ,
    EOMC_OwnerGameFriends	,
	EOMC_OtherGameFriends	,
	EOMC_DeleteGameFriend   ,
    EOMC_UserDeck           ,
	EOMC_RecommendFriend	,
	EOMC_FindFriend			,
	EOMC_CompareFriends		,

	EOMC_RankCommentFail	,
	EOMC_Top20List			,

	EOMC_BuyFail			,
	EOMC_BuySuccess			,
	EOMC_PurchasePermission ,

	EOMC_AddFriendSuccess	,
	EOMC_Combine_Success	,
	EOMC_Grade_Success		,
};






