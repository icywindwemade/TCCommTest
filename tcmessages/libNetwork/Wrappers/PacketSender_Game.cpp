//##############################################################################################
// PacketSender.cpp
//##############################################################################################
#include "PacketSender.h"
#include "GamePacket.h"

#include "OnlineDefine.h"
#include "OnlineHandler.h"
#include "UserInfo.h"
//#include "Client.h"
//#include "PlayerInfo.h"
//#include "GameInfo.h"
//#include "UserInfo.h"

//#include "GlobalDefine.h"

//#include "IU.h"


//##############################################################################################
// Game
//##############################################################################################
void OLReportCustomMatch( int InMsg, int InMatchMode, int InBattleMode, int InMatchTarget, long long InTargetKey, const char* InTargetNick, int InUserStatus )
{
	// Send Request , Will receive PTCL_REPORTING_CUSTOM_MATCH
	FnLog("NET: SEND, REPORTING_CUSTOM_MATCH to GameServer Type:%d Match:%d Battle:%d Target:%d", InMsg, InMatchMode, InBattleMode, InMatchTarget);

	//// Match Status for Invite
	//enum eUDUserInvitationStatus
	//{
	//	eUDUIS_NoMember = 0,	// user not cnd member and invite cnd world!
	//	eUDUIS_Unable,			// user can't receive invitation message
	//	eUDUIS_Enable,			// user can receive invitation message
	//	eUDUIS_Busy,			// user is busy( playing, ... )
	//};

    REPORTING_CUSTOM_MATCH NewPacket;
    
	NewPacket.UKey					= InTargetKey;
	NewPacket.ChannelID				= 0;
	NewPacket.MatchMessageType		= (unsigned char)InMsg;
	NewPacket.MatchMode				= (unsigned char)InMatchMode;
	NewPacket.MatchTarget			= (unsigned char)InMatchTarget;
	NewPacket.UserInvitationStatus  = (unsigned char)InUserStatus;
	NewPacket.BattleMode			= (unsigned char)InBattleMode;

	memset( NewPacket.Name, 0, UD_NICK_LEN );
	if( InTargetNick && strlen(InTargetNick) > 0 )
	{
		strncpy( NewPacket.Name, InTargetNick, UD_NICK_LEN-1 );
	}
    
	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );            
}

void OLRequestStartMatch( int InMatchMode, int InBattleMode, int InMatchTarget, long long InTargetKey/*=0*/, int InAbility/*=0*/, int MissionNO/*=0*/, int Ability/*=0*/ )
{
	// Send Request , Will receive PTCL_ANS_START_MATCH
	FnLog("NET: SEND, REQ_START_MATCH to GameServer %d %d %d ", InMatchMode, InBattleMode, InMatchTarget );

	OLRequestSaveDeck();

	REQ_START_MATCH NewPacket;

	NewPacket.MatchMode		= (unsigned char)InMatchMode;
	NewPacket.MatchTarget	= (unsigned char)InMatchTarget;
	NewPacket.Ability		= (unsigned char)InAbility;
	NewPacket.BattleMode	= (unsigned char)InBattleMode;
	NewPacket.UKey			= InTargetKey;
	NewPacket.MissionNo		= MissionNO;
	NewPacket.Ability		= (unsigned char)Ability;

	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
}

void OLRequestStopMatch()
{
	// Send Request , Will receive PTCL_ANS_STOP_MATCH
	FnLog("NET: SEND, REQ_STOP_MATCH to GameServer");

	REQ_STOP_MATCH NewPacket;

	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
}

void OLRequestStartGame( int InMapID )
{
	// Send Request , Will receive PTCL_ANS_START_GAME
	FnLog("NET: SEND, REQ_START_GAME to GameServer");

	OLRequestSaveDeck();

	REQ_START_GAME NewPacket;

	NewPacket.MapSetIndex = InMapID;

	for( int i=0; i<UD_ITEM_SLOT_COUNT; i++)
	{
		NewPacket.ProductID[i] = GUserInfo->QuickItemSlot[i].iPDKey;
	}

	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
}

void OLRequestStopGame( int InError )
{
	// Send Request , Will receive PTCL_ANS_STOP_GAME

	FnLog("NET: SEND, REQ_STOP_GAME to GameServer");

	REQ_STOP_GAME NewPacket;

	NewPacket.Reason = InError;

	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
}

void OLReportGameState( int InResult, int InType/*=0*/, int InOwnerID/*=0*/ )
{
	FnLog("NET: SEND, REPORTING_GAME_STATE to GameServer");

	REPORTING_GAME_STATE NewPacket;

	NewPacket.Type		= (unsigned char)InType;
	NewPacket.Result	= (unsigned char)InResult;
	NewPacket.Owner		= InOwnerID;

	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );            	
}

void OLRequestGameRecord( int InMissionID/*=0*/, int InRank/*=0*/, int nScore /*= 0*/ )
{
	FnLog("NET: SEND, REQ_GAME_RECORDING to GameServer");

	///CPlayerInfo* pPlayer = GGameInfo->MasterPlayer;

	REQ_GAME_RECORDING NewPacket;

	NewPacket.MissionID = InMissionID;
	NewPacket.Rank = (unsigned char)InRank;
	NewPacket.Score = nScore;
/*
	if( pPlayer )
	{
		NPlayerStats* pStats = pPlayer->GetStats();

		NewPacket.GainGold	= pStats->CatchGolds;
		NewPacket.UseGold	= pStats->SpendGolds;
		NewPacket.GainMana	= pStats->CatchManas;
		NewPacket.UseMana	= pStats->SpendManas;
	}
	else */
	{
		NewPacket.GainGold	= 0;
		NewPacket.UseGold	= 0;
		NewPacket.GainMana	= 0;
		NewPacket.UseMana	= 0;
	}

	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
}

void OLReplicateReady( int InType/*=1*/ )
{
	GNetworkManager->BeganSync();
	
	///if( false == GGameInfo->IsMulti() )
		///return;

	REPLICATION_READY NewPacket;
	NewPacket.Owner = GNetworkManager->GetSyncID();
	NewPacket.Time	= GNetworkManager->GetNetworkTime();
	NewPacket.Type	= InType;

	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );

	REPLICATION_DUMMY NewDummyPacket;
	NewDummyPacket.Owner = GNetworkManager->GetSyncID();

	OLSendPacket( EOCT_GameServer, &NewDummyPacket, sizeof(NewDummyPacket) );
	OLSendPacket( EOCT_GameServer, &NewDummyPacket, sizeof(NewDummyPacket) );
	OLSendPacket( EOCT_GameServer, &NewDummyPacket, sizeof(NewDummyPacket) );
}

void OLReplicateAction( int InAction, int InOwner, int InBase, int InCode, int InIndex, int InCost, int InX, int InY, int InGold, int InDerection/*=0*/ )
{
	// if( GGameInfo->EndingCastleID != 0 ) return;

	REPLICATION_ACTION	NewPacket;

	NewPacket.Frame		= GNetworkManager->GetSyncFrame();
	NewPacket.Action	= InAction;
	NewPacket.Owner		= InOwner;
	NewPacket.Base		= InBase;
	NewPacket.Code		= InCode;
	NewPacket.Index		= InIndex;
	NewPacket.Cost		= InCost;
	NewPacket.X			= InX;
	NewPacket.Y			= InY;
	NewPacket.Gold		= InGold;

// 	LogPrintf( "LJ Send : %d %d %d %d %d %d %d %d %d %d", 
// 		GGameInfo->PlayFrame, 
// 		NewPacket.Frame, 
// 		NewPacket.Action, 
// 		NewPacket.Owner, 
// 		NewPacket.Base, 
// 		NewPacket.Code, 
// 		NewPacket.Index, 
// 		NewPacket.Cost, 
// 		NewPacket.X, 
// 		NewPacket.Y);

	FnLog("NET: SEND, ACTION SF:%08d F:%08d A:%08d O:%08x B:%08x C:%08x I:%08d", 
		GNetworkManager->GetSyncFrame(), NewPacket.Frame, NewPacket.Action, NewPacket.Owner, NewPacket.Base, NewPacket.Code, NewPacket.Index );

	int NewDirection = NNetPacket::Allround;
	if( InDerection > 0 ) NewDirection = NNetPacket::External;
	else if( InDerection < 0 ) NewDirection = NNetPacket::Internal;

	GNetworkManager->PushSendSyncPacket( NewPacket.ptcl, &NewPacket, NewPacket.size, NewPacket.Frame, NewDirection );
}

void OLReplicatePing( int InPing )
{
	REPLICATION_PING NewPacket;

	NewPacket.Owner = GNetworkManager->GetSyncID();
	NewPacket.Frame	= GNetworkManager->GetSyncFrame();
	NewPacket.Count = 0;
	NewPacket.Time	= InPing;
	NewPacket.Type	= 1;

	GNetworkManager->PushSendSyncPacket( NewPacket.ptcl, &NewPacket, NewPacket.size, NewPacket.Frame, NNetPacket::External );    
	//OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
}

