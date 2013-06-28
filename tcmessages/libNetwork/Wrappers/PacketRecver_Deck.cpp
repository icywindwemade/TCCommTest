//##############################################################################################
// PacketRecver.cpp
//##############################################################################################
#include "PacketRecver.h"

#include "PacketSender.h"
#include "BuildDefine.h"
/*#include "OnlineHandler.h"
#include "Client.h"
#include "MessageHandler.h"
#include "UserInfo.h"
#include "GlobalInclude.h"
#include "InventoryScene.h"
 */

//##############################################################################################
// Deck
//##############################################################################################

RPHI( PTCL_ANS_COMBAT_SETTING				) 
{


    ANS_COMBAT_SETTING* RecvPacket = (ANS_COMBAT_SETTING*)(InPacket->Data);

    FnLog("");
	FnLog("NET: RECV, ANS_COMBAT_SETTING");
/*

	GUserInfo->DeckCastle.ItemSeqNo = RecvPacket->Castle;
	GUserInfo->DeckCastle.ItemID		= 0;

	NInventoryItem* Item = GUserInfo->GetCastleItem( GUserInfo->DeckCastle.ItemSeqNo );

	if( Item ) GUserInfo->DeckCastle.ItemID = Item->ItemID;
   */
    FnLog("Castle=%d\nMap=%d\n", RecvPacket->Castle, RecvPacket->Map);
}

RPHI( PTCL_ANS_CHANGE_COMBAT_SETTING		)
{
    printf("NET: RECV, ANS_CHANGE_COMBAT_SETTING\n");
    /*
	ANS_CHANGE_COMBAT_SETTING* RecvPacket = (ANS_CHANGE_COMBAT_SETTING*)(InPacket->Data);

	FnLog("NET: RECV, ANS_CHANGE_COMBAT_SETTING");

	HideProgressMessage();
	CInventoryScene* pScene = (CInventoryScene *)IUSceneManager().GetCurrentScene( eID_Scene_Inventory );
	if( RecvPacket->AnsCode == ANS_CHANGE_COMBAT_SETTING::eSuccess )
	{
		// Success
		FnLog("NET: RECV, ANS_CHANGE_COMBAT_SETTING success");

		for( int i = 0 ; i < UD_UNIT_SLOT_COUNT ; i++ )
		{
			GUserInfo->DeckUnit[i].ItemSeqNo		= RecvPacket->UnitSlotSeqNo[i];
			GUserInfo->DeckUnit[i].ItemID		= RecvPacket->UnitSlot[i];	
		}

		GUserInfo->DeckCastle.ItemSeqNo = RecvPacket->CastleSeqNo;
		GUserInfo->DeckCastle.ItemID		= 0;

		NInventoryItem* Item = GUserInfo->GetCastleItem( GUserInfo->DeckCastle.ItemSeqNo );
		if( Item ) GUserInfo->DeckCastle.ItemID = Item->ItemID;

		if( pScene )
		{
			pScene->ReceivedChangeTabSaveDeckSuccess();
		}
	}
	else
	{
		FnLog("NET: RECV, ANS_CHANGE_COMBAT_SETTING failed");

		ShowAlertMessage( EMBA_OK, EMCT_None, L"%s|| %s", IUGetLocalText( L"info_connect_error_sync" ), IUGetLocalText( L"info_error_retry" ) );
	}
     */
}