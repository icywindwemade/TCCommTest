//##############################################################################################
// PacketRecver.cpp
//##############################################################################################
#include "PacketRecver.h"

#include "PacketSender.h"
#include "BuildDefine.h"
/*
#include "OnlineHandler.h"
#include "Client.h"
#include "MessageHandler.h"
#include "PlayerInfo.h"
#include "GameInfo.h"
#include "UserInfo.h"
#include "GlobalInclude.h"
#include "GameScene.h"*/



enum eReward
{
	eR_Null,
	eR_Exp,
	eR_Gem,
	eR_Capsule,
	eR_Unit,
};

//##############################################################################################
// GameEnding
//##############################################################################################
RPHI( PTCL_EVENT_GAME_ENDING				)
{
	///EVENT_GAME_ENDING* RecvPacket = (EVENT_GAME_ENDING*)(InPacket->Data);

	FnLog("NET: RECV, EVENT_GAME_ENDING");  
/*
	if( GNetworkManager->IsSyncing() )
	{
	GNetworkManager->EndedSync();
	}

	CGameScene* pScene = (CGameScene*)IUSceneManager().GetCurrentScene( eID_Scene_Game );
	if( NULL != pScene )
		pScene->EndGame( RecvPacket->GameResult ); */
}

RPHI( PTCL_ANS_GAME_RECORDING				)
{
	///ANS_GAME_RECORDING* RecvPacket = (ANS_GAME_RECORDING*)(InPacket->Data);

	FnLog("NET: RECV, ANS_GAME_RECORDING");  /*

	ANS_GAME_RECORDING::PLAYER_RECORD* pRecordPacket; 

	//int PrevLevel = GUserInfo->Level;

	int RecordCount = MIN( RecvPacket->RecordCount, 4 );
	for( int i = 0; i < RecordCount; i++ )
	{
		pRecordPacket = &RecvPacket->Records[i];

		// For Multi
		CPlayerInfo* Player = GGameInfo->GetPlayer( pRecordPacket->UKey );
		if( Player )
		{
			//Player->Record.Portrait	= pRecordPacket->Portrait;
			Player->Record.WinCount		= pRecordPacket->WinCount;
			Player->Record.LoseCount		= pRecordPacket->LoseCount;
			Player->Record.GroupRank		= pRecordPacket->GroupRanking;
			Player->Record.LadderPoint	= pRecordPacket->LadderPoint;
			Player->Record.ChangeLP		= pRecordPacket->ChangeLP;
		}

		if( GOnlineInfo->UserKey == pRecordPacket->UKey )
		{
			GUserInfo->Exp				= pRecordPacket->CurExp;
			GUserInfo->ExpMax			= pRecordPacket->CurMaxExp;
			GUserInfo->Level				= pRecordPacket->CurLevel;
			GUserInfo->League			= pRecordPacket->League;
			GUserInfo->WinCount			= pRecordPacket->WinCount;
			GUserInfo->LoseCount			= pRecordPacket->LoseCount;
			GUserInfo->GroupRank			= pRecordPacket->GroupRanking;
			GUserInfo->LadderPoint		= pRecordPacket->LadderPoint;
			GUserInfo->ChangeLP			= pRecordPacket->ChangeLP;
	//		GUserInfo->ChangeGem			= pRecordPacket->ChangeGem;	//novic_money
			GUserInfo->ChangeExp			= pRecordPacket->ChangeExp;
			GUserInfo->ChangeGP			= pRecordPacket->ChangeGP;

			GUserInfo->BonusGP			= pRecordPacket->BonusGP;	//novic_money
			GUserInfo->BonusExp			= pRecordPacket->BonusExp;

			GUserInfo->GP				+= GUserInfo->ChangeGP;
			GUserInfo->GP				+= GUserInfo->BonusGP;
			if (GUserInfo->GP > UD_MAX_MONEY) GUserInfo->GP = UD_MAX_MONEY;

			GUserInfo->LadderPoint		+= GUserInfo->ChangeLP;

			GUserInfo->DeckPoint			= pRecordPacket->DeckPoint;

			// Reward Item
			GUserInfo->Rank					= pRecordPacket->Rank;
			GUserInfo->RewardItem.Type		= pRecordPacket->RewardArray[0].Reward;
			GUserInfo->RewardItem.ID			= pRecordPacket->RewardArray[0].RewardValue;
			GUserInfo->RewardItem.Count		= pRecordPacket->RewardArray[0].RewardCount;
			GUserInfo->FakeItem[0].Type		= pRecordPacket->RewardArray[1].Reward;
			GUserInfo->FakeItem[0].ID		= pRecordPacket->RewardArray[1].RewardValue;
			GUserInfo->FakeItem[0].Count		= pRecordPacket->RewardArray[1].RewardCount;
			GUserInfo->FakeItem[1].Type		= pRecordPacket->RewardArray[2].Reward;
			GUserInfo->FakeItem[1].ID		= pRecordPacket->RewardArray[2].RewardValue;
			GUserInfo->FakeItem[1].Count		= pRecordPacket->RewardArray[2].RewardCount;

			GUserInfo->RewardErrorCode		= pRecordPacket->RewardErrorCode;

			if ( GUserInfo->RewardItem.Type == eR_Capsule || GUserInfo->RewardItem.Type == eR_Unit )
			{
				NInventoryItem NewItem;
				NewItem.ItemSeqNo	= pRecordPacket->Item_Info.ItemSeqNo;
				NewItem.ItemID		= GUserInfo->RewardItem.ID;
				NewItem.ItemCount	= pRecordPacket->Item_Info.ItemTotalCount;
				NewItem.Category	= pRecordPacket->Item_Info.ItemCategory;
				NewItem.ExpireTime	= pRecordPacket->Item_Info.ItemExpiretime;
				GUserInfo->InsertUnitItem( NewItem );
			}

			CScene* pScene = IUSceneManager().GetCurrentScene();
			GOfflineReward->RemoveSavedRank();

			GGameInfo->RewardMissionNo = pRecordPacket->MissionID;

			if ( GOfflineReward && GOfflineReward->bWaitOfflineReward && pScene->SceneID == eID_Scene_Main )
			{
				GUserInfo->UpdateMissionProcess( pRecordPacket->MissionID, pRecordPacket->Rank, RecvPacket->Score[0] );

				if ( GUserInfo->RewardItem.Type != 0 )
				{
					GOfflineReward->OpenRewardPopup( pRecordPacket->ChangeGP, pRecordPacket->ChangeExp, RecvPacket->Score[0] );	//novic_money
				}
				else
				{
					GOfflineReward->SendReportGameRecord();
				}

			}
			else if ( GGameInfo->IsBattleModeByType( eUDBM_Mission ) )
			{
				GUserInfo->UpdateMissionProcess( pRecordPacket->MissionID, pRecordPacket->Rank, RecvPacket->Score[0] );
			}
		}
	}

	//GGameInfo->isLevelup = GGameInfo->CheckSectionLevelup( PrevLevel, GUserInfo->Level) ;
	GGameInfo->ReportGame(); */
}