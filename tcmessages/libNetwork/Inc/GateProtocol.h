#pragma once

#define CHECK_GATE_PROCESS(ptcl)( (ptcl >= DEFINE_GATE_PROTOCOL) && (ptcl < END_GATE_DEFINITION ) )

enum eLogin_Protocol
{
	GATE_PROTOCOL_VERSION = 20,

	DEFINE_GATE_PROTOCOL = 10000,

	// Gate Server Authentication
	PTCL_REQ_GATE_AUTHENTICATION,
	PTCL_ANS_GATE_AUTHENTICATION,

	// Get Game Information( Enterence Channel & Device )
	PTCL_REQ_GAME_INFO,
	PTCL_ANS_GAME_INFO,

	END_GATE_DEFINITION
};
