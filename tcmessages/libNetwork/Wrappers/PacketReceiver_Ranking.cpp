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
#include "UserInfo.h"
#include "GlobalInclude.h"
#include "PlatformHandler.h"

#include "GameInfo.h"
#include "MainScene.h"
#include "SingleResultScene.h"
#include "FriendsInfo.h"
#include "SingleLobbyChildScene_MissionRankInfo.h"
#include "SingleLobbyChildScene_MissionInfo.h"
#include "SingleResultScene.h" */


//##############################################################################################
// Ranking
//##############################################################################################
RPHI( PTCL_ANS_WEEKLY_RANKING_GAME_INFO					)
{
	/// ANS_WEEKLY_RANKING_GAME_INFO* RecvPacket = (ANS_WEEKLY_RANKING_GAME_INFO *)(InPacket->Data);

	FnLog("NET: RECV, ANS_WEEKLY_RANKING_GAMEINFO");
}

RPHI( PTCL_ANS_WEEKLY_RANKING					)
{
	ANS_WEEKLY_RANKING* RecvPacket = (ANS_WEEKLY_RANKING *)(InPacket->Data);

	FnLog("NET: RECV, ANS_WEEKLY_RANKING");

	if (RecvPacket->AnsCode == ANS_WEEKLY_RANKING::eSuccess)
	{
		
	}
	else
	{

	}
}

RPHI( PTCL_ANS_FRIEND_STAR_LIST					)
{
	/// ANS_FRIEND_STAR_LIST* RecvPacket = (ANS_FRIEND_STAR_LIST *)(InPacket->Data);

	FnLog("NET: RECV, ANS_FRIEND_STAR_LIST");
}

RPHI( PTCL_ANS_UPDATE_STAR_COUNT					)
{
	/// ANS_UPDATE_STAR_COUNT* RecvPacket = (ANS_UPDATE_STAR_COUNT *)(InPacket->Data);

	FnLog("NET: RECV, ANS_UPDATE_STAR_COUNT");
}

RPHI( PTCL_ANS_POST_BOX					)
{
	/// ANS_POST_BOX* RecvPacket = (ANS_POST_BOX *)(InPacket->Data);

	FnLog("NET: RECV, ANS_POST_BOX");
}

RPHI( PTCL_ANS_RECEIVE_POST					)
{
	ANS_RECEIVE_POST* RecvPacket = (ANS_RECEIVE_POST *)(InPacket->Data);

	FnLog("NET: RECV, ANS_RECEIVE_POST");

	if (RecvPacket->AnsCode == ANS_RECEIVE_POST::eSuccess)
	{
		
	}
}

RPHI( PTCL_ANS_RECEIVE_POST_ALL				)
{
	ANS_RECEIVE_POST_ALL* RecvPacket = (ANS_RECEIVE_POST_ALL *)(InPacket->Data);

	FnLog("NET: RECV, ANS_RECEIVE_POST_ALL");

	if (RecvPacket->AnsCode == ANS_RECEIVE_POST::eSuccess)
	{
		
	}
}

RPHI( PTCL_ANS_SEND_POST					)
{
	ANS_SEND_POST* RecvPacket = (ANS_SEND_POST *)(InPacket->Data);

	FnLog("NET: RECV, ANS_SEND_POST");

	if ( RecvPacket->Type == eUDPT_Boast )
		return;
	
	if (RecvPacket->AnsCode == ANS_SEND_POST::eSuccess)
	{	
		
	}
	else if (RecvPacket->AnsCode == ANS_SEND_POST::eFail || RecvPacket->AnsCode == ANS_SEND_POST::eFail_Refuse)
	{
	
	}
	else if (RecvPacket->AnsCode == ANS_SEND_POST::eFail_Time)
	{
		
	}
}

RPHI( PTCL_ANS_FRIEND_INVITATION_LIST					)
{
	/// ANS_FRIEND_INVITATION_LIST * RecvPacket = (ANS_FRIEND_INVITATION_LIST *)(InPacket->Data);

	FnLog("NET: RECV, ANS_FRIEND_INVITATION_LIST");
}

RPHI( PTCL_ANS_FRIEND_INVITATION					)
{
	ANS_FRIEND_INVITATION* RecvPacket = (ANS_FRIEND_INVITATION *)(InPacket->Data);

	FnLog("NET: RECV, ANS_FRIEND_INVITATION");

	switch( RecvPacket->InvitationType )
	{
	case ANS_FRIEND_INVITATION::eInviteGame:
		{
		
		}
		break;
	case ANS_FRIEND_INVITATION::eInviteOffline:
		{
		
		}
		break;
	}
}

RPHI( PTCL_ANS_PLAYER_INFO					)
{
	ANS_PLAYER_INFO* RecvPacket = (ANS_PLAYER_INFO *)(InPacket->Data);

	switch( RecvPacket->AnsCode )
	{
	case ANS_PLAYER_INFO::eSuccess:
		{

		}
		break;
	case ANS_PLAYER_INFO::eFail:
		{
	
		}
		break;
	}	
}

RPHI( PTCL_ANS_TOP20_WEEKLY_RANKING					)
{
/// 	ANS_TOP20_WEEKLY_RANKING* RecvPacket = (ANS_TOP20_WEEKLY_RANKING *)(InPacket->Data);
    FnLog("RECV: TOP20 weekly ranking");
/*
	GFriendsInfo->ResetMissionTopo20();

	for( int i = 0; i < UD_MISSION_RANK_COUNT; i++ )
	{
		if( RecvPacket->Member[i].ID[0]	== '\0' )
		{
			break;
		}

		FMissionTop20Info TempMissionTop20( 
			RecvPacket->Member[i].ID, 
			RecvPacket->Member[i].Name, 
			RecvPacket->Member[i].Score, 
			RecvPacket->Member[i].Message );

		GFriendsInfo->MissionTop20Info.AddItem( TempMissionTop20 );

	}

	HideProgressMessage();

	// if CurrentScene is MissionRankInfo
	Telegram* NewTelegram = new Telegram( 0, EOMC_Top20List, 0, CustomMessage::Online );
	IUMessage( NewTelegram );		*/
}

RPHI( PTCL_ANS_MISSION_OPEN			)
{
	/// ANS_MISSION_OPEN* RecvPacket = (ANS_MISSION_OPEN *)(InPacket->Data);

	FnLog("NET: RECV, ANS_MISSION_OPEN"); /*

	// Start Mission - Minus Star
	GUserInfo->StarCount = RecvPacket->MyStarCount;

	if( RecvPacket->AnsCode == ANS_MISSION_OPEN::eSuccess )
	{
		// Success
		FnLog("NET: RECV, ANS_MISSION_OPEN success");
	}
	else
	{
		// Failed
		FnLog("NET: RECV, ANS_MISSION_OPEN failed");

		switch( RecvPacket->AnsCode )
		{
		case ANS_MISSION_OPEN::eFail:
			{

			}
			break;
		case ANS_MISSION_OPEN::eFail_StarCount:
			{
				
			}
			break;
		case ANS_MISSION_OPEN::eFail_DuplicationOpen:
			{

			}
			break;
		case ANS_MISSION_OPEN::eFail_Data:
			{

			}
			break;
		case ANS_MISSION_OPEN::eFail_Ordering:
			{

			}
			break;
		default:
			{

			}
			break;
		}
	} */
}

RPHI( PTCL_ANS_UPDATE_RANK_COMMENT					)
{
	/// ANS_UPDATE_RANK_COMMENT* RecvPacket = (ANS_UPDATE_RANK_COMMENT *)(InPacket->Data);

	FnLog("NET: RECV, ANS_UPDATE_RANK_COMMENT failed"); /*

	if( RecvPacket->AnsCode == ANS_UPDATE_RANK_COMMENT::eSuccess )
	{
		
	}
	else
	{
		Telegram* NewTelegram = new Telegram( 0, EOMC_RankCommentFail, 0, CustomMessage::Online );
		IUMessage( NewTelegram );		 
	} */
}


RPHI( PTCL_ANS_FRIEND_INVITAION_STATE					)
{
	/// ANS_FRIEND_INVITAION_STATE* RecvPacket = (ANS_FRIEND_INVITAION_STATE *)(InPacket->Data);

	FnLog("NET: RECV, ANS_FRIEND_INVITAION_STATE Success");
}
