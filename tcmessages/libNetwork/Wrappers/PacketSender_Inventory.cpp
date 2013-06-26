//##############################################################################################
// PacketSender.cpp
//##############################################################################################
#include "PacketSender.h"
#include "GamePacket.h"
#include "NetworkManager.h"
#include "OnlineDefine.h"


#include "OnlineHandler.h"
//#include "Client.h"
//#include "PlatformHandler.h"
//#include "UserInfo.h"


//##############################################################################################
// Inventory
//##############################################################################################
// this function is combined in PTCL_ANS_INVENTORY - LMH 2013-04-18
void OLRequestLoadDeck()
{
	FnLog("NET: SEND, REQ_COMBAT_SETTING to GameServer");

	REQ_COMBAT_SETTING NewPacket;
	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
}


void OLRequestSaveDeck()
{
	FnLog("NET: SEND, REQ_CHANGE_COMBAT_SETTING to GameServer");

	REQ_CHANGE_COMBAT_SETTING NewPacket;
/*
	if( !GUserInfo ) return;

	//GOnlineInfo->NotifyOnlineMessage( eType_OnlineMessage_Progress, "Saving data" );

	for( int i = 0 ; i < UD_UNIT_SLOT_COUNT ; i++ )
	{
		NewPacket.UnitSlot[i]		= GUserInfo->DeckUnit[i].ItemID;
		NewPacket.UnitSlotSeqNo[i] = GUserInfo->DeckUnit[i].ItemSeqNo;
	}

	NewPacket.CastleSeqNo = GUserInfo->DeckCastle.ItemSeqNo;
 */

	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
}

void OLRequestInventoryUnits()
{
	FnLog("NET: SEND, REQ_INVENTORY to GameServer");

	REQ_INVENTORY NewPacket;

	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
}

void OLRequestInventoryCastles()
{
	FnLog("NET: SEND, REQ_CASTLE_INVENTORY to GameServer");

	REQ_CASTLE_INVENTORY NewPacket;

	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
}

void OLRequestDeleteItem( int ItemSeqNo )
{
	FnLog("NET: SEND, REQ_DELETE_ITEM to GameServer");

	REQ_DELETE_ITEM sendPacket;

	sendPacket.ItemSeqNo = ItemSeqNo;

	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestMixItem( int ItemSeqNo[] )
{
	FnLog("NET: SEND, REQ_MIX_ITEM to GameServer : SeqNo (%d %d %d %d)", ItemSeqNo[0], ItemSeqNo[1], ItemSeqNo[2], ItemSeqNo[3]);

	REQ_MIX_ITEM sendPacket;

	sendPacket.ItemSeqNo[0] = ItemSeqNo[0];
	sendPacket.ItemSeqNo[1] = ItemSeqNo[1];
	sendPacket.ItemSeqNo[2] = ItemSeqNo[2];
	sendPacket.ItemSeqNo[3] = ItemSeqNo[3];

	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestGrade( int ItemSeqNo[] )
{
	FnLog("NET: SEND, REQ_Grade to GameServer : SeqNo (%d %d %d %d)", ItemSeqNo[0], ItemSeqNo[1], ItemSeqNo[2], ItemSeqNo[3]);

	REQ_UPGRADE_ITEM sendPacket;

	sendPacket.ItemSeqNo[0] = ItemSeqNo[0];
	sendPacket.ItemSeqNo[1] = ItemSeqNo[1];
	sendPacket.ItemSeqNo[2] = ItemSeqNo[2];
	sendPacket.ItemSeqNo[3] = ItemSeqNo[3];

	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestOpenCapsule( int ItemSeqNo )
{
	FnLog("NET: SEND, REQ_OPEN_CAPSULE to GameServer");

	REQ_OPEN_CAPSULE sendPacket;

	sendPacket.ItemSeqNo = ItemSeqNo;

	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}



void OLRequestPurchase( int nID )
{
	FnLog("NET: SEND, REQ_PURCHASE to GameServer");

	REQ_PURCHASE sendPacket;

	sendPacket.ProductID = nID;

	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestPurchasePermission( int productID )
{
	FnLog("NET: SEND, REQ_PURCHASE_PERMISSION to GameServer");

	REQ_PURCHASE_PERMISSION sendPacket;

	sendPacket.ProductID = GOnlineInfo->ProductKey = productID;

	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

#if ANDROID
void OLRequestInAppPurchase( const char *receipt, char* pSignedDataPtr, char* pSignaturePtr )
#else	// WIN32 || IOS
void OLRequestInAppPurchase( const char *receipt )
#endif	// ANDROID
{
	FnLog("NET: SEND, REQ_IN_APP_PURCHASE to GameServer");

#if IOS

	REQ_IN_APP_PURCHASE sendPacket;
	sendPacket.ProductID = GOnlineInfo->ProductKey;    
	sendPacket.PermissionKey = GOnlineInfo->PermissionKey;    
	strcpy( sendPacket.Receipt, receipt );

	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );

#elif ANDROID
	
	REQ_ANDROID_IN_APP_PURCHASE sendPacket;

	//############## Consumption Begin ############
	//NewOrderID : 1351032584244551
	//OriginJson : {"orderId":"12999763169054705758.1351032584244551","packageName":"com.wemade.d21s","productId":"google.com.wemadeent.cnd.500gem","purchaseTime":1357258113000,"purchaseState":0,"purchaseToken":"jxphfhmckysjuowsgduyyzoa.AO-J1Ow0Lqkrk6pnWbEf50IkICPoovb93_sVivd-AHk3vsXb9M0PCKNcyyTQe6HxgiZxlJXKpBoGkpJ1mIXbRNlV3ifBsBEwNzwZC01prUAXK1FEmEF158VgAAYVqzCqvIE77Riqypv8KqmzHCac98i4V4Ow2IoEGA"}

	//OrderId : 12999763169054705758.1351032584244551
	//Sku : google.com.wemadeent.cnd.500gem
	//Signature : W8ebwfRLrobXfI3r/E+nyScte2YjPtJJWwGrfNCapLRd3QEmAHSu15oHe/DDk9gF5jtRJoDHf6Bg9D1LScxgS1ZgYqV3V7CGZJ7mubWgD/Ljlzt4zF1yagwtReC9LGepOQ85fD+nXW2OZsSiucY2COQpvgfhGxEN0Nt96nn5qhKN7E3q4er1AWBn/8sXRhjtwPtzP/nNn/xwdegkjpbmaXm6daHCV1wPV+G/7t54Jvnui3yrI8gttt07BjLg4V0KoAlIRkrT7udQyRBxGX/+QWYwK3mjy0yBwiM/rtCMDsW83zqXKcPuG3ibI3VqkhJc/MCXSHoPObocypmKzNHIJQ==
	//Token : jxphfhmckysjuowsgduyyzoa.AO-J1Ow0Lqkrk6pnWbEf50IkICPoovb93_sVivd-AHk3vsXb9M0PCKNcyyTQe6HxgiZxlJXKpBoGkpJ1mIXbRNlV3ifBsBEwNzwZC01prUAXK1FEmEF158VgAAYVqzCqvIE77Riqypv8KqmzHCac98i4V4Ow2IoEGA
	//############## Consumption End ##############
	
	if( pSignedDataPtr != 0x00 )
	{
		//char			Sign[32];
		memset( sendPacket.Sign, 0, 32);
		strncpy( sendPacket.Sign, pSignedDataPtr, 31 );
		//::strcpy( sendPacket.Sign, pSignedDataPtr );
	}
	if( pSignaturePtr != 0x00 )
	{
		//char			Signature[16];
		memset( sendPacket.Signature, 0, 16);
		strncpy( sendPacket.Signature, pSignaturePtr, 15 );
		//::strcpy( sendPacket.Signature, pSignaturePtr );
	}
	if( receipt != 0x00 )
	{
		//char			TransactionID[1024];	// Purchase unique key
		memset( sendPacket.TransactionID, 0, 1024);
		strncpy( sendPacket.TransactionID, receipt, 1023 );
		//::strcpy( sendPacket.TransactionID, receipt );
	}
	//sprintf( sendPacket.TransactionID, "%lld", GOnlineInfo->PermissionKey );
	
	//sendPacket.StoreType = eUDGooglePlay;
	sendPacket.StoreType = ClientStoreType();

	sendPacket.PermissionKey = GOnlineInfo->PermissionKey;
	sendPacket.ProductID = GOnlineInfo->ProductKey;
	

	//strcpy( sendPacket..Receipt, receipt ); 

	//REQ_IN_APP_PURCHASE sendPacket; 
	//strcpy( sendPacket.Receipt, receipt ); 
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
	
#else // WIN32
	REQ_IN_APP_PURCHASE sendPacket;
	strcpy( sendPacket.Receipt, receipt );
	sendPacket.PermissionKey = GOnlineInfo->PermissionKey;
	
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
#endif	// IOS
}

void OLRequestTStoreInfo()
{
	FnLog("NET: SEND, REQ_TSTORE_INFO to GameServer");

	//REQ_TSTORE_INFO NewPacket;

	//OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
}

