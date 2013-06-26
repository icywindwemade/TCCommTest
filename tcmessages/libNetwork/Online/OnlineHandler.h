//##############################################################################################
// OnlineHandler.h
//##############################################################################################
#ifndef _ONLINE_HANDLER_H_
#define _ONLINE_HANDLER_H_

#include "Core.h"
#include "NetworkManager.h"
#include "OnlineInfo.h"
#include "PacketEncrypt.h"

// Common Include ( Packet )
#include "TypeDefine.h"
#include "UnionDefine.h"
#include "Packet.h"
#include "GatePacket.h"
#include "GamePacket.h"

//#include "GlobalDefine.h"

//##############################################################################################
// GNetworkManager
//##############################################################################################

extern NNetworkManager* GNetworkManager;

//##############################################################################################
// Default
//##############################################################################################
extern void OLInit();
extern void OLFree();

//##############################################################################################
// Packet encryption
//##############################################################################################
extern void OLPacketEncrypt( int InConnectionIndex, PACKET* InOutPacket );
extern ePacketEncryptCode OLPacketDecrypt( int InConnectionIndex, PACKET* InPacket );

extern void OLSendPacket( int InConnectionIndex, void* InPacket, int InSize );
extern void OLSendGatePacket( void* InPacket, int InSize );
extern void OLSendGamePacket( void* InPacket, int InSize );

//##############################################################################################
// Callback I / O event
//##############################################################################################
// GateServer
extern void OLCallbackGateConnected( NNetworkConnection* InConnection, int InErrorCode );
extern void OLCallbackGateDisconnected( NNetworkConnection* InConnection, int InErrorCode );
extern void OLCallbackGateSendComplete( NNetworkConnection* InConnection, int InSize );
extern void OLCallbackGateRecvComplete( NNetworkConnection* InConnection, char* InData, int InSize );
extern int  OLCallbackGateCheckPacket( NNetworkConnection* InConnection, char* InData, int InSize );

// GameServer
extern void OLCallbackGameConnected( NNetworkConnection* InConnection, int InErrorCode );
extern void OLCallbackGameDisconnected( NNetworkConnection* InConnection, int InErrorCode );
extern void OLCallbackGameSendComplete( NNetworkConnection* InConnection, int InErrorCode );
extern void OLCallbackGameRecvComplete( NNetworkConnection* InConnection, char* InData, int InSize );
extern int	OLCallbackGameCheckPacket( NNetworkConnection* InConnection, char* InData, int InSize );

// Post update
extern void OLCallbackNetworkUpdated( int InElapsedTime );

//##############################################################################################
// Callback sync packet control
//##############################################################################################
extern int	OLCallbackBuildPacket( NNetworkConnection* InConnection, void* InOutData, int InSize );
extern void OLCallbackFlushSync( int InSyncBytes );
extern bool OLCallbackCompareSync( NNetPacket* InA, NNetPacket* InB );

//##############################################################################################
// Callback process packet
//##############################################################################################
extern void OLCallbackProcessPacket( NNetPacket* InPacket );

//##############################################################################################
// Connection control
//##############################################################################################
extern void OLConnectGateServer();
extern void OLDisconnectGateServer();
extern void OLEventConnectedGateServer( NNetEvent* InEvent );
extern void OLEventDisconnectedGateServer( NNetEvent* InEvent );

extern void OLConnectGameServer();
extern void OLDisconnectGameServer();
extern void OLEventConnectedGameServer( NNetEvent* InEvent );
extern void OLEventDisconnectedGameServer( NNetEvent* InEvent );

extern void OLDisconnectCurrentServer( bool InResetPatch=false );

//##############################################################################################
// Patch
//##############################################################################################
extern void OLPatchComplete( int InCode );
extern void OLInitGameData( int InOption=0 );

//##############################################################################################
// Member
//##############################################################################################
extern void OLResetMember();
extern void OLClearSaveMember();
extern bool OLExistSaveMember();
extern void OLWriteSaveMember();

extern void OLCheckAutoAddFriend();

//##############################################################################################
// Device
//##############################################################################################
extern void OLRefreshDeviceInfo();
extern void OLConditionalReplicatePing();

//##############################################################################################
// Match
//##############################################################################################
extern void OLBeganMatchMigrating( int InTargetChannelID );
extern void OLEndedMatchMigrating();

extern void OLBeganMatchInviting( int InBattleMode, int InMatchMode, int InMatchTarget=0, long long InTargetKey=0, const char* InTargetName=0, bool InInvited=false );
extern void OLEndedMatchInviting();

extern void OLBeganMatching( int InBattleMode, int InMatchMode, int InMatchTarget=-1, long long InMatchTargetKey=0, bool InNoMessage=false );
extern void OLEndedMatching( int InBattleMode=-1, int InMatchMode=-1 );

#endif // _ONLINE_HANDLER_H_