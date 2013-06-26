
//##############################################################################################
// PacketSender.cpp
//##############################################################################################
#include "PacketSender.h"
#include "GamePacket.h"

#include "OnlineHandler.h"
#include "OnlineHandler.h"
//#include "Client.h"


//##############################################################################################
// Profile
//##############################################################################################
void OLRequestProfilePrivateInfo()
{
	FnLog("NET: SEND, REQ_PRIVATE_INFO to GameServer");

	REQ_PRIVATE_INFO sendPacket;
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestProfileBaseInfo( long long wemadeID )
{
	FnLog("NET: SEND, REQ_BASE_INFO to GameServer");

	REQ_BASE_INFO sendPacket;
	sendPacket.UKey = wemadeID;

	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestProfileBattleRecommendation( long long wemadeID )
{
	FnLog("NET: SEND, REQ_BATTLE_RECOMMENDATION to GameServer");

	REQ_BATTLE_RECOMMENDATION sendPacket;
	sendPacket.UKey = wemadeID;

	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestProfileBattleHistory( long long wemadeID )
{
	FnLog("NET: SEND, REQ_BATTLE_HISTORY to GameServer");

	REQ_BATTLE_HISTORY sendPacket;
	sendPacket.UKey = wemadeID;

	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestProfileGroupRanking( long long wemadeID, int page )
{
	FnLog("NET: SEND, REQ_GROUP_RANKING to GameServer");

	REQ_GROUP_RANKING sendPacket;
	sendPacket.UKey = wemadeID;
	sendPacket.Page = (unsigned char)page;

	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestProfileAchievementHistory( long long wemadeID, int page )
{
	FnLog("NET: SEND, REQ_ACHIEVEMENT_HISTORY to GameServer");

	REQ_ACHIEVEMENT_HISTORY sendPacket;
	sendPacket.UKey = wemadeID;
	sendPacket.Page = (unsigned char)page;

	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}
