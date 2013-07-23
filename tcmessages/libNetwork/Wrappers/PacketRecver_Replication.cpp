//##############################################################################################
// PacketRecver.cpp
//##############################################################################################
#include "PacketRecver.h"

#include "PacketSender.h"
#include "BuildDefine.h"
#include "OnlineHandler.h"
//#include "Client.h"
//#include "MessageHandler.h"
//#include "PlayerInfo.h"
//#include "GameInfo.h"
//#include "PatchInfo.h"
//#include "Commands.h"
//#include "GlobalInclude.h"
//#include "GameScene.h"


//##############################################################################################
// Replication
//##############################################################################################
RPHI( PTCL_REPLICATION_READY					)
{
	///REPLICATION_READY* RecvPacket = (REPLICATION_READY*)(InPacket->Data);

	FnLog("NET: RECV, REPLICATION_READY ");
    /*
	CPlayerInfo* pPlayer = GGameInfo->GetPlayer( RecvPacket->Owner );

	if( pPlayer )
	{
		pPlayer->SetSyncState( RecvPacket->Type );
	}
	else
	{
		FnLog("NET: RECV, REPLICATION_READY Invalid Player ID");
	} */
}

RPHI( PTCL_REPLICATION_SYNC					)
{
	FnLog("NET: RECV, PTCL_REPLICATION_SYNC ");
	//REPLICATION_SYNC* RecvPacket = (REPLICATION_SYNC*)(InPacket->Data);
	//FnLog("NET: RECV, SYNC %08d %08d %08d %08x", GNetworkManager->GetSyncFrame(), RecvPacket->Frame, RecvPacket->MatchMessageType, RecvPacket->Owner );
}

RPHI( PTCL_REPLICATION_ACTION				)
{
	///REPLICATION_ACTION* RecvPacket = (REPLICATION_ACTION*)(InPacket->Data);
	FnLog("NET: RECV, PTCL_REPLICATION_ACTION ");
    

	//FnLog("NET: RECV, ACTION %08d %08d %08d %08x %08x %08x %08x %08x", 
	//	GNetworkManager->GetSyncFrame(), 
	//	RecvPacket->Frame, 
	//	RecvPacket->Action, 
	//	RecvPacket->Owner,
	//	RecvPacket->Base,
	//	RecvPacket->Code,
	//	RecvPacket->Index,
	//	GNetworkManager->GetSyncID() );
/*
	CGameCommand* NewCommand = new CGameCommand;

	NewCommand->Frame	= RecvPacket->Frame;
	NewCommand->Action	= RecvPacket->Action;
	NewCommand->Owner	= RecvPacket->Owner;
	NewCommand->Base	= RecvPacket->Base;
	NewCommand->Code	= RecvPacket->Code;
	NewCommand->Index	= RecvPacket->Index;
	NewCommand->Cost	= RecvPacket->Cost;
	NewCommand->X		= RecvPacket->X;
	NewCommand->Y		= RecvPacket->Y;

	GGameInfo->ExecSyncCommand( NewCommand );

	if( !GGameInfo->IsMatchModeByType( eUDMM_Replay ) )
	{
		GGameInfo->InsertGameCommand( NewCommand );
	}

	//LogPrintf( "LJ Recv : %d %d %d %d %d %d %d %d %d %d", GGameInfo->PlayFrame, NewCommand->Frame, NewCommand->Action, NewCommand->Owner, NewCommand->Base, NewCommand->Code, NewCommand->Index, NewCommand->Cost, NewCommand->X, NewCommand->Y);

	//delete NewCommand;
 */
}

RPHI( PTCL_REPLICATION_PING					)
{
	///REPLICATION_PING* RecvPacket = (REPLICATION_PING*)(InPacket->Data);

	FnLog("NET: RECV, PTCL_REPLICATION_PING ");
    /*
	CPlayerInfo* pPlayer = 0;
	if( GGameInfo->IsRunningGame() )
	{
		pPlayer = GGameInfo->GetPlayer( RecvPacket->Owner );

		if( pPlayer )
		{
			pPlayer->SetPingGrade( RecvPacket->Time );
		}
	}*/
}

RPHI( PTCL_REPLICATION_DUMMY				)
{
}
