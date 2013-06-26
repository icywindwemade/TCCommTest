//##############################################################################################
// NNetworkConnection.cpp
//##############################################################################################

#include "NetworkConnection.h"

#pragma warning( push )
#pragma warning( disable: 4100 ) // argument reference
#pragma warning( disable: 4127 ) // while( TRUE ) : TRUE is const type warning

//##############################################################################################
// NNetworkConnection
//##############################################################################################
NNetworkConnection::NNetworkConnection()
{
	IndexID	= -1;
	SocketID = -1;

	memset( RemoteIP, 0, 32 );
	RemotePort = 0;

	NetworkMode = ENCM_None;
	NetworkState = ENCS_Offline;

	ErrorCode = 0;
	SendBytes = 0;
	RecvBytes = 0;

	OnConnected = 0;
	OnDisconnected = 0;
	OnSendComplete = 0;
	OnRecvComplete = 0;
	OnCheckPacket = 0;

}

NNetworkConnection::~NNetworkConnection()
{
	Disconnect();
}

void NNetworkConnection::Init( int InIndex, const char* InIP, unsigned short InPort, 
							  NetCallbackConnected InConnected,
							  NetCallbackDisconnected InDisconnected,
							  NetCallbackSendComplete InSendComplete,
							  NetCallbackRecvComplete InRecvComplete,
							  NetCallbackCheckPacket InCheckPacket )
{
	// Force disconnect..
	Disconnect();

	IndexID = InIndex;

	strcpy( RemoteIP, InIP );
	RemotePort = InPort;

	NetworkMode = ENCM_None;
	NetworkState = ENCS_Offline;

	ErrorCode = 0;
	SendBytes = 0;
	RecvBytes = 0;

	OnConnected = InConnected;
	OnDisconnected = InDisconnected;
	OnSendComplete = InSendComplete;
	OnRecvComplete = InRecvComplete;
	OnCheckPacket = InCheckPacket;
}

void NNetworkConnection::Connect()
{
	if( NetworkState == ENCS_Online )
	{
		SaveErrorCode( true );
		Connected( ENEC_StateOnline );

		return;
	}

	if( ( SocketID = socket( PF_INET,SOCK_STREAM,0 ) ) < 0 ) 
	{
		SaveErrorCode();
		Connected( ENEC_FailedSocket );

		return;
	}

	memset( &RemoteAddr, 0, sizeof(RemoteAddr) );
	RemoteAddr.sin_family = AF_INET;
	RemoteAddr.sin_addr.s_addr = inet_addr( RemoteIP );
	RemoteAddr.sin_port = htons( RemotePort );

	NetworkMode = ENCM_Client;
	NetworkState = ENCS_Connecting;

	int Result = connect( SocketID, (struct sockaddr*)&RemoteAddr, sizeof(RemoteAddr) );
	if( Result == 0 )
	{
		// Success
		
		SetupSocketOption( SocketID, false ); // Set non-blocking

		NetworkState = ENCS_Online;

		SaveErrorCode( true );
		Connected( ENEC_Success );
	}
	else
	{
		// Failed ( Blocking mode )

		NetworkState = ENCS_Offline;

		SaveErrorCode();

		CloseSocket();

		Connected( ENEC_FailedConnect );
	}

	SendBytes = 0;
	RecvBytes = 0;
}

void NNetworkConnection::Disconnect()
{
	int OldState = NetworkState;

	NetworkMode = ENCM_None;
	NetworkState = ENCS_Offline;

	CloseSocket();

	switch( OldState )
	{
	case ENCS_Offline:		Disconnected( ENEC_StateOffline );		break;
	case ENCS_Connecting:	Disconnected( ENEC_PendingConnect );	break;
	default:				Disconnected( ENEC_Success );			break;
	}

	SendBytes		= 0;
	RecvBytes		= 0;
}

void NNetworkConnection::Tick()
{
	if( NetworkState != ENCS_Online ) return;

	int Bytes;
	unsigned int PacketSize;

	// Non blocking
	Bytes = recv( SocketID, RecvBuffer + RecvBytes, NET_RECV_BUFFER_SIZE - RecvBytes , 0 );
	if( Bytes > 0 )
	{
		// Receive any bytes
		RecvBytes += Bytes;

		while( true )
		{
			// Parse and check packet size
			if( OnCheckPacket )
			{
				PacketSize = OnCheckPacket( this, RecvBuffer, RecvBytes );
			}
			else
			{
				PacketSize = RecvBytes;
			}
			
			// Check packet size
			if( PacketSize > RecvBytes ) return;

			// Need more bytes
			if( PacketSize == 0 ) 
			{
                ErrorCode = ENEC_InvalidPacket;
				Disconnect();
				return;
			}

			// Received a completion packet
			if( OnRecvComplete ) OnRecvComplete( this, RecvBuffer, PacketSize );
			
			// Decrease receive count for next parse
			RecvBytes -= PacketSize;

			// Check connection before packet memory control
			if( NetworkState != ENCS_Online )
			{
				Disconnect();
				return;
			}

			// Delete a completion packet memory
			memmove( RecvBuffer, RecvBuffer + PacketSize, RecvBytes );
		}

	}
	else if( Bytes == 0 )
	{
		// Received close socket
        ErrorCode = ENEC_ClosedSocket;        
		Disconnect();
	}
	else
	{
		if( !IsPendingError() )
		{
			Disconnect();
		}
	}
}

int NNetworkConnection::SendPacket( void* InData, int InSize )
{
	if( NetworkState != ENCS_Online ) return 0;

	// Non-blocking
	int Result = send( SocketID, (const char*)InData, InSize, 0);

	if( Result <= 0 )
	{
		SaveErrorCode();
	}

	if( OnSendComplete ) OnSendComplete( this, Result );

	return Result;
}

int NNetworkConnection::PushSendPacket( void* InData, int InSize )
{
	if( ( SendBytes + InSize ) >= NET_SEND_BUFFER_SIZE )
	{
		return 0;
	}

	memcpy( SendBuffer + SendBytes, InData, InSize );

	SendBytes += InSize;

	return InSize;
}

int NNetworkConnection::FlushSendPackets()
{
	int Result = SendPacket( SendBuffer, SendBytes );

	SendBytes = 0;

	return Result;
}

bool NNetworkConnection::IsDisconnected()
{
	return (NetworkState == ENCS_Offline); 
}

bool NNetworkConnection::IsConnecting() 
{ 
	return (NetworkState == ENCS_Connecting); 
}

bool NNetworkConnection::IsConnected() 
{
	return (NetworkState == ENCS_Online); 
}

int NNetworkConnection::GetIndex()
{
	return IndexID; 
}

ENetConnectionMode NNetworkConnection::GetNetworkMode() const 
{ 
	return NetworkMode; 
}

ENetConnectionState NNetworkConnection::GetNetworkState() const 
{ 
	return NetworkState; 
}

unsigned int NNetworkConnection::GetErrorCode() 
{ 
	return ErrorCode; 
}

int NNetworkConnection::GetPendingSendBytes() 
{ 
	return SendBytes; 
}

void NNetworkConnection::Connected( int InErrorCode )
{
	if( OnConnected ) OnConnected( this, InErrorCode );
}

void NNetworkConnection::Disconnected( int InErrorCode )
{
	if( OnDisconnected ) OnDisconnected( this, InErrorCode );
}

void NNetworkConnection::SetupSocketOption( int InSocket, bool InAsync )
{
	if( InSocket < 0 ) return;

#ifdef NET_DEVICE_WIN

	unsigned long Ret;
	if( InAsync )
	{
		// Set blocking
		ioctlsocket( InSocket, FIOASYNC, &Ret );
	}
	else
	{
		// Set non-blocking
		ioctlsocket( InSocket, FIONBIO, &Ret );
	}

	// Set no delay
	int Opt = 1;
	setsockopt( InSocket, IPPROTO_TCP, 1, (const char*)&Opt, sizeof(Opt) );

	//// Set Reuse Address
	//setsockopt( InSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&Opt, sizeof(Opt) );

#else // NET_DEVICE_IOS || NET_DEVICE_ANDROID

	int Ret = 1;
	if( InAsync )
	{
		// Set blocking
		Ret = ioctl( InSocket, FIOASYNC, &Ret );
	}
	else
	{
		// Set non-blocking
		Ret = ioctl( InSocket, FIONBIO, &Ret );
	}

	// Set no delay
	int Opt = 1;
	setsockopt( InSocket, IPPROTO_TCP, 1, &Opt, sizeof(Opt) );

	//// Set Reuse Address
	//setsockopt( InSocket, SOL_SOCKET, SO_REUSEADDR, &Opt, sizeof(Opt) );

#endif // NET_DEVICE

}

void NNetworkConnection::CloseSocket()
{
	if( SocketID == -1 ) return;

#ifdef NET_DEVICE_WIN

	closesocket( SocketID );

#else // NET_DEVICE_ANDROID || NET_DEVICE_IOS

	close( SocketID );

#endif // NET_DEVICE

	SocketID = -1;
}

void NNetworkConnection::SaveErrorCode( bool InReset/*=false*/ )
{
	if( InReset )
{
		ErrorCode = 0;
		return;
	}

#ifdef NET_DEVICE_WIN

	ErrorCode = GetLastError();

#else // NET_DEVICE_ANDROID || NET_DEVICE_IOS

	ErrorCode = errno;

#endif // NET_DEVICE

}

bool NNetworkConnection::IsPendingError()
{
#ifdef NET_DEVICE_WIN

	if( GetLastError() != WSAEWOULDBLOCK ) 
	{
		ErrorCode = GetLastError();
		return false;
	}

#else // NET_DEVICE_ANDROID || NET_DEVICE_IOS

	if( errno != EWOULDBLOCK ) 
	{
		ErrorCode = errno;
		return false;
	}

#endif // NET_DEVICE

	return true;
}

#pragma warning( pop ) // 4100, 4127



