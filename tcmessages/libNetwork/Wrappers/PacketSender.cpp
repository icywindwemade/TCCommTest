//##############################################################################################
// PacketSender.cpp
//##############################################################################################
#include "PacketSender.h"
#include "GamePacket.h"

#include "OnlineHandler.h"
//#include "Client.h"
//#include "PlatformHandler.h"


//##############################################################################################
// Client
//##############################################################################################
void OLReportCheckPing( int InTime )
{
	PING_PACKET NewPacket;
/*
	NewPacket.Temp = InTime;

	if( GNetworkManager->GetCurrentConnection() )
	{
		OLSendPacket( GNetworkManager->GetCurrentConnection()->GetIndex(), &NewPacket, sizeof(NewPacket) );
	}*/
}

//##############################################################################################
// 
//##############################################################################################
void OLReportChangeGameStatus( int InBattleMode, int InScenePosition, int InStatus/*=0*/, int InTime/*=0*/ )
{
	FnLog("NET: SEND, REPORTING_GAME_POSITION to GameServer %d %d %d %d", InBattleMode, InScenePosition, InStatus, InTime );
/*
	GOnlineInfo->SetScenePosition( InScenePosition );

	REPORTING_GAME_POSITION NewPacket;

	NewPacket.BattleMode = (unsigned char)InBattleMode;
	NewPacket.ScenePosition = (unsigned char)InScenePosition;
	NewPacket.GameStatus = InStatus;
	NewPacket.GameTime = InTime;

	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );*/
}

void OLReportChangeGameOption( bool InMatchInvitation /*0:unable 1:enabel*/, bool InAllowMessage )
{
	FnLog("NET: SEND, REPORTING_GAME_OPTION to GameServer");
/*
	REPORTING_GAME_OPTION NewPacket;

	NewPacket.BattleInvitation = InMatchInvitation ? 1 : 0;
	NewPacket.PostReceive      = InAllowMessage ? 1 : 0;

	OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );*/
}