#pragma once

/*
 * UnionDefine.h
 * CND CommonDefine
 */

#define UD_IP_LEN							16				// IP Address String Length
#define UD_MAX_CHANNEL_COUNT				100				// Max Channel ( Game Server )
#define UD_MAX_CHANNEL_USERS				2300			// Max User Count of Channel( Game Server )

#define UD_ID_LEN							52				// CND ID
#define UD_PWD_LEN							32				// CND PassWord
#define UD_NICK_LEN							52				// CND Nick Name
#define UD_EMAIL_LEN						80				// CND E-Mail
#define UD_TOKEN_LEN						200				// CND Token(Facebook...)

#define UD_RNTK_LEN							200				// Remote Notification Token Key

#define UD_UUID_LEN							52				// UUID
#define UD_MAC_ADDRESS_LEN					32				// MAC Address
#define UD_DEVICE_MODEL_LEN					20				// Device Model
#define UD_DEVICE_STORE_LEN					20				// Device Store
#define UD_DEVICE_SYSTEM_VERSION_LEN		20				// Device OS Version

#define UD_DOMAIN_LEN						256

#define UD_UNITLISTITEM_COUNT				12				// UnitListUI Item Count
#define UD_UNITINVEN_SLOT_COUNT				6
#define UD_UNIT_SLOT_COUNT					6				// Unit Slots Count
#define UD_SKILL_SLOT_COUNT					2				// Skill Slots Count
#define UD_ITEM_SLOT_COUNT					3				// QuickItem Slots Count
#define UD_MAP_NAME_LEN						40				// Map Name string Length
#define UD_MISSION_COUNT					240				// Missions Count

#define UD_MISSION_RANK_COUNT				20
#define UD_MISSION_RANK_MSG_LEN				20

#define UD_CND_NAME_LEN						128
#define UD_MAX_MONEY						9999999			// Max Money(Gem, GP)

#define UD_MAX_PURCHASE_ITEM				10				// Max Purchase Item

// Item Category
enum eUDItemCategory
{
	eUDIC_Free		= 0	,	// Unlimited
	eUDIC_Time			,	// Limited Time
};

// OS Type
enum eUDOS
{
	eUDOS_Windows	= 0,	// Windows PC

	eUDOS_iOS		= 10,
	eUDOS_Android	= 20,
	eUDOS_WinMobile	= 30,

	eUDOS_Mac,
	eUDOS_Max,
};

// Wemade Auth Platform Code
enum eUDPlatformCode
{
	eUDPC_CND				= 0	,
	eUDPC_WemadeSocial		= 5,
	eUDPC_UUID				= 6,
	eUDPC_KakaoTalk			= 7,
	eUDPC_Guest				= 8,
	eUDPC_Line				= 9,
	eUDPC_FaceBook			= 10,
	eUDPC_Max
};

// Replicate GameAction
enum eUDGameAction
{
	eUDGA_None			= 0 ,
	eUDGA_BuildingSpawn		,
	eUDGA_BuildingUpgrade	,
	eUDGA_BuildingResell	,
	eUDGA_BuildingCancel	,
	eUDGA_BuildingPlay		,
	eUDGA_BuildingDie		,
	eUDGA_ItemTrain			,
	eUDGA_ItemCancel		,
	eUDGA_UnitSpawn			,
	eUDGA_UnitDie			,
	eUDGA_SkillSpawn		,
	eUDGA_SkillCancel		,
	eUDGA_SkillTake			,
	eUDGA_GoldTake			,
	eUDGA_Surrender			,
	eUDGA_Camera			,
	eUDGA_Event				,
	eUDGA_Message			,
	eUDGA_Test1				,
	eUDGA_Test2				,
	eUDGA_Test3				,
	eUDGA_Test4				,
	eUDGA_RankingItemSpawn	,
	eUDGA_RankingItemTake	,
	eUDGA_Max				,
};

// Network Model
enum eUDNetworkModel
{
	eUDNM_None	= 0	,
	eUDNM_WiFi		,
	eUDNM_WWAN		, // 3G
	eUDNM_Wire		,
	eUDNM_Max
};

enum eUDMatchMessageType
{
	// Server & Client Side
	eUDMMT_Cancel	= 0	,
	eUDMMT_Invite		,
	eUDMMT_Allow		,
	eUDMMT_Refuse		,
	eUDMMT_Confirm		,
	eUDMMT_Unable		,

	// Client Side
	eUDMMT_Wait			,
	eUDMMT_Close		,
	eUDMMT_Start	
};

enum eUDBattleMode
{
	eUDBM_None		= 0	,	
	eUDBM_Custom		,	
	eUDBM_Mission		,	
	eUDBM_Ladder		,
	eUDBM_Item			,
	eUDBM_Defense		,
	eUDBM_Max			
};

enum eUDMatchMode
{
	eUDMM_Single	= 0	, // Single
	eUDMM_Quick			, // Multi
	eUDMM_ReMatch		, // Multi
	eUDMM_Custom		, // Multi
	eUDMM_Friend		, // Friend
	eUDMM_Replay		, // Replay
	//eUDMM_Team		, // Multi
	eUDMM_Max			
};

enum eUDMatchTarget
{
	eUDMT_None		= 0		,
	eUDMT_Auto				,	// Quick Match Auto Invitation
	eUDMT_History			,	// Recent Battle History User
	eUDMT_Recommend			,	// Recommend User
	eUDMT_Friend			,	// Friend User
	eUDMT_Ranker			,	// Ranker
		
	eUDMT_Single = 100		,
	

	eUDMT_AI				,
	eUDMT_Ranking_AI = eUDMT_AI,
	eUDMT_Wave				,	
	eUDMT_AIWave			,

	eUDMT_Simulation = 200	,
	eUDMT_AISimulation		,
	eUDMT_Max
};

enum eUDMatchDifficulty
{
	eUDMD_None = 0,
	eUDMD_Easy,
	eUDMD_Normal,
	eUDMD_Hard,
	eUDMD_Max
};

// Match Status for Invite
enum eUDUserInvitationStatus
{
	eUDUIS_NoMember = 0,	// user not cnd member and invite cnd world!
	eUDUIS_Unable,			// user can't receive invitation message
	eUDUIS_Enable,			// user can receive invitation message
	eUDUIS_Busy,			// user is busy( playing, ... )
};

enum eUDScenePosition
{
	eUDSP_None			= 0,
	eUDSP_Background,

	eUDSP_Foreground	= 10,
	eUDSP_Title,
	eUDSP_Main,
	eUDSP_Inventory,
	eUDSP_Shop,
	eUDSP_Lobby,

	eUDSP_Battle		= 100,	
	eUDSP_Loading,
	eUDSP_InGame,
	eUDSP_Result,

	eUDSP_Max
};

enum eUDGameLeague
{
	UDGL_Bronze = 0	,
	UDGL_Silver		,
	UDGL_Gold		,
	UDGL_Platinum	,
	UDGL_Diamond	,
	UDGL_Max		,
};

enum eUDMissionRank
{
	eUDMR_F = 1	,
	eUDMR_D		,
	eUDMR_C		,
	eUDMR_B		,
	eUDMR_A		,
	eUDMR_S		,
};

enum eUDRewardItem
{
	eUDRI_None	= 0	,
	eUDRI_Exp		,
	eUDRI_Gem		,
	eUDRI_Capsule	,
	eUDRI_Unit		,
};

// Match Player Type
enum eUDMatchPlayerType
{
	eUDMPT_Player = 0,
	eUDMPT_AI,
};

// Client Device
enum eUDGameStatus
{
	eUDGS_None	= 0	,
	eUDGS_Ready		,
	eUDGS_Start		,
	eUDGS_Pause		,
	eUDGS_Resume	,
	eUDGS_End
};

// Reward Error Code
enum eUDRewardErrorCode
{
	eUDREC_Success = 0,				// Success
	eUDREC_Fail,					// Default Error
	eUDREC_DailyLimit,				// Dilay Limit
	eUDREC_RankDuplication,			// Rank Duplication Rewarde
	eUDREC_RankLimit,				// Rank Limit( C Rank )
	eUDREC_Data,					// Reward Data Error
};

// App Store Type
enum eUDClientStoreType
{
	eUDItunesStore = 0,	// Apple
	eUDTStore,			// SK
	eUDOllehMarket,		// KT
	eUDOZStore,			// LG
	eUDGooglePlay,		// Google
	eUDAmazon,			// Amazon
	eUDShandaStore,		// Shanda
	eUDTencentStore,	// Tencent
	eUDChinaMobile,		// China
	eUDDmemu,			// Docomo
	eUDDena,			// Dena

	eUDMaxStoreType,
};

// Game State Reporting Type
enum eUDGameStateReportingType
{
	eUDGSRT_GameEnd = 0,
	eUDGSRT_SyncError,
};

// Game Result Type
enum eUDGameResultType
{
	eUDGRT_Surrender = 0	,
	eUDGRT_Victory			,
	eUDGRT_Defeat			,
	eUDGRT_NoGame			,
	eUDGRT_Disconnected		,
	eUDGRT_Clear			,
	eUDGRT_Fail				,
	eUDGRT_Max				,
};

// Friend Type
enum eUDFriendType
{
	eUDFT_InGame = 0,		// From Game
	eUDFT_KakaoTalk,		// From KakaoTalk
	eUDFT_Max_Type,
};


enum eUDWemadeSocialRequestType
{
	eUDWSRT_SocialLink		= 1, // Social Link
	eUDWSRT_JoinMember		= 2, // Social Join
	eUDWSRT_QnA				= 3, // 1:1 BBS
	eUDWSRT_GameInfo		= 4, // Game Info
	eUDWSRT_FriendList		= 5, // Friend List
	eUDWSRT_Billing			= 6, // Billing
	eUDWSRT_FindAccount		= 7, // Find ID, Password
	eUDWSRT_AddEmail		= 100, // Add Email for KakaoTalk
	eUDWSRT_AddInfo			= 101, // Add Email or Phone Number for KakaoTalk
	eUDWSRT_NewAccount		= 201,
};

// Post type for sending
enum eUDPostType
{
	eUDPT_Star = 0,
	eUDPT_PremiumStar,
	eUDPT_Item,
	eUDPT_Boast,
};