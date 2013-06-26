#pragma once

#include "GateProtocol.h"

#pragma pack(push,1)

/* Gate Server Authentication */
struct REQ_GATE_AUTHENTICATION : PACKET
{
	unsigned long	ProtocolVersion;		// Protocol Version
	int				MajorVersion;			// Client Major Version
	int				MinorVersion;			// Client Minor Version
	
	int				StoreType;				// eUDClientStoreType

	REQ_GATE_AUTHENTICATION()
	{
		ptcl = PTCL_REQ_GATE_AUTHENTICATION;
		size = sizeof(*this);

		ProtocolVersion = GATE_PROTOCOL_VERSION;
		MajorVersion = 0;
		MinorVersion = 0;
		StoreType = 0;
	};
};

struct ANS_GATE_AUTHENTICATION : PACKET
{
	enum eAnsCode
	{
		eSuccess = 0,
		eFail,
		eFail_Protocol,					// Protocol Different
		eFail_Major,					// Major Version Different
		eFail_Minor,					// Minor Version Different
		eFail_Maintenance				// The server will be down for maintenance.
	};
	unsigned char	AnsCode;

	int				MajorVersion;		// Client Major Version
	int				MinorVersion;		// Client Minor Version

	char			PatchDomain[UD_DOMAIN_LEN];
	char			WemadeSocialDomain[UD_DOMAIN_LEN];
	char			EventDomain[UD_DOMAIN_LEN];

	int				StoreType;			// eUDClientStoreType

	enum eAllowGuestType
	{
		eDo_Not_Allow,
		eDo_limit_Allow,
		eDo_Full_Allow
	};
	unsigned char	AllowGuestType;		// eAllowGuestType

	ANS_GATE_AUTHENTICATION()
	{
		ptcl = PTCL_ANS_GATE_AUTHENTICATION;
		size = sizeof(*this);
	};
};

/* Get Game Information( Enterence Channel & Device ) */
struct REQ_GAME_INFO : PACKET
{
	REQ_GAME_INFO()
	{
		ptcl = PTCL_REQ_GAME_INFO;
		size = sizeof(*this);
	};
};

struct ANS_GAME_INFO : PACKET
{
	struct CHANNEL
	{
		unsigned short	ID;						// Channel ID
		unsigned short	OnlineClientCount;		// Channel Online Client Count
		unsigned short	FullClientCount;		// Channel Full Client Count

		char			IP[UD_IP_LEN];			// IP
		unsigned short	Port;					// Port
	};

	CHANNEL			Channel[UD_MAX_CHANNEL_COUNT];
	int				ChannelCount;

	ANS_GAME_INFO()
	{
		ptcl = PTCL_ANS_GAME_INFO;
		size = sizeof(*this);
	};
};

#pragma pack(pop)