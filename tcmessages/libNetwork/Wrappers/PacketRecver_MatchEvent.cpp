//##############################################################################################
// PacketRecver.cpp
//##############################################################################################
#include "PacketRecver.h"

#include "PacketSender.h"
#include "BuildDefine.h"
#include "OnlineHandler.h"
//#include "Client.h"
//#include "MessageHandler.h"
//#include "PlayerInfo.h"
//#include "GameInfo.h"
//#include "GameData.h"
//#include "UserInfo.h"



//##############################################################################################
// MatchEvent
//##############################################################################################
#if ( BK_INGAME_LEVEL == 1 )
RPHI( PTCL_EVENT_MATCH_COMPLETE		)
{
	// RecvPacket->RecvPacket->Players[i]->Level == TestLevel
	EVENT_MATCH_COMPLETE* RecvPacket = (EVENT_MATCH_COMPLETE*)(InPacket->Data);

	FnLog("NET: RECV, EVENT_MATCH_COMPLETE");

	if( !GOnlineInfo->IsMatching() ) 
	{
		OLRequestStopMatch();

		return;
	}

	char TempName[UD_NICK_LEN];
	bool bMulti = (RecvPacket->MatchTarget < eUDMT_Single);
	bool bSimulation = (RecvPacket->MatchTarget >= eUDMT_Simulation);

	// Save Last Mode
	GOnlineInfo->ChangeSceneMode( RecvPacket->BattleMode, RecvPacket->MatchMode, RecvPacket->MatchTarget );

	// Reset game info
	GGameInfo->ResetGame();
	GGameInfo->SetupPlayMode( RecvPacket->BattleMode, RecvPacket->MatchMode, bMulti, bSimulation );

	// Set player info
	CPlayerInfo* Player;
	UD_PLAYER* PlayerPacket;
	int	SlotUnitIDs[UD_UNIT_SLOT_COUNT];
	int	SlotUnitLevels[UD_UNIT_SLOT_COUNT];
	int	SlotSkillIDs[UD_SKILL_SLOT_COUNT];
	int	SlotCastleID;
	int PlayerCount = MIN( RecvPacket->PlayerCount, 4 );
	CMissionData* Mission = GGameInfo->GetMissionData();

	if( RecvPacket->MatchMode == eUDMM_Single )
		Mission = GGameData->GetMissionData( GGameInfo->GetMissionData()->m_MissionNo + GGameInfo->SelectMissionLevelNo );

	for( int i = 0; i < PlayerCount; i++ )
	{
		PlayerPacket = &RecvPacket->Players[i];

		// Set Player Name
		strcpy( TempName, PlayerPacket->Name );
		if( RecvPacket->MatchMode == eUDMM_Single && PlayerPacket->PlayerType == eUDMPT_AI && Mission )
		{
#if FINAL_RELEASE
			strcpy( TempName, Mission->m_AIName.c_str() ); // Set AI Name
#else
			sprintf( TempName, "%s(AI:%d) ", Mission->m_AIName.c_str(), Mission->m_AILevel );
#endif
		}

		// Castle Setting =======================================================================================
		SlotCastleID = PlayerPacket->CastleID;

		// if Single Mode 
		if ( RecvPacket->BattleMode == eUDBM_Mission )
		{
			if( Mission && Mission->m_PlayerCastle != 0 && PlayerPacket->UKey > 0 ) 
				SlotCastleID = Mission->m_PlayerCastle;
			if( Mission && Mission->m_AICastle != 0 && PlayerPacket->UKey < 0 ) 
				SlotCastleID = Mission->m_AICastle;
		}

		// Check Castle Data
		if( !GGameData->GetObjectData( SlotCastleID ) ) 
		{
			OLRequestStopMatch();
			OLRequestStopGame( 9020 );

			OLEndedMatching();

			if( !IsOpenedAlertMessage() ) 
				ShowAlertMessage( EMBA_OK, EMCT_BackScene, IUGetLocalText( L"info_battle_error_match" ) );

			return;
		}
		// Castle Setting =======================================================================================

		// Skill Setting =======================================================================================
		bool bChangeSkill = false;
		// if Single Mode 
		if ( RecvPacket->BattleMode == eUDBM_Mission && Mission )
		{
			if( PlayerPacket->UKey > 0 && Mission->m_PlayerSkill[0] != 0 )
			{
				for ( int i = 0; i < UD_SKILL_SLOT_COUNT; ++i ) SlotSkillIDs[i] = Mission->m_PlayerSkill[i];

				bChangeSkill = true;
			}
			if( PlayerPacket->UKey < 0 )
			{
				for ( int i = 0; i < UD_SKILL_SLOT_COUNT; ++i ) SlotSkillIDs[i] = Mission->m_AISkill[i];
				bChangeSkill = true;
			}
		}
		// Skill Setting =======================================================================================

		// Deck Setting =======================================================================================
		// if Multi Mode

		for ( int i = 0; i < UD_UNIT_SLOT_COUNT; ++i ) SlotUnitIDs[i] = PlayerPacket->UnitIDs[i];
		for ( int i = 0; i < UD_UNIT_SLOT_COUNT; ++i ) SlotUnitLevels[i] = PlayerPacket->UnitLevels[i];
		
		// if Single Mode 
		if ( RecvPacket->BattleMode == eUDBM_Mission && Mission )
		{
			if( PlayerPacket->UKey > 0 && Mission->m_PlayerUnit[0] != 0 ) 
			{
				for ( int i = 0; i < UD_UNIT_SLOT_COUNT; ++i ) SlotUnitIDs[i] = Mission->m_PlayerUnit[i];
				for ( int i = 0; i < UD_UNIT_SLOT_COUNT; ++i ) SlotUnitLevels[i] = 1;
			}
			if( PlayerPacket->UKey < 0 ) // Only MissionData Units
			{
				for ( int i = 0; i < UD_UNIT_SLOT_COUNT; ++i ) SlotUnitIDs[i] = Mission->m_AIUnit[i];
				// test code
				for ( int i = 0; i < UD_UNIT_SLOT_COUNT; ++i ) SlotUnitLevels[i] = PlayerPacket->UnitLevels[i];
			}

			PlayerPacket->Ability = (unsigned char)Mission->m_AILevel;
			PlayerPacket->Strategy = Mission->m_AIStratage;

		}
		// Deck Setting =======================================================================================
		
		// Create Player
		Player = GGameInfo->CreatePlayer( PlayerPacket->UKey, TempName, PlayerPacket->PlayerID, SlotCastleID, SlotUnitIDs, 
			SlotUnitLevels,
			PlayerPacket->Ability, PlayerPacket->Strategy, PlayerPacket->PlayerType );
		if( Player )
		{
			if( bChangeSkill ) Player->ChangeSkill( SlotSkillIDs );

			Player->SetNetworkModel( PlayerPacket->NetworkModel );
			Player->SetHost( Player->GetGamerID() == GAME_GAMER_ID_BEGIN );

			Player->Record.Portrait		= PlayerPacket->Portrait;
			Player->Record.WinCount		= PlayerPacket->WinCount;
			Player->Record.LoseCount	= PlayerPacket->LoseCount;
			Player->Record.GroupRank	= PlayerPacket->GroupRanking;
			Player->Record.LadderPoint	= PlayerPacket->LadderPoint;
			//Player->Record.ChangeLP	= pRecordPacket->ChangeLP;

			if( Player->UserKey == GOnlineInfo->UserKey )
			{
				GNetworkManager->SetSyncID( Player->GetID() );

				GGameInfo->ChangeMasterPlayer( Player );

				if( Player->IsHost() ) GGameInfo->SetHost();
			}
			else
			{
				GOnlineInfo->MatchTarget = RecvPacket->MatchTarget;
				GOnlineInfo->MatchTargetKey = PlayerPacket->UKey;
				memcpy( GOnlineInfo->MatchTargetNick, PlayerPacket->Name, UD_NICK_LEN );
			}
		}
	}

	OLEndedMatching( RecvPacket->BattleMode, RecvPacket->MatchMode );
}
#else
RPHI( PTCL_EVENT_MATCH_COMPLETE		)
{
	///EVENT_MATCH_COMPLETE* RecvPacket = (EVENT_MATCH_COMPLETE*)(InPacket->Data);

	FnLog("NET: RECV, EVENT_MATCH_COMPLETE");
/*
	if( !GOnlineInfo->IsMatching() ) 
	{
		OLRequestStopMatch();

		return;
	}

	char TempName[UD_NICK_LEN];
	bool bMulti = (RecvPacket->MatchTarget < eUDMT_Single);
	bool bSimulation = (RecvPacket->MatchTarget >= eUDMT_Simulation);

	// Save Last Mode
	GOnlineInfo->ChangeSceneMode( RecvPacket->BattleMode, RecvPacket->MatchMode, RecvPacket->MatchTarget );

	// Reset game info
	GGameInfo->ResetGame();
	GGameInfo->SetupPlayMode( RecvPacket->BattleMode, RecvPacket->MatchMode, bMulti, bSimulation );

	// Set player info
	CPlayerInfo* Player;
	UD_PLAYER* PlayerPacket;
	int	SlotUnitIDs[UD_UNIT_SLOT_COUNT];
	int	SlotSkillIDs[UD_SKILL_SLOT_COUNT];
	int	SlotCastleID;
	int PlayerCount = MIN( RecvPacket->PlayerCount, 4 );
	CMissionData* Mission = GGameInfo->GetMissionData();

	if( RecvPacket->MatchMode == eUDMM_Single )
		Mission = GGameData->GetMissionData( GGameInfo->GetMissionData()->m_MissionNo + GGameInfo->SelectMissionLevelNo );

	for( int i = 0; i < PlayerCount; i++ )
	{
		PlayerPacket = &RecvPacket->Players[i];

		// Set Player Name
		strcpy( TempName, PlayerPacket->Name );
		if( RecvPacket->MatchMode == eUDMM_Single && PlayerPacket->PlayerType == eUDMPT_AI && Mission )
		{
#if FINAL_RELEASE
			strcpy( TempName, Mission->m_AIName.c_str() ); // Set AI Name
#else
			sprintf( TempName, "%s(AI:%d) ", Mission->m_AIName.c_str(), Mission->m_AILevel );
#endif
		}

		// Castle Setting =======================================================================================
		SlotCastleID = PlayerPacket->CastleID;

		// if Single Mode 
		if ( RecvPacket->BattleMode == eUDBM_Mission )
		{
			if( Mission && Mission->m_PlayerCastle != 0 && PlayerPacket->UKey > 0 ) 
				SlotCastleID = Mission->m_PlayerCastle;
			if( Mission && Mission->m_AICastle != 0 && PlayerPacket->UKey < 0 ) 
				SlotCastleID = Mission->m_AICastle;
		}

		// Check Castle Data
		if( !GGameData->GetObjectData( SlotCastleID ) ) 
		{
			OLRequestStopMatch();
			OLRequestStopGame( 9020 );

			OLEndedMatching();

			if( !IsOpenedAlertMessage() ) 
				ShowAlertMessage( EMBA_OK, EMCT_BackScene, IUGetLocalText( L"info_battle_error_match" ) );

			return;
		}
		// Castle Setting =======================================================================================

		// Skill Setting =======================================================================================
		bool bChangeSkill = false;
		// if Single Mode 
		if ( RecvPacket->BattleMode == eUDBM_Mission && Mission )
		{
			if( PlayerPacket->UKey > 0 && Mission->m_PlayerSkill[0] != 0 )
			{
				for ( int i = 0; i < UD_SKILL_SLOT_COUNT; ++i ) SlotSkillIDs[i] = Mission->m_PlayerSkill[i];

				bChangeSkill = true;
			}
			if( PlayerPacket->UKey < 0 )
			{
				for ( int i = 0; i < UD_SKILL_SLOT_COUNT; ++i ) SlotSkillIDs[i] = Mission->m_AISkill[i];
				bChangeSkill = true;
			}
		}
		// Skill Setting =======================================================================================

		// Deck Setting =======================================================================================
		// if Multi Mode
		for ( int i = 0; i < UD_UNIT_SLOT_COUNT; ++i ) SlotUnitIDs[i] = PlayerPacket->UnitIDs[i];

		// if Single Mode 
		if ( RecvPacket->BattleMode == eUDBM_Mission && Mission )
		{
			if( PlayerPacket->UKey > 0 && Mission->m_PlayerUnit[0] != 0 ) 
				for ( int i = 0; i < UD_UNIT_SLOT_COUNT; ++i ) SlotUnitIDs[i] = Mission->m_PlayerUnit[i];
			if( PlayerPacket->UKey < 0 ) // Only MissionData Units
				for ( int i = 0; i < UD_UNIT_SLOT_COUNT; ++i ) SlotUnitIDs[i] = Mission->m_AIUnit[i];

			PlayerPacket->Ability = (unsigned char)Mission->m_AILevel;
			PlayerPacket->Strategy = Mission->m_AIStratage;
			
		}
		// Deck Setting =======================================================================================

		// Create Player
		Player = GGameInfo->CreatePlayer( PlayerPacket->UKey, TempName, PlayerPacket->PlayerID, SlotCastleID, SlotUnitIDs, 
											PlayerPacket->Ability, PlayerPacket->Strategy, PlayerPacket->PlayerType );
		if( Player )
		{
			if( bChangeSkill ) Player->ChangeSkill( SlotSkillIDs );

			Player->SetNetworkModel( PlayerPacket->NetworkModel );
			Player->SetHost( Player->GetGamerID() == GAME_GAMER_ID_BEGIN );

			Player->Record.Portrait		= PlayerPacket->Portrait;
			Player->Record.WinCount		= PlayerPacket->WinCount;
			Player->Record.LoseCount	= PlayerPacket->LoseCount;
			Player->Record.GroupRank	= PlayerPacket->GroupRanking;
			Player->Record.LadderPoint	= PlayerPacket->LadderPoint;
			//Player->Record.ChangeLP	= pRecordPacket->ChangeLP;

			if( Player->UserKey == GOnlineInfo->UserKey )
			{
				GNetworkManager->SetSyncID( Player->GetID() );

				GGameInfo->ChangeMasterPlayer( Player );

				if( Player->IsHost() ) GGameInfo->SetHost();
			}
			else
			{
				GOnlineInfo->MatchTarget = RecvPacket->MatchTarget;
				GOnlineInfo->MatchTargetKey = PlayerPacket->UKey;
				memcpy( GOnlineInfo->MatchTargetNick, PlayerPacket->Name, UD_NICK_LEN );
			}
		}
	}

	OLEndedMatching( RecvPacket->BattleMode, RecvPacket->MatchMode );
 */
}
#endif
RPHI( PTCL_EVENT_MATCH_CLOSE			)
{
	FnLog("NET: RECV, EVENT_QUICK_MATCH_CLOSE");
/*
	OLEndedMatchInviting();

	OLEndedMatching();

	ShowAlertMessage( EMBA_OK, EMCT_BackScene, IUGetLocalText( L"info_battle_error_match" ) ); */
}

RPHI( PTCL_EVENT_CHANGE_CONTROLLER			)
{
	/// EVENT_CHANGE_CONTROLLER* RecvPacket = (EVENT_CHANGE_CONTROLLER*)(InPacket->Data);

	FnLog("NET: RECV, EVENT_CHANGE_CONTROLLER");

	//ShowNoticeMessage( L"ChangePlayer ChangePlayer", 1000 );
	///GGameInfo->ChangePlayer( GNetworkManager->GetSyncNextFrame(), RecvPacket->UKey, RecvPacket->AIMode, RecvPacket->BattleMode );
}
