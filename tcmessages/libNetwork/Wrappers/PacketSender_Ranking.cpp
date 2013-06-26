//##############################################################################################
// PacketSender.cpp
//##############################################################################################
#include "PacketSender.h"

#include "OnlineHandler.h"
//#include "Client.h"
//#include "MessageHandler.h"
//#include "GlobalInclude.h"
//#include "MainScene.h"

#include "UserInfo.h"


//##############################################################################################
// Ranking
//##############################################################################################
void OLRequestWeeklyRanking(int nSendCount)
{
	FnLog("NET: SEND, REQ_WEEKLY_RANKING to GameServer");
	
	//REQ_WEEKLY_RANKING sendPacket;

	//memset( sendPacket.FriendID, 0, sizeof(char) * REQ_WEEKLY_RANKING::eMax_Search_Count * UD_NICK_LEN );

	//int nSendNum = 0;
	//for ( INT i = nSendCount; i < GFriendsManager->KakaoAppFriends.Num(); i++ )
	//{
	//	sprintf(sendPacket.FriendID[nSendNum], "%lld", GFriendsManager->KakaoAppFriends(i).KakaoFriendInfo->KakaoID);
	//	nSendNum++;

	//	if (nSendNum == REQ_WEEKLY_RANKING::eMax_Search_Count)
	//		break;
	//}

	//GFriendsManager->SendFriendNum += nSendNum;

	//OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestFriendStatus()
{
	FnLog("NET: SEND, REQ_FRIEND_STAR_LIS to GameServer");

	REQ_FRIEND_STAR_LIST sendPacket;
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestRankingInfo()
{
	FnLog("NET: SEND, REQ_WEEKLY_RANKING_GAMEINFO to GameServer");

	REQ_WEEKLY_RANKING_GAME_INFO sendPacket;
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestStarCount()
{
	FnLog("NET: SEND, REQ_UPDATE_STAR_COUNT to GameServer");

	REQ_UPDATE_STAR_COUNT sendPacket;
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestStarBox(int nPage)
{
	FnLog("NET: SEND, REQ_POST_BOX to GameServer");

	REQ_POST_BOX sendPacket;
	sendPacket.Page = (unsigned char)nPage;
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestRecvStar(int nPost)
{
	FnLog("NET: SEND, REQ_RECEIVE_POST to GameServer");

	REQ_RECEIVE_POST sendPacket;
	sendPacket.PostNo = nPost;
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestRecvStarAll()
{
	FnLog("NET: SEND, REQ_RECEIVE_POST_ALL to GameServer");

	REQ_RECEIVE_POST_ALL sendPacket;
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestSendPost(long long Target, long long UKey, bool bProud)
{
	FnLog("NET: SEND, REQ_SEND_POST to GameServer");

	char szId[52];
	sprintf(szId, "%lld", Target);

	REQ_SEND_POST sendPacket;
	if ( GUserInfo->PremiumStarCount > 0 )
	{
		sendPacket.Type= eUDPT_PremiumStar;
	}
	else
	{
		sendPacket.Type= eUDPT_Star;
	}

	if( true == bProud )
		sendPacket.Type= eUDPT_Boast;

	sendPacket.Count = 1;
	sendPacket.TargetUKey = UKey;
	strcpy(sendPacket.TargetID, szId);
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestSendPlayerInfo( long long UKey, const char *pID )
{
	FnLog("NET: SEND, REQ_PLAYER_INFO to GameServer");

	REQ_PLAYER_INFO sendPacket;
	sendPacket.UKey = UKey;
	strcpy( sendPacket.ID, pID );

	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestFrenInvite( const char* UserId, unsigned char szType )
{
	FnLog("NET: SEND, REQ_FRIEND_INVITATION to GameServer");

	REQ_FRIEND_INVITATION sendPacket;
	strcpy( sendPacket.ID, UserId );
	sendPacket.InvitationType = szType;
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestFrenInviteList()
{
	FnLog("NET: SEND, REQ_FRIEND_INVITATION_LIST to GameServer");

	REQ_FRIEND_INVITATION_LIST sendPacket;
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestGameComment( const char* pStr , int nMissionNo)
{
	FnLog("NET: SEND, REQ_UPDATE_RANK_COMMENT to GameServer");

	REQ_UPDATE_RANK_COMMENT sendPacket;
	sendPacket.MissionNo = nMissionNo;
	strcpy( sendPacket.comment, pStr );
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLReportTop20WeeklyRanking( int nMissionNo )
{
	FnLog("NET: SEND, REQ_TOP20_WEEKLY_RANKING to GameServer");

	REQ_TOP20_WEEKLY_RANKING sendPacket;
	sendPacket.MissionNo = nMissionNo;
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestMissionOpen( int nMissionNo )
{
	FnLog("NET: SEND, REQ_MISSION_OPEN to GameServer");

	REQ_MISSION_OPEN sendPacket;
	sendPacket.MissionNo = nMissionNo;
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}

void OLRequestFriendState( long long *pUKey, int iFriendCount )
{
	FnLog("NET: SEND, REQ_MISSION_OPEN to GameServer");

	REQ_FRIEND_INVITAION_STATE sendPacket;
	
	if( NULL != pUKey )
	{
		for( int i = 0; i < iFriendCount; i++ )
		{
			sendPacket.FriendUKey[i] = pUKey[i];
		}
	}

	sendPacket.FriendCount = iFriendCount;
	OLSendPacket( EOCT_GameServer, &sendPacket, sizeof(sendPacket) );
}
