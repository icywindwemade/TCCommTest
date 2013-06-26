//##############################################################################################
// NetworkManager.h
//##############################################################################################
#ifndef _NETWORK_MANAGER_H_
#define _NETWORK_MANAGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <vector>

#include "Core.h"
#include "NetworkConnection.h"

//##############################################################################################
// Callback
//##############################################################################################
class NNetPacket;
class NNetEvent;
typedef	void (* NetCallbackUpdated) ( int InElapsedTime );
typedef	void (* NetCallbackProcessPacket) ( NNetPacket* InPacket );
typedef	int (* NetCallbackBuildPacket) ( NNetworkConnection* InConnection, void* InOutData, int InSize );
typedef	void (* NetCallbackFlushSync) ( int InSyncBytes );
typedef	bool (* NetCallbackCompareSync) ( NNetPacket* InA, NNetPacket* InB );
typedef	void (* NetCallbackEvent) ( NNetEvent* InEvent );

//##############################################################################################
// NNetPacket
//##############################################################################################
#pragma warning( push )
#pragma warning( disable: 4200 )   // char Data[0]

class NNetPacket
{
public:
	enum
	{ 
		Internal = 0x1,
		External = 0x2,
		Allround = 0x3
	};

	static void* operator new( size_t InObjectSize, int InDataSize );
	static void operator delete( void* InObject, int InDataSize );

public:
	NNetPacket( void* InOwner, unsigned short InCode, void* InData, unsigned short InSize, unsigned short InFlag=Allround, int InFrame=0 );

public:
	void*			Owner;		// Connection
	int				Frame;
	unsigned short	Code;
	unsigned short	Size;
	unsigned short	Flag;

	char			Data[0];	// Start of the actual buffer, must remain the last
};

#pragma warning( pop ) // 4200

//##############################################################################################
// NNetEvent
//##############################################################################################
class NNetEvent
{
public:
	enum
	{ 
		Connected		= 0x0,
		Disconnected	= 0x1,
	};

public:
	NNetEvent( int InEventID );
	void ExecEvent();
	bool IsPending();

public:
	int					EventID;
	NNetworkConnection*	EventConnection;
	int					EventCode;
	int					ErrorCode;
	NetCallbackEvent	OnEvent;
};

//##############################################################################################
// NNetworkManager
//##############################################################################################
class NNetworkManager
{
public:
	static NNetworkManager* Instance();

public:
	NNetworkManager();
	~NNetworkManager();

	bool Start( int InMaxConnections, int InFixedFrameTime, int InLatencyFrame, 
				NetCallbackUpdated InUpdated,
				NetCallbackProcessPacket InProcessPacket, 
				NetCallbackBuildPacket InBuildPacket,
				NetCallbackFlushSync InFlushSync,
				NetCallbackCompareSync InCompareSync );
	void Stop();

	void SuspendThread();
	void ResumeThread();
	bool IsRunningThread();

	void Run();
	void Update( int InElapsedTime );

	bool Connect( int InConnectionIndex, const char* InAddr, unsigned short InPort, 
				  NetCallbackConnected InConnected,
				  NetCallbackDisconnected InDisconnected, 
				  NetCallbackSendComplete InSendComplete, 
				  NetCallbackRecvComplete InRecvComplete, 
				  NetCallbackCheckPacket InCheckPacket );

	bool Disconnect( int InConnectionIndex=-1 );

	bool HandleEvent( int EventID, NNetworkConnection* InConnection, int InEventCode, int InErrorCode, NetCallbackEvent InEvent );

	void Lock();
	void Unlock();

	int SendPacket( NNetworkConnection* InConnection, void* InData, int InSize, bool InSync=false );
	int SendPacket( int InConnectionIndex, void* InData, int InSize, bool InSync=false );
	
	void PushRecvDataPacket( NNetworkConnection* InConnection, unsigned short InProtocol, void* InData, unsigned short InSize );
	void PushRecvSyncPacket( NNetworkConnection* InConnection, unsigned short InProtocol, void* InData, unsigned short InSize, int InFrame );
	void PushSendSyncPacket( unsigned short InProtocol, void* InData, unsigned short InSize, int InFrame, int InDirection );
	int CompareSyncPacket( NNetPacket* InA, NNetPacket* InB );

	void DriveSync();
	void BeganSync();
	void EndedSync();
	void RenewSync();
	void CheckSync( int InRemoteFrame, int InSize );

	void SetSyncID( int InIndex=0 );
	int GetSyncID();

	void SetSyncRate( float InRate=1.0f );
	float GetSyncRate();

	bool IsSyncing();
	bool IsSyncDrive();
	bool IsSyncPause();

	int GetFixedFrameTime();
	int GetLatencyFrame();

	int GetErrorCode();
	int GetNetworkTime();
	int GetLastSendTime();
	int GetLastRecvTime();
	int GetSendBytes();
	int GetRecvBytes();

	int GetSyncFrame();
	int GetSyncTime();
	int GetSyncCheckFrame();
	int GetSyncRemoteFrame();
	int GetSyncNextFrame();
	int GetSyncExecTime();
	int GetSyncElapsedTime();
	int GetSyncLastSendTime();
	int GetSyncLastRecvTime();
	int GetSyncSendBytes();
	int GetSyncRecvBytes();
	int GetSyncSendPackets();
	int GetSyncRecvPackets();
	int GetSyncDelayCount();
	int GetSyncDelayTime();
	int GetSyncDelaySumTime();
	int GetSyncDelayMaxTime();

	NNetworkConnection* GetPendingConnection();
	NNetworkConnection* GetCurrentConnection();
	NNetworkConnection* GetConnection( int InIndex );
	int GetConnectionIndex( NNetworkConnection* InConnection );

private:
	void DispatchRecvDataPackets();
	void DispatchRecvSyncPackets();
	void DispatchOwnsSyncPackets();
	void DispatchMainSyncPackets();
	void DispatchSendSyncPackets();
	
	void ResetRecvSyncPackets();
	void ResetOwnsSyncPackets();
	void ResetMainSyncPackets();
	void ResetSendSyncPackets();

private:
	NNetworkConnection*			Connections;
	NNetworkConnection*			CurrentConnection;
	NNetworkConnection*			PendingConnection;

	NNetEvent*					EventConnected;
	NNetEvent*					EventDisconnected;

	int							ThreadFlag;
	int							MaxConnections;
	int							ErrorCode;
	int							FixedFrameTime;
	int							LatencyFrame;
	int							NetworkTime;
	int							LastSendTime;
	int							LastRecvTime;
	int							SendBytes;
	int							RecvBytes;

	float						SyncRate;
	int							SyncID;
	int							SyncTime;
	int							SyncFrame;
	int							SyncCheckFrame;
	int							SyncRemoteFrame;
	int							SyncElapsedTime;
	int							SyncLastSendTime;
	int							SyncLastRecvTime;
	int							SyncSendBytes;
	int							SyncRecvBytes;
	int							SyncSendPackets;
	int							SyncRecvPackets;
	int							SyncDelayCount;
	int							SyncDelayTime;
	int							SyncDelaySumTime;
	int							SyncDelayMaxTime;

	bool						bRunning; // Running (Tick, Update)
	bool						bSyncing; // Sync state
	bool						bSyncDrive; // Sync drive
	bool						bSyncPause; // Sync pause

	TArray<NNetPacket*>			QueueRecvDataPackets;
	TArray<NNetPacket*>			QueueRecvSyncPackets;
	TArray<NNetPacket*>			QueueSendSyncPackets;
	TArray<NNetPacket*>			QueueOwnsSyncPackets;
	TArray<NNetPacket*>			QueueMainSyncPackets;

	NetCallbackUpdated			OnUpdated;
	NetCallbackProcessPacket	OnProcessPacket;
	NetCallbackBuildPacket		OnBuildPacket;
	NetCallbackFlushSync		OnFlushSync;
	NetCallbackCompareSync		OnCompareSync;
};

#endif // _NETWORK_MANAGER_H_



