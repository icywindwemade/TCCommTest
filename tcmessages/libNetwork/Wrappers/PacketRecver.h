//##############################################################################################
// PacketRecver.h
//##############################################################################################
#ifndef _PACKET_RECVER_H_
#define _PACKET_RECVER_H_

#include "Core.h"
#include "NetworkManager.h"

// Common Include ( Packet )
#include "TypeDefine.h"
#include "UnionDefine.h"
#include "Packet.h"
#include "GatePacket.h"
#include "GamePacket.h"

//##############################################################################################
// 
//##############################################################################################
extern TMap<unsigned short, NetCallbackProcessPacket> GPacketHandlerMap;

#define RPHD( x ) extern void OLRecv_##x( NNetPacket* InPacket );
#define RPHI( x ) void OLRecv_##x( NNetPacket* InPacket )
#define RPH( x ) GPacketHandlerMap.Set( x, OLRecv_##x );

//##############################################################################################
// Receive
//##############################################################################################
RPHD( CLIENT_KEEPALIVE                      );
RPHD( CLIENT_GRACEFUL_DISCONNECT			);
RPHD( CLIENT_PING		                    );

///////////////////////////////////////////////////////////////////////////////////////
// Notification from GameServer
///////////////////////////////////////////////////////////////////////////////////////	
RPHD( PTCL_GAME_NOTIFICATION				);
RPHD( PTCL_EVENT_ACHIEVEMENT_COMPLETION		);
RPHD( PTCL_NOTIFY_ITEM_EXPIRE		);

///////////////////////////////////////////////////////////////////////////////////////
// GateServer Authentication
///////////////////////////////////////////////////////////////////////////////////////	
RPHD( PTCL_ANS_GATE_AUTHENTICATION			);
RPHD( PTCL_ANS_GAME_INFO					);

///////////////////////////////////////////////////////////////////////////////////////
// GameServer Authentication
///////////////////////////////////////////////////////////////////////////////////////	
RPHD( PTCL_ANS_GAME_LOGIN					);
RPHD( PTCL_ANS_GAME_LOGOUT					);
RPHD( PTCL_ANS_DELETE_MEMBER				);
RPHD( PTCL_ANS_CURRENT_TIME					);

///////////////////////////////////////////////////////////////////////////////////////
// Deck
///////////////////////////////////////////////////////////////////////////////////////	
RPHD( PTCL_ANS_COMBAT_SETTING				);
RPHD( PTCL_ANS_CHANGE_COMBAT_SETTING		);

///////////////////////////////////////////////////////////////////////////////////////
// Inventory
///////////////////////////////////////////////////////////////////////////////////////	
RPHD( PTCL_ANS_INVENTORY					);
RPHD( PTCL_ANS_CASTLE_INVENTORY				);
RPHD( PTCL_ANS_DELETE_ITEM					);
RPHD( PTCL_ANS_MIX_ITEM						);
RPHD( PTCL_ANS_UPGRADE_ITEM					);
RPHD( PTCL_ANS_OPEN_CAPSULE					);

///////////////////////////////////////////////////////////////////////////////////////
// Store
///////////////////////////////////////////////////////////////////////////////////////	
RPHD( PTCL_ANS_PURCHASE						);
RPHD( PTCL_ANS_PURCHASE_PERMISSION			);
RPHD( PTCL_ANS_IN_APP_PURCHASE				);
RPHD( PTCL_ANS_ANDROID_IN_APP_PURCHASE		);

///////////////////////////////////////////////////////////////////////////////////////
// Friend
///////////////////////////////////////////////////////////////////////////////////////	
RPHD( PTCL_ANS_DECK							);
RPHD( PTCL_ANS_FRIEND_LIST					);
RPHD( PTCL_ANS_FRIEND_RECOMMENDATION_LIST	);
RPHD( PTCL_ANS_SEARCH_FRIEND				);
RPHD( PTCL_ANS_ADD_FRIEND					);
RPHD( PTCL_ANS_DELETE_FRIEND					);
RPHD( PTCL_ANS_FRIEND_MATCH_IN_GAME				);

///////////////////////////////////////////////////////////////////////////////////////
// Match Invitation
///////////////////////////////////////////////////////////////////////////////////////	
RPHD( PTCL_REPORTING_CUSTOM_MATCH			);

///////////////////////////////////////////////////////////////////////////////////////
// GameMatch
///////////////////////////////////////////////////////////////////////////////////////	
RPHD( PTCL_ANS_START_MATCH					);
RPHD( PTCL_ANS_STOP_MATCH					);
RPHD( PTCL_ANS_START_GAME					);
RPHD( PTCL_ANS_STOP_GAME					);

///////////////////////////////////////////////////////////////////////////////////////
// MatchEvent
///////////////////////////////////////////////////////////////////////////////////////	
RPHD( PTCL_EVENT_MATCH_COMPLETE				);
RPHD( PTCL_EVENT_MATCH_CLOSE				);
RPHD( PTCL_EVENT_CHANGE_CONTROLLER			);

///////////////////////////////////////////////////////////////////////////////////////
// GameEnding
///////////////////////////////////////////////////////////////////////////////////////	
RPHD( PTCL_EVENT_GAME_ENDING				);
RPHD( PTCL_ANS_GAME_RECORDING				);

///////////////////////////////////////////////////////////////////////////////////////
// Replication
///////////////////////////////////////////////////////////////////////////////////////	
// Replication Start
RPHD( PTCL_REPLICATION_READY				);
RPHD( PTCL_REPLICATION_SYNC					);
RPHD( PTCL_REPLICATION_ACTION				);
RPHD( PTCL_REPLICATION_PING					);
RPHD( PTCL_REPLICATION_DUMMY				);
// Replication End

///////////////////////////////////////////////////////////////////////////////////////
// Profile
///////////////////////////////////////////////////////////////////////////////////////
RPHD( PTCL_ANS_PRIVATE_INFO					);
RPHD( PTCL_ANS_BASE_INFO					);
RPHD( PTCL_ANS_BATTLE_RECOMMENDATION		);
RPHD( PTCL_ANS_BATTLE_HISTORY				);
RPHD( PTCL_ANS_GROUP_RANKING				);
RPHD( PTCL_ANS_ACHIEVEMENT_HISTORY			);
RPHD( PTCL_ANS_MISSION_HISTORY				);
RPHD( PTCL_ANS_EVENT_RECORDING				);

///////////////////////////////////////////////////////////////////////////////////////
// Ranking
///////////////////////////////////////////////////////////////////////////////////////
RPHD( PTCL_ANS_WEEKLY_RANKING_GAME_INFO						);
RPHD( PTCL_ANS_WEEKLY_RANKING								);
RPHD( PTCL_ANS_FRIEND_STAR_LIST								);
RPHD( PTCL_ANS_UPDATE_STAR_COUNT							);
RPHD( PTCL_ANS_POST_BOX										);
RPHD( PTCL_ANS_RECEIVE_POST								);
RPHD( PTCL_ANS_RECEIVE_POST_ALL							);
RPHD( PTCL_ANS_SEND_POST									);
RPHD( PTCL_ANS_PLAYER_INFO									);
RPHD( PTCL_ANS_FRIEND_INVITATION							);
RPHD( PTCL_ANS_FRIEND_INVITATION_LIST					);
RPHD( PTCL_ANS_TOP20_WEEKLY_RANKING							);
RPHD( PTCL_ANS_MISSION_OPEN							);
RPHD( PTCL_ANS_UPDATE_RANK_COMMENT							);
RPHD( PTCL_ANS_FRIEND_INVITAION_STATE							);

//##############################################################################################
// Register 
//##############################################################################################

inline void RegisterPacketHandler()
{
    RPH( CLIENT_KEEPALIVE                       );
    RPH( CLIENT_GRACEFUL_DISCONNECT             );
	RPH( CLIENT_PING		                    );

	///////////////////////////////////////////////////////////////////////////////////////
	// Notification from GameServer
	///////////////////////////////////////////////////////////////////////////////////////	
	RPH( PTCL_GAME_NOTIFICATION					);
	RPH( PTCL_NOTIFY_ITEM_EXPIRE				);
	RPH( PTCL_EVENT_ACHIEVEMENT_COMPLETION		);

	///////////////////////////////////////////////////////////////////////////////////////
	// GateServer Authentication
	///////////////////////////////////////////////////////////////////////////////////////	
	RPH( PTCL_ANS_GATE_AUTHENTICATION			);
	RPH( PTCL_ANS_GAME_INFO						);

	///////////////////////////////////////////////////////////////////////////////////////
	// GameServer Authentication
	///////////////////////////////////////////////////////////////////////////////////////	
	RPH( PTCL_ANS_GAME_LOGIN					);
    RPH( PTCL_ANS_GAME_LOGOUT					);    
	RPH( PTCL_ANS_DELETE_MEMBER					);
	RPH( PTCL_ANS_CURRENT_TIME					);

	///////////////////////////////////////////////////////////////////////////////////////
	// Deck
	///////////////////////////////////////////////////////////////////////////////////////	
	RPH( PTCL_ANS_COMBAT_SETTING				);
	RPH( PTCL_ANS_CHANGE_COMBAT_SETTING			);

	///////////////////////////////////////////////////////////////////////////////////////
	// Inventory
	///////////////////////////////////////////////////////////////////////////////////////	
	RPH( PTCL_ANS_INVENTORY						);
	RPH( PTCL_ANS_CASTLE_INVENTORY				);
	RPH( PTCL_ANS_DELETE_ITEM					);
	RPH( PTCL_ANS_MIX_ITEM						);
	RPH( PTCL_ANS_UPGRADE_ITEM					);
	RPH( PTCL_ANS_OPEN_CAPSULE					);

	///////////////////////////////////////////////////////////////////////////////////////
	// Store
	///////////////////////////////////////////////////////////////////////////////////////	
	RPH( PTCL_ANS_PURCHASE						);
	RPH( PTCL_ANS_PURCHASE_PERMISSION			);
	RPH( PTCL_ANS_IN_APP_PURCHASE				);
	RPH( PTCL_ANS_ANDROID_IN_APP_PURCHASE		);

	///////////////////////////////////////////////////////////////////////////////////////
	// Friend
	///////////////////////////////////////////////////////////////////////////////////////	
	RPH( PTCL_ANS_DECK							);
	RPH( PTCL_ANS_FRIEND_LIST					);    
	RPH( PTCL_ANS_FRIEND_RECOMMENDATION_LIST	);
	RPH( PTCL_ANS_SEARCH_FRIEND					);
	RPH( PTCL_ANS_ADD_FRIEND					);
	RPH( PTCL_ANS_DELETE_FRIEND					);
	RPH( PTCL_ANS_FRIEND_MATCH_IN_GAME			);

	///////////////////////////////////////////////////////////////////////////////////////
	// MatchInvitation
	///////////////////////////////////////////////////////////////////////////////////////	
	RPH( PTCL_REPORTING_CUSTOM_MATCH			);

	///////////////////////////////////////////////////////////////////////////////////////
	// GameMatch
	///////////////////////////////////////////////////////////////////////////////////////	
	RPH( PTCL_ANS_START_MATCH					);
	RPH( PTCL_ANS_STOP_MATCH					);
	RPH( PTCL_ANS_START_GAME					);
	RPH( PTCL_ANS_STOP_GAME						);

	///////////////////////////////////////////////////////////////////////////////////////
	// MatchEvent
	///////////////////////////////////////////////////////////////////////////////////////	
	RPH( PTCL_EVENT_MATCH_COMPLETE				);
	RPH( PTCL_EVENT_MATCH_CLOSE					);
	RPH( PTCL_EVENT_CHANGE_CONTROLLER			);

	///////////////////////////////////////////////////////////////////////////////////////
	// GameEnding
	///////////////////////////////////////////////////////////////////////////////////////	
	RPH( PTCL_EVENT_GAME_ENDING					);
	RPH( PTCL_ANS_GAME_RECORDING				);

	///////////////////////////////////////////////////////////////////////////////////////
	// Replication
	///////////////////////////////////////////////////////////////////////////////////////	
	// Replication Start
	RPH( PTCL_REPLICATION_READY					);
	RPH( PTCL_REPLICATION_SYNC					);
	RPH( PTCL_REPLICATION_ACTION				);
	RPH( PTCL_REPLICATION_PING					);
	RPH( PTCL_REPLICATION_DUMMY					);
	// Replication End

	///////////////////////////////////////////////////////////////////////////////////////
	// Profile
	///////////////////////////////////////////////////////////////////////////////////////
	RPH( PTCL_ANS_PRIVATE_INFO					);
	RPH( PTCL_ANS_BASE_INFO						);
	RPH( PTCL_ANS_BATTLE_RECOMMENDATION			);
	RPH( PTCL_ANS_BATTLE_HISTORY				);
	RPH( PTCL_ANS_GROUP_RANKING					);
	RPH( PTCL_ANS_ACHIEVEMENT_HISTORY			);
	RPH( PTCL_ANS_MISSION_HISTORY				);
	RPH( PTCL_ANS_EVENT_RECORDING				);

	///////////////////////////////////////////////////////////////////////////////////////
	// Ranking
	///////////////////////////////////////////////////////////////////////////////////////
	RPH( PTCL_ANS_WEEKLY_RANKING_GAME_INFO					);
	RPH( PTCL_ANS_WEEKLY_RANKING								);
	RPH( PTCL_ANS_FRIEND_STAR_LIST								);
	RPH( PTCL_ANS_UPDATE_STAR_COUNT								);
	RPH( PTCL_ANS_POST_BOX										);
	RPH( PTCL_ANS_RECEIVE_POST									);
	RPH( PTCL_ANS_RECEIVE_POST_ALL								);
	RPH( PTCL_ANS_SEND_POST										);
	RPH( PTCL_ANS_PLAYER_INFO									);
	RPH( PTCL_ANS_FRIEND_INVITATION							);
	RPH( PTCL_ANS_FRIEND_INVITATION_LIST						);
	RPH( PTCL_ANS_TOP20_WEEKLY_RANKING							);
	RPH( PTCL_ANS_MISSION_OPEN							);
	RPH( PTCL_ANS_UPDATE_RANK_COMMENT							);
	RPH( PTCL_ANS_FRIEND_INVITAION_STATE							);
}


#endif // _PACKET_RECVER_H_
