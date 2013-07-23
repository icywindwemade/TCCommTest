//##############################################################################################
// PacketRecver.cpp
//##############################################################################################
#include "PacketRecver.h"

#include "PacketSender.h"
#include "BuildDefine.h"
#include "UserInfo.h"
#include "Telegram.h"
//#include "OnlineHandler.h"
//#include "Client.h"
//#include "MessageHandler.h"
//#include "GlobalInclude.h"
//#include "InventoryScene.h"
//#include "CapsuleCombinePopup.h"
//#include "CapsuleInfoPopup.h"
//#include "StoreScene.h"


//##############################################################################################
// Inventory
//##############################################################################################

RPHI( PTCL_ANS_INVENTORY					)
{
	ANS_INVENTORY* RecvPacket = (ANS_INVENTORY *)(InPacket->Data);

FnLog("");
	FnLog("NET: RECV, ANS_INVENTORY");
    FnLog("Total Items = %d", RecvPacket->ItemCount);


	int deckCnt = 0;

	for( int i = 0 ; i < RecvPacket->ItemCount ; i++ )
	{

		NInventoryItem NewItem;
		NewItem.ItemSeqNo	= RecvPacket->Item[i].ItemSeqNo;
		NewItem.ItemID		= RecvPacket->Item[i].ItemID;
		NewItem.ItemCount	= RecvPacket->Item[i].ItemCount;

		if( NewItem.ItemCount > 0 )
			NewItem.ItemCount = 1;
		else
			NewItem.ItemCount = 0;

		NewItem.Category	= RecvPacket->Item[i].Category;
		NewItem.ExpireTime	= RecvPacket->Item[i].ExpireTime;
		NewItem.ItemLevel		= RecvPacket->Item[i].Level;
		NewItem.ItemGradePoint  = RecvPacket->Item[i].Exp;

		/// GUserInfo->InsertUnitItem( NewItem );
/*
		if( deckCnt < UD_UNIT_SLOT_COUNT && RecvPacket->Item[i].isEquip )
		{
			GUserInfo->DeckUnit[deckCnt].ItemSeqNo	= RecvPacket->Item[i].ItemSeqNo;
			GUserInfo->DeckUnit[deckCnt].ItemID		= RecvPacket->Item[i].ItemID;
			deckCnt++;
		} */

		if( RecvPacket->Item[i].isEquip && deckCnt >= UD_UNIT_SLOT_COUNT )
		{
			LogPrintf("IsEquip error! it's over than 6!");
			//exit(0); //need fix
		}
        
        FnLog("---------------------------------------");
        FnLog("ItemSeqNo:%d", NewItem.ItemSeqNo);
        FnLog("ItemID:%d", NewItem.ItemID);
        FnLog("ItemCount:%d", NewItem.ItemCount);
        FnLog("Category:%d", NewItem.Category);
        FnLog("ExpireTime:%d", (int)NewItem.ExpireTime);
        FnLog("ItemLevel:%d", NewItem.ItemLevel);
        FnLog("ItemGradePoint:%d", NewItem.ItemGradePoint);
	}

	// Page Control
//	GUserInfo->UnitItemsPage		= RecvPacket->Page;
//	GUserInfo->UnitItemsPageCount	= RecvPacket->PageCount;
//	GUserInfo->UnitInvenCount		= RecvPacket->InventorySize; 
}

RPHI( PTCL_ANS_CASTLE_INVENTORY				)
{
	ANS_CASTLE_INVENTORY* RecvPacket = (ANS_CASTLE_INVENTORY *)(InPacket->Data);

FnLog("");
	FnLog("NET: RECV, PTCL_ANS_CASTLE_INVENTORY");
    FnLog("Total Items = %d", RecvPacket->ItemCount);
	for( int i = 0 ; i < RecvPacket->ItemCount ; i++ )
	{
		NInventoryItem NewItem;
		NewItem.ItemSeqNo	= RecvPacket->Item[i].ItemSeqNo;
		NewItem.ItemID		= RecvPacket->Item[i].ItemID;
		NewItem.ItemCount	= RecvPacket->Item[i].ItemCount;
		NewItem.Category	= RecvPacket->Item[i].Category;
		NewItem.ExpireTime	= RecvPacket->Item[i].ExpireTime;
        
        FnLog("---------------------------------------");
        FnLog("ItemSeqNo:%d", NewItem.ItemSeqNo);
        FnLog("ItemID:%d", NewItem.ItemID);
        FnLog("ItemCount:%d", NewItem.ItemCount);
        FnLog("Category:%d", NewItem.Category);
        FnLog("ExpireTime:%lld", NewItem.ExpireTime);
        
		///GUserInfo->InsertCastleItem( NewItem );
	}
    
	///GUserInfo->CastleItemsPage		= RecvPacket->Page;
	///GUserInfo->CastleItemsPageCount	= RecvPacket->PageCount;
}

RPHI( PTCL_ANS_DELETE_ITEM					)
{
///	ANS_DELETE_ITEM* RecvPacket = (ANS_DELETE_ITEM *)(InPacket->Data);

	FnLog("NET: RECV, ANS_DELETE_ITEM");
/*
	if( RecvPacket->AnsCode == ANS_DELETE_ITEM::eSuccess )
	{
		// Success 

		GUserInfo->RemoveUnitItem( RecvPacket->ItemSeqNo );
	}
	else
	{
		// Failed 

		FnLog("NET: RECV, ANS_DELETE_ITEM failed");

		switch( RecvPacket->AnsCode )
		{
		case ANS_DELETE_ITEM::eFail:
		case ANS_DELETE_ITEM::eFail_None:
			GUserInfo->RemoveUnitItem( RecvPacket->ItemSeqNo );
			break;
		case ANS_DELETE_ITEM::eFail_NonExpire:
			break;
		}
	}*/
}

RPHI( PTCL_ANS_MIX_ITEM						)
{
	ANS_MIX_ITEM* RecvPacket = (ANS_MIX_ITEM *)(InPacket->Data);

	FnLog("NET: RECV, ANS_MIX_ITEM");
	/*
	CCapsuleCombinePopup* pScene = 0;
	if( IUSceneManager().GetCurrentScene()->GetChildScene( eID_Scene_CapsuleCombinePopup ) )
	{
		pScene = (CCapsuleCombinePopup *)IUSceneManager().GetCurrentScene()->GetChildScene();
	}
	*/
	if( RecvPacket->AnsCode == ANS_MIX_ITEM::eSuccess )
	{
		// Success

		// Delete Mix Item
		for( int i = 0; i < 4; i++ ) 
			GUserInfo->RemoveUnitItem( RecvPacket->ItemSeqNo[i] );

		// Add Capsule Item
		NInventoryItem NewItem;
		NewItem.ItemSeqNo	= RecvPacket->Item.ItemSeqNo;
		NewItem.ItemID		= RecvPacket->Item.ItemID;
		NewItem.ItemCount	= RecvPacket->Item.ItemCount;
		NewItem.Category	= RecvPacket->Item.Category;
		NewItem.ExpireTime	= RecvPacket->Item.ExpireTime;
		GUserInfo->InsertUnitItem( NewItem );

		// Refresh Gem
	//	GUserInfo->Gem = RecvPacket->Gem;	//novic_money

		//if( pScene ) pScene->OnlineMessageCombineSuccess( RecvPacket->Item.ItemID , RecvPacket->Item.Category ); 
//		Telegram* NewTelegram = new Telegram( 0, EOMC_Combine_Success, RecvPacket->Item.ItemID, CustomMessage::Online );
//		IUMessage( NewTelegram );
		
	}
	else
	{
		FnLog("NET: RECV, ANS_MIX_ITEM Failed");

		//ShowAlertMessage( EMBA_OK, EMCT_None, IUGetLocalText( L"info_combine_error" ) );
	}
}


RPHI( PTCL_ANS_UPGRADE_ITEM					)
{
	ANS_UPGRADE_ITEM* RecvPacket = (ANS_UPGRADE_ITEM *)(InPacket->Data);

	FnLog("NET: RECV, ANS_UPGRADE_ITEM");


	if( RecvPacket->AnsCode == ANS_UPGRADE_ITEM::eSuccess )
	{
        FnLog("Success: GP=%d", RecvPacket->GP);

		//CInventoryScene* pScene =  (CInventoryScene *)IUSceneManager().GetCurrentScene();

		///GUserInfo->GP = RecvPacket->GP;
		
		//if( pScene ) pScene->OnlineMessageGradeSuccess();

		///Telegram* NewTelegram = new Telegram( 0, EOMC_Grade_Success, 0, CustomMessage::Online );
		///IUMessage( NewTelegram );
	}
	else
	{
		FnLog("NET: RECV, ANS_UPGRADE_ITEM Failed : AnsCode (%d)", RecvPacket->AnsCode);

		if( RecvPacket->AnsCode == ANS_UPGRADE_ITEM::eFail_Balance ) {
            FnLog("info_item_buy_error_gold");
			///ShowAlertMessage( EMBA_OK, EMCT_None, IUGetLocalText( L"info_item_buy_error_gold" ) );
        }
		else {
			///ShowAlertMessage( EMBA_OK, EMCT_None, IUGetLocalText( L"info_unit_grow_fail_01" ) );
            FnLog("info_unit_grow_fail_01");
        }
	}

}

RPHI( PTCL_ANS_OPEN_CAPSULE					)
{
	ANS_OPEN_CAPSULE* RecvPacket = (ANS_OPEN_CAPSULE*)(InPacket->Data);

	FnLog("NET: RECV, ANS_OPEN_CAPSULE");

	/// CCapsuleInfoScene* pScene = 0;

    /*
	if( IUSceneManager().GetCurrentScene()->GetChildScene( eID_Scene_CapsuleInfoPopup )  )
	{
		pScene = (CCapsuleInfoScene *)IUSceneManager().GetCurrentScene()->GetChildScene();
	}
     */

	if( RecvPacket->AnsCode == ANS_OPEN_CAPSULE::eSuccess )
	{
		// Success

		// Remove Capsule Item
		GUserInfo->RemoveUnitItem( RecvPacket->ItemSeqNo );

		// Add Unit Item
		NInventoryItem NewItem;
		NewItem.ItemSeqNo	= RecvPacket->AcquiredItemSeqNo;
		NewItem.ItemID		= RecvPacket->AcquiredItemID;
		NewItem.ItemCount	= RecvPacket->AcquiredItemCount;
		NewItem.Category	= RecvPacket->AcquiredCategory;
		NewItem.ExpireTime	= RecvPacket->AcquiredItemExpireTime;
		GUserInfo->InsertUnitItem( NewItem );


            FnLog("---------------------------------------");
            FnLog("ItemSeqNo:%d", NewItem.ItemSeqNo);
            FnLog("ItemID:%d", NewItem.ItemID);
            FnLog("ItemCount:%d", NewItem.ItemCount);
            FnLog("Category:%d", NewItem.Category);
            FnLog("ExpireTime:%d", (int)NewItem.ExpireTime);
            

		///if( pScene ) pScene->OnlineMessageOpenSuccess( RecvPacket->AcquiredItemID );
	}
	else
	{
		// Failed

		FnLog("NET: RECV, ANS_MIX_ITEM Failed");

	///	if( pScene ) pScene->OnlineMessageError( IUGetLocalText( L"info_connect_error_sync" ) );
	}
}