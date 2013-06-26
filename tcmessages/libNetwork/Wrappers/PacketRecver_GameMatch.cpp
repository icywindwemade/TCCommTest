//##############################################################################################
// PacketRecver.cpp
//##############################################################################################
#include "PacketRecver.h"

#include "PacketSender.h"
#include "BuildDefine.h"
#include "OnlineHandler.h"
//#include "Client.h"
//#include "MessageHandler.h"
//#include "PlayerInfo.h"
//#include "GameInfo.h"
//#include "GlobalInclude.h"
//#include "UserInfo.h"
//#include "PlatformHandler.h"


//##############################################################################################
// Game
//##############################################################################################
RPHI( PTCL_ANS_START_MATCH			)
{
	ANS_START_MATCH* RecvPacket = (ANS_START_MATCH*)(InPacket->Data);

	FnLog("NET: RECV, ANS_START_MATCH");

	if( RecvPacket->AnsCode == ANS_START_MATCH::eSuccess )
	{
		// Success

		FnLog("NET: RECV, ANS_START_MATCH success");
	}
	else
	{
		// Failed

		/// OLEndedMatching();

		FnLog("NET: RECV, ANS_START_MATCH failed");
/*
		switch( RecvPacket->AnsCode )
		{
		case ANS_START_MATCH::eFail_Equip:
			ShowAlertMessage( EMBA_OK, EMCT_BackScene, IUGetLocalText( L"info_battle_error_slot" ) );
			break;
		case ANS_START_MATCH::eFail:
		default:
			ShowAlertMessage( EMBA_OK, EMCT_BackScene, IUGetLocalText( L"info_battle_error_match" ) );
			break;
		}*/
	}

	
}

RPHI( PTCL_ANS_STOP_MATCH				)
{
	ANS_STOP_MATCH* RecvPacket = (ANS_STOP_MATCH*)(InPacket->Data);

	FnLog("NET: RECV, ANS_STOP_MATCH");

	OLEndedMatching();

	if( RecvPacket->AnsCode == ANS_STOP_MATCH::eSuccess )
	{
		// Success

		FnLog("NET: RECV, ANS_STOP_MATCH success");
/*
		if( IUSceneManager().GetCurrentScene( eID_Scene_Battle ) )
		{
			OLEndedMatchInviting();

			if( GOnlineInfo->GetSceneBattleMode() == eUDBM_Mission )
				ChangeScene( eID_Scene_SingleLobby );
			else
				ChangeScene( eID_Scene_MultiLobby );
		}	
	}
	else
	{
		// Failed

		// Skip
		return; */
	} 
}

RPHI( PTCL_ANS_START_GAME					)
{
	ANS_START_GAME* RecvPacket = (ANS_START_GAME*)(InPacket->Data);

	FnLog("NET: RECV, ANS_START_GAME");    

	if( RecvPacket->AnsCode == ANS_START_GAME::eSuccess )
	{
		FnLog("NET: RECV, ANS_START_GAME Success");

		// Receive complete start game ( host and guest at same time )

		// Close any child scene
        /*
		IUSceneManager().GetCurrentScene()->CloseChildScene();

		// Set map name
		const CMapIndexData* pXmlMapData = GGameData->GetMapIndexData( RecvPacket->MapID );
		if( pXmlMapData )
		{
			GGameInfo->SetMapID( RecvPacket->MapID );
			GGameInfo->SetMapName( pXmlMapData->m_Resource.c_str() );

			int PlayerCount = MIN( RecvPacket->DeckCount, 4 );
			for( int i = 0; i < PlayerCount; i++ )
			{
				CPlayerInfo* Player = GGameInfo->GetPlayer( RecvPacket->Deck[i].PlayerID );
				if( Player )
				{
					Player->ChangeDeck( RecvPacket->Deck[i].CastleID, RecvPacket->Deck[i].UnitIDs );
				}
			}

			CPlayerInfo* Player = GGameInfo->GetPlayer( GOnlineInfo->UserKey );
			if( NULL != Player )
			{
				for( int i=0; i<UD_ITEM_SLOT_COUNT; i++)
					Player->SetInstantItem(i, RecvPacket->InstantItemID[i] );
			}

// 			if( GGameInfo->IsBattleModeByType( eUDBM_Ranking) )
// 				HideProgressMessage();

			// Change loading scene
			ChangeScene( eID_Scene_Loading );


		}
		else
		{
			OLRequestStopGame( 1 );

			if( !IsOpenedAlertMessage() )
			{
				ShowAlertMessage( EMBA_OK, EMCT_BackScene, IUGetLocalText( L"info_battle_error_match" ) );
			}
		} */
	}
	else
	{
		if( GOnlineInfo->IsMatching() )
		{
			FnLog("NET: RECV, ANS_START_GAME Failed");
/*
			OLEndedMatching();

			if( !IsOpenedAlertMessage() )
			{
				ShowAlertMessage( EMBA_OK, EMCT_None, IUGetLocalText( L"info_battle_error_match" ) );
			}
		}
		else
		{
			OLRequestStopGame( 4444 );

			if( !IsOpenedAlertMessage() )
			{
				ShowAlertMessage( EMBA_OK, EMCT_BackScene, IUGetLocalText( L"info_battle_error_match" ) );
			} */
		}
	}

//	GUserInfo->GP = RecvPacket->MyGP;
//	GUserInfo->Gem = RecvPacket->MyGem;
//	GUserInfo->StarCount = RecvPacket->MyStarCount;
//
}

RPHI( PTCL_ANS_STOP_GAME						)
{
	//ANS_STOP_GAME* RecvPacket = (ANS_STOP_GAME*)(InPacket->Data);

	FnLog("NET: RECV, ANS_STOP_GAME");

}
