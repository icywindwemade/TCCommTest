#pragma once 

#define	CLIENT_KEEPALIVE				2				// Client Heart Beat Protocol
#define CLIENT_GRACEFUL_DISCONNECT		3				// Client Graceful Disconnection
#define CLIENT_PING						12				// Client Ping Packet

#define MASKKEY							0x1f
#define STARTMARK						0x0a000		// 0x00 ~ 0x0f

struct PACKET  
{
	unsigned short Mark;	
	unsigned short CheckSum;
	unsigned short ptcl;
	unsigned short size;

	unsigned short GetMark()			{ return (Mark & STARTMARK);		}
	unsigned short GetSequenceNum()		{ return (Mark ^ STARTMARK) >> 5;	}
	unsigned short GetCheckSum()		{ return CheckSum;					}
	unsigned short GetPtcl()			{ return ptcl;						}
	unsigned short GetSize()			{ return size;						}	
};

struct PING_PACKET : PACKET
{
	int Temp;

	PING_PACKET()
	{
		ptcl = CLIENT_PING;
		size = sizeof(*this);
	};
};
