
#include "UserInfo.h"

#include "OnlineHandler.h"
//#include "GlobalInclude.h"
//#include "GameInfo.h"
//#include "MessageHandler.h"
//#include "MessageDefine.h"

//##############################################################################################
// NGroupRank
//##############################################################################################
NGroupRank::NGroupRank()
{
    memset( Name, 0, UD_NICK_LEN);
    UserKey = 0;
    
    WinCount = 0;
    LoseCount = 0;
    
    LadderPoint = 0;
    
    Ranking = 0;
    Portrait = 0;
}

//##############################################################################################
// NAchievement
//##############################################################################################
NAchievement::NAchievement()
: ID( 0 )
, bSuccess( false )
, Year( 0 )
, Month( 0 )
, Day( 0 )
{
}

//##############################################################################################
// GUserInfo
//##############################################################################################

CUserInfo* GUserInfo=0;

//##############################################################################################
// CUserInfo
//##############################################################################################
CUserInfo* CUserInfo::Instance() 
{ 
	static CUserInfo StaticInstance;
	return &StaticInstance; 
}

CUserInfo::CUserInfo()
{
	ResetAll();
}

CUserInfo::~CUserInfo()
{
	ResetAll();
}

void CUserInfo::ResetAll()
{
	StarCount = 0;
	PremiumStarCount = 0;
	PurchaseItemCount = 0;

	ResetAccount();
	ResetRecord();
	ResetInventory();
	ResetDeck();
	ResetReward();
	///ResetMissionHistories();
	ResetExpireItem();
	//ResetQuickItemSlot();
    
    ResetGroupRanks();
}

void CUserInfo::ResetExpireItem()
{
	for( int i=0; i<eMax_Item; i++)
		ExpireItem[i] = 0;
};

void CUserInfo::ResetAccount()
{
	memset( Nick, 0, UD_NICK_LEN ); 
	Gem = 0;
	GP = 0;
	Status = 0;
    FriendStatus = 0;
}

void CUserInfo::ResetRecord()
{
	Portrait = 0;
	Level = 0;
	Exp = 0;
	ExpMax = 0;
	League = 0;
	Group = 0;
	WinCount = 0;			
	LoseCount = 0;		
	GroupRank = 0;
	LadderPoint = 0;
	ChangeLP = 0;
	ChangeGP = 0;
	ChangeExp = 0;
	ChangeGP = 0;
	AchieveCount = 0;
	ChangeTargetRankerIndex = -1;

	bMatchInvitation = false;

	bAllowAlarm = false;

}

bool CUserInfo::CanMatchInvitation()
{
	return bMatchInvitation;
}

void CUserInfo::ResetInventory()
{
	if( UnitItems.Num() > 0 )
	{
		for( int i = UnitItems.Num()-1; i >= 0; i-- )
		{
			NInventoryItem* TargetItem = UnitItems(i);
			if( TargetItem ) delete TargetItem;

			UnitItems.Remove(i);
		}

		UnitItems.Empty();
	}

	UnitItemsPage = 0;
	UnitItemsPageCount = 0;

	if( CastleItems.Num() > 0 )
	{
		for( int i = CastleItems.Num()-1; i >= 0; i-- )
		{
			NInventoryItem* TargetItem = CastleItems(i);
			if( TargetItem ) delete TargetItem;

			CastleItems.Remove(i);
		}

		CastleItems.Empty();
	}

	CastleItemsPage = 0;
	CastleItemsPageCount = 0;

}

void CUserInfo::InsertUnitItem( NInventoryItem& InItem , NInventoryItem* PrevItem /*= NULL*/ )
{	
	if( UnitItems.Num() > 0 )
	{
		for( int i = 0; i < UnitItems.Num(); i++ )
		{
			NInventoryItem* TargetItem = UnitItems(i);
			if( TargetItem && TargetItem->ItemSeqNo == InItem.ItemSeqNo )
			{
				if( NULL != PrevItem )
					*PrevItem = *TargetItem;
				TargetItem->Copy( &InItem );
				return;
			}
		}
	}

	NInventoryItem* NewItem = new NInventoryItem;
	NewItem->Copy( &InItem );

	UnitItems.AddItem( NewItem );
}

void CUserInfo::RemoveUnitItem( int InItemSeqNo )
{
	if( UnitItems.Num() > 0 )
	{
		for( int i = 0; i < UnitItems.Num(); i++ )
		{
			NInventoryItem* TargetItem = UnitItems(i);
			if( TargetItem && TargetItem->ItemSeqNo == InItemSeqNo )
			{
				TargetItem->ItemCount--;
				if( TargetItem->ItemCount <= 0 )
				{
					UnitItems.Remove(i);
					delete TargetItem;

					// remove slot item
					RemoveDeckItem( InItemSeqNo );

					return;
				}
			}
		}
	}
}


void CUserInfo::UpdateUnitItem( int InItemSeqNo, int InLevel, int InGradePoint )
{
	if( UnitItems.Num() > 0 )
	{
		for( int i = 0; i < UnitItems.Num(); i++ )
		{
			NInventoryItem* TargetItem = UnitItems(i);
			if( TargetItem && TargetItem->ItemSeqNo == InItemSeqNo )
			{
				
				UnitItems(i)->ItemLevel = InLevel;
				UnitItems(i)->ItemGradePoint = InGradePoint;

					// remove slot item

					return;
				
			}
		}
	}
}


void CUserInfo::RemoveCastleItem( int InItemSeqNo )
{
	for( int i = 0; i < CastleItems.Num(); i++ )
	{
		NInventoryItem* TargetItem = CastleItems(i);
		if( TargetItem && TargetItem->ItemSeqNo == InItemSeqNo )
		{
			TargetItem->ItemCount--;
			if( TargetItem->ItemCount <= 0 )
			{
				CastleItems.Remove(i);
				delete TargetItem;

				break;
			}
		}
	}

	if ( CastleItems(0) )
	{
		NInventoryItem* TargetItem = CastleItems(0);
		DeckCastle.ItemSeqNo = TargetItem->ItemSeqNo;
		DeckCastle.ItemID = TargetItem->ItemID;
	}
}

//void CUserInfo::RemoveQuickItem( int InItemSeqNo )
//{
//	for( int i = 0; i < QuickItems.Num(); i++ )
//	{
//		NInventoryItem* TargetItem = QuickItems(i);
//		if( TargetItem && TargetItem->ItemSeqNo == InItemSeqNo )
//		{
//			TargetItem->ItemCount--;
//			if( TargetItem->ItemCount <= 0 )
//			{
//				QuickItems.Remove(i);
//				delete TargetItem;
//
//				break;
//			}
//		}
//	}
//
//	if ( QuickItems(0) )
//	{
//		NInventoryItem* TargetItem = QuickItems(0);
//		DeckQuick.ItemSeqNo = TargetItem->ItemSeqNo;
//		DeckQuick.ItemID = TargetItem->ItemID;
//	}
//}

NInventoryItem* CUserInfo::GetUnitItem( int InItemSeqNo )
{
	if( UnitItems.Num() > 0 )
	{
		for( int i = 0; i < UnitItems.Num(); i++ )
		{
			NInventoryItem* TargetItem = UnitItems(i);
			if( TargetItem && TargetItem->ItemSeqNo == InItemSeqNo )
			{
				return TargetItem;
			}
		}
	}

	return 0;
}

NInventoryItem* CUserInfo::GetUnitItemByID( int InItemID, int nitemCategory /*= -1*/ )
{
	if( UnitItems.Num() > 0 )
	{
		for( int i = 0; i < UnitItems.Num(); i++ )
		{
			NInventoryItem* TargetItem = UnitItems(i);
			if( TargetItem && TargetItem->ItemID == InItemID )
			{
				if( -1 == nitemCategory || TargetItem->Category == nitemCategory )
					return TargetItem;
			}
		}
	}

	return 0;
}

void CUserInfo::RemoveDeckItem( int itemSeqNo )
{
	for( int i = 0; i < UD_UNIT_SLOT_COUNT; i++ )
	{
		if( DeckUnit[i].ItemSeqNo == itemSeqNo )
		{
			DeckUnit[i].ItemID = 0;
			DeckUnit[i].ItemSeqNo = 0;

			break;
		}
	}
}


void CUserInfo::InsertCastleItem( NInventoryItem& InItem, NInventoryItem* PrevItem /*= NULL*/ )
{
	if( CastleItems.Num() > 0 )
	{
		for( int i = 0; i < CastleItems.Num(); i++ )
		{
			NInventoryItem* TargetItem = CastleItems(i);
			if( TargetItem && TargetItem->ItemSeqNo == InItem.ItemSeqNo )
			{
				if( NULL != PrevItem )
					*PrevItem = *TargetItem;
				TargetItem->Copy( &InItem );
				return;
			}
		}
	}

	NInventoryItem* NewItem = new NInventoryItem;
	NewItem->Copy( &InItem );
	CastleItems.AddItem( NewItem );
}

void CUserInfo::InsertCastleItem( NInventoryItem& InItem , int category, NInventoryItem* PrevItem /*= NULL*/ )
{
	if( CastleItems.Num() > 0 )
	{
		for( int i = 0; i < CastleItems.Num(); i++ )
		{
			NInventoryItem* TargetItem = CastleItems(i);
			if( TargetItem && TargetItem->ItemSeqNo == InItem.ItemSeqNo && TargetItem->Category == InItem.Category )
			{
				if( NULL != PrevItem )
					*PrevItem = *TargetItem;
				TargetItem->Copy( &InItem );
				return;
			}
		}
	}

	NInventoryItem* NewItem = new NInventoryItem;
	NewItem->Copy( &InItem );
	CastleItems.AddItem( NewItem );
}

NInventoryItem* CUserInfo::GetCastleItem( int InItemSeqNo )
{
	if( CastleItems.Num() > 0 )
	{
		for( int i = 0; i < CastleItems.Num(); i++ )
		{
			NInventoryItem* TargetItem = CastleItems(i);
			if( TargetItem && TargetItem->ItemSeqNo == InItemSeqNo )
			{
				return TargetItem;
			}
		}
	}

	return 0;
}

NInventoryItem* CUserInfo::GetCastleItemByID( int InID , int itemCategory )
{
	if( CastleItems.Num() > 0 )
	{
		for( int i = 0; i < CastleItems.Num(); i++ )
		{
			NInventoryItem* TargetItem = CastleItems(i);
			if( TargetItem && TargetItem->ItemID == InID && TargetItem->ItemCount > 0 && TargetItem->Category == itemCategory )
			{
				return TargetItem;
			}
		}
	}

	return 0;
}

int	 CUserInfo::GetDeckCount()
{
	int nCount = 0;
	for( int i=0; i<UD_UNIT_SLOT_COUNT; i++ )
	{
		if( GUserInfo->DeckUnit[i].ItemID > 0 )
			nCount++;
	}

	return nCount;
}

void CUserInfo::ResetDeck()
{
	memset( DeckUnit, 0, sizeof(NDeckSlot)*UD_UNIT_SLOT_COUNT );
	memset( &DeckCastle, 0, sizeof(NDeckSlot) );

	DeckPoint = 0;
}

void CUserInfo::ResetReward()
{
	Rank = 0;

	memset( &RewardItem, 0, sizeof(NRewardItem) );
	memset( FakeItem, 0, sizeof(NRewardItem)*REWARD_MAX_FAKE_ITEM );
}

/*
void CUserInfo::ResetMissionHistories()
{
	if( MissionHistories.Num() > 0 )
	{
		for( int i = MissionHistories.Num()-1; i >= 0; i-- )
		{
			NMissionHistory* TargetHistory = MissionHistories(i);
			MissionHistories.Remove(i);
			if( TargetHistory ) delete TargetHistory;
		}

		MissionHistories.Empty();
	}

	bChangeMissionScore_NewRecord = false;
}

NMissionHistory* CUserInfo::InsertMissionHistory( NMissionHistory* InHistory )
{
	if( !InHistory ) return 0;
    
	if( MissionHistories.Num() > 0 )
	{
		for( int i = 0; i < MissionHistories.Num(); i++ )
		{
			NMissionHistory* TargetHistory = MissionHistories(i);
			if( TargetHistory && TargetHistory->MissionNo == InHistory->MissionNo )
			{
				TargetHistory->Copy( InHistory );
				return TargetHistory;
			}
		}
	}
    
	MissionHistories.AddItem( InHistory );
    
	return InHistory;
}

NMissionHistory* CUserInfo::GetMissionHistory( int InMissionNo )
{
	if( MissionHistories.Num() > 0 )
	{
		for( int i = 0; i < MissionHistories.Num(); i++ )
		{
			NMissionHistory* TargetHistory = MissionHistories(i);
			if( TargetHistory && TargetHistory->MissionNo == InMissionNo )
			{
				return TargetHistory;
			}
		}
	}

	return 0;
}

NMissionHistory* CUserInfo::GetNextMissionHistory( int InMissionNo )
{
	CMissionData* NextMissionData = GGameData->GetOffsetMissionData( GGameData->GetMissionData( InMissionNo ) );

	if( NextMissionData )
	{
		int NextMissionNo = NextMissionData->m_MissionNo;

		for( int i = 0; i < MissionHistories.Num(); i++ )
		{
			NMissionHistory* TargetHistory = MissionHistories(i);
			if( TargetHistory && TargetHistory->MissionNo == NextMissionNo )
			{
				return TargetHistory;
			}
		}
	}

	return 0;
}

bool CUserInfo::UpdateMissionInfo( NMissionHistory* pHistory, int nLock, int InRank, int nScore  )
{
	if( NULL == pHistory )	return false;

	bool bChangeSocre = false;
	if( pHistory->BestScore < nScore )
	{
		bChangeSocre = true;
		pHistory->Score = nScore;
	}

	pHistory->Rank = (pHistory->Rank < InRank) ? InRank : pHistory->Rank;
	pHistory->Lock			= nLock;

	return bChangeSocre;
}

bool CUserInfo::UpdateMissionProcess( int InMissionNo , int InRank, int nScore )
{
    ////  TO BE IMPLEMENTED.....
    
    

	// 2013-03-12 12:15 plan to fix(temp code)
	//----------------------------------------------------------------------------------------------
	// Update Mission History
	//----------------------------------------------------------------------------------------------
	CMissionData* pCurrData = GGameData->GetMissionData( InMissionNo );
	if( NULL == pCurrData )	return false;

	//	Update Current Mission History
	NMissionHistory* pCurrHis = GUserInfo->GetMissionHistory( pCurrData->m_MissionNo );
	if( NULL == pCurrHis )
		pCurrHis = new NMissionHistory( pCurrData->m_MissionNo );

	bChangeMissionScore_NewRecord = UpdateMissionInfo( pCurrHis, 1, InRank, nScore );
	pCurrHis = GUserInfo->InsertMissionHistory( pCurrHis );

	//----------------------------------------------------------------------------------------------
	// Update Next Mission Level Unlock
	//----------------------------------------------------------------------------------------------
	if( ESMML_Hard != GGameInfo->SelectMissionLevel && InRank > eUDMR_C )
	{
		NMissionHistory* pNextHis = GUserInfo->GetMissionHistory( pCurrData->m_MissionNo + ESMML_Interval );
		if( NULL == pNextHis )
			pNextHis = new NMissionHistory( pCurrData->m_MissionNo + ESMML_Interval );
		UpdateMissionInfo( pNextHis, 1, 0, 0 );
		pNextHis = GUserInfo->InsertMissionHistory( pNextHis );
	}
	
	//----------------------------------------------------------------------------------------------
	// Update Next Mission & Theme Unlock
	//----------------------------------------------------------------------------------------------
	if( ESMML_Easy == GGameInfo->SelectMissionLevel )
	{
		CMissionData* pNextData = NULL;
		// 안에 sorting 을 하기 때문에 테마오픈 수정해야함
		// 2013-03-12
		if( 8 == pCurrData->m_Number )
		{
			pNextData = GGameData->GetOffsetMissionData( pCurrData, 17 );
			if( NULL == pNextData )	return false;
		}
		else
		{
			pNextData = GGameData->GetOffsetMissionData( pCurrData, 1 );
			if( NULL == pNextData )	return false;
		}
	
		//	Open Next Mission
		if( pCurrData != pNextData && InRank > eUDMR_C )
		{
			// Update Next Mission Unlock
			bool bNewOpenTheme = false;
			NMissionHistory* pHis = GUserInfo->GetMissionHistory( pNextData->m_MissionNo );
			if( NULL == pHis )
			{
				pHis = new NMissionHistory( pNextData->m_MissionNo );
				bNewOpenTheme = true;
			}
		
			UpdateMissionInfo( pHis, 1, 0, 0 );
			pHis = GUserInfo->InsertMissionHistory( pHis );

			GGameInfo->SetMissionAllClear( false );

			// Update Next Theme Unlock
			//	Open Next Theme Message
			if( bNewOpenTheme && NULL != pNextData && pCurrData->m_MapTheme != pNextData->m_MapTheme )
			{
				CThemeData *pThemeData = NULL;	
			
				pThemeData = GGameData->GetThemeData( pNextData->m_MapTheme );		
			
				if( NULL != pThemeData )
				{
					GGameInfo->SetMissionAllClear( true );
					GGameInfo->SetThemeID( pThemeData->m_ThemeNo );
					//GGameInfo->ThemeUpMessage( pThemeData );
				}
			}
		}
	}

	return true;
}		

bool 
CUserInfo::IsFirstClearMission( int InMissionNo )
{
	NMissionHistory* TargetHistory = GetMissionHistory( InMissionNo );

	if( TargetHistory && TargetHistory->Rank > eUDMR_F )
	{
		return false;
	}

	return true;
} */

//void CUserInfo::RemoveFriend( long long InUkey )
//{
//	if( MyFriends.Num() > 0 )
//	{
//		for( int i = 0; i < MyFriends.Num(); i++ )
//		{
//			NFriend* TargetItem = MyFriends(i);
//			if( TargetItem && TargetItem->UKey == InUkey )
//			{
//				MyFriends.Remove(i);
//				delete TargetItem;
//				return;
//			}
//		}
//	}
//}

void CUserInfo::SetExpireItem( int* pArrItem )
{
	for( int i=0; i<eMax_Item; i++)
		ExpireItem[i] = pArrItem[i];
}
/*
void CUserInfo::ShowExpireItemBox()
{
	CScene* pScene = IUSceneManager().GetCurrentScene();
	if( pScene->SceneID < eID_Scene_Main )
		return;

	int lastIndex = 0;

	for ( int i = 0 ; i < eMax_Item ; i++ )
	{
		if ( ExpireItem[i] != 0 )
			++lastIndex;
		else
			break;
	}

	if( lastIndex == 0 )
		return;

	for (int i = 0 ; i < lastIndex ; i++ )
	{
		int buttonType = EMBA_OK | EMBA_Cancel;

		if( 0 == ExpireItem[i] )
			continue;

		NInventoryItem* pInvenItem = NULL;
		NInventoryItem* pInvenCastle = NULL;

		pInvenItem = GUserInfo->GetUnitItem( ExpireItem[i] );
		pInvenCastle = GUserInfo->GetCastleItem( ExpireItem[i] );

		if ( i == (lastIndex-1) )
			buttonType |= EMBA_Option;

		if (pInvenItem)
		{
			ShowExpireDateMessage( EMBID_ExpireDate, buttonType, pInvenItem->ItemID, pInvenItem->ExpireTime);
			GUserInfo->RemoveUnitItem(ExpireItem[i]);
		}

		if (pInvenCastle)
		{
			ShowExpireDateMessage( EMBID_ExpireDate, buttonType, pInvenCastle->ItemID, pInvenCastle->ExpireTime);
			GUserInfo->RemoveCastleItem(ExpireItem[i]);
		}

		ExpireItem[i] = 0;
	}
} */

void CUserInfo::ResetQuickItemSlot()
{
	memset( &QuickItemSlot, 0, sizeof( NItemSlot ) * UD_ITEM_SLOT_COUNT ); 
}

void CUserInfo::RemoveQuickItemSlot( const int& iPDKey )
{
	for( int i = 0; i < UD_ITEM_SLOT_COUNT; i++ )
	{
		if( QuickItemSlot[i].iPDKey == iPDKey )
		{
			QuickItemSlot[i].iItemID	= 0;
			QuickItemSlot[i].iSeqNum	= 0;
			QuickItemSlot[i].iPDKey		= 0;
			break;
		}
	}
}
/*
bool CUserInfo::GetItemByFuncKey( int InFuncKey, int* OutValue )
{
	for( int i=0; i < UD_ITEM_SLOT_COUNT; i++ )
	{
		if( 0 == QuickItemSlot[i].iItemID )
			continue;

		const CTB_InstantItemData* pGetData = GGameData->GetInstantItemData( QuickItemSlot[i].iItemID );
		if( NULL == pGetData )	continue;

		if( InFuncKey != pGetData->GetFuncKey() )
			continue;

		OutValue[0] = pGetData->GetValue1();
		OutValue[1] = pGetData->GetValue2();

		return true;
	}

	return false;
} */

void CUserInfo::ResetGroupRanks()
{
    for( int i = 0; i < GroupRanks.Num(); i++ )
    {
        NGroupRank* TargetRank = GroupRanks(i);
        if( TargetRank )
            delete TargetRank;
    }
    
    GroupRanks.Empty();
    
    GroupRankUserKey = 0;
}

void CUserInfo::ResetAchievements()
{
    for( int i = 0; i < Achievements.Num(); i++ )
    {
        NAchievement* TargetAchieve = Achievements(i);
        if( TargetAchieve )
            delete TargetAchieve;
    }
    
    Achievements.Empty();
    
    AchievementUserKey = 0;
    /*
    // Achievement Data
    for( int i = 0 ; i < GGameData->GetAchievementDataCount() ; i++ )
    {
        const CAchievementData* AData = GGameData->GetAchievementData( i );
        if( AData )
        {
            NAchievement* NewAchievement = new NAchievement();
            NewAchievement->ID = AData->m_ID;
            
            Achievements.AddItem( NewAchievement );
        }
    } */
}

NAchievement* CUserInfo::GetAchievement( int InID )
{
    for( int i = 0; i < Achievements.Num(); i++ )
    {
        NAchievement* TargetAchieve = Achievements(i);
        if( TargetAchieve && TargetAchieve->ID == InID )
            return TargetAchieve;
    }
    
    return 0;
}
/*
bool CUserInfo::ChangeRankingScene()
{
	int nTotal = 0;
	if( MissionHistories.Num() > 0 )
	{
		for( int i = 0; i < MissionHistories.Num(); i++ )
		{
			NMissionHistory* TargetHistory = MissionHistories(i);
			if( NULL != TargetHistory  )
				nTotal += TargetHistory->Score;
		}
	}

	// term 1. WeeklyScore Change
	if( WeeklyScore < nTotal )
	{
		//if( GFriendsManager->KakaoAppFriends.Num() <= 0 )
		//	return false;


		//int nMyIndex = -1;
		//for (int index = 0; index < GFriendsManager->KakaoAppFriends.Num(); index++)
		//{
		//	if (GOnlineInfo->UserKey == GFriendsManager->KakaoAppFriends(index).KakaoFriendInfo->CnDUKey)
		//	{
		//		GFriendsManager->KakaoAppFriends(index).KakaoFriendInfo->Score = nTotal;
		//		nMyIndex = index;
		//		break;
		//	}
		//}
		//
		//WeeklyScore = nTotal;	

		//if  (nMyIndex > -1)
		//{
		//	// term 2. Search High Score
		//	for ( INT i = 0; i < nMyIndex; i++ )
		//	{
		//		if( GFriendsManager->KakaoAppFriends(i).KakaoFriendInfo->Score < GFriendsManager->KakaoAppFriends(nMyIndex).KakaoFriendInfo->Score )
		//		{	
		//			ChangeTargetRankerIndex = i;
		//			return true;
		//		}				
		//	
		//	}
		//}

	}

	return false;
} */
