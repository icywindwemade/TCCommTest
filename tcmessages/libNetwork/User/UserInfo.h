//##############################################################################################
// UserInfo.h
//##############################################################################################
#ifndef _USER_INFO_H_
#define _USER_INFO_H_

#include "Core.h"
#include "UnionDefine.h"
#include "BuildDefine.h"
//#include "GlobalDefine.h"


//##############################################################################################
//
//##############################################################################################

#define REWARD_MAX_FAKE_ITEM			2
#define INVENTORY_ITEM_COMBINE_COUNT	4
#define INVENTORY_ITEM_GRADE_COUNT	4

//##############################################################################################
//
//##############################################################################################
struct NInventoryItem
{
	int			ItemSeqNo;
	int			ItemID;
	int			ItemCount;
	int			ItemLevel;
	int			ItemGradePoint;
	int			Category;
	long long	ExpireTime;

	NInventoryItem()
		: ItemSeqNo( 0 )
		, ItemID( 0 )
		, ItemCount( 0 )
		, ItemLevel( 1 )
		, ItemGradePoint( 0 )
		, Category( 0 )
		, ExpireTime( 0 )
	{
	}

	void Copy( NInventoryItem* InSourceItem )
	{
		if( !InSourceItem ) return;

		ItemSeqNo = InSourceItem->ItemSeqNo;
		ItemID = InSourceItem->ItemID;
		ItemCount = InSourceItem->ItemCount;
		ItemLevel = InSourceItem->ItemLevel;
		ItemGradePoint = InSourceItem->ItemGradePoint;
		Category = InSourceItem->Category;
		ExpireTime = InSourceItem->ExpireTime;
	}
};

struct NRewardItem
{
	int Type;
	int ID;
	int Count;
	NRewardItem()
		: Type( 0 )
		, ID( 0 )
		, Count( 0 )
	{
	}
};

struct NDeckSlot
{
	int ItemSeqNo;
	int ItemID;
	NDeckSlot()
		: ItemSeqNo( 0 )
		, ItemID( 0 )
	{
	}
};

struct NMissionHistory
{
	int		MissionNo;
	int		Lock;
	int		Rank;
	int		Score;
	int		BestScore;

	NMissionHistory( int nMissionNo )
	{
		MissionNo	= nMissionNo;
		Lock		= 0;
		Rank		= 0;
		Score		= 0;
		BestScore   = 0;
	}

	void Copy( NMissionHistory* InHistory )
	{
		Lock = InHistory->Lock;
		Rank = InHistory->Rank;
		Score = InHistory->Score;
		BestScore = InHistory->BestScore;
	}
};

struct NPostBox
{
	int			No;							// Unique Key
	int			Count;						// Star Count
	long long	DeliveryDateTime;			// Delivery Date Time( YYYY/MM/DD/hh/mm/ss )
	char		SenderID[UD_ID_LEN];		// ID
	char		SenderName[UD_NICK_LEN];	// Name

//	FKakaoFriendInfo*	KakaoFriendInfo;
//	UBOOL				bDrawPortrait;
 
	NPostBox()
	{
		No					= 0;
		Count				= 0;
		DeliveryDateTime	= 0;
		SenderID[0]			= 0;
		SenderName[0]		= 0;

	//	bDrawPortrait = FALSE;
	//	KakaoFriendInfo =  NULL;
	}

	void Copy( NPostBox* InPost )
	{
		No = InPost->No;
		Count = InPost->Count;
		DeliveryDateTime = InPost->DeliveryDateTime;
		strcpy(SenderID,InPost->SenderID);
		strcpy(SenderName, InPost->SenderName);
	//	KakaoFriendInfo = InPost->KakaoFriendInfo;
	//	bDrawPortrait = InPost->bDrawPortrait;
	}
};

struct NItemSlot
{
	int iSeqNum;
	int iItemID;
	int iPDKey;
	NItemSlot()
		: iSeqNum( 0 )
		, iItemID( 0 )
		, iPDKey( 0 )
	{
	}
};

struct NGroupRank
{
    char			Name[UD_NICK_LEN];			// Platform Nickname
    long long		UserKey;
    
    int				WinCount;					// Total
    int				LoseCount;					// Total
    
    int				LadderPoint;
    
    int				Ranking;
    int				Portrait;
    
    NGroupRank();
};

struct NAchievement
{
	int     ID;
	bool    bSuccess;
    
	int     Year;
	int     Month;
	int     Day;
    
    NAchievement();
};

//##############################################################################################
// CUserInfo
//##############################################################################################
class CUserInfo
{
public:
	static CUserInfo* Instance();

public:
	CUserInfo();
	~CUserInfo();

	void ResetAll();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Account
	void ResetAccount();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Record
	void ResetRecord();
	bool CanMatchInvitation();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Inventory
	void ResetInventory();

	// Unit Items
	void InsertUnitItem( NInventoryItem& InItem, NInventoryItem* PrevItem = NULL );
	void RemoveUnitItem( int InItemSeqNo );
	void UpdateUnitItem( int InItemSeqNo, int InLevel, int InGradePoint );
	NInventoryItem* GetUnitItem( int InItemSeqNo );
	NInventoryItem* GetUnitItemByID( int InItemID, int nitemCategory = -1 );

	// Deck
	void RemoveDeckItem( int itemSeqNo );

	// Castle Items
	void InsertCastleItem( NInventoryItem& InItem , NInventoryItem* PrevItem = NULL );
	void InsertCastleItem( NInventoryItem& InItem , int category, NInventoryItem* PrevItem = NULL );
	void RemoveCastleItem( int InItemSeqNo );
	NInventoryItem* GetCastleItem( int InItemSeqNo );
	NInventoryItem* GetCastleItemByID( int InID , int itemCategory );

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Deck
	void ResetDeck();	
	int	 GetDeckCount();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Reward
	void ResetReward();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Mission History
//
//	void ResetMissionHistories();
//	NMissionHistory* InsertMissionHistory( NMissionHistory* InHistory );
//	NMissionHistory* GetMissionHistory( int InMissionNo );
//	NMissionHistory* GetNextMissionHistory( int InMissionNo );
//	
//	bool IsFirstClearMission( int InMissionNo );
//	bool UpdateMissionProcess( int InMissionNo , int InRank, int nScore );
//	bool UpdateMissionInfo( NMissionHistory* pHistory, int nLock, int InRank, int nScore  );
//	bool ChangeRankingScene();

	//////////////////////////////////////////////////////////////////////////////////////////////////////    
	// ExpireItem 
	void ResetExpireItem();
	void SetExpireItem( int* pArrItem );
	void ShowExpireItemBox();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// QuickItemSlot
	void RemoveQuickItemSlot( const int& iPDKey );
	void ResetQuickItemSlot();	
	bool GetItemByFuncKey( int InFuncKey, int* OutValue );
    
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// GroupRanks
    void ResetGroupRanks();
    
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Achievements
    void ResetAchievements();
    NAchievement* GetAchievement( int InID );

public:
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Account
	char						Nick[UD_NICK_LEN];
	int							GP;
	int							Gem;
	int							Status;    
	int							FriendStatus;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Record
	int							Portrait;
	int							Level;
	int							Exp;
	int							ExpMax;
	int							League;
	int							Group;
	int							WinCount;			
	int							LoseCount;		
	int							GroupRank;
	int							LadderPoint;
	int							ChangeLP;
	int							ChangeExp;
	int							ChangeGP;
	int							AchieveCount;

	int							BonusGP;		
	int							BonusExp;
	//novic_rank
	int							BestScore;						// Ranking Best Score
	int							WeeklyScore;					// Weekly Ranking Score;
	int							LastWeeklyScore;				// LastWeekly Ranking Score
	int							ChangeTargetRankerIndex;		// ChangeRankerIndex

	int							StarCount;						// My star count
	int							PremiumStarCount;				// Premium star count
	
	long long					PlatformUserKeyToSend;
	long long					GameUserKeyToSend;
	//
	bool						bMatchInvitation;
	bool						bAllowAlarm;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Inventory
	int							UnitInvenCount;
	int							UnitItemsPage;
	int							UnitItemsPageCount;
	TArray<NInventoryItem*>		UnitItems;
	int							CastleItemsPage;
	int							CastleItemsPageCount;
	TArray<NInventoryItem*>		CastleItems;
    //////////////////////////////////////////////////////////////////////////////////////////////////////
	// Store Purchase
	NInventoryItem				PurchaseItem[UD_MAX_PURCHASE_ITEM];
	int							PurchaseItemCount;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Deck
	NDeckSlot					DeckUnit[UD_UNIT_SLOT_COUNT];
	NDeckSlot					DeckCastle;	
	int							DeckPoint;
    
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// QuickItem
	NItemSlot					QuickItemSlot[UD_ITEM_SLOT_COUNT];
    
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Reward ( Game Result )
	int							Rank;
	NRewardItem					RewardItem;
	NRewardItem					FakeItem[REWARD_MAX_FAKE_ITEM];
	int							RewardErrorCode;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Mission History
	int b1;
	int b2;
	int b3;
	int b4;
	TArray<NMissionHistory*>	MissionHistories;
	bool						bChangeMissionScore_NewRecord;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Expire Item
	enum{eMax_Item = 10,};
	int							ExpireItem[eMax_Item];

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Group Ranks
	TArray<NGroupRank*>         GroupRanks;
    long long                   GroupRankUserKey;
    
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Achievements
	TArray<NAchievement*>       Achievements;
    long long                   AchievementUserKey;
    
};

extern CUserInfo* GUserInfo;
/*
IMPLEMENT_COMPARE_POINTER( NFriend, UserInfo,
{
	if ( A->Score > B->Score )
	{
		return 1;
	}

	return -1;
}
);*/

#endif // _USER_INFO_H_

