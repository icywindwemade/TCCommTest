//##############################################################################################
// PacketRecver.cpp
//##############################################################################################
#include "PacketRecver.h"

#include "PacketSender.h"
#include "BuildDefine.h"
#include "OnlineHandler.h"
#include "OnlineDefine.h"
//#include "Client.h"
//#include "MessageHandler.h"
//#include "GameInfo.h"
//#include "GlobalInclude.h"
//#include "MessageDefine.h"
//#include "UserInfo.h"
//#include "MainScene.h"



//##############################################################################################
// GPacketHandlerMap
//##############################################################################################
TMap<unsigned short, NetCallbackProcessPacket> GPacketHandlerMap;

//##############################################################################################
// Receive
//##############################################################################################
RPHI( CLIENT_KEEPALIVE                      )
{
	PACKET* RecvPacket = (PACKET*)(InPacket->Data);

	int NewIndex = EOCT_GameServer;
	if( GNetworkManager->GetCurrentConnection() )
	{
		NewIndex = GNetworkManager->GetConnectionIndex( GNetworkManager->GetCurrentConnection() );
	}

	FnLog("NET: PING, CLIENT_KEEPALIVE" );
    
	OLSendPacket( NewIndex, RecvPacket, RecvPacket->size );
}

RPHI( CLIENT_GRACEFUL_DISCONNECT             )
{
}

RPHI( CLIENT_PING		                    )
{
	PING_PACKET* RecvPacket = (PING_PACKET*)(InPacket->Data);

	GOnlineInfo->RenewPingTime( GNetworkManager->GetNetworkTime(), RecvPacket->Temp );

	OLConditionalReplicatePing();

	//FnLog("NET: PING, Time: %d Grade: %d", GOnlineInfo->GetPingTime(), GOnlineInfo->GetPingGrade() );
}

RPHI( PTCL_EVENT_ACHIEVEMENT_COMPLETION		)
{
	///EVENT_ACHIEVEMENT_COMPLETION* recvPacket = (EVENT_ACHIEVEMENT_COMPLETION*)(InPacket->Data);

	FnLog("NET: RECV, EVENT_ACHIEVEMENT_COMPLETION");
	/*
	const CAchievementData *pData = GGameData->GetAchievementData( recvPacket->AchievementID );

	if( !pData ) return;

	WCHAR wszTitle[64];
	WCHAR wszComment[64];
	char szIcon[64];
	sprintf(szIcon,"ui_ico_ach%d",pData->m_IconID);

	GetWideString( pData->m_Title.c_str(), wszTitle, 64 );
	GetWideString( pData->m_Comment.c_str(), wszComment, 64 );

	if( pData != NULL )
	{
		int TempTime = 0;
		int TempMode = 0;

		if( GGameInfo->IsRunningGame() )
		{
			if( GGameInfo->IsMulti() )
			{
				TempTime = 1000;

				if( pData->m_CheckPoint == 3 ) TempTime += 12000;
			}
		}
        
        if( pData->m_CheckPoint == 1 ) TempTime = 1000;

		ShowEventMessage( IUGetLocalText(wszTitle), IUGetLocalText(wszComment), szIcon, 3000, TempMode, TempTime, pData->m_CheckPoint );//iconModule, 3000 );

		IUGetSoundManager().PlaySound( GGameData->GetResourceSound( S129_Achieve_Window ) );
	} */
}

RPHI( PTCL_GAME_NOTIFICATION					)
{
/// GAME_NOTIFICATION* RecvPacket = (GAME_NOTIFICATION*)(InPacket->Data);
    FnLog(" RECV: Game Notification.");
    
/*
	if( RecvPacket->iDisplayTime > 0 )
	{
		WCHAR TempWideText[256] = {0,};
		if( RecvPacket->bType >= 2 )
		{
			if( strlen( RecvPacket->szTitle ) > 0 )
			{
				GetWideString( RecvPacket->szTitle, TempWideText, 256 );
			}
		}
		else
		{
			if( strlen( RecvPacket->szMessage ) > 0 )
			{
				GetWideString( RecvPacket->szMessage, TempWideText, 256 );
			}
		}

		if( wcslen( TempWideText ) > 0 )
		{
			ShowNoticeMessage( TempWideText, RecvPacket->iDisplayTime*1000, 0, RecvPacket->bType == 1 ? 1 : 0 );
		}
	}

	if( RecvPacket->bType >= 100 )
	{
		if( RecvPacket->iDisplayTime > 0 )
		{
			GOnlineInfo->AutoNotification.State = 1;
			GOnlineInfo->AutoNotification.Type = RecvPacket->bType;
			GOnlineInfo->AutoNotification.Link = RecvPacket->bLinkType;

			strcpy( GOnlineInfo->AutoNotification.Title, RecvPacket->szTitle );
			strcpy( GOnlineInfo->AutoNotification.Desc, RecvPacket->szMessage );
			strcpy( GOnlineInfo->AutoNotification.URL, RecvPacket->szLink );

			CMainScene* pScene = (CMainScene*)(IUSceneManager().GetCurrentScene( eID_Scene_Main ));
			if( pScene )
				pScene->CheckAutoNotification();
		}
		else
		{
			GOnlineInfo->AutoNotification.State = 0;
		}
	}
	else if( RecvPacket->bType >= 2 )
	{
		if( RecvPacket->iDisplayTime > 0 )
		{
			GOnlineInfo->Notification.State = 1;
			GOnlineInfo->Notification.Type = RecvPacket->bType;
			GOnlineInfo->Notification.Link = RecvPacket->bLinkType;

			strcpy( GOnlineInfo->Notification.Title, RecvPacket->szTitle );
			strcpy( GOnlineInfo->Notification.Desc, RecvPacket->szMessage );
			strcpy( GOnlineInfo->Notification.URL, RecvPacket->szLink );
		}
		else
		{
			GOnlineInfo->Notification.State = 0;
		}
	} */

}

RPHI( PTCL_NOTIFY_ITEM_EXPIRE					)
{
///	NOTIFY_ITEM_EXPIRE * RecvPacket = (NOTIFY_ITEM_EXPIRE *)(InPacket->Data);
    FnLog(" RECV: Item Expired");
// 	NInventoryItem* pInvenItem;
// 	NInventoryItem* pInvenCastle;

// 	int lastIndex = 0;
// 
// 	for ( int i = 0 ; i < 10 ; i++ )
// 	{
// 		if ( RecvPacket->ItemSeq[i] != 0 )
// 			++lastIndex;
// 		else
// 			break;
// 	}

///	GUserInfo->SetExpireItem(RecvPacket->ItemSeq);
///	GUserInfo->ShowExpireItemBox();

// 	for (int i = 0 ; i < lastIndex ; i++ )
// 	{
// 		int buttonType = EMBA_OK | EMBA_Cancel;
// 
// 		pInvenItem = GUserInfo->GetUnitItem( RecvPacket->ItemSeq[i] );
// 		pInvenCastle = GUserInfo->GetCastleItem( RecvPacket->ItemSeq[i] );
// 
// 		if ( i == (lastIndex-1) )
// 			buttonType |= EMBA_Option;
// 
// 		if (pInvenItem)
// 		{
// 			ShowExpireDateMessage( EMBID_ExpireDate, buttonType, pInvenItem->ItemID, pInvenItem->ExpireTime);
// 			GUserInfo->RemoveUnitItem(RecvPacket->ItemSeq[i]);
// 		}
// 
// 		if (pInvenCastle)
// 		{
// 			ShowExpireDateMessage( EMBID_ExpireDate, buttonType, pInvenCastle->ItemID, pInvenCastle->ExpireTime);
// 			GUserInfo->RemoveCastleItem(RecvPacket->ItemSeq[i]);
// 		}
// 	}
}

