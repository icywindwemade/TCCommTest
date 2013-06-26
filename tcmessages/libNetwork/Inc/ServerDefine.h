#ifndef __SERVER_DEFINE_H__
#define __SERVER_DEFINE_H__

#include "BuildDefine.h"

//##############################################################################################
//	Server
//##############################################################################################
#define SERVER_PORT_KOR                 20001							// Gate server port number
#define	SERVER_ADDR_ALPHA				"118.36.245.5"					// Alpha

#if FINAL_RELEASE
//############################################################################################//

#define URL_GATE_SERVER_PORT                SERVER_PORT_KOR
#define URL_GATE_SERVER_ADDR				SERVER_ADDR_ALPHA

//############################################################################################//
#else // Debug
//############################################################################################//

#define URL_GATE_SERVER_PORT                SERVER_PORT_KOR
#define URL_GATE_SERVER_ADDR				SERVER_ADDR_ALPHA

//############################################################################################//
#endif	// FINAL_RELEASE































#endif	// __SERVER_DEFINE_H__
