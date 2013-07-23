//##############################################################################################
// PacketRecver.cpp
//##############################################################################################
#include "PacketRecver.h"

#include "PacketSender.h"
#include "BuildDefine.h"
#include "OnlineHandler.h"
//#include "Client.h"
//#include "Telegram.h"
//#include "MessageHandler.h"
//#include "UserInfo.h"
//#include "GlobalInclude.h"
//#include "FriendsInfo.h"
//#include "TutorialInfo.h"



//##############################################################################################
// Profile
//##############################################################################################
RPHI( PTCL_ANS_PRIVATE_INFO					)
{
	ANS_PRIVATE_INFO* RecvPacket = (ANS_PRIVATE_INFO *)(InPacket->Data);

    FnLog("");
    FnLog("NET: RECV, ANS_PRIVATE_INFO");
    FnLog("--------------------------------");
    FnLog("Gem:%d"	  	        ,RecvPacket->Gem);
	FnLog("GP:%d"			,RecvPacket->GP);	//novic_money
	FnLog("LadderPoint:%d"		,RecvPacket->LadderPoint);
	FnLog("DeckPoint:%d"		,RecvPacket->DeckPoint);
	FnLog("bMatchInvitation:%d"	,RecvPacket->BattleInvitation);
	GOnlineInfo->bAllowMessage      = RecvPacket->PostReceive == 0 ? false : true;
    
    /*
     

	GUserInfo->Gem				= RecvPacket->Gem;
	GUserInfo->GP				=	RecvPacket->GP;	//novic_money
	GUserInfo->LadderPoint		= RecvPacket->LadderPoint;
	GUserInfo->DeckPoint			= RecvPacket->DeckPoint;
	GUserInfo->bMatchInvitation	= RecvPacket->BattleInvitation == 0 ? false : true;
	GOnlineInfo->bAllowMessage =  RecvPacket->PostReceive == 0 ? false : true; 
     */
}

RPHI( PTCL_ANS_BASE_INFO					)
{
	ANS_BASE_INFO *recvPacket = (ANS_BASE_INFO *)(InPacket->Data);

	FnLog("NET: RECV, ANS_BASE_INFO");
    FnLog("--------------------------------");
    FnLog("Name: %s", recvPacket->Name);
    FnLog("Portrait: %d", recvPacket->Portrait);
    FnLog("Level: %d", recvPacket->Level);
    FnLog("Exp: %d", recvPacket->Exp);
    FnLog("EndExp: %d", recvPacket->EndExp);
    FnLog("League: %d", recvPacket->League);
    FnLog("Leagroup: %d", recvPacket->LeagueGroup);
    FnLog("WinCount: %d", recvPacket->WinCount);
    FnLog("LoseCount: %d", recvPacket->LoseCount);
    FnLog("LeagueGroupRanking: %d", recvPacket->LeagueGroupRanking);
    FnLog("AchievementCompleteCount: %d", recvPacket->AchievementCompletionCount);
    
	if( GOnlineInfo->UserKey == recvPacket->UKey )
	{
		strcpy( GOnlineInfo->PlatformUserName, "" );
		strcpy( GOnlineInfo->UserNick, recvPacket->Name );

    }

/*
        strcpy( GUserInfo->Nick, recvPacket->Name );

		GUserInfo->Portrait		= recvPacket->Portrait;
		GUserInfo->Level			= recvPacket->Level;
		GUserInfo->Exp			= recvPacket->Exp;	
		GUserInfo->ExpMax		= recvPacket->EndExp;
		GUserInfo->League		= recvPacket->League;
		GUserInfo->Group		= recvPacket->LeagueGroup;
		GUserInfo->WinCount		= recvPacket->WinCount;
		GUserInfo->LoseCount	= recvPacket->LoseCount;
		GUserInfo->GroupRank	= recvPacket->LeagueGroupRanking;
		GUserInfo->AchieveCount = recvPacket->AchievementCompletionCount;

//		// send message to child scene - plan to fix bk
//		ProfilePage *pProfilePage = (ProfilePage *)IUSceneManager().GetCurrentScene()->GetChildScene(eID_Scene_Child_ProfilePage);
//		if (pProfilePage)
//		{
//			ProfileTab *pProfileTab = pProfilePage->GetProfileTab(EPTID_Profile);
//			if (pProfileTab) ((ProfileInfo *)pProfileTab)->NetworkBaseInfoSuccess( recvPacket );
//		}
        
		// Send Message
		Telegram* NewTelegram = new Telegram( 0, EOMC_UserDetail, 0, CustomMessage::Online );
		IUGameManager().Message( NewTelegram );
	}
	// Recveied FriendTab_FriendDetails
	else
	{
		GFriendsInfo->UserDetailProfile.AchievementCompletionCount	= recvPacket->AchievementCompletionCount;
		GFriendsInfo->UserDetailProfile.EndExp						= recvPacket->EndExp;
		GFriendsInfo->UserDetailProfile.Exp							= recvPacket->Exp;
		GFriendsInfo->UserDetailProfile.FriendStatus				= recvPacket->FriendStatus;
		GFriendsInfo->UserDetailProfile.League						= recvPacket->League;
		GFriendsInfo->UserDetailProfile.LeagueGroupRanking			= recvPacket->LeagueGroupRanking;
		GFriendsInfo->UserDetailProfile.Level						= recvPacket->Level;
		GFriendsInfo->UserDetailProfile.LoseCount					= recvPacket->LoseCount;
		strcpy( GFriendsInfo->UserDetailProfile.UserNick, recvPacket->Name );
		GFriendsInfo->UserDetailProfile.Portrait					= recvPacket->Portrait;
		GFriendsInfo->UserDetailProfile.Status						= recvPacket->Status;
		GFriendsInfo->UserDetailProfile.UserKey						= recvPacket->UKey;
		GFriendsInfo->UserDetailProfile.WinCount					= recvPacket->WinCount;

		// Send Message to FriendTab_FriendDetails
		Telegram* NewTelegram = new Telegram( 0, EOMC_FriendDetail, 0, CustomMessage::Online );
		IUGameManager().Message( NewTelegram );
	}

	

#if ( BK_NOT_USE == 0 )
	if( IUSceneManager().GetCurrentScene()->GetChildScene() != NULL )
	{
		if( IUSceneManager().GetCurrentScene()->GetChildScene(eID_Scene_Child_FriendPage) )
		{
			FriendPage* pScene = (FriendPage *)IUSceneManager().GetCurrentScene()->GetChildScene();

			if( pScene )
			{
				// message
				((FriendInfo*)(pScene->GetCurrentTab()))->RecieveFriendProfileSuccess( recvPacket );
			}
		}

		if( IUSceneManager().GetCurrentScene()->GetChildScene( eID_Scene_RankingInfo ) )
		{
			HideProgressMessage();

			CRankingInfoScene* pRankingInfo = (CRankingInfoScene*)IUSceneManager().GetCurrentScene()->GetChildScene();

			if( NULL == pRankingInfo )
				return;

//			pRankingInfo->SetFriendInfo(recvPacket->UKey,
//				recvPacket->BestScore,
//				recvPacket->WeeklyScore,
//				recvPacket->FirstClassCount,
//				recvPacket->SecondClassCount,
//				recvPacket->ThirdClassCount,
//				recvPacket->Level,
//				recvPacket->WinCount);

			FnLog("NET: RECV, ANS_WEEKLY_RANKING_BASE_INFO::eSuccess" );
		}
	}
#endif
 */
}

RPHI( PTCL_ANS_BATTLE_RECOMMENDATION		)
{
	//ANS_BATTLE_RECOMMENDATION *recvPacket = (ANS_BATTLE_RECOMMENDATION *)(InPacket->Data);

	FnLog("NET: RECV, ANS_BATTLE_RECOMMENDATION");
/*
    Telegram* NewMsg = new Telegram( 0, EOMC_BattleRecommends, 0, CustomMessage::Online );
    IUGameManager().Message( NewMsg ); */
}

RPHI( PTCL_ANS_BATTLE_HISTORY				)
{
	//ANS_BATTLE_HISTORY* recvPacket = (ANS_BATTLE_HISTORY *)(InPacket->Data);

	FnLog("NET: RECV, ANS_BATTLE_HISTORY"); /*
    
    Telegram* NewMsg = new Telegram( 0, EOMC_BattleHistory, 0, CustomMessage::Online );
    IUGameManager().Message( NewMsg ); */
}

RPHI( PTCL_ANS_GROUP_RANKING				)
{
	ANS_GROUP_RANKING * RecvPacket = (ANS_GROUP_RANKING *)(InPacket->Data);

	FnLog("NET: RECV, ANS_GROUP_RANKING %d/%d" , RecvPacket->Page, RecvPacket->PageCount );
   /*
    for( int i = 0; i < RecvPacket->MemberCount; i++ )
    {
        NGroupRank* NewRank = new NGroupRank;
        NewRank->UserKey = RecvPacket->Member[i].UKey;
        NewRank->WinCount = RecvPacket->Member[i].WinCount;
        NewRank->LoseCount = RecvPacket->Member[i].LoseCount;
        NewRank->LadderPoint = RecvPacket->Member[i].LadderPoint;
        NewRank->Ranking = RecvPacket->Member[i].Ranking;
        NewRank->Portrait = RecvPacket->Member[i].Portrait;
        memcpy( NewRank->Name, RecvPacket->Member[i].Name, UD_NICK_LEN );
        
        GUserInfo->GroupRanks.AddItem( NewRank );
    }
    
    int UserFlag = 1; // 1 is Other
    if( RecvPacket->UKey == GOnlineInfo->UserKey )
    {
        GUserInfo->League = RecvPacket->League;
        GUserInfo->Group = RecvPacket->LeagueGroup;
        GUserInfo->GroupRank = RecvPacket->LeagueGroupRanking;
        UserFlag = 0; // 0 is Owner(My Rank)
    }
    
    if( RecvPacket->Page < RecvPacket->PageCount )
    {
        OLRequestProfileGroupRanking( GUserInfo->GroupRankUserKey, RecvPacket->Page+1 );
        return;
    }
    
    Telegram* NewMsg = new Telegram( 0, EOMC_GroupRanks, UserFlag, CustomMessage::Online );
    IUGameManager().Message( NewMsg ); */
}

RPHI( PTCL_ANS_ACHIEVEMENT_HISTORY			)
{
	///ANS_ACHIEVEMENT_HISTORY * RecvPacket = (ANS_ACHIEVEMENT_HISTORY *)(InPacket->Data);

	FnLog("NET: RECV, ANS_ACHIEVEMENT_HISTORY");
    /*
	// on success
	for( int i = 0 ; i < RecvPacket->AchievementCount; i++ )
	{
        NAchievement* Target = GUserInfo->GetAchievement( RecvPacket->Achievement[i].ID );
        if( Target )
        {
            Target->bSuccess = true;
            
            getDates( RecvPacket->Achievement[i].CompletionDate, Target->Year, Target->Month, Target->Day );
        }
	}
    
	if( RecvPacket->Page < RecvPacket->PageCount )
    {
        OLRequestProfileAchievementHistory( GUserInfo->AchievementUserKey, RecvPacket->Page + 1 );
        
        return;
    }
    
    Telegram* NewMsg = new Telegram( 0, EOMC_Achievements, 0, CustomMessage::Online );
    IUGameManager().Message( NewMsg ); */
}

RPHI( PTCL_ANS_MISSION_HISTORY					)
{
	ANS_MISSION_HISTORY* RecvPacket = ( ANS_MISSION_HISTORY *)(InPacket->Data);

    FnLog("");
	FnLog("NET: RECV,  ANS_MISSION_HISTORY");


	///GUserInfo->ResetMissionHistories();

///	int nMissionNo = 0;

	for( int i = 0 ; i < RecvPacket->nMissionCount ; i++ )
	{
        /*
		CMissionData* pCurrData = NULL;
		pCurrData = GGameData->GetMissionData( RecvPacket->aMission[i].MissionNo );
		if( NULL == pCurrData )
			continue;

		NMissionHistory* NewHistory = new NMissionHistory( RecvPacket->aMission[i].MissionNo );
		NewHistory->Lock			= RecvPacket->aMission[i].Lock;
		NewHistory->Rank			= RecvPacket->aMission[i].Rank;
		NewHistory->Score			= RecvPacket->aMissionScore[i];
		NewHistory->BestScore		= RecvPacket->aMissionBestScore[i];

		NMissionHistory* ResultHistory = GUserInfo->InsertMissionHistory( NewHistory );

		if( ResultHistory != NewHistory ) delete NewHistory;

		if( 0 == i )	nMissionNo = NewHistory->MissionNo;
         */
        FnLog("--------------------------------");
        FnLog("Mission#:%d", RecvPacket->aMission[i].MissionNo);
        FnLog("Lock:%d", RecvPacket->aMission[i].Lock);
        FnLog("Rank:%d", RecvPacket->aMission[i].Rank);
        FnLog("Score:%d", RecvPacket->aMissionScore[i]);
        FnLog("Best Score:%d", RecvPacket->aMissionBestScore[i]);
        
	}

	// Update 
// 	{
// 		CMissionData* pCurrData = NULL;
// 		CMissionData* pNextData = NULL;
// 		NMissionHistory* pHistory = NULL;
// 
// 		//	제일 처음부터 순환
// 		pCurrData = GGameData->GetMissionData( nMissionNo );
// 		while( NULL != pCurrData ) 
// 		{
// 			//	내 저장소에서 히스토리 얻기
// 			//	1. NULL 이 아니면 내가 가지고 있는 미션정보이므로 손대지 않아도 된다. 
// 			//	2. NULL 이라면 다음 미션 정보를 만들어야 하므로 생성한다. 
// 			pHistory = GUserInfo->GetMissionHistory( pCurrData->m_MissionNo );
// 			if( NULL == pHistory )
// 			{
// 				NMissionHistory* NewHistory = new NMissionHistory;
// 				NewHistory->MissionNo		= pCurrData->m_MissionNo;
// 				NewHistory->Lock			= 0;
// 				NewHistory->Rank			= eUDMR_F;
// 				NewHistory->Score			= 0;
// 
// 				GUserInfo->InsertMissionHistory( NewHistory );
// 				break;
// 			}
// 
// 			pCurrData = GGameData->GetOffsetMissionData( pCurrData );
// 		}
// 	}  
}

RPHI( PTCL_ANS_EVENT_RECORDING				)
{
	ANS_EVENT_RECORDING* RecvPacket = ( ANS_EVENT_RECORDING *)(InPacket->Data);
    
	FnLog("\nNET: RECV,  PTCL_ANS_EVENT_RECORDING");
    FnLog("--------------------------------------");
    FnLog("EventID: %d\nAnsCode: %d", RecvPacket->EventID, RecvPacket->AnsCode);
    
	if( RecvPacket->AnsCode == ANS_EVENT_RECORDING::eSuccess )
	{
		if( RecvPacket->EventID == 1501 )
			GOnlineInfo->EventID2 = RecvPacket->EventID;
		else if( RecvPacket->EventID == 1502 )
{
			/// GTutorialInfo->DoneTutorial = true;
            FnLog("DoneTutorial=true.");
        }
		else
			GOnlineInfo->EventID = RecvPacket->EventID;
	}
	else
	{
		if( RecvPacket->EventID == 1501 )
			GOnlineInfo->EventID2 = 0;
		else if( RecvPacket->EventID == 1502 )
		{
		//	if( GTutorialInfo->DoneTutorial )
		//	GTutorialInfo->DoneTutorial = false;
            FnLog("DoneTutorial=false.");
		}
		else
			GOnlineInfo->EventID = 0;
	} 

    
}
