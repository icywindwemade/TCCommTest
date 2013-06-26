//##############################################################################################
// NetworkManager.cpp
//##############################################################################################

#include "NetworkManager.h"
#include "IUDefine.h"
//#include "IU.h"

#pragma warning( push )
#pragma warning( disable : 4127 )   // while( TRUE ) : TRUE is const type warning

bool CompareSyncFrame( NNetPacket* A, NNetPacket* B )
{
	return ( A->Frame < B->Frame );
}

//##############################################################################################
// NNetPacket
//##############################################################################################
void* NNetPacket::operator new( size_t InObjectSize, int InDataSize )
{
	void* NewMem = new char[InObjectSize + InDataSize];

	return NewMem;
}

void NNetPacket::operator delete( void* InObject, int /*InDataSize*/ )
{
	delete [] (char*)InObject;
}  

NNetPacket::NNetPacket( void* InOwner, unsigned short InCode, void* InData, unsigned short InSize, unsigned short InFlag/*=Allround*/, int InFrame/*=0*/ )
{
	Owner		= InOwner;
	Frame		= InFrame;
	Code		= InCode;
	Size		= InSize;
	Flag		= InFlag;

	if( InData && InSize > 0 )
	{
		memcpy( Data, InData, InSize );
	}
}

//##############################################################################################
// NNetPacket
//##############################################################################################
NNetEvent::NNetEvent( int InEventID )
	: EventID( InEventID )
	, EventConnection( 0 )
	, EventCode( 0 )
	, ErrorCode( 0 )
	, OnEvent( 0 )
{
}

void NNetEvent::ExecEvent()
{
	if( OnEvent ) OnEvent( this );

	EventConnection = 0;
	EventCode = 0;
	ErrorCode = 0;
	OnEvent = 0;
}

bool NNetEvent::IsPending()
{
	if( EventConnection ) return true;

	return false;
}

//##############################################################################################
// Network Thread
//##############################################################################################
#ifdef NET_DEVICE_WIN

HANDLE NetThread = 0;
CRITICAL_SECTION NetLock;
unsigned int __stdcall NetworkThreadProc( void* InParam )

#else //  NET_DEVICE_IOS | NET_DEVICE_ANDROID

pthread_t NetThread = 0;
pthread_mutex_t NetLock;
pthread_cond_t NetCond = PTHREAD_COND_INITIALIZER;

void* NetworkThreadProc( void* InParam )

#endif // NET_DEVICE_WIN
{
	( (NNetworkManager*)InParam )->Run();

#ifdef NET_DEVICE_WIN
	return 0;
#else //  NET_DEVICE_IOS
	LogPrintf("pthread_exit()");
	pthread_exit((void *)NULL);
#endif // NET_DEVICE_WIN

}

//##############################################################################################
// NNetworkManager
//##############################################################################################
NNetworkManager* NNetworkManager::Instance() 
{ 
	static NNetworkManager StaticInstance;
	return &StaticInstance; 
}

NNetworkManager::NNetworkManager()
{
	ThreadFlag = 0;
	Connections = 0;
	CurrentConnection = 0;
	PendingConnection = 0;

	EventConnected = 0;
	EventDisconnected = 0;

	MaxConnections	= 0;
	FixedFrameTime	= 0;
	LatencyFrame = 0;
	NetworkTime = 0;
	LastSendTime = 0;
	LastRecvTime = 0;
	SendBytes = 0;
	RecvBytes = 0;

	SyncRate = 1.0f;
	bRunning = false;
	bSyncing = false;
	bSyncPause = false;

	OnUpdated = 0;
	OnProcessPacket = 0;
	OnBuildPacket = 0;
	OnFlushSync = 0;
	OnCompareSync = 0;

	// Lock control
#ifdef NET_DEVICE_WIN

	InitializeCriticalSection( &NetLock );

#else	// NET_DEVICE_IOS | NET_DEVICE_ANDROID

	pthread_mutex_init( &NetLock, 0 );
	pthread_cond_init( &NetCond, 0 );

#endif // NET_DEVICE_WIN

}

NNetworkManager::~NNetworkManager()
{
	Stop();

	// Lock control
#ifdef NET_DEVICE_WIN

	DeleteCriticalSection( &NetLock );

#else
	pthread_cond_destroy( &NetCond );
	pthread_mutex_destroy( &NetLock );
#endif // NET_DEVICE_WIN
	
}

bool NNetworkManager::Start( int InMaxConnections, int InFixedFrameTime, int InLatencyFrame,
							 NetCallbackUpdated InUpdated,
							 NetCallbackProcessPacket InProcessPacket, 
							 NetCallbackBuildPacket InBuildPacket,
							 NetCallbackFlushSync InFlushSync,
							 NetCallbackCompareSync InCompareSync )
{
	if( bRunning ) return false;

	MaxConnections	= InMaxConnections;

	// Create connections
	Connections	= new NNetworkConnection[MaxConnections];
	PendingConnection = 0;

	// Create Event
	EventConnected = new NNetEvent( NNetEvent::Connected );
	EventDisconnected = new NNetEvent( NNetEvent::Disconnected );

	FixedFrameTime = InFixedFrameTime;
	LatencyFrame = InLatencyFrame;

	// Set handler
	OnUpdated = InUpdated;
	OnProcessPacket = InProcessPacket;
	OnBuildPacket = InBuildPacket;
	OnFlushSync = InFlushSync;
	OnCompareSync = InCompareSync;
	
	// Start thread
	bRunning = true;

#ifdef NET_DEVICE_WIN
	// Init WinSock 
	WSADATA wsaData;
	ErrorCode = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	if( ErrorCode != 0 ) return false;

	// Start Thread
	unsigned int ThreadID;
	NetThread = HANDLE(_beginthreadex( NULL, 0, NetworkThreadProc, this, 0, &ThreadID ) );
	if( NetThread == INVALID_HANDLE_VALUE || NetThread == 0 )
	{
		ErrorCode = GetLastError();
		bRunning = false;

		return false;
	}

#else // NET_DEVICE_IOS | NET_DEVICE_ANDROID
	int Result = pthread_create( &NetThread, 0, NetworkThreadProc, this );
	if( Result != 0 )
	{
		ErrorCode = Result;
		bRunning = false;

		return false;
	}

#endif // NET_DEVICE

	//ThreadFlag = 0;

	return true;
}

void NNetworkManager::Stop()
{
	if( !bRunning ) return;

	bRunning = false;

#ifdef NET_DEVICE_WIN
	// Stop thread
	if( NetThread != INVALID_HANDLE_VALUE && NetThread != 0 )
	{
		WaitForSingleObject( NetThread, INFINITE );
		CloseHandle( NetThread );
	}

	// Clear WinSock
	WSACleanup();

#else	// NET_DEVICE_IOS | NET_DEVICE_ANDROID

	LogPrintf( "NNetworkManager::Stop()");

	// Stop thread
	//void* ReturnCode;
	//pthread_join( NetThread, &ReturnCode );

	LogPrintf( "pthread_join( NetThread, &ReturnCode );");
#endif

	// Delete connections
	if( Connections )
	{
		delete[] Connections;
		Connections = 0;
	}
	
	PendingConnection = 0;
	MaxConnections = 0;

	if( EventConnected )
	{
		delete EventConnected;
		EventConnected = 0;
	}

	if( EventDisconnected )
	{
		delete EventDisconnected;
		EventDisconnected = 0;
	}

	ResetRecvSyncPackets();
	ResetOwnsSyncPackets();
	ResetMainSyncPackets();
	ResetSendSyncPackets();

	//ThreadFlag = 0;
}

void NNetworkManager::SuspendThread()
{
	Lock();

	ThreadFlag--;

	Unlock();
}

void NNetworkManager::ResumeThread()
{
	Lock();

	ThreadFlag++;
#ifdef NET_DEVICE_WIN
#else	// NET_DEVICE_IOS | NET_DEVICE_AOS
	pthread_cond_signal( &NetCond );
#endif	// NET_DEVICE_WIN

	Unlock();
}

bool NNetworkManager::IsRunningThread()
{
	return (ThreadFlag > 0);
}

void NNetworkManager::Run()
{
	// This function must be called by network thread

	while( bRunning )
	{
#ifdef NET_DEVICE_WIN
#else	// NET_DEVICE_IOS | NET_DEVICE_AOS
		Lock();
		while( ThreadFlag <= 0 )
			pthread_cond_wait( &NetCond, &NetLock );
		Unlock();
#endif	// NET_DEVICE_WIN

		// Process pending connect
		NNetworkConnection* NewConnection = PendingConnection;
		if( NewConnection )
		{
			NewConnection->Connect();
			PendingConnection = 0;
		}

		// Process connection event( I/O event )  
		for( int i = 0; i < MaxConnections; i++ )
		{
			Connections[i].Tick();
		}

#ifdef NET_DEVICE_WIN
		Sleep(5);
#else	// NET_DEVICE_IOS | NET_DEVICE_ANDROID
		usleep(5000);
#endif
	}
	LogPrintf("NetworkManager out of Run()");
}

void NNetworkManager::Update( int InElapsedTime )
{
	// This function must be called by main loop

	if( !bRunning ) return;

	NetworkTime += InElapsedTime;

	if( EventDisconnected && EventDisconnected->EventConnection )
	{
		EventDisconnected->ExecEvent();
	}

	if( EventConnected && EventConnected->EventConnection )
	{
		EventConnected->ExecEvent();
	}

	// Dispatch normal data packets
	DispatchRecvDataPackets();
	
	if( bSyncing )
	{
		if( InElapsedTime > 0 )
		{
			// Statistic sync elapsed time
			SyncElapsedTime += InElapsedTime;

			// Check sync frame
			if( SyncCheckFrame + LatencyFrame == SyncFrame )
			{
				// Compare remote frame
				if( SyncCheckFrame > SyncRemoteFrame )
				{
					// Paused, Wait for remote client

					if( !bSyncPause )
					{
						// Statistic delay count
						SyncDelayCount++;
					}

					// Statistic sum delay time
					SyncDelayTime += InElapsedTime;
					SyncDelaySumTime += InElapsedTime;

					// Statistic max delay time
					if( SyncDelayMaxTime <= SyncDelayTime ) 
					{
						SyncDelayMaxTime = SyncDelayTime;
					}

					// Set pause
					bSyncPause = true;
				}
				else
				{
					// Continue, Remote client state is good 

					// Statistic reset delay time
					SyncDelayTime = 0;

					bSyncPause = false;

					DispatchRecvSyncPackets();
					DispatchOwnsSyncPackets();
					DispatchMainSyncPackets();

					DispatchSendSyncPackets();

					// Update check frame
					SyncCheckFrame += LatencyFrame;
				}
			}
		}
	}
	else
	{
		// Garbage collection

		ResetRecvSyncPackets();
		ResetOwnsSyncPackets();
		ResetMainSyncPackets();
		ResetSendSyncPackets();
	}

	// Post update
	OnUpdated( InElapsedTime );
}

bool NNetworkManager::Connect( int InConnectionIndex, const char* InAddr, unsigned short InPort, 
							   NetCallbackConnected InConnected,
							   NetCallbackDisconnected InDisconnected, 
							   NetCallbackSendComplete InSendComplete, 
							   NetCallbackRecvComplete InRecvComplete, 
							   NetCallbackCheckPacket InCheckPacket )
{
	if( InConnectionIndex >= MaxConnections || InConnectionIndex < 0 )
	{
		ErrorCode = ENEC_InvalidIndex;
		return false;
	}

	if( Connections[InConnectionIndex].GetNetworkState() != ENCS_Offline )
	{
		ErrorCode = ENEC_InvalidState;
		return false;
	}

	if( PendingConnection )
	{
		ErrorCode = ENEC_PendingConnect;
		return false;
	}

	char TempIP[128];
	struct hostent* host = 0;
	host = gethostbyname( InAddr );

	if( host )	strcpy( TempIP, inet_ntoa( *(struct in_addr*)*host->h_addr_list ) ); 
	else		strcpy( TempIP, InAddr ); 

	Connections[InConnectionIndex].Init( InConnectionIndex, TempIP, InPort, 
										   InConnected, 
										   InDisconnected, 
										   InSendComplete, 
										   InRecvComplete, 
										   InCheckPacket );

	CurrentConnection = &Connections[InConnectionIndex];
	PendingConnection = &Connections[InConnectionIndex];

	return true;
}

bool NNetworkManager::Disconnect( int InConnectionIndex/*=-1*/ )
{
	if( InConnectionIndex < 0 )
	{
		if( !CurrentConnection )
		{
			ErrorCode = ENEC_InvalidState;
			return false;
		}

		if( !CurrentConnection->IsDisconnected() )
		{
			CurrentConnection->Disconnect();
		}

		CurrentConnection = 0;

		return true;	
	}
	else
	{
		if( InConnectionIndex >= MaxConnections )
		{
			ErrorCode = ENEC_InvalidIndex;
			return false;
		}

		if( !Connections[InConnectionIndex].IsDisconnected() )
		{
			Connections[InConnectionIndex].Disconnect();
		}

		if( CurrentConnection == &Connections[InConnectionIndex] )
		{
			CurrentConnection = 0;
		}

		return true;
	}
}

bool NNetworkManager::HandleEvent( int EventID, NNetworkConnection* InConnection, int InEventCode, int InErrorCode, NetCallbackEvent InEvent )
{
	if( EventID == NNetEvent::Connected && EventConnected )
	{
		if( EventConnected->IsPending() ) return false;

		EventConnected->EventConnection = InConnection;
		EventConnected->EventCode = InEventCode;
		EventConnected->ErrorCode = InErrorCode;
		EventConnected->OnEvent = InEvent;

		return true;
	}

	if( EventID == NNetEvent::Disconnected && EventDisconnected )
	{
		if( EventDisconnected->IsPending() ) return false;

		EventDisconnected->EventConnection = InConnection;
		EventDisconnected->EventCode = InEventCode;
		EventDisconnected->ErrorCode = InErrorCode;
		EventDisconnected->OnEvent = InEvent;

		return true;
	}

	 return false;
}

void NNetworkManager::Lock()
{
#ifdef NET_DEVICE_WIN

	EnterCriticalSection( &NetLock );

#else	// NET_DEVICE_IOS | NET_DEVICE_ANDROID

	pthread_mutex_lock( &NetLock );

#endif // NET_DEVICE_WIN
}

void NNetworkManager::Unlock()
{
#ifdef NET_DEVICE_WIN

	LeaveCriticalSection( &NetLock );

#else	// NET_DEVICE_IOS | NET_DEVICE_ANDROID

	pthread_mutex_unlock( &NetLock );

#endif // NET_DEVICE_WIN
}

int NNetworkManager::SendPacket( NNetworkConnection* InConnection, void* InData, int InSize, bool InSync )
{
	if( InConnection && InConnection->IsConnected() )
	{
		OnBuildPacket( InConnection, InData, InSize );

		int Result = InConnection->SendPacket( InData, InSize );

		if( Result > 0 )
		{
			// Statistic 
			LastSendTime	= NetworkTime;
			SendBytes    += Result;

			if( InSync )
			{
				// Statistic
				SyncLastSendTime = NetworkTime;
				SyncSendBytes += Result;
				SyncSendPackets++;
			}
		}
		else
		{
			ErrorCode = ENEC_FailedSend;
		}

		return Result;
	}
	
	ErrorCode = ENEC_InvalidState;

	return 0;
}

int NNetworkManager::SendPacket( int InConnectionIndex, void* InData, int InSize, bool InSync )
{
	if( InConnectionIndex >= MaxConnections || InConnectionIndex < 0 )
	{
		ErrorCode = ENEC_InvalidIndex;
		
		return 0;
	}

	return SendPacket( &Connections[InConnectionIndex], InData, InSize, InSync );
}

void NNetworkManager::PushRecvDataPacket( NNetworkConnection* InConnection, unsigned short InProtocol, void* InData, unsigned short InSize )
{
	// Statistic
	LastRecvTime   = NetworkTime;
	RecvBytes		+= InSize;

	NNetPacket* InternalPacket	= new(InSize)NNetPacket( InConnection, 
												  InProtocol, 
												  InData, 
												  InSize, 
												  NNetPacket::Internal );

	QueueRecvDataPackets.AddItem( InternalPacket );
}

void NNetworkManager::PushRecvSyncPacket( NNetworkConnection* InConnection, unsigned short InProtocol, void* InData, unsigned short InSize, int InFrame )
{
	// Statistic
	LastRecvTime = NetworkTime;
	SyncLastRecvTime = NetworkTime;
	RecvBytes += InSize;
	SyncRecvBytes += InSize;
	SyncRecvPackets++;

	NNetPacket* InternalPacket	= new(InSize)NNetPacket( InConnection, 
												  InProtocol, 
												  InData, 
												  InSize, 
												  NNetPacket::Internal,
												  InFrame );
	QueueRecvSyncPackets.AddItem( InternalPacket );
}

void NNetworkManager::PushSendSyncPacket( unsigned short InProtocol, void* InData, unsigned short InSize, int InFrame, int InDirection )
{
	NNetPacket* InternalPacket	= new(InSize)NNetPacket( CurrentConnection, 
												  InProtocol, 
												  InData, 
												  InSize, 
												  InDirection,
												  InFrame );

	QueueSendSyncPackets.AddItem( InternalPacket );
}

int NNetworkManager::CompareSyncPacket( NNetPacket* InA, NNetPacket* InB )
{
	if( OnCompareSync )
	{
		if( OnCompareSync( InA, InB ) ) 
		{
			return 1; // Change
		}
		else
		{
			return 0; // Not Change
		}
	}

	return 0; // Not Change
}

void NNetworkManager::DriveSync()
{
	bSyncDrive = true;

	if( OnFlushSync ) OnFlushSync( -1 );
}

void NNetworkManager::BeganSync()
{
	SyncRate = 1.0f;

	bSyncPause = false;
	bSyncing = true;
	bSyncDrive = false;

	SyncTime = 0;
	SyncFrame = 0;
	SyncCheckFrame = 0;//-LatencyFrame;
	SyncRemoteFrame = -LatencyFrame;

	// Statistic
	SyncElapsedTime	= 0;
	SyncSendBytes = 0;
	SyncRecvBytes = 0;
	SyncSendPackets = 0;
	SyncRecvPackets = 0;

	// Statistic
	SyncDelayCount = 0;
	SyncDelayTime = 0;
	SyncDelaySumTime = 0;
	SyncDelayMaxTime = 0;
}

void NNetworkManager::EndedSync()
{
	SyncCheckFrame += LatencyFrame;

	SyncRate = 1.0f;

	bSyncing = false;
	bSyncPause = false;
}

void NNetworkManager::RenewSync()
{
	SyncTime += FixedFrameTime;

	SyncFrame++;
}

void NNetworkManager::CheckSync( int InRemoteFrame, int InSize )
{
	SyncRemoteFrame = InRemoteFrame;

	// Statistic
	LastRecvTime = NetworkTime;
	SyncLastRecvTime = NetworkTime;
	RecvBytes += InSize;
	SyncRecvBytes += InSize;
	SyncRecvPackets++;

	NNetPacket* InternalPacket	= new(InSize)NNetPacket( 0,
												  0xFFFF,
												  &InRemoteFrame,
												  sizeof(int), 
												  NNetPacket::Internal );

	QueueRecvSyncPackets.AddItem( InternalPacket );
}

void NNetworkManager::SetSyncID( int InID )
{
	SyncID = InID;
}

int NNetworkManager::GetSyncID()
{
	return SyncID;
}

void NNetworkManager::SetSyncRate( float InRate/*=1.0f*/ )
{
	if( InRate <= 0.0f ) InRate = 1.0f;
	if( InRate > 2.0f ) InRate = 2.0f;

	SyncRate = InRate;
}

float NNetworkManager::GetSyncRate()
{
	if( SyncRate > 2.0f ) SyncRate = 2.0f;

	return SyncRate;
}

bool NNetworkManager::IsSyncing() 
{
	return bSyncing;
}

bool NNetworkManager::IsSyncDrive()
{
	return bSyncDrive;
}

bool NNetworkManager::IsSyncPause()
{
	return bSyncPause;
}

int NNetworkManager::GetFixedFrameTime()
{
	return FixedFrameTime;
}

int NNetworkManager::GetLatencyFrame()
{
	return LatencyFrame;
}

int NNetworkManager::GetErrorCode()
{
	return ErrorCode;
}

int NNetworkManager::GetNetworkTime()
{
	return NetworkTime;
}

int NNetworkManager::GetLastSendTime()
{
	return LastSendTime;
}

int NNetworkManager::GetLastRecvTime()
{
	return LastRecvTime;
}

int NNetworkManager::GetSendBytes()
{
	return SendBytes;
}

int NNetworkManager::GetRecvBytes()
{
	return RecvBytes;
}

int NNetworkManager::GetSyncFrame()
{
	return SyncFrame;
}

int NNetworkManager::GetSyncTime()
{
	return SyncTime;
}

int NNetworkManager::GetSyncCheckFrame()
{
	return SyncCheckFrame;
}

int NNetworkManager::GetSyncRemoteFrame()
{
	return SyncRemoteFrame;
}

int NNetworkManager::GetSyncNextFrame()
{
	return (SyncCheckFrame + LatencyFrame);
}

int NNetworkManager::GetSyncExecTime()
{
	if( bSyncing )
	{
		return ( ( (LatencyFrame * 2) - (SyncFrame % LatencyFrame) + 1 ) * FixedFrameTime);
	}
	
	return 0; //(LatencyFrame * FixedFrameTime);
}

int NNetworkManager::GetSyncElapsedTime()
{
	return SyncElapsedTime;
}

int NNetworkManager::GetSyncLastSendTime()
{
	return SyncLastSendTime;
}

int NNetworkManager::GetSyncLastRecvTime()
{
	return SyncLastRecvTime;
}

int NNetworkManager::GetSyncSendBytes()
{
	return SyncSendBytes;
}

int NNetworkManager::GetSyncRecvBytes()
{
	return SyncRecvBytes;
}

int NNetworkManager::GetSyncSendPackets()
{
	return SyncSendPackets;
}

int NNetworkManager::GetSyncRecvPackets()
{
	return SyncRecvPackets;
}

int NNetworkManager::GetSyncDelayCount()
{
	return SyncDelayCount;
}

int NNetworkManager::GetSyncDelayTime()
{
	return SyncDelayTime;
}

int NNetworkManager::GetSyncDelaySumTime()
{
	return SyncDelaySumTime;
}

int NNetworkManager::GetSyncDelayMaxTime()
{
	return SyncDelayMaxTime;
}

NNetworkConnection* NNetworkManager::GetPendingConnection()
{
	return PendingConnection;
}

NNetworkConnection* NNetworkManager::GetCurrentConnection()
{
	return CurrentConnection;
}

NNetworkConnection* NNetworkManager::GetConnection( int InIndex )
{
	if( InIndex >= 0 && InIndex < MaxConnections )
	{
		return &Connections[InIndex];
	}

	return 0;
}

int NNetworkManager::GetConnectionIndex( NNetworkConnection* InConnection )
{
	for( int i = 0; i < MaxConnections; i ++ )
	{
		if( &Connections[i] == InConnection )
		{
			return i;
		}
	}
	
	return -1;
}

void NNetworkManager::DispatchRecvDataPackets()
{
	Lock();

	if( QueueRecvDataPackets.Num() > 0 )
	{
		for( int i = 0; i < QueueRecvDataPackets.Num(); i++ )
		{
			NNetPacket* NewPacket = QueueRecvDataPackets(i);
			if( NewPacket )
			{
				if( OnProcessPacket ) OnProcessPacket( NewPacket );

				delete[] NewPacket;
			}
		}

		QueueRecvDataPackets.Empty();
	}

	Unlock();
}

void NNetworkManager::DispatchRecvSyncPackets()
{
	Lock();

	while( QueueRecvSyncPackets.Num() > 0 )
	{
		NNetPacket* NewPacket = QueueRecvSyncPackets( 0 );
		if( NewPacket )
		{
			QueueMainSyncPackets.AddItem( NewPacket );
			QueueRecvSyncPackets.RemoveItem( NewPacket );

			if ( NewPacket->Code == 0xFFFF )
			{
				break;
			}
		}
	}
	
	Unlock();
}

void NNetworkManager::DispatchOwnsSyncPackets()
{
	// This function is called at check time

	// Process packet data(received prev frame)

	if( QueueOwnsSyncPackets.Num() > 0 )
	{
		for( int i = 0; i < QueueOwnsSyncPackets.Num(); i++ )
		{
			QueueMainSyncPackets.AddItem( QueueOwnsSyncPackets(i) );
		}

		QueueOwnsSyncPackets.Empty();
	}
}

IMPLEMENT_COMPARE_POINTER( NNetPacket, NetworkManager,
{ 
	return NNetworkManager::Instance()->CompareSyncPacket( A, B );
} 
);

void NNetworkManager::DispatchMainSyncPackets()
{
	// This function is called at check time

	Sort<USE_COMPARE_POINTER( NNetPacket, NetworkManager )>( &QueueMainSyncPackets(0), QueueMainSyncPackets.Num() );
	
	for( int i = 0; i < QueueMainSyncPackets.Num(); i++ )
	{
		NNetPacket* NewPacket = QueueMainSyncPackets(i);
		if( NewPacket )
		{
			if( OnProcessPacket ) OnProcessPacket( NewPacket );

			delete[] NewPacket;
		}
	}

	QueueMainSyncPackets.Empty();
}

void NNetworkManager::DispatchSendSyncPackets()
{
	// This function is called at check time

	int ResultBytes = 0;
    
	for( int i = 0; i < QueueSendSyncPackets.Num(); i++ )
	{
		NNetPacket* NewPacket = QueueSendSyncPackets( i );
		if( NewPacket )
		{
			if( NewPacket->Flag & NNetPacket::External )
			{
				int Result = SendPacket( CurrentConnection, NewPacket->Data, NewPacket->Size, true );
				if( Result > 0 )
				{
					ResultBytes += Result;
				}
			}

			if( NewPacket->Flag & NNetPacket::Internal )
			{
				// Insert packets to next process 
				QueueOwnsSyncPackets.AddItem( NewPacket );
			}
			else
			{
				delete[] NewPacket;
			}
		}
	}

	QueueSendSyncPackets.Empty();

	OnFlushSync( ResultBytes );
}


void NNetworkManager::ResetRecvSyncPackets()
{
	// Garbage collection

	Lock();

	if( QueueRecvSyncPackets.Num() > 0 )
	{
		

		for( int i = 0; i < QueueRecvSyncPackets.Num(); i ++ )
		{
			NNetPacket* NewPacket = QueueRecvSyncPackets( i );
			if( NewPacket ) delete[] NewPacket;
		}
		
		QueueRecvSyncPackets.Empty();

		
	}

	Unlock();
}

void NNetworkManager::ResetOwnsSyncPackets()
{
	// Garbage collection

	if( QueueOwnsSyncPackets.Num() > 0 )
	{
		for( int i = 0; i < QueueOwnsSyncPackets.Num(); i++ )
		{
			NNetPacket* NewPacket = QueueOwnsSyncPackets( i );
			if( NewPacket ) delete[] NewPacket;
		}

		QueueOwnsSyncPackets.Empty();
	}
}

void NNetworkManager::ResetMainSyncPackets()
{
	// Garbage collection

	if( QueueMainSyncPackets.Num() > 0 )
	{
		for ( INT i = 0; i < QueueMainSyncPackets.Num(); i++ )
		{
			NNetPacket* NewPacket = QueueMainSyncPackets( i );
			if( NewPacket ) delete[] NewPacket;
		}

		QueueMainSyncPackets.Empty();
	}
}

void NNetworkManager::ResetSendSyncPackets()
{
	// Garbage collection

	if( QueueSendSyncPackets.Num() > 0 )
	{
		for ( INT i = 0; i < QueueSendSyncPackets.Num(); i++ )
		{
			NNetPacket* NewPacket = QueueSendSyncPackets( i );
			if( NewPacket ) delete[] NewPacket;
		}

		QueueSendSyncPackets.Empty();
	}
}

#pragma warning( pop ) // 4127

void LogPrintf( const char *pszFmt, ... )
{
#if !FINAL_RELEASE
    
#if ANDROID
	char szBuf[1024];
	va_list argList;
	va_start( argList, pszFmt);
	vsnprintf( szBuf, 1024, pszFmt, argList );
	va_end( argList );
    
	JniUtil::LOGPrintf( szBuf );
    
#elif IOS
	char szBuf[1024];
	va_list argList;
	va_start( argList, pszFmt);
	vsnprintf( szBuf, 1024, pszFmt, argList );
	va_end( argList );
    
    printf( "%s", szBuf );
    

    
#else // WIN32
	char szBuf[1024];
	va_list argList;
	va_start( argList, pszFmt);
	vsnprintf( szBuf, 1024, pszFmt, argList );
	va_end( argList );
    
	int pos = strlen(szBuf);
	if( szBuf[pos -1] != '\n' )
	{
		szBuf[pos] = '\n';
		szBuf[pos+1] = 0;
        
	}
	OutputDebugString( szBuf );
#endif	// ANDROID
    
#endif // !FINAL_RELEASE
}
