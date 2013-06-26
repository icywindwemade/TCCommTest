//##############################################################################################
// NetworkConnection.h
//##############################################################################################
#ifndef _NETWORK_CONNECTION_H_
#define _NETWORK_CONNECTION_H_

#if ANDROID
	#define NET_DEVICE_ANDROID	1

#elif IOS
	#define NET_DEVICE_IOS		1

#else // WIN32

	#define NET_DEVICE_WIN		1

#endif	// ANDROID

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#ifdef NET_DEVICE_WIN
	#include <windows.h>
	#include <process.h>
	#include <wincon.h>
	#include <time.h>
	#pragma comment(lib, "ws2_32.lib") // include winsock2 lib
	#pragma comment(lib, "Netapi32.lib") // include winsock2 lib

#define NET_SEND_BUFFER_SIZE	16384
#define NET_RECV_BUFFER_SIZE	65536

#elif defined(NET_DEVICE_IOS)
	#include <ifaddrs.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <net/if_dl.h>
	#include <arpa/inet.h>
	#include <pthread.h>
	#include <errno.h>
	#include <unistd.h>

#define NET_SEND_BUFFER_SIZE	16384
#define NET_RECV_BUFFER_SIZE	65536

#else // defined(NET_DEVICE_ANDROID)
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <sys/endian.h> 
	#include <sys/time.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <net/if.h>
	#include <arpa/inet.h>
	#include <pthread.h>
	#include <errno.h>
	#include <unistd.h>
	#include <linux/in.h>

#define NET_SEND_BUFFER_SIZE	65536
#define NET_RECV_BUFFER_SIZE	65536

#endif // NET_DEVICE

typedef struct sockaddr_in		NetSockAddr;

//##############################################################################################
// NetCallback
//##############################################################################################
class NNetworkConnection;
typedef	void (* NetCallbackConnected) ( NNetworkConnection* InConnection, int InErrorCode );
typedef	void (* NetCallbackDisconnected) ( NNetworkConnection* InConnection, int InErrorCode );
typedef	void (* NetCallbackSendComplete) ( NNetworkConnection* InConnection, int InSize );
typedef	void (* NetCallbackRecvComplete) ( NNetworkConnection* InConnection, char* InData, int InSize );
typedef	int (* NetCallbackCheckPacket) ( NNetworkConnection* InConnection, char* InData, int InSize );

//##############################################################################################
// ENetErrorCode
//##############################################################################################
enum ENetErrorCode
{
	ENEC_Success			= 0	,
	ENEC_StateOnline			,
	ENEC_StateOffline			,
	ENEC_FailedSocket			,
	ENEC_FailedConnect			,
	ENEC_FailedSend				,
	ENEC_Overflow				,
	ENEC_InvalidIndex			,
	ENEC_InvalidState			,
	ENEC_InvalidReference		,
	ENEC_PendingConnect			,
	ENEC_InvalidPacket			,
    ENEC_ClosedSocket           ,
	ENEC_Max
};

//##############################################################################################
// ENetConnectionMode
//##############################################################################################
enum ENetConnectionMode
{
	ENCM_None	= 0x00	,
	ENCM_Client	= 0x01	, // Client ( Connect Server )
	ENCM_Guest	= 0x02	, // Guest  ( Connect Host   )
	ENCM_Host	= 0x04	, // Host	( Accept  Guest  )
	ENCM_Max	= 0x08	,
};

//##############################################################################################
// ENetConnectionState
//##############################################################################################
enum ENetConnectionState
{
	ENCS_Offline	= 0	, // Disabled Connection
	ENCS_Connecting		, // Connecting Remote Peer ( Non-Blocking Mode )
	ENCS_Acceping		, // Accepting Remote Peer  ( Non-Blocking Mode )
	ENCS_Online			, // Enabled Connection
	ENCS_Max			,
};

//##############################################################################################
// NNetworkConnection
//##############################################################################################
class NNetworkConnection
{
public:
	NNetworkConnection();
	~NNetworkConnection();

	void Init( int InIndex, const char* InIP, unsigned short InPort,
			   NetCallbackConnected InConnected, 
			   NetCallbackDisconnected InDisconnected, 
			   NetCallbackSendComplete InSendComplete, 
			   NetCallbackRecvComplete InRecvComplete, 
			   NetCallbackCheckPacket InCheckPacket );

	void Connect();
	void Disconnect();

	void Tick();

	int SendPacket( void* InData, int InSize );
	int PushSendPacket( void* InData, int InSize );
	int FlushSendPackets();

	bool IsDisconnected();
	bool IsConnecting();
	bool IsConnected();

	int GetIndex();
	int GetSocket();
	ENetConnectionMode GetNetworkMode() const;
	ENetConnectionState GetNetworkState() const;
	unsigned int GetErrorCode();
	int GetPendingSendBytes();

private:
	void Connected( int InErrorCode );
	void Disconnected( int InErrorCode );
	void SetupSocketOption( int InSocket, bool InAsync=false );
	void CloseSocket();
	void SaveErrorCode( bool InReset=false );
	bool IsPendingError();

private:
	int						IndexID;
	int						SocketID;
	NetSockAddr				RemoteAddr;
	char					RemoteIP[32];
	unsigned short			RemotePort;

	ENetConnectionMode		NetworkMode;
	ENetConnectionState		NetworkState;

	unsigned int			ErrorCode;
	unsigned int			SendBytes;
	unsigned int			RecvBytes;

	char					SendBuffer[NET_SEND_BUFFER_SIZE];
	char					RecvBuffer[NET_RECV_BUFFER_SIZE];

	NetCallbackConnected	OnConnected;
	NetCallbackDisconnected	OnDisconnected;
	NetCallbackSendComplete OnSendComplete;
	NetCallbackRecvComplete OnRecvComplete;
	NetCallbackCheckPacket	OnCheckPacket;
};



#endif // _NETWORK_CONNECTION_H_