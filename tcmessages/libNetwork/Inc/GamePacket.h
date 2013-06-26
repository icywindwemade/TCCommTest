#pragma once
#include "Packet.h"
#include "GameProtocol.h"
#include "UnionDefine.h"


#pragma pack(push,1)

/* Login */
struct UD_PLATFORM
{
	unsigned char	PlatformCode;					// eUDPlatformCode

	char			ID[UD_ID_LEN];			// ID
	char			Pwd[UD_PWD_LEN];		// Password
	char			Name[UD_NICK_LEN];		// Name
	char			Token[UD_TOKEN_LEN];	// Token

	UD_PLATFORM()
	{
		PlatformCode = 0;

		ID[0] = '\0';
		Name[0] = '\0';
		Token[0] = '\0';
	}
};
struct REQ_GAME_LOGIN : PACKET
{
	unsigned long	ProtocolVersion;									// Packet Protocl Version

	int				MajorVersion;										// Client Binary Major Version
	int				MinorVersion;										// Client Binary Minor Version

	int				ChannelID;											// Enterance Channel ID

	UD_PLATFORM		Platform;											// Platform

	unsigned char	NetworkModel;										// eUDNetworkModel

	unsigned char	OS;													// OS
	int				OSTopVersion;										// OS Version(example: if ios version is 4.3.1, OSVersion is 4)	

	char			RNTK[UD_RNTK_LEN];									// ReomoteNotificationTokenKey(IOS, Android)
	char			MACAddress[UD_MAC_ADDRESS_LEN];						// MAC Address
	char			UUID[UD_UUID_LEN];									// UUID
	char			DeviceModel[UD_DEVICE_MODEL_LEN];					// Device( iPhone3GS, iPhone 4G, iPhone4GS, GalaxyS2....	
	char			DeviceStore[UD_DEVICE_MODEL_LEN];					// iTunes, T-Store...
	char			DeviceSystemVersion[UD_DEVICE_SYSTEM_VERSION_LEN];	// Device System Version

	unsigned int	AndroidCRC;											// Only Android

	REQ_GAME_LOGIN()
	{
		ptcl = PTCL_REQ_GAME_LOGIN;
		size = sizeof(*this);

		ProtocolVersion = GAME_PROTOCOL_VERSION;
		MajorVersion = 0;
		MinorVersion = 0;

		ChannelID = 0;

		OS = 0;
		NetworkModel = 0;

		RNTK[0] = '\0';
		MACAddress[0] = '\0';
		UUID[0] = '\0';
		DeviceModel[0] = '\0';
		DeviceStore[0] = '\0';	
		DeviceSystemVersion[0] = '\0';

		AndroidCRC = 0;
	};
};

struct ANS_GAME_LOGIN : PACKET
{
	enum eAnsCode
	{
		eSuccess = 0,						// Success
		eFail,								// Fail
		eFail_Protocol,						// Protocol Version Different
		eFail_Major,						// Major Version Different
		eFail_Minor,						// Minor Version Different
		eFail_NoAccount,					// No Account
		eFail_Duplication,					// Duplication Login
		eFail_Full_Channel,					// Full
		eFail_Auth,							// Wemade Auth
		eFail_Logoff,						// Wemade log off
		eFail_KakaoTalk_MoreInformation,	// KakaoTalk more information(email or phone)
		eFail_Token,						// Third party platform token 
		eFail_Name,							// Need Name
		eFail_AndroidCRC,					// Fail Android CRC Checking
		eFail_MemberDeletion,				// Member Deletion
		eFail_Line_Auth,					// line auth fail
	};

	unsigned char	AnsCode;
	long long		UKey;								// CND User Key
	long long		gUid;								// Weamde Social User Key(gUid)

	int				MajorVersion;						// Client Binary Major Version
	int				MinorVersion;						// Client Binary Minor Version

	int				authToken;

	int				ConUID;								// Connection Unique ID( 0 ~ )
	unsigned short	MemberDeletionRemainingHours;		// CND Member Deleteion Remaining Hours

	unsigned char	NewWeek;				// 0:False, 1:True

	ANS_GAME_LOGIN()
	{
		ptcl = PTCL_ANS_GAME_LOGIN;
		size = sizeof(*this);

		AnsCode = eFail;
		UKey = 0;
		authToken = 0;
	};
};

/* Logout */
struct REQ_GAME_LOGOUT : PACKET
{
	REQ_GAME_LOGOUT()
	{
		ptcl = PTCL_REQ_GAME_LOGOUT;
		size = sizeof(*this);
	};
};
struct ANS_GAME_LOGOUT : PACKET
{
	enum eAnsCode
	{
		eSuccess = 0,
		eFail
	};
	unsigned char AnsCode;

	ANS_GAME_LOGOUT()
	{
		ptcl = PTCL_ANS_GAME_LOGOUT;
		size = sizeof(*this);
	};
};

/* Search Name */
struct REQ_SEARCH_NAME : PACKET
{
	char Name[UD_NICK_LEN];

	REQ_SEARCH_NAME()
	{
		ptcl = PTCL_REQ_SEARCH_NAME;
		size = sizeof(*this);
	};
};

struct ANS_SEARCH_NAME : PACKET
{
	enum eReturnCode
	{
		eRtn_NotFound = 0,
		eRtn_Exist
	};
	unsigned char ReturnCode;

	ANS_SEARCH_NAME()
	{
		ptcl = PTCL_ANS_SEARCH_NAME;
		size = sizeof(*this);
	};
};

/* Delete Member */
struct REQ_DELETE_MEMBER : PACKET
{
	REQ_DELETE_MEMBER()
	{
		ptcl = PTCL_REQ_DELETE_MEMBER;
		size = sizeof(*this);
	};
};

struct ANS_DELETE_MEMBER : PACKET
{
	enum eAnsCode
	{
		eSuccess = 0,
		eFail,
		eFail_NonMember,	// Nonmember
		eFail_Social,		// Wemade Social Error
	};
	unsigned char AnsCode;

	ANS_DELETE_MEMBER()
	{
		ptcl = PTCL_ANS_DELETE_MEMBER;
		size = sizeof(*this);
	};
};

/* 접속자 수 */
struct REQ_CONNECT_COUNT : PACKET
{
	REQ_CONNECT_COUNT()
	{
		ptcl = PTCL_REQ_CONNECT_COUNT;
		size = sizeof(*this);
	};
};
struct ANS_CONNECT_COUNT : PACKET
{
	int ConnectCount;			// 접속자 수

	ANS_CONNECT_COUNT()
	{
		ptcl = PTCL_ANS_CONNECT_COUNT;
		size = sizeof(*this);
	};
};

/* 서버 현재 시간 로드 */
struct REQ_CURRENT_TIME : PACKET
{
	REQ_CURRENT_TIME()
	{
		ptcl = PTCL_REQ_CURRENT_TIME;
		size = sizeof(*this);
	};
};
struct ANS_CURRENT_TIME : PACKET
{
	long long Time;		// YYYY/MM/DD/HH/mm/ss

	ANS_CURRENT_TIME()
	{
		ptcl = PTCL_ANS_CURRENT_TIME;
		size = sizeof(*this);
	};
};

/* Change Auto Login Option */
struct REPORTING_AUTO_LOGIN : PACKET
{
	unsigned char IsAutoLogin;	// TRUE:Use, FALSE:Disuse

	REPORTING_AUTO_LOGIN()
	{
		ptcl = PTCL_REPORTING_AUTO_LOGIN;
		size = sizeof(*this);
	};
};

/* Change Game Option */
struct REPORTING_GAME_OPTION : PACKET
{
	// 0:Unable, 1:Enable
	unsigned char BattleInvitation;

	// 0:refuse , 1:allow
	unsigned char PostReceive;

	REPORTING_GAME_OPTION()
	{
		ptcl = PTCL_REPORTING_GAME_OPTION;
		size = sizeof(*this);
	};
};

/* Gmaer Position In Game */
struct REPORTING_GAME_POSITION : PACKET
{
	unsigned char	BattleMode;
	unsigned char	ScenePosition;

	int				GameStatus;
	int				GameTime;

	REPORTING_GAME_POSITION()
	{
		ptcl = PTCL_REPORTING_GAME_POSITION;
		size = sizeof(*this);
	};
};

/* Private Information */
struct REQ_PRIVATE_INFO : PACKET
{
	REQ_PRIVATE_INFO()
	{
		ptcl = PTCL_REQ_PRIVATE_INFO;
		size = sizeof(*this);
	};
};

struct ANS_PRIVATE_INFO : PACKET
{
	int				LadderPoint;
	int				Gem;			// Cash
	int				GP;				// Game Money

	// 0:Unable, 1:Enable
	unsigned char	BattleInvitation;

	int				DeckPoint;

	// 0:refuse , 1:allow
	unsigned char	PostReceive;

	ANS_PRIVATE_INFO()
	{
		ptcl = PTCL_ANS_PRIVATE_INFO;
		size = sizeof(*this);
	};
};

/* Base Information */
struct REQ_BASE_INFO : PACKET
{
	long long	UKey;						// Target

	REQ_BASE_INFO()
	{
		ptcl = PTCL_REQ_BASE_INFO;
		size = sizeof(*this);
	};
};

struct ANS_BASE_INFO : PACKET
{
	long long		UKey;							// Request

	char			Name[UD_NICK_LEN];				// Platform Nickname

	int				WinCount;						// Total
	int				LoseCount;						// Total

	int				League;							// League
	int				LeagueGroup;
	int				LeagueGroupRanking;				// Ranking( 1 ~ N )

	int				Exp;							// Current
	int				EndExp;							// End
	int				Level;							// Level

	int				AchievementCompletionCount;		// Complete

	int				Portrait;
	unsigned char	Status;							// eUDUserInvitationStatus

	unsigned char	FriendStatus;					// 0:Not Friend / 1:Friend

	ANS_BASE_INFO()
	{
		ptcl = PTCL_ANS_BASE_INFO;
		size = sizeof(*this);
	};
};

// Player Information
struct REQ_PLAYER_INFO : PACKET
{
	// Target
	char		ID[UD_NICK_LEN];					// ID
	long long	UKey;								// InGame Unique Key

	REQ_PLAYER_INFO()
	{
		ptcl = PTCL_REQ_PLAYER_INFO;
		size = sizeof(*this);
	};
};
struct ANS_PLAYER_INFO : PACKET
{
	enum			eAnsCode { eSuccess=0, eFail };
	unsigned char	AnsCode;

	char			ID[UD_NICK_LEN];				// ID
	long long		UKey;							// InGame Unique Key

	int				WinCount;						// Total

	int				Exp;							// Current
	int				EndExp;							// End
	int				Level;							// Level	

	unsigned char	Status;							// eUDUserInvitationStatus

	int				BestWeeklyScore;				// Ranking Best Score
	int				LastWeeklyScore;				// LastWeekly Ranking Score
	int				WeeklyScore;					// Weekly Ranking Score;

	int				FirstClassCount;
	int				SecondClassCount;
	int				ThirdClassCount;

	int				UnitIDs[UD_UNIT_SLOT_COUNT];	// Deck Unit Item ID

	long long		OfflinInviteDateTime;			// YYYY/MM/DD/hh/mm/ss

	ANS_PLAYER_INFO()
	{
		ptcl = PTCL_ANS_PLAYER_INFO;
		size = sizeof(*this);
	};
};

/* Group Ranking */
struct REQ_GROUP_RANKING : PACKET
{
	long long		UKey;					// Target
	unsigned char	Page;						// 1 ~ N

	REQ_GROUP_RANKING()
	{
		ptcl = PTCL_REQ_GROUP_RANKING;
		size = sizeof(*this);
	};
};

struct ANS_GROUP_RANKING : PACKET
{
	struct MEMBER
	{
		char			Name[UD_NICK_LEN];			// Platform Nickname
		long long		UKey;

		int				WinCount;					// Total
		int				LoseCount;					// Total

		int				LadderPoint;

		int				Ranking;
		int				Portrait;

		MEMBER()
		{
			Name[0] = '\0';
			UKey = 0;

			WinCount = 0;
			LoseCount = 0;

			LadderPoint = 0;

			Ranking = 0;
			Portrait = 0;
		}
	};

	MEMBER			Member[10];
	unsigned char	MemberCount;

	unsigned char	Page;						// Request Page
	unsigned char	PageCount;

	long long		UKey;					// Request UKey

	// UKey Ranking
	int				League;						
	int				LeagueGroup;
	int				LeagueGroupRanking;

	ANS_GROUP_RANKING()
	{
		ptcl = PTCL_ANS_GROUP_RANKING;
		size = sizeof(*this);
	};
};

/* Achievement History */
struct REQ_ACHIEVEMENT_HISTORY : PACKET
{
	long long		UKey;
	unsigned char	Page;				// 1 ~ N

	REQ_ACHIEVEMENT_HISTORY()
	{
		ptcl = PTCL_REQ_ACHIEVEMENT_HISTORY;
		size = sizeof(*this);
	};
};

struct ANS_ACHIEVEMENT_HISTORY : PACKET
{
	struct ACHIEVEMENT
	{
		int ID;							// ID
		int CompletionDate;				// Format : yyyymmdd

		ACHIEVEMENT()
		{
			ID = 0;
			CompletionDate = 0;
		}
	};

	ACHIEVEMENT		Achievement[10];
	int				AchievementCount;

	long long		UKey;

	unsigned char	Page;				// Request Page
	unsigned char	PageCount;

	ANS_ACHIEVEMENT_HISTORY()
	{
		ptcl = PTCL_ANS_ACHIEVEMENT_HISTORY;
		size = sizeof(*this);
	};
};

/* Private Deck Setting */
struct REQ_COMBAT_SETTING : PACKET
{
	REQ_COMBAT_SETTING()
	{
		ptcl = PTCL_REQ_COMBAT_SETTING;
		size = sizeof(*this);
	};
};

struct ANS_COMBAT_SETTING : PACKET
{
	int	UnitSlot[UD_UNIT_SLOT_COUNT];
	int	UnitSlotSeqNo[UD_UNIT_SLOT_COUNT];

	int	Map;
	int Castle;

	ANS_COMBAT_SETTING()
	{
		ptcl = PTCL_ANS_COMBAT_SETTING;
		size = sizeof(*this);
	};
};

/* Change Private Deck Setting */
struct REQ_CHANGE_COMBAT_SETTING : PACKET
{
	int	UnitSlot[UD_UNIT_SLOT_COUNT];
	int	UnitSlotSeqNo[UD_UNIT_SLOT_COUNT];

	int	Map;
	int CastleSeqNo;

	REQ_CHANGE_COMBAT_SETTING()
	{
		ptcl = PTCL_REQ_CHANGE_COMBAT_SETTING;
		size = sizeof(*this);
	};
};

struct ANS_CHANGE_COMBAT_SETTING : PACKET
{
	enum eAnsCode
	{
		eSuccess = 0,	// 성공
		eFail,			// 실패
	};
	int	AnsCode;

	int	UnitSlot[UD_UNIT_SLOT_COUNT];
	int	UnitSlotSeqNo[UD_UNIT_SLOT_COUNT];

	int	Map;
	int CastleSeqNo;

	ANS_CHANGE_COMBAT_SETTING()
	{
		ptcl = PTCL_ANS_CHANGE_COMBAT_SETTING;
		size = sizeof(*this);
	};
};

/* Deck */
struct REQ_DECK : PACKET
{
	long long UKey;

	REQ_DECK()
	{
		ptcl = PTCL_REQ_DECK;
		size = sizeof(*this);
	};
};
struct ANS_DECK : PACKET
{
	long long	UKey;

	int			UnitIDs[UD_UNIT_SLOT_COUNT];
	int			CastleID;

	ANS_DECK()
	{
		ptcl = PTCL_ANS_DECK;
		size = sizeof(*this);
	};
};

/* User Mission History */
struct REQ_MISSION_HISTORY : PACKET
{
	REQ_MISSION_HISTORY()
	{
		ptcl = PTCL_REQ_MISSION_HISTORY;
		size = sizeof(*this);
	};
};

struct ANS_MISSION_HISTORY : PACKET
{
	struct sMission
	{
		int				MissionNo;

		unsigned char	Lock;	//0 : Lock, 1 : Unlock
		unsigned char	Rank;
		sMission()
		{
			MissionNo	= 0;
			Lock		= 0;
			Rank		= 0;
		}
	};

	sMission		aMission[UD_MISSION_COUNT];
	unsigned char	nMissionCount;

	int				aMissionScore[UD_MISSION_COUNT];
	int				aMissionBestScore[UD_MISSION_COUNT];

	ANS_MISSION_HISTORY()
	{
		ptcl = PTCL_ANS_MISSION_HISTORY;
		size = sizeof(*this);
	};
};

// Mission Open
struct REQ_MISSION_OPEN : PACKET
{
	int MissionNo;

	REQ_MISSION_OPEN()
	{
		ptcl = PTCL_REQ_MISSION_OPEN;
		size = sizeof(*this);
	};
};
struct ANS_MISSION_OPEN : PACKET
{
	enum eAnsCode 
	{ 
		eSuccess=0,
		eFail,
		eFail_StarCount,			// not enough starcount
		eFail_DuplicationOpen,		// already open
		eFail_Data,					// data error
		eFail_Ordering,				// invalid ordering			
	};
	unsigned char	AnsCode;
	int				MissionNo;
	int				MyStarCount;

	ANS_MISSION_OPEN()
	{
		ptcl = PTCL_ANS_MISSION_OPEN;
		size = sizeof(*this);
	};
};

/* Load Inventory */
struct REQ_INVENTORY : PACKET
{
	unsigned char Page;

	REQ_INVENTORY()
	{
		ptcl = PTCL_REQ_INVENTORY;
		size = sizeof(*this);

		Page = 1;
	};
};

struct UD_ITEM
{
	int			ItemSeqNo;			// Item Unique Number
	int			ItemID;				// Item ID
	int			ItemCount;			// Item Count
	int			Category;			// Item Type(eUDItemCategory)
	long long	ExpireTime;			// Item Expire Time
	int			Level;				// Item Level
	int			Exp;				// Item Exp
	int			isEquip;			// Item is Equiped

	UD_ITEM()
	{
		ItemSeqNo = 0;
		ItemID = 0;
		ItemCount = 0;
		Category = 0;
		ExpireTime = 0;
		Level = 0;
		Exp = 0;
		isEquip = 0;
	}
};
struct ANS_INVENTORY : PACKET
{
	UD_ITEM			Item[100];
	int				ItemCount;

	unsigned char	Page;
	unsigned char	PageCount;

	int				InventorySize;

	ANS_INVENTORY()
	{
		ptcl = PTCL_ANS_INVENTORY;
		size = sizeof(*this);

		ItemCount = 0;
		Page = 1;
		PageCount = 1;
		InventorySize = 0;
	};
};

/* Castle Inventory */
struct REQ_CASTLE_INVENTORY : PACKET
{
	unsigned char Page;

	REQ_CASTLE_INVENTORY()
	{
		ptcl = PTCL_REQ_CASTLE_INVENTORY;
		size = sizeof(*this);

		Page = 1;
	};
};
struct ANS_CASTLE_INVENTORY : PACKET
{
	UD_ITEM			Item[20];
	int				ItemCount;

	unsigned char	Page;
	unsigned char	PageCount;

	ANS_CASTLE_INVENTORY()
	{
		ptcl = PTCL_ANS_CASTLE_INVENTORY;
		size = sizeof(*this);

		ItemCount = 0;
		Page = 1;
		PageCount = 1;
	};
};

/* Delete Item */
struct REQ_DELETE_ITEM : PACKET
{
	int ItemSeqNo;			// Item Unique Number

	REQ_DELETE_ITEM()
	{
		ptcl = PTCL_REQ_DELETE_ITEM;
		size = sizeof(*this);
	};
};
struct ANS_DELETE_ITEM : PACKET
{
	enum eAnsCode
	{
		eSuccess = 0,			// Success
		eFail,				// Fail
		eFail_None,			// Not Exist Item
		eFail_NonExpire,	// Not Expire Time
	};
	unsigned char	AnsCode;
	int		ItemSeqNo;		// Item Unique Key

	ANS_DELETE_ITEM()
	{
		ptcl = PTCL_ANS_DELETE_ITEM;
		size = sizeof(*this);
	};
};

/* Mix Item */
struct REQ_MIX_ITEM : PACKET
{
	int ItemSeqNo[4];					// Item Unique Number

	REQ_MIX_ITEM()
	{
		ptcl = PTCL_REQ_MIX_ITEM;
		size = sizeof(*this);

		ItemSeqNo[0] = 0;
		ItemSeqNo[1] = 0;
		ItemSeqNo[2] = 0;
		ItemSeqNo[3] = 0;
	};
};
struct ANS_MIX_ITEM : PACKET
{
	enum eAnsCode
	{
		eSuccess = 0,
		eFail,
	};
	unsigned char	AnsCode;

	int				ItemSeqNo[4];		// Item Unique Number
	UD_ITEM			Item;				// Obtain Item(Capsule)

	ANS_MIX_ITEM()
	{
		ptcl = PTCL_ANS_MIX_ITEM;
		size = sizeof(*this);
	};
};

/* 캡슐 열기 */
struct REQ_OPEN_CAPSULE : PACKET
{
	int			ItemSeqNo;				// Item Unique Key

	REQ_OPEN_CAPSULE()
	{
		ptcl = PTCL_REQ_OPEN_CAPSULE;
		size = sizeof(*this);
	};
};
struct ANS_OPEN_CAPSULE : PACKET
{
	enum eAnsCode 
	{
		eSuccess = 0,
		eFail
	};
	unsigned char	AnsCode;

	int				ItemSeqNo;				// Item Unique Number

	int				AcquiredItemSeqNo;		// Item Unique Number
	int				AcquiredItemID;			// Item ID
	int				AcquiredItemCount;		// Item Count
	int				AcquiredCategory;		// Item Type(eUDItemCategory)
	long long		AcquiredItemExpireTime;	// Item Expire Time

	ANS_OPEN_CAPSULE()
	{
		ptcl = PTCL_ANS_OPEN_CAPSULE;
		size = sizeof(*this);
	};
};

/* Purchase */
struct REQ_PURCHASE : PACKET
{
	int				ProductID;			// Product ID

	REQ_PURCHASE()
	{
		ptcl = PTCL_REQ_PURCHASE;
		size = sizeof(*this);
	};
};
struct ANS_PURCHASE : PACKET
{
	enum eAnsCode 
	{
		eSuccess = 0,					// Success
		eFail,							// Fail
		eFail_Repurchase,				// Repurchase
		eFail_Balance,					// Not Enough balance
		eFail_SaleTime,					// Sale time over
		eFail_MaxStarCount,				// over max starcount
		eFail_MaxMoney,					// over max money
		eFail_MaxItem,					// over max item
		eFail_MaxInventory,				// omver max inventory size
	};

	unsigned char	AnsCode; 
	int				ProductID;			// Shop Product ID	

	int				Gem;				// Remaining Cash
	int				GP;					// Remaining Game Money

	int				ItemCount;
	UD_ITEM			Item[UD_MAX_PURCHASE_ITEM];

	int				MyStarCount;
	int				InventorySize;

	ANS_PURCHASE()
	{
		ptcl = PTCL_ANS_PURCHASE;
		size = sizeof(*this);
	};
};

/* Purchase Permission */
struct REQ_PURCHASE_PERMISSION : PACKET
{
	int ProductID;

	REQ_PURCHASE_PERMISSION()
	{
		ptcl = PTCL_REQ_PURCHASE_PERMISSION;
		size = sizeof(*this);
	};
};
struct ANS_PURCHASE_PERMISSION : PACKET
{
	enum eAnsCode
	{
		eSuccess = 0,
		eFail,							// Unknown
		eFail_DailyOverpayment,			// daily overpayment
		eFail_MonthlyOverpayment,		// monthly overpayment
		eFail_MaxMoney,					// Max Money Limit
	};

	unsigned char	AnsCode;
	long long		PermissionKey;

	ANS_PURCHASE_PERMISSION()
	{
		ptcl = PTCL_ANS_PURCHASE_PERMISSION;
		size = sizeof(*this);
	};
};

/* IOS In App Purchase */
struct REQ_IN_APP_PURCHASE : PACKET
{
	int			ProductID;
	long long	PermissionKey;		
	char		Receipt[4096];		

	//long		itemDeliverySequences;	// USE CASE LINE	-- 13.02.19 not activate, make later
	//long		transactionID;			// USE CASE LINE

	REQ_IN_APP_PURCHASE()
	{
		ptcl = PTCL_REQ_IN_APP_PURCHASE;
		size = sizeof(*this);
	};
};
struct ANS_IN_APP_PURCHASE : PACKET
{
	enum eAnsCode
	{
		eSuccess = 0,				// success
		eFail_NoItem,				// cannot find iteminfo
		eFail_Expired,				// product has expired
		eFail_TranID,				// TranID already exists
		eFail_Store,				// Apple Store Error
		eFail_PermissionKey,		// Invalid Permission Key		
		eFail_Unknown,				// Unknown
	};

	unsigned char	AnsCode;
	int				Gem;		// Total Gem

	ANS_IN_APP_PURCHASE()
	{
		ptcl = PTCL_ANS_IN_APP_PURCHASE;
		size = sizeof(*this);
	};
};

/* Android in app purchase */
struct REQ_ANDROID_IN_APP_PURCHASE : PACKET
{
	unsigned char	StoreType;				// eUDClientStoreType
	char			TransactionID[1024];	// Purchase unique key
	int				ProductID;
	long long		PermissionKey;

	char			Signature[16];
	char			Sign[32];

	REQ_ANDROID_IN_APP_PURCHASE()
	{
		ptcl = PTCL_REQ_ANDROID_IN_APP_PURCHASE;
		size = sizeof(*this);
	};
};
struct ANS_ANDROID_IN_APP_PURCHASE : PACKET
{
	enum eAnsCode
	{
		eSuccess = 0,				// success
		eFail_NoItem,				// cannot find iteminfo
		eFail_Expired,				// product has expired
		eFail_TranID,				// TranID already exists
		eFail_Store,				// Store Error
		eFail_PermissionKey,		// Invalid Permission Key
		eFail_Signature,			// Invalid SIgnature
		eFail_Sign,					// Invalid SIgn
		eFail_Unknown,				// Unknown
	};

	unsigned char	AnsCode;	
	int				Gem;		// Total Gem

	ANS_ANDROID_IN_APP_PURCHASE()
	{
		ptcl = PTCL_ANS_ANDROID_IN_APP_PURCHASE;
		size = sizeof(*this);
	};
};

/* Battle Recommendation */
struct REQ_BATTLE_RECOMMENDATION : PACKET
{
	long long UKey;			// Target

	REQ_BATTLE_RECOMMENDATION()
	{
		ptcl = PTCL_REQ_BATTLE_RECOMMENDATION;
		size = sizeof(*this);
	};
};

struct UD_BATTLE_MEMBER
{
	long long		UKey;
	unsigned char	Status;
	char			Name[UD_NICK_LEN];
	int				Portrait;

	int				BattleCount;
	int				WinCount;
	int				LoseCount;

	UD_BATTLE_MEMBER()
	{
		UKey = 0;
		Status = eUDUIS_Unable;
		Name[0] = '\0';
		Portrait = 0;

		BattleCount = 0;
		WinCount = 0;
		LoseCount = 0;
	}
};

struct ANS_BATTLE_RECOMMENDATION : PACKET
{
	long long			UKey;		// CND UserKey
	UD_BATTLE_MEMBER	Member;

	ANS_BATTLE_RECOMMENDATION()
	{
		ptcl = PTCL_ANS_BATTLE_RECOMMENDATION;
		size = sizeof(*this);
	};
};

/* Battle History */
struct REQ_BATTLE_HISTORY : PACKET
{
	long long UKey;			// Target

	REQ_BATTLE_HISTORY()
	{
		ptcl = PTCL_REQ_BATTLE_HISTORY;
		size = sizeof(*this);
	};
};

struct ANS_BATTLE_HISTORY : PACKET
{
	long long			UKey;

	UD_BATTLE_MEMBER	Member[10];
	int					MemberCount;

	ANS_BATTLE_HISTORY()
	{
		ptcl = PTCL_ANS_BATTLE_HISTORY;
		size = sizeof(*this);

		MemberCount = 0;
	};
};

/* Get Friend List */
struct REQ_FRIEND_LIST : PACKET
{
	long long		UKey;
	unsigned short	Page;

	REQ_FRIEND_LIST()
	{
		ptcl = PTCL_REQ_FRIEND_LIST;
		size = sizeof(*this);
	};
};

struct UD_FRIEND
{
	long long		UKey;				// CND User Key
	unsigned char	Status;				// eUDUserInvitationStatus

	char			ID[UD_ID_LEN];
	char			Name[UD_NICK_LEN];

	int				Portrait;

	unsigned char	Type;				// eUDFriendType

	UD_FRIEND()
	{
		UKey = 0;
		Status = eUDUIS_Unable;

		ID[0] = '\0';
		Name[0] = '\0';

		Portrait = 0;
		Type	= 0;
	};
};
struct ANS_FRIEND_LIST : PACKET
{
	long long		UKey;

	UD_FRIEND		Friend[10];
	unsigned char	FriendCount;

	unsigned short	Page;			// 1~
	unsigned short	PageCount;		// 1~

	ANS_FRIEND_LIST()
	{
		ptcl = PTCL_ANS_FRIEND_LIST;
		size = sizeof(*this);
	};
};

/* Friend Recommendation List */
struct REQ_FRIEND_RECOMMENDATION_LIST : PACKET
{
	REQ_FRIEND_RECOMMENDATION_LIST()
	{
		ptcl = PTCL_REQ_FRIEND_RECOMMENDATION_LIST;
		size = sizeof(*this);
	};
};
struct ANS_FRIEND_RECOMMENDATION_LIST : PACKET
{
	UD_FRIEND		Friend[5];
	unsigned char	FriendCount;

	ANS_FRIEND_RECOMMENDATION_LIST()
	{
		ptcl = PTCL_ANS_FRIEND_RECOMMENDATION_LIST;
		size = sizeof(*this);
	};
};

/* Search Friend */
struct REQ_SEARCH_FRIEND : PACKET
{
	char Name[UD_NICK_LEN];

	REQ_SEARCH_FRIEND()
	{
		ptcl = PTCL_REQ_SEARCH_FRIEND;
		size = sizeof(*this);
	};
};

struct ANS_SEARCH_FRIEND : PACKET
{
	enum eAnsCode
	{
		eSuccess = 0,
		eFail,
	};
	unsigned char	AnsCode;

	unsigned char	AnsQueryCount;

	long long		UKey;
	char			Name[UD_NICK_LEN];

	unsigned char	Status;				// eUDUserInvitationStatus
	unsigned char	IsFriend;

	int				Portrait;

	ANS_SEARCH_FRIEND()
	{
		ptcl = PTCL_ANS_SEARCH_FRIEND;
		size = sizeof(*this);
	};
};

/* Add Friend */
struct REQ_ADD_FRIEND : PACKET
{
	long long	UKey;
	char		Name[UD_NICK_LEN];				// Name(Facebook)
	unsigned char	AddType;					// eUDFriendType

	REQ_ADD_FRIEND()
	{
		ptcl = PTCL_REQ_ADD_FRIEND;
		size = sizeof(*this);
	};
};

struct ANS_ADD_FRIEND : PACKET
{
	enum eAnsCode
	{
		eSuccess = 0,
		eFail,
		eFail_Duplication,
		eFail_NoMember,
		eFail_None_AddType,
	};

	unsigned char	AnsCode;
	UD_FRIEND		Friend;

	ANS_ADD_FRIEND()
	{
		ptcl = PTCL_ANS_ADD_FRIEND;
		size = sizeof(*this);
	};
};

// Friend Invitation List
struct REQ_FRIEND_INVITATION_LIST : PACKET
{
	REQ_FRIEND_INVITATION_LIST()
	{
		ptcl = PTCL_REQ_FRIEND_INVITATION_LIST;
		size = sizeof(*this);
	};
};
struct ANS_FRIEND_INVITATION_LIST : PACKET
{
	unsigned short	Page;		// 1 ~ n
	unsigned short	PageCount;	// 1 ~ n

	long long		InvitedID[10];
	unsigned char	InvitedCount;

	ANS_FRIEND_INVITATION_LIST()
	{
		ptcl = PTCL_ANS_FRIEND_INVITATION_LIST;
		size = sizeof(*this);
	};
};

// Invite Friend
struct REQ_FRIEND_INVITATION : PACKET
{
	char			ID[UD_ID_LEN];

	enum eType		{ eInviteGame = 0, eInviteOffline };
	unsigned char	InvitationType;

	REQ_FRIEND_INVITATION()
	{
		ptcl = PTCL_REQ_FRIEND_INVITATION;
		size = sizeof(*this);
	};
};
struct ANS_FRIEND_INVITATION : PACKET
{
	char			ID[UD_ID_LEN];
	int				MyStarCount;

	enum eType		{ eInviteGame = 0, eInviteOffline };
	unsigned char	InvitationType;
	long long		InvitationDateTime;		// YYYY/MM/DD/hh/mm/ss

	ANS_FRIEND_INVITATION()
	{
		ptcl = PTCL_ANS_FRIEND_INVITATION;
		size = sizeof(*this);
	};
};

/* Start Match */
struct REQ_START_MATCH : PACKET
{
	unsigned char	MatchMode;		// eUDMatchMode
	unsigned char	MatchTarget;	// eUDMatchTarget
	unsigned char	Ability;		// difficulty
	unsigned char	BattleMode;		// eUDBattleMode(UnionDefine.h)
	long long		UKey;			// CND UserKey
	int				MissionNo;		// Single Mission No.(eMatchMode::eSingle)

	REQ_START_MATCH()
	{
		ptcl = PTCL_REQ_START_MATCH;
		size = sizeof(*this);
	};
};
struct ANS_START_MATCH : PACKET
{
	enum eAnsCode
	{
		eSuccess			= 0,
		eFail				= 1,
		eFail_Equip			= 2,
		eFail_StarCount		= 3,
	};

	unsigned char	AnsCode;
	unsigned char	MatchMode;		// eUDMatchMode
	unsigned char	MatchTarget;	// eUDMatchTarget
	unsigned char	Ability;		// difficulty
	unsigned char	BattleMode;		// eUDBattleMode(UnionDefine.h)

	// Custom Gamer's CND UserKey
	long long		UKey;

	// Single Mission No.(eMatchMode::eSingle)
	int				MissionNo;

	ANS_START_MATCH()
	{
		ptcl = PTCL_ANS_START_MATCH;
		size = sizeof(*this);
	};
};

/* Stop Match */
struct REQ_STOP_MATCH : PACKET
{
	REQ_STOP_MATCH()
	{
		ptcl = PTCL_REQ_STOP_MATCH;
		size = sizeof(*this);
	};
};

struct ANS_STOP_MATCH : PACKET
{
	enum eAnsCode
	{
		eSuccess	= 0,
		eFail		= 1,
	};
	unsigned char AnsCode;

	ANS_STOP_MATCH()
	{
		ptcl = PTCL_ANS_STOP_MATCH;
		size = sizeof(*this);
	};
};

/* Custom Match */
struct REPORTING_CUSTOM_MATCH : PACKET
{
	long long		UKey;							// CND UserKey
	char			Name[UD_NICK_LEN];				// Name(Facebook)
	int				ChannelID;						// ChannelID

	unsigned char	MatchMessageType;				// eUDMatchMessageType
	unsigned char	MatchMode;						// eUDMatchMode
	unsigned char	MatchTarget;					// eUDMatchTarget
	unsigned char	UserInvitationStatus;			// eUDUserInvitationStatus

	unsigned char	BattleMode;						// eUDBattleMode(UnionDefine.h)

	REPORTING_CUSTOM_MATCH()
	{
		ptcl = PTCL_REPORTING_CUSTOM_MATCH;
		size = sizeof(*this);

		UKey = 0;
		Name[0] = '\0';
		ChannelID = 0;

		MatchMessageType = eUDMMT_Cancel;		
		MatchMode = eUDMM_Single;
		MatchTarget = eUDMT_None; 
		UserInvitationStatus = eUDUIS_Unable;
		BattleMode = eUDBM_None;
	};
};

/* Match Complete */
struct UD_PLAYER
{
	long long		UKey;		// ID > 0,   ( AI is -1, -2, ... )
	char			Name[UD_NICK_LEN];

	unsigned char	PlayerType;	// eUDMatchPlayerType
	int				PlayerID;	// 0x0F000000 ( Team: 0x01000000,0x02000000,0x04000000,0x08000000 ) 
	// 0x00F00000 ( Player: 0x00100000,0x00200000,0x00400000,0x00800000 ) 
	// 0xF0000000 ( System: Not used ) 
	int				CastleID;
	int				UnitIDs[UD_UNIT_SLOT_COUNT];
	int				UnitLevels[UD_UNIT_SLOT_COUNT];
	int				WinCount;		
	int				LoseCount;
	int				GroupRanking;
	int				LadderPoint;

	unsigned char	Ability;			// AI Ability( 0 = None, 1~5 = level )
	int				Strategy;			// AI Strategy
	unsigned char	NetworkModel;		// eUDNetworkModel
	int				Ping;				// (/ms)
	int				Portrait;

	UD_PLAYER()
	{
		UKey			= 0;
		Name[0]			= '\0';

		PlayerType		= eUDMPT_Player;
		PlayerID		= 0;

		CastleID		= 0;

		for (int i = 0; i < UD_UNIT_SLOT_COUNT; i++)
		{
			UnitIDs[i] = 0;
			UnitLevels[i] = 0;
		}

		WinCount		= 0;
		LoseCount		= 0;
		GroupRanking	= 0;
		LadderPoint		= 0;

		Ability			= 0;
		Strategy		= 0;
		NetworkModel	= 0;
		Ping			= 0;
		Portrait		= 0;
	}
};

struct EVENT_MATCH_COMPLETE : PACKET
{
	unsigned char	MatchMode;		// eUDMatchMode
	unsigned char	MatchTarget;	// eUDMatchTarget	
	unsigned char	BattleMode;		// eUDBattleMode

	UD_PLAYER		Players[4];
	unsigned char	PlayerCount;	// Match Players

	EVENT_MATCH_COMPLETE()
	{
		ptcl = PTCL_EVENT_MATCH_COMPLETE;
		size = sizeof(*this);
	};
};

/* Match Close */
struct EVENT_MATCH_CLOSE : PACKET
{
	EVENT_MATCH_CLOSE()
	{
		ptcl = PTCL_EVENT_MATCH_CLOSE;
		size = sizeof(*this);
	};
};

/* Achievement Completion */
struct EVENT_ACHIEVEMENT_COMPLETION : PACKET
{
	int AchievementID;

	EVENT_ACHIEVEMENT_COMPLETION()
	{
		ptcl = PTCL_EVENT_ACHIEVEMENT_COMPLETION;
		size = sizeof(*this);
	};
};

/* Event Change Controller */
struct EVENT_CHANGE_CONTROLLER : PACKET
{
	long long		UKey;		// Change Player UKey

	unsigned char	AIMode;		// AI difficulty
	unsigned char	BattleMode; // eUDBattleMode(UnionDefine.h)

	EVENT_CHANGE_CONTROLLER()
	{
		ptcl = PTCL_EVENT_CHANGE_CONTROLLER;
		size = sizeof(*this);
	};
};


//////////////////////////////////////////////////////////////////////////
/* Start Game */
struct REQ_START_GAME : PACKET
{
	// Union
	int	MapSetIndex;

	// 0 oor ProductID
	int ProductID[UD_ITEM_SLOT_COUNT];

	REQ_START_GAME()
	{
		ptcl = PTCL_REQ_START_GAME;
		size = sizeof(*this);
	};
};

struct UD_DECK
{
	int				PlayerID;

	int				CastleID;
	int				UnitIDs[UD_UNIT_SLOT_COUNT];

	UD_DECK()
	{		
		PlayerID = 0;
		CastleID = 0;

		for( int i=0; i<UD_UNIT_SLOT_COUNT; i++ )
		{
			UnitIDs[i] = 0;
		}
	}
};
struct ANS_START_GAME : PACKET
{
	enum eAnsCode
	{
		eSuccess = 0,
		eFail,
		eFail_InstantItem,
		eFail_StarCount,
	};
	unsigned char	AnsCode;

	// Union
	int				MapID;

	UD_DECK			Deck[4];
	unsigned char	DeckCount;

	int				MyGem;
	int				MyGP;

	// 0 oor ItemID
	int				InstantItemID[UD_ITEM_SLOT_COUNT];

	int				MyStarCount;

	ANS_START_GAME()
	{
		ptcl = PTCL_ANS_START_GAME;
		size = sizeof(*this);
	};
};

/* Stop Game */
struct REQ_STOP_GAME : PACKET
{
	int Reason;

	REQ_STOP_GAME()
	{
		ptcl = PTCL_REQ_STOP_GAME;
		size = sizeof(*this);
	};
};

struct ANS_STOP_GAME : PACKET
{
	enum eAnsCode
	{
		eSuccess = 0,
		eFail,
	};
	unsigned char AnsCode;

	ANS_STOP_GAME()
	{
		ptcl = PTCL_ANS_STOP_GAME;
		size = sizeof(*this);
	};
};

/* Sync Ready */
struct REPLICATION_READY : PACKET
{
	int	Owner;
	int Time;
	int Type;

	REPLICATION_READY()
	{
		ptcl = PTCL_REPLICATION_READY;
		size = sizeof(*this);
	};
};

/* Check game update frame */
struct REPLICATION_SYNC : PACKET
{
	int	Owner;
	int Frame;
	int Type;

	REPLICATION_SYNC()
	{
		ptcl = PTCL_REPLICATION_SYNC;
		size = sizeof(*this);
	};
};

struct REPLICATION_ACTION : PACKET
{
	int	Frame;		// Command Frame
	int	Action;		// Command Action
	int Owner;		// Player ID
	int	Base;		// Base ID ( C Eight(8) ID )
	int	Code;		// Object Type, Object Data Type, Type
	int Index;		// Item, Skill, Object, Slot Index
	int	Cost;		// Cost ( Gold, Mana )
	int	X, Y;		// Location
	int Gold;		// Gold

	REPLICATION_ACTION()
	{
		ptcl = PTCL_REPLICATION_ACTION;
		size = sizeof(*this);
	};
};

/* Game ping */
struct REPLICATION_PING : PACKET
{
	int	Owner;
	int Frame;
	int Count;
	int Time;
	int Type;

	REPLICATION_PING()
	{
		ptcl = PTCL_REPLICATION_PING;
		size = sizeof(*this);
	};
};

/* Dummy game data */ 
struct REPLICATION_DUMMY : PACKET
{
	int		Owner;
	char	Data[32];

	REPLICATION_DUMMY()
	{
		ptcl = PTCL_REPLICATION_DUMMY;
		size = sizeof(*this);
	};
};

/* Reporting Single Game State */
struct REPORTING_GAME_STATE : PACKET
{
	unsigned char	Type;		// eUDGameStateReportingType
	unsigned char	Result;		// eUDGameResultType
	int				Owner;		// PlayerID

	REPORTING_GAME_STATE()
	{
		ptcl = PTCL_REPORTING_GAME_STATE;
		size = sizeof(*this);
	};
};

/* 엔딩 이벤트 */
struct EVENT_GAME_ENDING : PACKET
{
	unsigned char GameResult;		// eUDGameResultType

	EVENT_GAME_ENDING()
	{
		ptcl = PTCL_EVENT_GAME_ENDING;
		size = sizeof(*this);
	};
};

/* 엔딩 정보 요청 */
struct REQ_GAME_RECORDING : PACKET
{
	int				MissionID;
	unsigned char	Rank;

	int				GainGold;
	int				UseGold;

	int				GainMana;
	int				UseMana;

	int				Score;

	REQ_GAME_RECORDING()
	{
		ptcl = PTCL_REQ_GAME_RECORDING;
		size = sizeof(*this);
	};
};

struct ANS_GAME_RECORDING : PACKET
{
	enum eReward
	{
		eR_Null = 0,
		eR_Exp,
		eR_GP,
		eR_Capsule,
		eR_Unit		
	};

	struct INVENTORY_INFO
	{
		int				ItemCategory;
		int				ItemSeqNo;
		int				ItemTotalCount;
		long long		ItemExpiretime;

		INVENTORY_INFO()
		{
			ItemCategory	= 0;
			ItemSeqNo		= 0;
			ItemTotalCount	= 0;
			ItemExpiretime	= 0;
		}
	};

	struct REWARD_DATA
	{
		unsigned char	Reward;
		int				RewardValue;		// GP, Exp, Unit ...
		int				RewardCount;

		REWARD_DATA()
		{
			Reward		= (eReward)0;
			RewardValue	= 0;
			RewardCount	= 0;
		}
	};

	struct PLAYER_RECORD
	{
		long long		UKey;	// ID > 0,   ( AI is -1, -2, ... )

		int				WinCount;
		int				LoseCount;
		int				GroupRanking;
		int				LadderPoint;
		int				League;
		int				ChangeLP;
		int				ChangeGP;
		int				ChangeExp;

		int				BonusGP;		
		int				BonusExp;

		// Current Game Info
		int				CurLevel;
		int				CurExp;
		int				CurMaxExp;

		// eUDBattleMode(UnionDefine.h)
		int				BattleMode;

		// Mission
		int				MissionID;			// MIssion ID(Number)
		int				Rank;				// Mission Ending Rank
		
		// Reward
		REWARD_DATA		RewardArray[3];		// [0] : Real Reward, [1], [2] : Fake Reward

		INVENTORY_INFO	Item_Info;			// if( Real == Unit )

		int				DeckPoint;

		unsigned char	RewardErrorCode;	// eUDRewardErrorCode

		PLAYER_RECORD()
		{
			UKey				= 0;

			WinCount			= 0;
			LoseCount			= 0;
			GroupRanking		= 0;
			LadderPoint			= 0;
			League				= 0;
			ChangeLP			= 0;
			ChangeGP			= 0;
			ChangeExp			= 0;

			BonusGP				= 0;
			BonusExp			= 0;

			CurLevel			= 0;
			CurExp				= 0;
			CurMaxExp			= 0;

			BattleMode			= 0;

			MissionID			= 0;
			Rank				= 0;

			DeckPoint			= 0;

			RewardErrorCode		= eUDREC_Success;
		}
	};

	unsigned char	RecordCount;
	PLAYER_RECORD	Records[4];

	int				Score[4];

	ANS_GAME_RECORDING()
	{
		ptcl = PTCL_ANS_GAME_RECORDING;
		size = sizeof(*this);
	};
};

// Rank Comment 
struct REQ_UPDATE_RANK_COMMENT : PACKET
{
	int		MissionNo;
	char	comment[UD_MISSION_RANK_MSG_LEN];

	REQ_UPDATE_RANK_COMMENT()
	{
		ptcl = PTCL_REQ_UPDATE_RANK_COMMENT;
		size = sizeof(*this);
	};
};
struct ANS_UPDATE_RANK_COMMENT : PACKET
{
	enum			eAnsCode { eSuccess=0, eFail };
	unsigned char	AnsCode;
	int				MissionNo;

	ANS_UPDATE_RANK_COMMENT()
	{
		ptcl = PTCL_ANS_UPDATE_RANK_COMMENT;
		size = sizeof(*this);
	};
};

//////////////////////////////////////////////////////////////////////////
/* Friend InvitationState */
struct REQ_FRIEND_INVITAION_STATE : PACKET
{
	long long		FriendUKey[100];
	int				FriendCount;

	REQ_FRIEND_INVITAION_STATE()
	{
		ptcl = PTCL_REQ_FRIEND_INVITAION_STATE;
		size = sizeof(*this);
	}
};

struct ANS_FRIEND_INVITAION_STATE : PACKET
{
	struct MEMBER
	{
		long long		UKey;
		unsigned char	Status;							// eUDUserInvitationStatus

		MEMBER()
		{
			UKey = 0;
			Status = 0;
		}
	};
	
	MEMBER		Member[100];
	int			FriendCount;

	ANS_FRIEND_INVITAION_STATE()
	{
		ptcl = PTCL_ANS_FRIEND_INVITAION_STATE;
		size = sizeof(*this);
	}

};

/* Top20 WeeklyRanking */
struct REQ_TOP20_WEEKLY_RANKING : PACKET
{
	int				MissionNo;

	REQ_TOP20_WEEKLY_RANKING()
	{
		ptcl = PTCL_REQ_TOP20_WEEKLY_RANKING;
		size = sizeof(*this);
	}
};

struct ANS_TOP20_WEEKLY_RANKING : PACKET
{
	struct MEMBER
	{
		char			ID[UD_NICK_LEN];
		char			Name[UD_NICK_LEN];
		int				Score;
		char			Message[UD_MISSION_RANK_MSG_LEN];

		MEMBER()
		{
			ID[0]		= '\0';
			Name[0]		= '\0';
			Score		= 0;
			Message[0] = '\0';
		}
	};

	MEMBER			Member[UD_MISSION_RANK_COUNT];
	long long		ExpireTime;

	ANS_TOP20_WEEKLY_RANKING()
	{
		ptcl = PTCL_ANS_TOP20_WEEKLY_RANKING;
		size = sizeof(*this);
	}
};

/* WeeklyRanking */
struct REQ_WEEKLY_RANKING_GAME_INFO : PACKET
{
	REQ_WEEKLY_RANKING_GAME_INFO()
	{
		ptcl = PTCL_REQ_WEEKLY_RANKING_GAME_INFO;
		size = sizeof(*this);
	}
};

struct ANS_WEEKLY_RANKING_GAME_INFO : PACKET
{
	long long		ExpireTime;

	int				MyStarCount;
	long long		MyStarChargingTime;

	unsigned char	SendablePremiumStarCount;

	int				MyTotalPostCount;
	
	ANS_WEEKLY_RANKING_GAME_INFO()
	{
		ptcl = PTCL_ANS_WEEKLY_RANKING_GAME_INFO;
		size = sizeof(*this);
	}
};

struct REQ_WEEKLY_RANKING :PACKET
{
	enum eMaxSearch {eMax_Search_Count = 10 };
	
	char			FriendID[eMax_Search_Count][UD_ID_LEN];
	unsigned char	isLastPacket;				// 0 : false, 1:last(true);

	REQ_WEEKLY_RANKING()
	{
		ptcl = PTCL_REQ_WEEKLY_RANKING;
		size = sizeof(*this);
	}
};

struct ANS_WEEKLY_RANKING :PACKET
{
	enum eAnsCode { eSuccess = 0, eFail };
	enum eMaxSearch { eMax_Search_Count = 10 };
	enum eStarState { eTransmissive = 0, eAlreadySend, eRefuse };

	struct MEMBER
	{
		char			ID[UD_NICK_LEN];
		long long		UKey;
		int				Score;
		int				LastScore;
		unsigned char	StarState;

		MEMBER()
		{
			ID[0]		= '\0';
			UKey		= 0;
			Score		= 0;
			LastScore	= 0;
			StarState	= 0;
		}
	};

	unsigned char		AnsCode;
	MEMBER				Member[eMax_Search_Count];
	unsigned char		isLastPacket;				// 0 : false, 1:last(true);
	unsigned char		AnsQueryCount;

	ANS_WEEKLY_RANKING()
	{
		ptcl = PTCL_ANS_WEEKLY_RANKING;
		size = sizeof(*this);
	}
};

struct REQ_FRIEND_STAR_LIST : PACKET
{
	unsigned short	Page;
	
	REQ_FRIEND_STAR_LIST()
	{
		ptcl = PTCL_REQ_FRIEND_STAR_LIST;
		size = sizeof(*this);
	}
};

struct ANS_FRIEND_STAR_LIST : PACKET
{
	enum eMaxSearch
	{
		eMax_Search_Count = 10,
	};

	enum eStarState
	{
		eTransmissive = 0, eAlreadySend,  eRefuse
	};

	struct MEMBER
	{
		long long		Ukey;
		unsigned char	StarState;

		MEMBER()
		{
			Ukey		= 0;
			StarState	= 0;
		}
	};

	MEMBER			Member[eMax_Search_Count];
	unsigned short	FriendCount;
	unsigned short	CurPage;
	unsigned short	LastPage;

	ANS_FRIEND_STAR_LIST()
	{
		ptcl = PTCL_ANS_FRIEND_STAR_LIST;
		size = sizeof(*this);
	}
};

struct REQ_MEDAL_RECORDING : PACKET
{
	enum eMedalClass{ eFirstClass = 0, eSecondClass, eThirdClass };

	unsigned char MedalClass;

	REQ_MEDAL_RECORDING()
	{
		ptcl = PTCL_REQ_MEDAL_RECORDING;
		size = sizeof(*this);
	}
};

struct REQ_UPDATE_STAR_COUNT : PACKET
{
	REQ_UPDATE_STAR_COUNT()
	{
		ptcl = PTCL_REQ_UPDATE_STAR_COUNT;
		size = sizeof(*this);
	}
};
struct ANS_UPDATE_STAR_COUNT : PACKET
{
	int				MyStarCount;
	long long		MyStarChargingTime;

	unsigned char	SendablePremiumStarCount;

	ANS_UPDATE_STAR_COUNT()
	{
		ptcl = PTCL_ANS_UPDATE_STAR_COUNT;
		size = sizeof(*this);
	}
};

//////////////////////////////////////////////////////////////////////////
// Post Box 
struct REQ_POST_BOX : PACKET
{
	// 1 ~ 10
	unsigned char Page;

	REQ_POST_BOX()
	{
		ptcl = PTCL_REQ_POST_BOX;
		size = sizeof(*this);
	}
};

struct UD_POST
{
	int			No;							// Unique Key
	int			Count;						// Star Count
	long long	DeliveryDateTime;			// Delivery Date Time( YYYY/MM/DD/hh/mm/ss )
	char		SenderID[UD_ID_LEN];		// ID
	char		SenderName[UD_NICK_LEN];	// Name

	UD_POST()
	{
		No					= 0;
		Count				= 0;
		DeliveryDateTime	= 0;
		SenderID[0]			= 0;
		SenderName[0]		= 0;
	}
};
struct ANS_POST_BOX : PACKET
{
	// 1 ~ 10
	unsigned char	Page;
	unsigned char	PageCount;

	UD_POST			Post[10];
	unsigned char	PostCount;

	ANS_POST_BOX()
	{
		ptcl = PTCL_ANS_POST_BOX;
		size = sizeof(*this);
	}
};

struct REQ_RECEIVE_POST : PACKET
{
	int PostNo;		// UD_POST::No

	REQ_RECEIVE_POST()
	{
		ptcl = PTCL_REQ_RECEIVE_POST;
		size = sizeof(*this);
	}
};

struct ANS_RECEIVE_POST : PACKET
{
	enum			eAnsCode { eSuccess = 0, eFail };
	unsigned char	AnsCode;

	int				PostNo;		// UD_POST::No
	int				MyStarCount;	// use if AnsCode = eSuccess
	int				MyTotalPostCount;

	ANS_RECEIVE_POST()
	{
		ptcl = PTCL_ANS_RECEIVE_POST;
		size = sizeof(*this);
	}
};

struct REQ_RECEIVE_POST_ALL : PACKET
{
	REQ_RECEIVE_POST_ALL()
	{
		ptcl = PTCL_REQ_RECEIVE_POST_ALL;
		size = sizeof(*this);
	}
};

struct ANS_RECEIVE_POST_ALL : PACKET
{
	enum			eAnsCode { eSuccess = 0, eFail };
	unsigned char	AnsCode;

	int				MyStarCount;	// use if AnsCode = eSuccess
	int				MyTotalPostCount;

	ANS_RECEIVE_POST_ALL()
	{
		ptcl = PTCL_ANS_RECEIVE_POST_ALL;
		size = sizeof(*this);
	}
};

struct REQ_SEND_POST : PACKET
{
	// eUDPostType
	unsigned char	Type;
	int				ItemID;
	int				Count;

	long long		TargetUKey;
	char			TargetID[UD_ID_LEN];

	REQ_SEND_POST()
	{
		ptcl = PTCL_REQ_SEND_POST;
		size = sizeof(*this);
	}
};

struct ANS_SEND_POST : PACKET
{
	enum			eAnsCode { eSuccess = 0, eFail, eFail_Refuse, eFail_Time, eFail_Type, eFail_ShortagePremiumStarCount };
	unsigned char	AnsCode;

	// eUDPostType
	unsigned char	Type;
	int				ItemID;
	int				Count;

	long long		TargetUKey;
	char			TargetID[UD_ID_LEN];

	ANS_SEND_POST()
	{
		ptcl = PTCL_ANS_SEND_POST;
		size = sizeof(*this);
	}
};

//////////////////////////////////////////////////////////////////////////
/* Event */
struct REQ_EVENT_RECORDING :PACKET
{
	enum ePurpose
	{
		eCheck = 0,
		eSave,
	};

	int			EventID;
	int			Purpose;

	REQ_EVENT_RECORDING()
	{
		ptcl = PTCL_REQ_EVENT_RECORDING;
		size = sizeof(*this);
	}
};

struct ANS_EVENT_RECORDING : PACKET
{
	enum eAnsCode
	{
		eSuccess = 0,
		eFail,
	};

	int				EventID;
	unsigned char	AnsCode;

	ANS_EVENT_RECORDING()
	{
		ptcl = PTCL_ANS_EVENT_RECORDING;
		size = sizeof(*this);
	}
};

//////////////////////////////////////////////////////////////////////////
/* Client Log */
struct REPORTING_LOG : PACKET
{
	char Log[1024];

	REPORTING_LOG()
	{
		ptcl = PTCL_REPORTING_LOG;
		size = sizeof(*this);
	};
};

//////////////////////////////////////////////////////////////////////////
/* Notification */
struct GAME_NOTIFICATION : PACKET
{
	unsigned char		bType;				// 0: 일반 1:긴급 2:게시
	long long	llTime;				// 예약시간 (예) 2010년 1월 1일 13시 10분 => 201001011310)
	int			iDisplayTime;		// 표시시간 (초단위)
	unsigned char		bLinkType;			// 0:없음 1: 게임링크 2: 웹링크
	char		szLink[128];		// 링크 ("none","main","profile","inventory","shop" or http://.....)
	char		szTitle[64];		// 제목 (타입이 게시 일때만 유효함)
	char		szMessage[512];		// 내용

	GAME_NOTIFICATION()
	{
		ptcl = PTCL_GAME_NOTIFICATION;
		size = sizeof(*this);
	};
};

//////////////////////////////////////////////////////////////////////////
struct NOTIFY_ITEM_EXPIRE : PACKET
{
	enum eMaxItem
	{
		eMax_Item = 10,
	};

	int		ItemSeq[eMax_Item];

	NOTIFY_ITEM_EXPIRE()
	{
		ptcl = PTCL_NOTIFY_ITEM_EXPIRE;
		size = sizeof(*this);
	};
};

struct REQ_FRIEND_MATCH_IN_GAME : PACKET
{
	enum eMaxSearch
	{
		eMax_Search_Count = 10,
	};

	char		FriendID[eMax_Search_Count][UD_ID_LEN];				// Name(Facebook)

	REQ_FRIEND_MATCH_IN_GAME()
	{
		ptcl = PTCL_REQ_FRIEND_MATCH_IN_GAME;
		size = sizeof(*this);
	};
};

struct ANS_FRIEND_MATCH_IN_GAME : PACKET
{
	enum eMaxSearch
	{
		eMax_Search_Count = 10,
	};

	enum eAnsCode
	{
		eSuccess = 0,
		eFail,
	};

	unsigned char AnsCode; 

	char		FriendID[eMax_Search_Count][UD_ID_LEN];				// Name(Facebook) 
	unsigned char IsFriend[eMax_Search_Count];

	ANS_FRIEND_MATCH_IN_GAME()
	{
		ptcl = PTCL_ANS_FRIEND_MATCH_IN_GAME;
		size = sizeof(*this);
	};
};

struct REQ_DELETE_FRIEND : PACKET
{
	enum eReqType
	{
		eDelete_Friend=0,
		eUpdate_Friend,
	};

	unsigned char	ReqType;
	long long		FriendUKey;

	REQ_DELETE_FRIEND()
	{
		ptcl = PTCL_REQ_DELETE_FRIEND;
		size = sizeof(*this);
	};
};

struct ANS_DELETE_FRIEND : PACKET
{
	enum eReqType
	{
		eDelete_Friend=0,
		eUpdate_Friend,
	};

	unsigned char	ReqType;
	UD_FRIEND		Friend;

	enum eAnsCode
	{
		eSuccess = 0,
		eFail,
		eFail_NoMember,
		eFail_Unknown_Req,
	};

	unsigned char	AnsCode;
	long long		FriendUKey;

	ANS_DELETE_FRIEND()
	{
		ptcl = PTCL_ANS_DELETE_FRIEND;
		size = sizeof(*this);
	};
};

/* Upgrade Item */
struct REQ_UPGRADE_ITEM : PACKET
{
	int ItemSeqNo[4];					// Item Unique Number

	REQ_UPGRADE_ITEM()
	{
		ptcl = PTCL_REQ_UPGRADE_ITEM;
		size = sizeof(*this);

		ItemSeqNo[0] = 0;
		ItemSeqNo[1] = 0;
		ItemSeqNo[2] = 0;
		ItemSeqNo[3] = 0;
	};
};
struct ANS_UPGRADE_ITEM : PACKET
{
	enum eAnsCode
	{
		eSuccess = 0,
		eFail_Balance,					// Not Enough Money
		eFail_MaxLevel,					// Base Unit Level is MAX
		eFail_BaseIDisNull,				// Base Item ID is Null
		eFail_SubIDisNull,				// Sub Item ID is Null
		eFail
	};
	unsigned char	AnsCode;

	int				ItemSeqNo[4];		// Item Unique Number
	UD_ITEM			Item;				// Upgrade Base Item
	int				GP;					// Remaining Game Money

	ANS_UPGRADE_ITEM()
	{
		ptcl = PTCL_ANS_UPGRADE_ITEM;
		size = sizeof(*this);
	};
};

#pragma pack(pop)
