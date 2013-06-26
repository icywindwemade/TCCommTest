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
#include "MultiResultScene.h"
*/


//##############################################################################################
// MatchInvitation
//##############################################################################################
RPHI( PTCL_REPORTING_CUSTOM_MATCH			)
{
	REPORTING_CUSTOM_MATCH* RecvPacket = (REPORTING_CUSTOM_MATCH*)(InPacket->Data);

	FnLog("NET: RECV, REPORTING_CUSTOM_MATCH Type:%d Match:%d Battle:%d Target:%d Status:%d", 
		RecvPacket->MatchMessageType, RecvPacket->MatchMode, RecvPacket->BattleMode, RecvPacket->MatchTarget, 
		RecvPacket->UserInvitationStatus );
/*
	WCHAR TempWideText[256] = {0,};

	switch( RecvPacket->MatchMessageType )
	{
	case eUDMMT_Cancel:
		{
			if( !GOnlineInfo->IsMatchInviting() ) return;

			OLEndedMatchInviting();

			ShowMatchMessage( RecvPacket->MatchMessageType, EMBA_OK, 0, IUGetLocalText( L"info_battle_error_match" ), 0, 3000 );
		}
		break;
	case eUDMMT_Invite:
		{
			// Can not match
			if( !GUserInfo->CanMatchInvitation()	|| 
				GOnlineInfo->IsMatchInviting()		||
				GOnlineInfo->IsMatching()			||
				IUSceneManager().GetCurrentScene( eID_Scene_Title ) )
			{
				OLReportCustomMatch( eUDMMT_Unable, RecvPacket->MatchMode, RecvPacket->BattleMode, RecvPacket->MatchTarget, RecvPacket->UKey, 0, RecvPacket->UserInvitationStatus );
				return;
			}

			OLBeganMatchInviting( RecvPacket->BattleMode, RecvPacket->MatchMode, RecvPacket->MatchTarget, RecvPacket->UKey, RecvPacket->Name, true );
		}
		break;
	case eUDMMT_Allow:
		{
			OLReportCustomMatch( eUDMMT_Confirm, RecvPacket->MatchMode, RecvPacket->BattleMode, RecvPacket->MatchTarget, RecvPacket->UKey, RecvPacket->Name, RecvPacket->UserInvitationStatus );

			OLBeganMatching( RecvPacket->BattleMode, RecvPacket->MatchMode, RecvPacket->MatchTarget, RecvPacket->UKey );
		}
		break;
	case eUDMMT_Refuse:
		{
			OLEndedMatchInviting();

			if( strlen( RecvPacket->Name ) > 0 ) GetWideString( RecvPacket->Name, TempWideText, UD_NICK_LEN );
			wcscpy( TempWideText + wcslen(TempWideText), IUGetLocalText( L"info_battle_invite_refuse" ) );
			ShowMatchMessage( RecvPacket->MatchMessageType, EMBA_OK, 0, TempWideText, 0, 3000 );

			CMultiResultScene* TargetScene = (CMultiResultScene *)IUSceneManager().GetCurrentScene( eID_Scene_MultiResult );
			if( NULL != TargetScene ) 
				TargetScene->DisableRematch();
//
//			FriendPage* pScene = (FriendPage *)IUSceneManager().GetCurrentScene()->GetChildScene( eID_Scene_Child_FriendPage );
//					if( pScene && pScene->GetTabID() == FriendTab_FriendList)
//					{
//						((FriendList*)(pScene->GetCurrentTab()))->drawFriendList();
//					}
//				}
		}
		break;
	case eUDMMT_Unable:
		{
			OLEndedMatchInviting();

			ShowMatchMessage( RecvPacket->MatchMessageType, EMBA_OK, 0, IUGetLocalText( L"info_battle_invite_error" ), 0, 3000 );

			CMultiResultScene* TargetScene = (CMultiResultScene *)IUSceneManager().GetCurrentScene( eID_Scene_MultiResult );
			if( NULL != TargetScene ) 
				TargetScene->DisableRematch();
		}
		break;
	case eUDMMT_Confirm:
		{
			HideMatchMessage();

			if( GNetworkManager->IsSyncing() )
			{
			GNetworkManager->EndedSync();
			}

			if( GOnlineInfo->GetSelectedChannel() && GOnlineInfo->GetSelectedChannel()->ID == RecvPacket->ChannelID )
			{
				OLBeganMatching( RecvPacket->BattleMode, RecvPacket->MatchMode, RecvPacket->MatchTarget, RecvPacket->UKey, true );
			}
			else
			{
				NServerChannel* FoundChannel = GOnlineInfo->FindChannel( RecvPacket->ChannelID );
				if( FoundChannel )
				{
					ShowProgressMessage();

					OLBeganMatchMigrating( FoundChannel->ID );
				}
				else
				{
					OLReportCustomMatch( eUDMMT_Unable, RecvPacket->MatchMode, RecvPacket->BattleMode, RecvPacket->MatchTarget, RecvPacket->UKey, RecvPacket->Name, RecvPacket->UserInvitationStatus );

					OLEndedMatchInviting();

					ShowMatchMessage( RecvPacket->MatchMessageType, EMBA_OK, 0, IUGetLocalText( L"info_battle_error_match" ), 0, 3000 );
				}
			}
		}
		break;
		//case eUDMMT_Wait:
		//	break;
		//case eUDMMT_Close:
		//	m_ButtonType = EMBA_OK;
		//	break;
	default:
		{
			FnLog("NET: RECV, REPORTING_FRIEND_MATCH Invalid match" );

			OLEndedMatchInviting();

			ShowMatchMessage( RecvPacket->MatchMessageType, EMBA_OK, 0, IUGetLocalText( L"info_battle_invite_error" ), 0, 3000 );
		}
	} */
}
