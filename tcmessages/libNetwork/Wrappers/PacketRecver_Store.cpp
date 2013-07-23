//##############################################################################################
// PacketRecver.cpp
//##############################################################################################
#include "PacketRecver.h"

#include "PacketSender.h"
#include "BuildDefine.h"

#include "OnlineHandler.h"
//#include "Client.h"
//#include "MessageHandler.h"
//#include "UserInfo.h"
//#include "GlobalInclude.h"
//#include "StoreScene.h"


//##############################################################################################
// Store
//##############################################################################################
RPHI( PTCL_ANS_PURCHASE					)
{
	ANS_PURCHASE* RecvPacket = (ANS_PURCHASE*)(InPacket->Data);

	FnLog("NET: RECV, ANS_PURCHASE");
//	NInventoryItem PrevItem;
//
//	GUserInfo->Gem = RecvPacket->Gem;
//	GUserInfo->GP = RecvPacket->GP;	//novic_money
    FnLog("ItemCount=%d", RecvPacket->ItemCount);

	if( RecvPacket->AnsCode == ANS_PURCHASE::eSuccess )
	{	
		///GUserInfo->PurchaseItemCount = RecvPacket->ItemCount;
        /*
		if( 1 < RecvPacket->ItemCount )
		{
			for( int i = 0; i < RecvPacket->ItemCount; i++ )
			{
				if( RecvPacket->Item[i].ItemID >= START_INDEX_TOWER && RecvPacket->Item[i].ItemID < START_INDEX_SKILL )
				{
					GUserInfo->PurchaseItem[i].ItemSeqNo 	= RecvPacket->Item[i].ItemSeqNo;
					GUserInfo->PurchaseItem[i].ItemID		= RecvPacket->Item[i].ItemID;
					GUserInfo->PurchaseItem[i].ItemCount  	= RecvPacket->Item[i].ItemCount;
					GUserInfo->PurchaseItem[i].Category		= RecvPacket->Item[i].Category;
					GUserInfo->PurchaseItem[i].ExpireTime	= RecvPacket->Item[i].ExpireTime;
					GUserInfo->InsertUnitItem( GUserInfo->PurchaseItem[i] , &PrevItem );
				}
			}
		}
		else
		{		
			if( RecvPacket->ProductID == INDEX_INVENTORYSIZE )
			{
				GUserInfo->UnitInvenCount			= RecvPacket->InventorySize;
			}
			else if( RecvPacket->Item[0].ItemID >= START_INDEX_TOWER && RecvPacket->Item[0].ItemID < START_INDEX_SKILL )
			{
				GUserInfo->PurchaseItem[0].ItemSeqNo 	= RecvPacket->Item[0].ItemSeqNo;
				GUserInfo->PurchaseItem[0].ItemID		= RecvPacket->Item[0].ItemID;
				GUserInfo->PurchaseItem[0].ItemCount  	= RecvPacket->Item[0].ItemCount;
				GUserInfo->PurchaseItem[0].Category		= RecvPacket->Item[0].Category;
				GUserInfo->PurchaseItem[0].ExpireTime	= RecvPacket->Item[0].ExpireTime;
				GUserInfo->InsertUnitItem( GUserInfo->PurchaseItem[0] , &PrevItem );
			}
			else if( (RecvPacket->Item[0].ItemID >= START_INDEX_CASTLE && RecvPacket->Item[0].ItemID < START_INDEX_BUILDING) )
			{
				GUserInfo->PurchaseItem[0].ItemSeqNo   	= RecvPacket->Item[0].ItemSeqNo;
				GUserInfo->PurchaseItem[0].ItemID		= RecvPacket->Item[0].ItemID;
				GUserInfo->PurchaseItem[0].ItemCount	    = RecvPacket->Item[0].ItemCount;
				GUserInfo->PurchaseItem[0].Category		= RecvPacket->Item[0].Category;
				GUserInfo->PurchaseItem[0].ExpireTime	= RecvPacket->Item[0].ExpireTime;
				GUserInfo->InsertCastleItem( GUserInfo->PurchaseItem[0] , RecvPacket->Item[0].Category, &PrevItem );
			}

		}
		Telegram* NewTelegram = new Telegram( 0, EOMC_BuySuccess, 0, CustomMessage::Online );
		IUGameManager().Message( NewTelegram );
         */
        
        for( int i = 0 ; i < RecvPacket->ItemCount ; i++ )
        {

            
            FnLog("---------------------------------------");
            FnLog("ItemSeqNo:%d", RecvPacket->Item[i].ItemSeqNo);
            FnLog("ItemID:%d", RecvPacket->Item[i].ItemID);
            FnLog("ItemCount:%d", RecvPacket->Item[i].ItemCount);
            FnLog("Category:%d", RecvPacket->Item[i].Category);
            FnLog("ExpireTime:%lld", RecvPacket->Item[i].ExpireTime);
            
            ///GUserInfo->InsertCastleItem( NewItem );
        }
	}
	else
	{
        FnLog("------> Failure");
        /* The following causes some linker failure at LocalText::ParseLocalization
		if( RecvPacket->AnsCode == ANS_PURCHASE::eFail )
			wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_connect_error_sync" ) );
		else if( RecvPacket->AnsCode == ANS_PURCHASE::eFail_Repurchase )
			wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_time_1day_busy" ) );
		else if( RecvPacket->AnsCode == ANS_PURCHASE::eFail_Balance )
			wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_buy_error_gold" ) );
		else if( RecvPacket->AnsCode == ANS_PURCHASE::eFail_SaleTime )
			wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_buy_error_expiration" ) );
		else if( RecvPacket->AnsCode == ANS_PURCHASE::eFail_MaxStarCount )
			wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_buy_error_expiration" ) );
		else if( RecvPacket->AnsCode == ANS_PURCHASE::eFail_MaxMoney )
			wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_ranking_cp_excess" ) );
		else if( RecvPacket->AnsCode == ANS_PURCHASE::eFail_MaxItem )
			wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_inventory_unit_full_01" ) );
		else if( RecvPacket->AnsCode == ANS_PURCHASE::eFail_MaxInventory )
			wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_inventory_full_01" ) );
		else
			wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_connect_error" ) );

		Telegram* NewTelegram = new Telegram( 0, EOMC_BuyFail, 0, CustomMessage::Online );
		IUGameManager().Message( NewTelegram ); */
	}
}

RPHI( PTCL_ANS_PURCHASE_PERMISSION				)
{
	/// ANS_PURCHASE_PERMISSION *recvPacket = (ANS_PURCHASE_PERMISSION *)(InPacket->Data);

	FnLog("NET: RECV, ANS_IN_APP_PURCHASE");
/*
	bool bSuccess = false;

	GOnlineInfo->PermissionKey = recvPacket->PermissionKey;

	if( recvPacket->AnsCode == ANS_PURCHASE_PERMISSION::eFail_DailyOverpayment )
	{
		wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_buy_limit_day" ) );
	}
	else if( recvPacket->AnsCode == ANS_PURCHASE_PERMISSION::eFail_MonthlyOverpayment )
	{
		wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_buy_limit_month" ) );
	}
	else if( recvPacket->AnsCode == ANS_PURCHASE_PERMISSION::eFail_MaxMoney )	//novic_money
	{
		wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"Info_alert_gem_max" ) );
	}
	else if( recvPacket->AnsCode == ANS_PURCHASE_PERMISSION::eFail )
	{
		wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_connect_error_sync" ) );
	}
	else if( recvPacket->AnsCode == ANS_PURCHASE_PERMISSION::eSuccess )
	{
		bSuccess = true;
	}

	if( bSuccess == true )
	{
		Telegram* NewTelegram = new Telegram( 0, EOMC_PurchasePermission, 0, CustomMessage::Online );
		IUGameManager().Message( NewTelegram );
	}
	else
	{
		Telegram* NewTelegram = new Telegram( 0, EOMC_BuyFail, 0, CustomMessage::Online );
		IUGameManager().Message( NewTelegram );
	} */
}

RPHI( PTCL_ANS_IN_APP_PURCHASE					)
{
/// 	ANS_IN_APP_PURCHASE *recvPacket = (ANS_IN_APP_PURCHASE *)(InPacket->Data);

	FnLog("NET: RECV, ANS_IN_APP_PURCHASE");
/*
	bool bSuccess = false;

	if( recvPacket->AnsCode == ANS_IN_APP_PURCHASE::eSuccess )
	{
		bSuccess = true;

		// save data clear if success
		GSaveData->SetValue( "InAppPurchase", "" );
		GSaveData->SaveData();
	}
	else
	{
		if( recvPacket->AnsCode == ANS_IN_APP_PURCHASE::eFail_NoItem )			// cannot find iteminfo
		{
			wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_buy_fail" ) );
		}
		else if( recvPacket->AnsCode == ANS_IN_APP_PURCHASE::eFail_Expired )	// product has expired
		{
			wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_buy_fail" ) );
		}
		else if( recvPacket->AnsCode == ANS_IN_APP_PURCHASE::eFail_TranID )		// TranID already exists
		{
			wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_buy_fail" ) );
		}
		else if( recvPacket->AnsCode == ANS_IN_APP_PURCHASE::eFail_Store )		// Apple Store Error
		{
			wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_buy_fail" ) );
		}
		else if( recvPacket->AnsCode == ANS_IN_APP_PURCHASE::eFail_Unknown )	// Unknown
		{
			wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_buy_fail" ) );
		}
	}

	// update gem
	GUserInfo->Gem = recvPacket->Gem;

	if( bSuccess == true )
	{
		Telegram* NewTelegram = new Telegram( 0, EOMC_BuySuccess, 0, CustomMessage::Online );
		IUGameManager().Message( NewTelegram );
	}
	else
	{
		Telegram* NewTelegram = new Telegram( 0, EOMC_BuyFail, 0, CustomMessage::Online );
		IUGameManager().Message( NewTelegram );
	} */
}

RPHI( PTCL_ANS_ANDROID_IN_APP_PURCHASE		)
{
	/// ANS_ANDROID_IN_APP_PURCHASE *recvPacket = (ANS_ANDROID_IN_APP_PURCHASE *)(InPacket->Data);

	FnLog("NET: RECV, ANS_ANDROID_IN_APP_PURCHASE");
/*
	bool bSuccess = false;

	if( recvPacket->AnsCode == ANS_ANDROID_IN_APP_PURCHASE::eSuccess )
	{
		bSuccess = true;
	}
	else
	{
		switch( recvPacket->AnsCode )
		{
		case ANS_ANDROID_IN_APP_PURCHASE::eFail_NoItem:
			{
				wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_buy_fail" ) );
			}
			break;
		case ANS_ANDROID_IN_APP_PURCHASE::eFail_Expired:
			{
				wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_buy_fail" ) );
			}
			break;
		case ANS_ANDROID_IN_APP_PURCHASE::eFail_TranID:
			{
				wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_buy_fail" ) );
			}
			break;
		case ANS_ANDROID_IN_APP_PURCHASE::eFail_Store:
		case ANS_ANDROID_IN_APP_PURCHASE::eFail_Signature :
		case ANS_ANDROID_IN_APP_PURCHASE::eFail_Sign :
			{
				wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_buy_fail" ) );
			}
			break;
		case ANS_ANDROID_IN_APP_PURCHASE::eFail_PermissionKey:
			{
				wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_buy_fail" ) );
			}
			break;
		case ANS_ANDROID_IN_APP_PURCHASE::eFail_Unknown:
			{
				wcscpy( GOnlineInfo->ErrorMsg, IUGetLocalText( L"info_item_buy_fail" ) );
			}
			break;
		}
	}

	// update gem
	GUserInfo->Gem = recvPacket->Gem;

	if( bSuccess == true )
	{
		Telegram* NewTelegram = new Telegram( 0, EOMC_BuySuccess, 0, CustomMessage::Online );
		IUGameManager().Message( NewTelegram );
	}
	else
	{
		Telegram* NewTelegram = new Telegram( 0, EOMC_BuyFail, 0, CustomMessage::Online );
		IUGameManager().Message( NewTelegram );
	} */
}
