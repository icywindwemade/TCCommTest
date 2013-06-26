#pragma once

#define CHECK_GAME_PROCESS(ptcl)( (ptcl >= DEFINE_GAME_PROTOCOL) && (ptcl < END_GAME_DEFINITION ) )

enum eGame_Protocol
{
	GAME_PROTOCOL_VERSION	= 147,

	DEFINE_GAME_PROTOCOL	= 30000,

	//////////////////////////////////////////////////////////////////////////
	// Channel Login

	/* Login */
	PTCL_REQ_GAME_LOGIN,
	PTCL_ANS_GAME_LOGIN,

	/* Logout */
	PTCL_REQ_GAME_LOGOUT,
	PTCL_ANS_GAME_LOGOUT,

	/* Search Name */
	PTCL_REQ_SEARCH_NAME,
	PTCL_ANS_SEARCH_NAME,

	//////////////////////////////////////////////////////////////////////////
	// Lobby

	/* 접속자 수 */
	PTCL_REQ_CONNECT_COUNT,
	PTCL_ANS_CONNECT_COUNT,

	/* 서버 현재 시간 로드 */
	PTCL_REQ_CURRENT_TIME,
	PTCL_ANS_CURRENT_TIME,

	/* Change Auto Login Option */
	PTCL_REPORTING_AUTO_LOGIN,	

	/* Change Game Option */
	PTCL_REPORTING_GAME_OPTION,

	/* Gmaer Position In Game */
	PTCL_REPORTING_GAME_POSITION,

	/* Private Information */
	PTCL_REQ_PRIVATE_INFO,
	PTCL_ANS_PRIVATE_INFO,

	/* Base Information */
	PTCL_REQ_BASE_INFO,
	PTCL_ANS_BASE_INFO,

	/* Group Ranking */
	PTCL_REQ_GROUP_RANKING,
	PTCL_ANS_GROUP_RANKING,

	/* Achievement History */
	PTCL_REQ_ACHIEVEMENT_HISTORY,
	PTCL_ANS_ACHIEVEMENT_HISTORY,

	/* Personal Deck Setting */
	PTCL_REQ_COMBAT_SETTING,
	PTCL_ANS_COMBAT_SETTING,

	/* Change Personal Deck Setting */
	PTCL_REQ_CHANGE_COMBAT_SETTING,
	PTCL_ANS_CHANGE_COMBAT_SETTING,

	//////////////////////////////////////////////////////////////////////////
	/* Deck */
	PTCL_REQ_DECK,
	PTCL_ANS_DECK,

	/* User Mission History */
	PTCL_REQ_MISSION_HISTORY,
	PTCL_ANS_MISSION_HISTORY,

	//////////////////////////////////////////////////////////////////////////
	/* Load Inventory */
	PTCL_REQ_INVENTORY,
	PTCL_ANS_INVENTORY,

	/* Castle Inventory */
	PTCL_REQ_CASTLE_INVENTORY,
	PTCL_ANS_CASTLE_INVENTORY,

	/* 아이템 삭제 */
	PTCL_REQ_DELETE_ITEM,
	PTCL_ANS_DELETE_ITEM,

	/* 아이템 조합 */
	PTCL_REQ_MIX_ITEM,
	PTCL_ANS_MIX_ITEM,

	/* 캡슐 열기 */
	PTCL_REQ_OPEN_CAPSULE,
	PTCL_ANS_OPEN_CAPSULE,

	//////////////////////////////////////////////////////////////////////////
	/* Purchse */
	PTCL_REQ_PURCHASE,
	PTCL_ANS_PURCHASE,

	/* Purchase Permission */
	PTCL_REQ_PURCHASE_PERMISSION,
	PTCL_ANS_PURCHASE_PERMISSION,

	/* IOS In App Purchase */
	PTCL_REQ_IN_APP_PURCHASE,
	PTCL_ANS_IN_APP_PURCHASE,

	/* Android in app purchase */
	PTCL_REQ_ANDROID_IN_APP_PURCHASE,
	PTCL_ANS_ANDROID_IN_APP_PURCHASE,

	//////////////////////////////////////////////////////////////////////////
	/* Battle Recommendation */
	PTCL_REQ_BATTLE_RECOMMENDATION,
	PTCL_ANS_BATTLE_RECOMMENDATION,

	/* Battle History */
	PTCL_REQ_BATTLE_HISTORY,
	PTCL_ANS_BATTLE_HISTORY,

	//////////////////////////////////////////////////////////////////////////
	/* Friend List */
	PTCL_REQ_FRIEND_LIST,
	PTCL_ANS_FRIEND_LIST,

	/* Friend Recommendation List */
	PTCL_REQ_FRIEND_RECOMMENDATION_LIST,
	PTCL_ANS_FRIEND_RECOMMENDATION_LIST,

	/* Search Friend */
	PTCL_REQ_SEARCH_FRIEND,
	PTCL_ANS_SEARCH_FRIEND,

	/* Add Friend */
	PTCL_REQ_ADD_FRIEND,
	PTCL_ANS_ADD_FRIEND,

	//////////////////////////////////////////////////////////////////////////
	/* Start Match */
	PTCL_REQ_START_MATCH,
	PTCL_ANS_START_MATCH,

	/* Stop Match */
	PTCL_REQ_STOP_MATCH,
	PTCL_ANS_STOP_MATCH,

	/* Custom Match */
	PTCL_REPORTING_CUSTOM_MATCH,

	/* Match Complete */
	PTCL_EVENT_MATCH_COMPLETE,

	/* Match Close */
	PTCL_EVENT_MATCH_CLOSE,

	/* Achievement Completion */
	PTCL_EVENT_ACHIEVEMENT_COMPLETION,

	/* Event Change Controller */
	PTCL_EVENT_CHANGE_CONTROLLER,

	//////////////////////////////////////////////////////////////////////////
	// Room

	/* Start Game */
	PTCL_REQ_START_GAME,
	PTCL_ANS_START_GAME,

	/* Stop Game */
	PTCL_REQ_STOP_GAME,
	PTCL_ANS_STOP_GAME,

	//////////////////////////////////////////////////////////////////////////
	/* Game replication start point protocol  */
	PTCL_REPLICATION_START,

	/* Game sync ready */
	PTCL_REPLICATION_READY,

	/* Check game update frame */
	PTCL_REPLICATION_SYNC,

	/* Game sync action */
	PTCL_REPLICATION_ACTION,

	/* Game ping */
	PTCL_REPLICATION_PING,

	/* Game Dummy data */
	PTCL_REPLICATION_DUMMY,

	/* Game replication end point protocol  */
	PTCL_REPLICATION_END,
	//////////////////////////////////////////////////////////////////////////

	/* Reporting Game State */
	PTCL_REPORTING_GAME_STATE,

	/* Game Ending Event */
	PTCL_EVENT_GAME_ENDING,	

	/* Request Ending Info */
	PTCL_REQ_GAME_RECORDING,
	PTCL_ANS_GAME_RECORDING,

	//////////////////////////////////////////////////////////////////////////

	/* KakaoTalk Event Recording */
	PTCL_REQ_EVENT_RECORDING,
	PTCL_ANS_EVENT_RECORDING,

	//////////////////////////////////////////////////////////////////////////
	/* Client Log */
	PTCL_REPORTING_LOG,

	//////////////////////////////////////////////////////////////////////////
	/* Notification */
	PTCL_GAME_NOTIFICATION,

	//////////////////////////////////////////////////////////////////////////
	PTCL_NOTIFY_ITEM_EXPIRE,

	PTCL_REQ_FRIEND_MATCH_IN_GAME,
	PTCL_ANS_FRIEND_MATCH_IN_GAME,

	PTCL_REQ_DELETE_FRIEND,
	PTCL_ANS_DELETE_FRIEND,

	//////////////////////////////////////////////////////////////////////////
	/* Delete Member */
	PTCL_REQ_DELETE_MEMBER,
	PTCL_ANS_DELETE_MEMBER,

	//////////////////////////////////////////////////////////////////////////
	// For AOS

	// Player Information
	PTCL_REQ_PLAYER_INFO,
	PTCL_ANS_PLAYER_INFO,

	//////////////////////////////////////////////////////////////////////////
	/* Friend Invitation List */
	PTCL_REQ_FRIEND_INVITATION_LIST,
	PTCL_ANS_FRIEND_INVITATION_LIST,

	/* Friend Invitation */
	PTCL_REQ_FRIEND_INVITATION,
	PTCL_ANS_FRIEND_INVITATION,

	//////////////////////////////////////////////////////////////////////////
	/* TOP 20 WeeklyRanking */
	PTCL_REQ_TOP20_WEEKLY_RANKING,
	PTCL_ANS_TOP20_WEEKLY_RANKING,
	
	PTCL_REQ_WEEKLY_RANKING_GAME_INFO,
	PTCL_ANS_WEEKLY_RANKING_GAME_INFO,

	PTCL_REQ_WEEKLY_RANKING,
	PTCL_ANS_WEEKLY_RANKING,

	/* star list */
	PTCL_REQ_FRIEND_STAR_LIST,
	PTCL_ANS_FRIEND_STAR_LIST,

	/* Medal */ 
	PTCL_REQ_MEDAL_RECORDING,

	PTCL_REQ_UPDATE_STAR_COUNT,
	PTCL_ANS_UPDATE_STAR_COUNT,

	//////////////////////////////////////////////////////////////////////////

	/* Post Box */
	PTCL_REQ_POST_BOX,
	PTCL_ANS_POST_BOX,

	PTCL_REQ_RECEIVE_POST,
	PTCL_ANS_RECEIVE_POST,

	PTCL_REQ_RECEIVE_POST_ALL,
	PTCL_ANS_RECEIVE_POST_ALL,

	PTCL_REQ_SEND_POST,
	PTCL_ANS_SEND_POST,	

	// Rank Comment 
	PTCL_REQ_UPDATE_RANK_COMMENT,
	PTCL_ANS_UPDATE_RANK_COMMENT,

	// Mission Open
	PTCL_REQ_MISSION_OPEN,
	PTCL_ANS_MISSION_OPEN,	

	/* Friend InvitaionState */
	PTCL_REQ_FRIEND_INVITAION_STATE,
	PTCL_ANS_FRIEND_INVITAION_STATE,

	/*upgrade item */
	PTCL_REQ_UPGRADE_ITEM,
	PTCL_ANS_UPGRADE_ITEM,

	END_GAME_DEFINITION
};
