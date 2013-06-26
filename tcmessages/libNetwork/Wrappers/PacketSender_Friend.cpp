//##############################################################################################
// PacketSender.cpp
//##############################################################################################
#include "PacketSender.h"
#include "GamePacket.h"
#include "stdio.h"

#include "OnlineHandler.h"
//#include "Client.h"
//#include "MessageHandler.h"
//#include "UserInfo.h"
//#include "../Android/CppSource/JniUtil.h"


//##############################################################################################
// Friend
//##############################################################################################
void OLRequestFriendList(long long UKey, int page)
{
	// Send Request , Will receive PTCL_ANS_FRIEND_LIST

	FnLog("NET: SEND, REQ_FRIEND_LIST to GameServer");
    
	REQ_FRIEND_LIST sendPacket;
	sendPacket.UKey = UKey;
	sendPacket.Page = (unsigned short)page;
    
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestRecFriendList()
{
	FnLog("NET: SEND, REQ_FRIEND_RECOMMENDATION_LIST to GameServer");

	REQ_FRIEND_RECOMMENDATION_LIST sendPacket;

	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

extern void OLRequestFriendDeck(long long UKey)
{
	FnLog("NET: SEND, REQ_DECK to GameServer");

	REQ_DECK sendPacket;
	sendPacket.UKey = UKey;

	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestAddFriend( long long InUserKey, const char* InUserNick , unsigned char inAddType )
{
	// Send Request , Will receive PTCL_ANS_FRIEND_LIST

	FnLog("NET: SEND, REQ_ADD_FRIEND to GameServer");

	//ShowProgressMessage();
    
	REQ_ADD_FRIEND NewPacket;
	NewPacket.UKey = InUserKey;
	NewPacket.AddType = inAddType;//eUDFT_InGame;
	strcpy( NewPacket.Name, InUserNick );
    
	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
}

void OLRequestDeleteFriend( long long InUserKey )
{
	// Send Request , Will receive PTCL_ANS_FRIEND_LIST
	FnLog("NET: SEND, REQ_DELETE_FRIEND to GameServer");

	//ShowProgressMessage();

	REQ_DELETE_FRIEND NewPacket;
	NewPacket.FriendUKey = InUserKey;
	NewPacket.ReqType = REQ_DELETE_FRIEND::eDelete_Friend;

	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );	
}


void OLRequestSearchFriend( const char* InUserNick )
{
	// Send Request , Will receive PTCL_ANS_FRIEND_LIST 

	FnLog("NET: SEND, REQ_SEARCH_FRIEND to GameServer");

	REQ_SEARCH_FRIEND sendPacket;
	strcpy( sendPacket.Name, InUserNick );
	
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestFriendCompare( const char (*friendNames)[UD_NICK_LEN], int count )
{
	FnLog("NET: SEND, REQ_FRIEND_MATCH_IN_GAME to GameServer");

	REQ_FRIEND_MATCH_IN_GAME sendPacket;

	memset( sendPacket.FriendID, 0, sizeof(char) * REQ_FRIEND_MATCH_IN_GAME::eMax_Search_Count * UD_NICK_LEN );

	for( int i = 0 ; i < count ; i++ )
	{
		strcpy( sendPacket.FriendID[i], friendNames[i] );	
	}
	

	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}
