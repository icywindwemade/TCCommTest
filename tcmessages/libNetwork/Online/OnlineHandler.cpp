//##############################################################################################
// OnlineHandler.cpp
//##############################################################################################
#include "OnlineHandler.h"
///#include "Client.h"
///#include "PlatformHandler.h"
#include "UserInfo.h"
///#include "MessageHandler.h"
///#include "Client.h"
#include "PacketSender.h"
#include "PacketRecver.h"
#include "BuildDefine.h"
///#include "GlobalInclude.h"
#include "PatchInfo.h"
//#include "GameInfo.h"
//#include "PlayerInfo.h"
//#include "GameScene.h"
//#include "TitleScene.h"
//#include "MainScene.h"
//#include "FriendsInfo.h"
//#include "GlobalDefine.h"
#include "ServerDefine.h"
extern int ClientNetworkStatus( const char* InHost );
//#define FnLog printf

#if defined ( WIN32 )
DWORD	GetINI_Str( OUT char* szValue, IN INT ValueSize, IN LPCSTR Filename, IN LPCSTR AppName, IN LPCSTR KeyName, IN LPCSTR DefaultValue )
{
	TCHAR	szFullPath[1024] = {0,};
	TCHAR	szCurrentPath[1024] = {0,};
	DWORD	dwRetValue = 0;
	GetCurrentDirectory( 1024, szCurrentPath );
	sprintf_s( szFullPath, 1024, _T("%s\\%s"), szCurrentPath, Filename );

	strcpy_s( szValue, ValueSize, DefaultValue );
	dwRetValue = GetPrivateProfileString( AppName, KeyName, DefaultValue, szValue, ValueSize, szFullPath );
	return dwRetValue;
}
#endif

//##############################################################################################
// GNetworkManager
//##############################################################################################
extern NNetworkManager* GNetworkManager;

CPacketEncrypt GPacketEncrypt[EOCT_Max];

//##############################################################################################
// Network Init / Free
//##############################################################################################
void OLInit()
{
	// IUGameManager().SetFrameTime( ONLINE_FRAME_TIME );

	// Register packet protocol
	RegisterPacketHandler();

	// Net start
	bool bResult = GNetworkManager->Start( EOCT_Max, ONLINE_FRAME_TIME, ONLINE_LATENCY_FRAME, 
										OLCallbackNetworkUpdated,
										OLCallbackProcessPacket, 
										OLCallbackBuildPacket,
										OLCallbackFlushSync,
										OLCallbackCompareSync );

	if( !bResult )
	{
		// Error
		LogPrintf("NET: ####### Start Error #########");
	}

#if IOS //|| ANDROID
	if( bResult )
	{
		GNetworkManager->ResumeThread();
	}
#endif

	GOnlineInfo->SetNetworkModel( ClientNetworkStatus( ONLINE_NETWORK_CHECK_URL ) );

	GPacketEncrypt[EOCT_GateServer].Init();
	GPacketEncrypt[EOCT_GameServer].Init();
}

void OLFree()
{
	GNetworkManager->Stop();
}

//##############################################################################################
// CPacketEncrypt
//##############################################################################################

void OLPacketEncrypt( int InConnectionIndex, PACKET* InOutPacket )
{
	GPacketEncrypt[InConnectionIndex].ApplyUnionMark( InOutPacket );
	GPacketEncrypt[InConnectionIndex].ApplyCheckSum( InOutPacket );
	GPacketEncrypt[InConnectionIndex].ApplyMask( (char*)InOutPacket + 2, 2 );
}

ePacketEncryptCode OLPacketDecrypt( int InConnectionIndex, PACKET* InPacket )
{
	// Check start mark
	if( InPacket->GetMark() != STARTMARK)
	{ 
		// Process error
		return ePE_StartMark;
	}

	// Change mask key
	if( InPacket->GetSequenceNum() == 0 )
	{
		GPacketEncrypt[InConnectionIndex].SetNextMask( ((WORD)(InPacket->Mark << 11)) >> 11 );
	}             

	// Set mask
	GPacketEncrypt[InConnectionIndex].ApplyMask( (char*)InPacket+2, 2 );

	// Check sequence
	if( !GPacketEncrypt[InConnectionIndex].CheckSequenceNum( (BYTE)InPacket->GetSequenceNum() ) )
	{
		return ePE_Sequence;
	}

	// Check checksum
	if( !GPacketEncrypt[InConnectionIndex].CheckCheckSum( InPacket ) )
	{
		return ePE_Checksum;
	}

	return ePE_OK;
}

void OLSendPacket( int InConnectionIndex, void* InPacket, int InSize )
{
	int Result = GNetworkManager->SendPacket( InConnectionIndex, InPacket, InSize );

	int nPtcl = 0;

	if( Result <= 0 )
	{
		// Send failed

		int ErrorCode = 0;
		NNetworkConnection* TargetConnection = GNetworkManager->GetConnection( InConnectionIndex );
		if( TargetConnection ) 
		{
			ErrorCode = TargetConnection->GetErrorCode();
		}

		FnLog("NET: SEND, Send failed %d / %d / ptcl=%d", GNetworkManager->GetErrorCode(), ErrorCode, nPtcl );
	}
#if defined ( _DEBUG )
	else
	{
		PACKET* pPakcet = (PACKET*)InPacket;
		if( pPakcet )
		{
			nPtcl = pPakcet->GetPtcl();
			if( nPtcl != CLIENT_PING )
				FnLog("NET: SEND, ptcl=%d", nPtcl );
            if( nPtcl != CLIENT_KEEPALIVE ) {
				FnLog("NET: SEND, ptcl=%d", nPtcl );
            }
		}
	}
#endif
}

void OLSendGatePacket( void* InPacket, int InSize )
{
	OLSendPacket( EOCT_GateServer, InPacket, InSize );
}

void OLSendGamePacket( void* InPacket, int InSize )
{
	OLSendPacket( EOCT_GameServer, InPacket, InSize );
}

//##############################################################################################
// Callback functions
//##############################################################################################
void OLCallbackGateConnected( NNetworkConnection* InConnection, int InErrorCode )
{
	// Callback connected

	// Encrypt init
	if( InErrorCode == ENEC_Success )
	{
		GPacketEncrypt[EOCT_GateServer].Init();
	}

	// Delegate to Main Thread
	bool bResult = GNetworkManager->HandleEvent( NNetEvent::Connected, InConnection, InErrorCode, InConnection->GetErrorCode(), OLEventConnectedGateServer );
	if( !bResult )
	{
		// ToDo: HandleEvent Error
	}
}

void OLCallbackGateDisconnected( NNetworkConnection* InConnection, int InErrorCode )
{
	// Callback disconnected

	// Delegate to Main Thread
	bool bResult = GNetworkManager->HandleEvent( NNetEvent::Disconnected, InConnection, InErrorCode, InConnection->GetErrorCode(), OLEventDisconnectedGateServer );
	if( !bResult )
	{
		// ToDo: HandleEvent Error
	}

	if( InErrorCode == ENEC_Success )
	{
		GPacketEncrypt[EOCT_GateServer].Init();
	}
}

void OLCallbackGateSendComplete( NNetworkConnection* InConnection, int InSize )
{
	if( InSize <= 0 )
	{
		// Send error

		FnLog("NET: SEND, Send GateServer Error %d", InConnection->GetErrorCode() );
	}
}

void OLCallbackGateRecvComplete( NNetworkConnection* InConnection, char* InData, int InSize )
{
	PACKET* RecvPacket = (PACKET*)InData;

	ePacketEncryptCode ResultCode = OLPacketDecrypt( InConnection->GetIndex(), RecvPacket );

	if( ResultCode != ePE_OK )
	{
		// Decrypt error

		FnLog("NET: DECRYPT, GateServer Packet decrypt error %d", ResultCode );

		OLDisconnectGateServer();
		
		///ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s|| %s", IUGetLocalText( L"info_connect_error_sync" ), IUGetLocalText( L"info_error_restart" ) );

		return;
	}

	// Enqueue new received packet
	{
		GNetworkManager->Lock();

		GNetworkManager->PushRecvDataPacket( InConnection, RecvPacket->ptcl, RecvPacket, RecvPacket->size );

		GNetworkManager->Unlock();
	}
}

int OLCallbackGateCheckPacket( NNetworkConnection* InConnection, char* InData, int InSize )
{
	PACKET* RecvPacket = (PACKET*)InData;

	return RecvPacket->size;
}

void OLCallbackGameConnected( NNetworkConnection* InConnection, int InErrorCode )
{
	// Callback connected

	// Encrypt init
	if( InErrorCode == ENEC_Success )
	{
		GPacketEncrypt[EOCT_GameServer].Init();
	}

	// Delegate to Main Thread
	bool bResult = GNetworkManager->HandleEvent( NNetEvent::Connected, InConnection, InErrorCode, InConnection->GetErrorCode(), OLEventConnectedGameServer );
	if( !bResult )
	{
		// ToDo: HandleEvent Error
	}
}

void OLCallbackGameDisconnected( NNetworkConnection* InConnection, int InErrorCode )
{
	// Callback disconnected

	bool bResult = GNetworkManager->HandleEvent( NNetEvent::Disconnected, InConnection, InErrorCode, InConnection->GetErrorCode(), OLEventDisconnectedGameServer );
	if( !bResult )
	{
		// ToDo: HandleEvent Error
	}

	if( InErrorCode == ENEC_Success )
	{
		GPacketEncrypt[EOCT_GameServer].Init();
	}
}

void OLCallbackGameSendComplete( NNetworkConnection* InConnection, int InSize )
{
	if( InSize <= 0 )
	{
		// Send error

		FnLog("NET: SEND, Send GameServer Error %d", InConnection->GetErrorCode() );
	}
}

void OLCallbackGameRecvComplete( NNetworkConnection* InConnection, char* InData, int InSize )
{
	PACKET* RecvPacket = (PACKET*)InData;

	ePacketEncryptCode Result = OLPacketDecrypt( InConnection->GetIndex(), RecvPacket );

	if( Result != ePE_OK )
	{
		// Decrypt error

		FnLog("NET: DECRYPT, GameServer Packet decrypt error %d", Result );

		OLDisconnectGameServer();

///		ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s||%s", IUGetLocalText( L"info_connect_error_sync" ), IUGetLocalText( L"info_error_restart" ) );

		return;
	}

	// Enqueue new received packet
	{
		GNetworkManager->Lock();

		if( RecvPacket->ptcl == PTCL_REPLICATION_SYNC || RecvPacket->ptcl == PTCL_REPLICATION_ACTION )
		{
			switch( RecvPacket->ptcl )
			{
			case PTCL_REPLICATION_ACTION:
				{
					REPLICATION_ACTION* pPacket = (REPLICATION_ACTION*)InData;

					GNetworkManager->PushRecvSyncPacket( InConnection, RecvPacket->ptcl, RecvPacket, RecvPacket->size, pPacket->Frame );
				}
				break;
			case PTCL_REPLICATION_SYNC:
				{
					REPLICATION_SYNC* pPacket = (REPLICATION_SYNC*)InData;
/*
					GGameInfo->RenewSync( pPacket->Owner, pPacket->Frame );

					if( GGameInfo->CheckSync( GNetworkManager->GetSyncCheckFrame() ) )
					{
						GNetworkManager->CheckSync( pPacket->Frame, pPacket->size );
					}
*/
					FnLog("NET: RECV, SYNC %08d %08d AAAA", GNetworkManager->GetSyncFrame(), pPacket->Frame );
				}
				break;
			}
		}
		else
		{
			switch( RecvPacket->ptcl )
			{
			//case CLIENT_KEEPALIVE: 
			//	OLSendPacket( InConnection->GetIndex(), RecvPacket, RecvPacket->size );
			//	break;
			case CLIENT_GRACEFUL_DISCONNECT: 
				GOnlineInfo->DidKickout();
				break;
			default: 
				GNetworkManager->PushRecvDataPacket( InConnection, RecvPacket->ptcl, RecvPacket, RecvPacket->size );
				break;
			}
		}

		GNetworkManager->Unlock();
	}
}

int OLCallbackGameCheckPacket( NNetworkConnection* InConnection, char* InData, int InSize )
{
	PACKET* RecvPacket = (PACKET*)InData;

	return RecvPacket->size;
}

int GNetworkConnectingTime	= 0;

void OLCallbackNetworkUpdated( int InElapsedTime )
{
    
	//FnLog("OLCallbackNetworkUpdated\n");
    NNetworkConnection* TargetConnection = GNetworkManager->GetCurrentConnection();

	int NetTime = GNetworkManager->GetNetworkTime();

	if( GNetworkManager->IsSyncing() )
	{
		if(	(NetTime - GNetworkManager->GetLastRecvTime() > ONLINE_SYNC_LATENCY_TIME) &&
			(NetTime - GNetworkManager->GetLastRecvTime() < ONLINE_SYNC_LATENCY_TIME + ONLINE_SYNC_INTERVAL_TIME) )
		{
			REPLICATION_DUMMY NewPacket;
			OLSendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
		}
		/*
		if( NetTime - GNetworkManager->GetSyncLastRecvTime() > ONLINE_SYNC_LATENCY_TIME + ONLINE_SYNC_LATENCY_TIME )
		{
			GGameInfo->WaitGame( true );
		}
		else
		{
			GGameInfo->WaitGame( false );
		} */

		if( !GNetworkManager->IsSyncDrive() )///	&& GGameInfo->CanSync()	)
		{
			// Sync Run 
			GNetworkManager->DriveSync();
		}
	}
    /*
	if( GGameInfo->IsRunningGame() && GGameInfo->IsMatchModeByType( eUDMM_Replay ) )
	{
		GGameInfo->UpdateReplay( InElapsedTime );
	} */

	// Check connect timeout
	if( TargetConnection )
	{
		//if( !TargetConnection ) pConnection = pPending;
		if( TargetConnection->IsConnecting() )
		{
			GNetworkConnectingTime += InElapsedTime;
			if( GNetworkConnectingTime > ONLINE_CONNECT_TIMEOUT )
			{
				GNetworkConnectingTime = 0;

				FnLog("NET: TIMEOUT, Connecting timeout");
				GNetworkManager->Disconnect();
			}
		}
	}


	// Check Ping
	if( GOnlineInfo->IsLogin() )
	{
		int NetworkTime = GNetworkManager->GetNetworkTime();
		if( GOnlineInfo->PingSendTime == GOnlineInfo->PingRecvTime && GOnlineInfo->PingRecvTime + ONLINE_PING_CHECK_TIME < NetworkTime )
		{
			GOnlineInfo->PingSendTime = NetworkTime;

			OLReportCheckPing( GOnlineInfo->PingSendTime );
		}

		if( GOnlineInfo->PingSendTime > GOnlineInfo->PingRecvTime && GOnlineInfo->PingSendTime + ONLINE_PING_GRADE_BAD < NetworkTime )
		{
			GOnlineInfo->RenewPingTime( NetworkTime, GOnlineInfo->PingSendTime );

			OLConditionalReplicatePing();
		}
	}

}

//##############################################################################################
// Flush packets
//##############################################################################################
int OLCallbackBuildPacket( NNetworkConnection* InConnection, void* InOutData, int InSize )
{
	// Encrypt packet
	OLPacketEncrypt( InConnection->GetIndex(), (PACKET*)InOutData );

	// Same packet size
	return InSize;
}

void OLCallbackFlushSync( int InSyncBytes )
{
	//int Result;
	//int Frame = InSyncBytes < 0 ? 0 : GNetworkManager->GetSyncFrame();
    FnLog("OLCallbackFlushSync");
/*
	if( false == GGameInfo->IsMulti() )
	{
		if( GGameInfo->IsHost() )
		{
			TArray<CPlayerInfo*>& Players = GGameInfo->Players;
			for( int i = 0; i < Players.Num(); i++ )
			{
				CPlayerInfo* NewPlayer = Players(i);

				if( NewPlayer && NewPlayer->IsAI() )
				{
					GNetworkManager->CheckSync( Frame, sizeof(REPLICATION_SYNC) );

					//if( Result <= 0 ) GNetworkManager->Disconnect();
				}
			}
		}

		return;
	}

	if( InSyncBytes < ONLINE_SYNC_MIN_PACKET )
	{
		REPLICATION_DUMMY NewDummyPacket;

		Result = GNetworkManager->SendPacket( EOCT_GameServer, &NewDummyPacket, sizeof(NewDummyPacket), true );

		if( Result <= 0 )
		{
			//FnLog("NET: SEND, REPLICATION_DUMMY error");
			//OLDisconnectGameServer();
			GNetworkManager->Disconnect();
		}
	}

	REPLICATION_SYNC NewPacket;

	NewPacket.Frame = Frame;
	NewPacket.Owner = GNetworkManager->GetSyncID();
	NewPacket.Type	= 0;

	Result = GNetworkManager->SendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );

	GGameInfo->RenewSync( NewPacket.Owner, Frame );

	if( GGameInfo->IsHost() )
	{
		TArray<CPlayerInfo*>& Players = GGameInfo->Players;
		for( int i = 0; i < Players.Num(); i++ )
		{
			CPlayerInfo* NewPlayer = Players(i);
			
			if( NewPlayer && NewPlayer->IsAI() )
			{
				NewPacket.Frame = Frame;
				NewPacket.Owner = NewPlayer->GetID();
				NewPacket.Type	= 0;
				Result = GNetworkManager->SendPacket( EOCT_GameServer, &NewPacket, sizeof(NewPacket) );
				GNetworkManager->CheckSync( Frame, sizeof(REPLICATION_SYNC) );

				//if( Result <= 0 ) GNetworkManager->Disconnect();
			}
		}
	} */
}

bool OLCallbackCompareSync( NNetPacket* InA, NNetPacket* InB )
{
	if( InA->Frame > InB->Frame )
	{
		return true; // Swap
	}
	else if( InA->Frame == InB->Frame )
	{
		REPLICATION_ACTION* ReA = (REPLICATION_ACTION*)(InA->Data);
		REPLICATION_ACTION* ReB = (REPLICATION_ACTION*)(InB->Data);

		if( ReA->Owner > ReB->Owner  )
		{
			return true; // Swap
		}
		else if( ReA->Owner == ReB->Owner )
		{
			if( ReA->Action > ReB->Action )
			{
				return true; // Swap
			}
			else if( ReA->Action == ReB->Action )
			{
				if( ReA->Base > ReB->Base )
				{
					return true; // Swap
				}
				else if( ReA->Base == ReB->Base )
				{
					if( ReA->Code > ReB->Code )
					{
						return true; // Swap
					}
					else if( ReA->Code == ReB->Code )
					{
						if( ReA->Index > ReB->Index )
						{
							return true; // Swap
						}
					}
				}
			}
		}
	}

	return false; // Not Swap
}

//##############################################################################################
// PacketHandler
//##############################################################################################
void OLCallbackProcessPacket( NNetPacket* InPacket )
{
	if( InPacket->Code == 0xFFFF )
		return;

	NetCallbackProcessPacket LocalCallbackProcessPacket = GPacketHandlerMap.FindRef(InPacket->Code);
	if( LocalCallbackProcessPacket )
		LocalCallbackProcessPacket( InPacket );
}

//##############################################################################################
// Connection control
//##############################################################################################
void OLConnectGateServer()
{
	NNetworkConnection* TargetConnection = GNetworkManager->GetConnection( EOCT_GateServer );

	if( TargetConnection && TargetConnection->IsDisconnected() )
	{
#ifndef URL_GATE_SERVER_ADDR

		FnLog("NET: CONNECT, Empty Gate Server Address!!!!!!!!!!");
		///ShowAlertMessage( EMBA_OK, EMCT_Exit, L"The server address does not exist!!!!!!!!!" );
		return;
#endif

		char strServer[1024] = { URL_GATE_SERVER_ADDR };
		char strTarget[1024] = {0,};
        // SERVER_ADDR_INTERNAL URL_GATE_SERVER_ADDR
#if defined ( WIN32 )
		GetINI_Str(strTarget,1024,"Option.ini","CONNECT_SERVER","IP",strServer);
#else
		strcpy( strTarget, strServer );
#endif

		if( GNetworkManager->Connect(	EOCT_GateServer, strTarget, URL_GATE_SERVER_PORT,
										OLCallbackGateConnected, 
										OLCallbackGateDisconnected,
										OLCallbackGateSendComplete,
										OLCallbackGateRecvComplete,
										OLCallbackGateCheckPacket ) )
		{
			 // Pending success
			FnLog("NET: CONNECT, Connecting GateServer %s:%d", strTarget,URL_GATE_SERVER_PORT);

			///ShowProgressMessage();
		}
		else
		{
			// Pending failed, Can't connect
			if( GNetworkManager->GetErrorCode() != ENEC_PendingConnect )
			{
				FnLog("NET: CONNECT, Can not connecting GateServer, Connection is invalid state");

				if( !GOnlineInfo->CanNetwork() )
				{
					///ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s|| %s", IUGetLocalText( L"info_connect_error" ), IUGetLocalText( L"info_check_network" ) );
				}
				else
				{
					///ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s|| %s", IUGetLocalText( L"info_connect_error" ), IUGetLocalText( L"info_error_retry" ) );
				}

				GOnlineInfo->ResetChannelMigration();
			}
			else
			{
				// Skip
				FnLog("NET: CONNECT, Already connecting GateServer");
			}
		}
	}
	else
	{
		// Skip
		FnLog("NET: CONNECT, Can not connecting GateServer, Connection is null point or Already connected");
	}
}

void OLDisconnectGateServer()
{
	NNetworkConnection* TargetConnection = GNetworkManager->GetConnection( EOCT_GateServer );
	if( TargetConnection && !TargetConnection->IsDisconnected() )
	{
		GNetworkManager->Disconnect( EOCT_GateServer );
	}
}

void OLEventConnectedGateServer( NNetEvent* InEvent )
{
	if( !InEvent ) return;

	if( InEvent->EventCode == ENEC_Success )
	{
		// Connect success
		FnLog("NET: CONNECT, Connected GateServer");

		// Authenticate to GateServer
		OLRequestAuthGateServer();
	}
	else
	{
		// Connect failed..  , Cause server shut down or can't fine WiFi zone, etc...
		FnLog("NET: CONNECT, Connect GateServer error %d %d", InEvent->EventCode, InEvent->ErrorCode );

		// Check disconnect for next connect
		OLDisconnectGateServer();

		// Error control
		switch( InEvent->EventCode )
		{
		case ENEC_StateOnline: return; // Already Connected
			break;
		case ENEC_FailedSocket:
		case ENEC_FailedConnect:
		default:
			{
				GOnlineInfo->SetNetworkModel( ClientNetworkStatus( ONLINE_NETWORK_CHECK_URL ) );
				if( !GOnlineInfo->CanNetwork() )
				{
					// By Network ( WiFi, 3G, ... )
					///ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s|| %s..", IUGetLocalText( L"info_connect_error" ), IUGetLocalText( L"info_check_network" ) );
				}
				else if( InEvent->ErrorCode == 61 || InEvent->ErrorCode == 10061 )
				{
					// By Server
					///ShowAlertMessage( EMBA_OK, EMCT_Inspect, L"%s|| %s..", IUGetLocalText( L"info_server_check" ), IUGetLocalText( L"info_endgame" ) );
				}
				else
				{
					// By Route
					///ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s|| %s..", IUGetLocalText( L"info_connect_error" ), IUGetLocalText( L"info_error_retry" ) );
				}
			}
		}

		GOnlineInfo->ResetChannelMigration();
	}
}

void OLEventDisconnectedGateServer( NNetEvent* InEvent )
{
	if( !InEvent ) return;

	if( InEvent->EventCode == ENEC_Success )
	{
		// Disconnected Success
		FnLog("NET: DISCONNECT, Disconnected GateServer");

		if( InEvent->ErrorCode == ENEC_ClosedSocket )
		{
			// Disconnected by Server
			FnLog("NET: DISCONNECT, Disconnected GateServer By Server ClosedSocket");

			///ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s|| %s", IUGetLocalText( L"info_disconnect" ), IUGetLocalText( L"info_error_restart" ) );
			return;
		}

		/// if( GPatchInfo->IsNeedFirstPatch() ) return;
	}
	else
	{
		// Skip
		FnLog("NET: DISCONNECT, Disconnected GateServer failed %d %d", InEvent->EventCode, InEvent->ErrorCode );
	}
		}

void OLConnectGameServer()
{
	NNetworkConnection* TargetConnection = GNetworkManager->GetConnection( EOCT_GameServer );
	if( TargetConnection && TargetConnection->IsDisconnected() )
	{
		NServerChannel* TargetChannel = GOnlineInfo->GetSelectedChannel();
		if( !TargetChannel ) 
		{
			// Skip
			FnLog("NET: CONNECT, Server channel info is null point");
			return;
		}

		if( GNetworkManager->Connect(	EOCT_GameServer, TargetChannel->IP, TargetChannel->Port,
										OLCallbackGameConnected, 
										OLCallbackGameDisconnected,
										OLCallbackGameSendComplete,
										OLCallbackGameRecvComplete,
										OLCallbackGameCheckPacket ) )
		{
			// Pending success
			FnLog("NET: CONNECT, Connecting GameServer");

			///ShowProgressMessage();
		}
		else
		{
			// Pending failed, Can't connect
			if( GNetworkManager->GetErrorCode() != ENEC_PendingConnect )
			{
				FnLog("NET: CONNECT, Can not connecting GameServer, Connection is invalid state");

				if( !GOnlineInfo->CanNetwork() )
				{
					///ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s||%s", IUGetLocalText( L"info_connect_error" ), IUGetLocalText( L"info_check_network" ) );
				}
				else
				{
					///ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s||%s", IUGetLocalText( L"info_connect_error" ), IUGetLocalText( L"info_error_retry" ) );
				}
			}
			else
			{
				// Skip
				FnLog("NET: CONNECT, Already connecting GameServer");
			}
		}
	}
	else
	{
		// Skip
		FnLog("NET: CONNECT, Can not connecting GameServer, Connection is null point or Already connected");
	}  
}

void OLDisconnectGameServer()
{
	NNetworkConnection* TargetConnection = GNetworkManager->GetConnection( EOCT_GameServer );
	if( TargetConnection && !TargetConnection->IsDisconnected() )
	{
		GNetworkManager->Disconnect( EOCT_GameServer );
	}
}

void OLEventConnectedGameServer( NNetEvent* InEvent )
{
	if( !InEvent ) return;

	if( InEvent->EventCode == ENEC_Success )
	{
		// Connect success

		// Connected to GameServer
		FnLog("NET: CONNECT, Connected GameServer");

		// Send game login packet( REQ_GAME_LOGIN )
		OLRequestGameLogin();
	}
	else
	{
		// Connect failed..  , Cause server shut down or can't fine WiFi zone, etc...
		FnLog("NET: CONNECT, Connect GameServer error %d %d", InEvent->EventCode, InEvent->ErrorCode );

		OLDisconnectGameServer();

		// Error control
		switch( InEvent->EventCode )
		{
		case ENEC_StateOnline: return; // Already Connected
			break;
		case ENEC_FailedSocket:
		case ENEC_FailedConnect:
		default:
			{
				GOnlineInfo->SetNetworkModel( ClientNetworkStatus( ONLINE_NETWORK_CHECK_URL ) );
				if( !GOnlineInfo->CanNetwork() )
				{
					// By Network ( WiFi, 3G, ... )
					///ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s||%s..", IUGetLocalText( L"info_connect_error" ), IUGetLocalText( L"info_check_network" ) );
				}
				else if( InEvent->ErrorCode == 61 || InEvent->ErrorCode == 10061 )
				{
					// By Server
					///ShowAlertMessage( EMBA_OK, EMCT_Inspect, L"%s||%s..", IUGetLocalText( L"info_server_check" ), IUGetLocalText( L"info_endgame" ) );
				}
				else
				{
					// By Route
					///ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s||%s..", IUGetLocalText( L"info_connect_error" ), IUGetLocalText( L"info_error_retry" ) );
				}
			}
		}
	}
}

void OLEventDisconnectedGameServer( NNetEvent* InEvent )
{
	if( !InEvent ) return;
/*
	CScene*	CurrentScene = IUSceneManager().GetCurrentScene();

	OLEndedMatching();

	if( IsOpenedExitMessage() )
	{
		HideExitMessage();
	}
*/
	if( InEvent->EventCode == ENEC_Success )
	{
		FnLog("NET: DISCONNECT, Disconnected GameServer %d", InEvent->ErrorCode );

		bool bOldLogin = GOnlineInfo->IsLogin();

		GOnlineInfo->DidLogout();
/*
		if( GGameInfo->IsRunningGame() )
		{
			if( GGameInfo->IsMulti() )
			{
				if( GNetworkManager->IsSyncing() )
				{
					GNetworkManager->EndedSync();
				}

				if( GGameInfo->GameScene && GGameInfo->IsUpdatePlaying() )
				{
					GGameInfo->GameScene->EndGame( eUDGRT_Disconnected );
				}
			}
			else
			{
				// Single
				if( !GOnlineInfo->IsChannelMigrating() )
				{
					ShowNoticeMessage( IUGetLocalText( L"info_disconnect" ), 1000 );
				}
			}

			//GGameInfo->ReportGame();
		}
		else*/
		{
			if( bOldLogin )
			{
				if( GOnlineInfo->CanNetwork() && !GOnlineInfo->IsKickout() ) // && CurrentScene->SceneID != eID_Scene_Battle )
				{
					// Skip
					FnLog("NET: DISCONNECT, Disconnected GameServer No Action %d %d", InEvent->EventCode, InEvent->ErrorCode );
/*
					if( GGameInfo->IsMulti() )
					{
						if( CurrentScene->SceneID != eID_Scene_Game )
						{
							if( GNetworkManager->IsSyncing() )
							{
								GNetworkManager->EndedSync();
							}

							if( GGameInfo->GameScene && GGameInfo->IsUpdatePlaying() )
							{
								GGameInfo->GameScene->EndGame( eUDGRT_Disconnected );
							}
						}
						else // if( pScene->GetID() != eID_Scene_Loading )
						{
							GGameInfo->bDisconnected = true;
						}
					}
					else
					{
						// Single
						if( !GOnlineInfo->IsChannelMigrating() && InEvent->ErrorCode == ENEC_ClosedSocket )
						{
							ShowNoticeMessage( IUGetLocalText( L"info_disconnect" ), 1000 );
						}
					} */
				}
				else
				{
					if( GOnlineInfo->IsKickout() )
					{
						FnLog("NET: DISCONNECT, Alert Restart %d", GOnlineInfo->GetNetworkModel() );

						///ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s||%s", IUGetLocalText( L"info_disconnect" ), IUGetLocalText( L"info_error_restart" ) );
					}
					else
					{
						FnLog("NET: DISCONNECT, Alert Reconnect %d", GOnlineInfo->GetNetworkModel() );

						///ShowAlertMessage( EMBA_OK, EMCT_Reconnect, L"%s||%s", IUGetLocalText( L"info_disconnect" ), IUGetLocalText( L"info_check_network" ) );
					}
				}
			}

			GOnlineInfo->DidKickin();
		}

		if( GOnlineInfo->IsChannelMigrating() )
		{
			OLAuthenticateLogin();
		}
	}
	else
	{
		// Skip
		FnLog("NET: DISCONNECT, Disconnected GameServer failed %d %d", InEvent->EventCode, InEvent->ErrorCode );
	}	
}

void OLDisconnectCurrentServer( bool InResetPatch/*=false*/ )
{
   /// if( InResetPatch )
      ///  GPatchInfo->ResetFirstPatch();
    
	GNetworkManager->Disconnect();
}

//##############################################################################################
// Patch
//##############################################################################################
void OLPatchComplete( int InCode )
{
    
    FnLog("NET: OLPatchComplete:%d", InCode);
    
    /*
	switch( InCode )
	{
	case EPRC_Success:
		{
			char VersionString[10];
			sprintf( VersionString, "%d", GPatchInfo->GetNewBuildVersion() );
			GSaveData->SetValue( "VERSION", VersionString );
			GSaveData->SaveData();

			OLInitGameData();

			GMessageInfo->Activate( true );
            
            Telegram* NewMsg = new Telegram( 0, EOMC_PatchEnded, 0, CustomMessage::Online );
            IUGameManager().Message( NewMsg );

//			CTitleScene *pScene = (CTitleScene *)IUSceneManager().GetCurrentScene( eID_Scene_Title );
//			if( pScene ) pScene->PatchCompleted();

		}
		break;
	case EPRC_SuccessSkip:
		{
			char VersionString[10];
			sprintf( VersionString, "%d", GPatchInfo->GetNewBuildVersion() );
			GSaveData->SetValue( "VERSION", VersionString );
			GSaveData->SaveData();

            Telegram* NewMsg = new Telegram( 0, EOMC_PatchEnded, 0, CustomMessage::Online );
            IUGameManager().Message( NewMsg );
            
//			CTitleScene *pScene = (CTitleScene *)IUSceneManager().GetCurrentScene( eID_Scene_Title );
//			if( pScene ) pScene->PatchCompleted();

		}
		break;
	case EPRC_DownloadFailed:
		{
			// All File Download failed
			if( !IsOpenedAlertMessage(EMCT_Exit) )
			{
				///ShowAlertMessage( EMBA_OK, EMCT_Exit, L"%s||%s||%s", IUGetLocalText( L"info_error_patch" ), IUGetLocalText( L"info_error_restart" ), IUGetLocalText( L"info_endgame" ) );
			}
		}
		break;
	case EPRC_InfoFailed:
		{
			if( !IsOpenedAlertMessage(EMCT_Exit) )
			{
				///ShowAlertMessage( EMBA_OK, EMCT_Exit, L"%s||%s||%s", IUGetLocalText( L"info_error_patch" ), IUGetLocalText( L"info_error_restart" ), IUGetLocalText( L"info_endgame" ) );
			}
		}
		break;
	case EPRC_InvalidFiles:
		{
			if( !IsOpenedAlertMessage(EMCT_Exit) )
			{
				///ShowAlertMessage( EMBA_OK, EMCT_Exit, L"%s||%s", IUGetLocalText( L"info_error_patch" ), IUGetLocalText( L"info_endgame" ) );
			}
		}
		break;
	}
*/
}

void OLInitGameData( int InOption/*=0*/ )
{
	/*if( !GMessageInfo->IsActivated() )
	{
		GMessageInfo->Activate( true );
	}

	bool bCheckCrc = GGameData->CheckCrc( "gameData.xml", 0 );
	bCheckCrc = GGameData->CheckCrc( "product.xml", 0 );

//	GGameData->LoadData("gameData.xml");
	GGameData->LoadProduct( "product.xml" );
	GGameData->LoadRankingScore( "Ranking_Score.xml" );
	GGameData->LoadTB_InstantItem( "InstantItemData.xml" );
	GGameData->Load_Grade( "GradeData.xml" );

	// Set ranking score condition datas.
	GGameInfo->SetRankingScoreData( GGameData->GetRankScoreData() );

	// setting
	{
		bool bEnable;
		char* SavedValue;
		SavedValue = (char*)GSaveData->GetValue( "soundEffect" );
		bEnable = ( strcmp( SavedValue, "" ) == 0 || strcmp( SavedValue, "true" ) == 0 );
		IUGetSoundManager().SetSoundOn( bEnable );

		SavedValue = (char*)GSaveData->GetValue( "soundMusic" );
		bEnable = ( strcmp( SavedValue, "" ) == 0 || strcmp( SavedValue, "true" ) == 0 );
		IUGetSoundManager().SetBGMOn( bEnable );

		SavedValue = (char*)GSaveData->GetValue( "network3G" );
		bEnable = ( strcmp( SavedValue, "" ) == 0 || strcmp( SavedValue, "true" ) == 0 );
		GOnlineInfo->UseNetworkWWAN( bEnable );
	}

	// load unit icon texture
	std::vector<CObjectData *> objectList = GGameData->GetObjectList();
	for( std::vector<CObjectData *>::iterator it = objectList.begin() ; it != objectList.end() ; it++ )
	{
		if( !(*it)->m_Icon.empty() )
		{
			IUAttributeManager().GetAttribute( (*it)->m_Icon );
		}
	}

	// play background sound
	EnginePlayBGM( S002_BG ); */
}

//##############################################################################################
// Member
//##############################################################################################

void OLResetMember()
{
	GOnlineInfo->ResetAccount();
	GUserInfo->ResetAll();
	///GFriendsInfo->ResetAll();

	///ShowAlertMessage( EMBA_OK, EMCT_Restart, IUGetLocalText( L"info_unregister_success" ) );
    
    
    /*
	switch( InCode )
	{
	case EPRC_Success:
		{
			char VersionString[10];
			sprintf( VersionString, "%d", GPatchInfo->GetNewBuildVersion() );
			GSaveData->SetValue( "VERSION", VersionString );
			GSaveData->SaveData();

			OLInitGameData();

			GMessageInfo->Activate( true );
            
            Telegram* NewMsg = new Telegram( 0, EOMC_PatchEnded, 0, CustomMessage::Online );
            IUGameManager().Message( NewMsg );

//			CTitleScene *pScene = (CTitleScene *)IUSceneManager().GetCurrentScene( eID_Scene_Title );
//			if( pScene ) pScene->PatchCompleted();

		}
		break;
	case EPRC_SuccessSkip:
		{
			char VersionString[10];
			sprintf( VersionString, "%d", GPatchInfo->GetNewBuildVersion() );
			GSaveData->SetValue( "VERSION", VersionString );
			GSaveData->SaveData();

            Telegram* NewMsg = new Telegram( 0, EOMC_PatchEnded, 0, CustomMessage::Online );
            IUGameManager().Message( NewMsg );
            
//			CTitleScene *pScene = (CTitleScene *)IUSceneManager().GetCurrentScene( eID_Scene_Title );
//			if( pScene ) pScene->PatchCompleted();

		}
		break;
	case EPRC_DownloadFailed:
		{
			// All File Download failed
			if( !IsOpenedAlertMessage(EMCT_Exit) )
			{
				///ShowAlertMessage( EMBA_OK, EMCT_Exit, L"%s||%s||%s", IUGetLocalText( L"info_error_patch" ), IUGetLocalText( L"info_error_restart" ), IUGetLocalText( L"info_endgame" ) );
			}
		}
		break;
	case EPRC_InfoFailed:
		{
			if( !IsOpenedAlertMessage(EMCT_Exit) )
			{
				///ShowAlertMessage( EMBA_OK, EMCT_Exit, L"%s||%s||%s", IUGetLocalText( L"info_error_patch" ), IUGetLocalText( L"info_error_restart" ), IUGetLocalText( L"info_endgame" ) );
			}
		}
		break;
	case EPRC_InvalidFiles:
		{
			if( !IsOpenedAlertMessage(EMCT_Exit) )
			{
				///ShowAlertMessage( EMBA_OK, EMCT_Exit, L"%s||%s", IUGetLocalText( L"info_error_patch" ), IUGetLocalText( L"info_endgame" ) );
			}
		}
		break;
	}
*/
}
/*
void OLInitGameData( int InOption )
{
	if( !GMessageInfo->IsActivated() )
	{
		GMessageInfo->Activate( true );
	}

	bool bCheckCrc = GGameData->CheckCrc( "gameData.xml", 0 );
	bCheckCrc = GGameData->CheckCrc( "product.xml", 0 );

//	GGameData->LoadData("gameData.xml");
	GGameData->LoadProduct( "product.xml" );
	GGameData->LoadRankingScore( "Ranking_Score.xml" );
	GGameData->LoadTB_InstantItem( "InstantItemData.xml" );
	GGameData->Load_Grade( "GradeData.xml" );

	// Set ranking score condition datas.
	GGameInfo->SetRankingScoreData( GGameData->GetRankScoreData() );

	// setting
	{
		bool bEnable;
		char* SavedValue;
		SavedValue = (char*)GSaveData->GetValue( "soundEffect" );
		bEnable = ( strcmp( SavedValue, "" ) == 0 || strcmp( SavedValue, "true" ) == 0 );
		IUGetSoundManager().SetSoundOn( bEnable );

		SavedValue = (char*)GSaveData->GetValue( "soundMusic" );
		bEnable = ( strcmp( SavedValue, "" ) == 0 || strcmp( SavedValue, "true" ) == 0 );
		IUGetSoundManager().SetBGMOn( bEnable );

		SavedValue = (char*)GSaveData->GetValue( "network3G" );
		bEnable = ( strcmp( SavedValue, "" ) == 0 || strcmp( SavedValue, "true" ) == 0 );
		GOnlineInfo->UseNetworkWWAN( bEnable );
	}

	// load unit icon texture
	std::vector<CObjectData *> objectList = GGameData->GetObjectList();
	for( std::vector<CObjectData *>::iterator it = objectList.begin() ; it != objectList.end() ; it++ )
	{
		if( !(*it)->m_Icon.empty() )
		{
			IUAttributeManager().GetAttribute( (*it)->m_Icon );
		}
	}

	// play background sound
	EnginePlayBGM( S002_BG );
} */

//##############################################################################################
// Member
//##############################################################################################
/*
void OLResetMember()
{
	GOnlineInfo->ResetAccount();
	GUserInfo->ResetAll();
	///FResetRegistration();
}
*/
void OLClearSaveMember()
{
///	GSaveData->SetValue( "LogInID", "" );
	///GSaveData->SaveData();
}

bool OLExistSaveMember()
{
    
#if WIN32
    // For CND Login
    
    GOnlineInfo->PlatformCode = eUDPC_CND;
    GOnlineInfo->UseGuest( false );

    return false;
    
#else
    // For Platform( Kakao, HSP, Facebook, WeMe... ) Login
    /*
	const char* TempUserID = GSaveData->GetValue( "LogInID" );
	if( 0 != TempUserID[0] )
	{
		strcpy( GOnlineInfo->PlatformUserID, TempUserID ) ;
        
        const char* TempUserPW = GSaveData->GetValue( "LogInPW" );
        if( 0 != TempUserPW[0] )
        {
            strcpy( GOnlineInfo->PlatformUserPW, TempUserPW ) ;
        }
        
        GOnlineInfo->PlatformCode = PFGetPlatformCode();
        GOnlineInfo->UseGuest( false );
        
        // For WeMe(WemadeSocial) Auto Login
        const char* TempPlatform = GSaveData->GetValue( "LogInPlatform" );
        if( 0 != TempPlatform[0] && !strcmp( TempPlatform, "WeMe") )
        {
            GOnlineInfo->PlatformCode = eUDPC_WemadeSocial;            
        }
        
        return true;
	} 
    
	return false;
*/
    return true;
    
#endif

}

void OLWriteSaveMember()
{
    if( GOnlineInfo->PlatformCode == eUDPC_Guest || GOnlineInfo->PlatformCode == eUDPC_CND )
        return;
 /*
    if( strlen( GOnlineInfo->PlatformUserID ) > 0 )
    {
        GSaveData->SetValue( "LogInID", GOnlineInfo->PlatformUserID );
        //GSaveData->SaveData();
    }

    if( strlen( GOnlineInfo->PlatformUserPW ) > 0 )
    {
        GSaveData->SetValue( "LogInPW", GOnlineInfo->PlatformUserPW );
        //GSaveData->SaveData();
    }
    
    if( GOnlineInfo->PlatformCode == eUDPC_WemadeSocial )
    {
        GSaveData->SetValue( "LogInPlatform", "WeMe" );
        //GSaveData->SaveData();
    }
    else
    {
        GSaveData->SetValue( "LogInPlatform", "Platform" );
        //GSaveData->SaveData();        
    }
    
    GSaveData->SaveData(); */
}

// not use
void OLCheckAutoAddFriend()
{
	if( !GOnlineInfo->IsLogin() ) return;

/*
 long long ResultKeyValue = 0;
	char ResultNick[UD_NICK_LEN] = {0,};

	ResultKeyValue = PFGetAddedFriend( ResultNick );
	if( strlen( ResultNick ) > 0 )
	{
		char TempKakaoID[64];
		sprintf( TempKakaoID, "%lld", ResultKeyValue );
		//GFriendsManager->ArgFriendKakaoID = ResultKeyValue;
		OLRequestAddFriend( 0, TempKakaoID, eUDFT_KakaoTalk );            
	} */
}

//##############################################################################################
// Device
//##############################################################################################
void OLRefreshDeviceInfo()
{
    /*
	ClientUniqueID( GOnlineInfo->UDID );
	ClientDeviceModel( GOnlineInfo->DeviceModel );
	ClientStoreName( GOnlineInfo->DeviceStore );
	GOnlineInfo->DeviceOSVer = ClientSystemVersion( GOnlineInfo->DeviceVersion );
	GOnlineInfo->DeviceOS = ClientSystemType();

	const char* ValueText = GSaveData->GetValue( "DeviceAppUUID" );
	if( strlen( ValueText ) > 0 )    
	{
		strcpy( GOnlineInfo->UUID, ValueText );
	}
	else
	{
		ClientGeneratedID( GOnlineInfo->UUID );

		GSaveData->SetValue( "DeviceAppUUID", GOnlineInfo->UUID );
		GSaveData->SaveData();
	} */
}

void OLConditionalReplicatePing()
{
    /*
	if( GGameInfo->IsMulti() && GGameInfo->IsRunningGame() && GNetworkManager->IsSyncing() )
	{
		OLReplicatePing( GOnlineInfo->GetPingGrade() );

		CPlayerInfo* Player = GGameInfo->GetPlayer( GNetworkManager->GetSyncID() );
		if( Player ) Player->PingGrade = GOnlineInfo->GetPingGrade();
	} */
}

//##############################################################################################
// Match
//##############################################################################################
void OLBeganMatchMigrating( int InTargetChannelID )
{
	GOnlineInfo->BeganMatchMigrating();

	GOnlineInfo->MigrateChannel( InTargetChannelID );

	OLDisconnectCurrentServer();

	//OLAuthenticateLogin();
}

void OLEndedMatchMigrating()
{
	GOnlineInfo->EndedMatchMigrating();
}

void OLBeganMatchInviting( int InBattleMode, int InMatchMode, int InMatchTarget/*=0*/, long long InTargetKey/*=0*/, const char* InTargetName/*=0*/, bool InInvited/*=false*/ )
{
	GOnlineInfo->BeganMatchInviting( InBattleMode, InMatchMode, InMatchTarget, InTargetKey, InTargetName );

/**
 WCHAR TempWideText[256] = {0,};
	char TempText[128] = {0,};

	if( InInvited )
	{
		// Accepted Match Invitation

		if( InBattleMode == eUDBM_Item )	
		{
			strcpy( TempText, "Common01_Message_ItemMode_Icon" ); 
		}
		else //if( RecvPacket->BattleMode == eUDBM_Ladder ) 
		{
			strcpy( TempText, "Common01_Message_LadderMode_Icon" );
		}

		if( InTargetName && strlen( InTargetName ) > 0 && InMatchTarget == eUDMT_Friend )
		{
			GetWideString( InTargetName, TempWideText, UD_NICK_LEN-1 );
			wcscpy( TempWideText + wcslen(TempWideText), IUGetLocalText( L"info_battle_invite" ) );
		}
		else
		{
			wcscpy( TempWideText, IUGetLocalText( L"info_battle_invite_who" ) );
		}

		HideMatchMessage();

		ShowMatchMessage( eUDMMT_Invite, EMBA_OK, 1, TempWideText, TempText, 10000 );
	}
	else
	{
		// Send Match Invitation

		OLReportCustomMatch( eUDMMT_Invite, GOnlineInfo->MatchMode, GOnlineInfo->BattleMode, GOnlineInfo->MatchTarget, GOnlineInfo->MatchTargetKey, GOnlineInfo->MatchTargetNick, 0 );

		ShowMatchMessage( eUDMMT_Wait, EMBA_Cancel, 2, IUGetLocalText( L"info_battle_invite_wait" ), 0, 20000 );
	} */
}

void OLEndedMatchInviting()
{
	if( GOnlineInfo->IsMatchInviting() )
	{
		GOnlineInfo->EndedMatchInviting();
	}

	///HideMatchMessage();
}

void OLBeganMatching( int InBattleMode, int InMatchMode, int InMatchTarget/*=-1*/, long long InMatchTargetKey/*=0*/, bool InNoMessage/*=false*/ )
{
    /*
	OLEndedMatchMigrating();
	OLEndedMatchInviting();

	GOnlineInfo->BeganMatching( InBattleMode, InMatchMode, InMatchTarget, InMatchTargetKey );

	int NewAIAbility = 0;
	int NewMissionNo = 0;

	switch( GOnlineInfo->BattleMode )
	{
	case eUDBM_Mission:
		{
			NewAIAbility = GGameInfo->GetMissionData()->m_AILevel;
			NewMissionNo = GGameInfo->GetMissionData()->m_MissionNo + GGameInfo->SelectMissionLevelNo;
		}
		break;
	}

	float bMulti = (GOnlineInfo->MatchTarget < eUDMT_Single);
	if( bMulti )
	{
		if( !InNoMessage )
		{
		ShowMatchMessage( eUDMMT_Start, EMBA_Cancel, 2, IUGetLocalText( L"info_find_user" ), 0, 30000 );
		}

		CScene* TargetScene = IUSceneManager().GetCurrentScene();
		if( TargetScene && TargetScene->GetChildScene() )
		{
			TargetScene->CloseChildScene();
		}
	}

	OLRequestStartMatch( GOnlineInfo->MatchMode, GOnlineInfo->BattleMode, GOnlineInfo->MatchTarget, GOnlineInfo->MatchTargetKey, NewAIAbility, NewMissionNo, GGameInfo->nRankingGameLv ); */
}

void OLEndedMatching( int InBattleMode/*=-1*/, int InMatchMode/*=-1*/ )
{
    /*
	HideMatchMessage( true );
	//HideMatchMessage();

	GOnlineInfo->EndedMatching( InBattleMode, InMatchMode );

	if( InBattleMode < 0 && InMatchMode < 0 ) return;

	if( GOnlineInfo->MatchMode == eUDMM_Single )
	{
		if( GOnlineInfo->BattleMode == eUDBM_Mission && GGameInfo->GetMissionData() )
		{
			OLRequestStartGame( GGameInfo->GetMissionData()->m_Map[0] );
		}
	}
	else
	{
		ChangeScene( eID_Scene_Battle );
	} */
}







