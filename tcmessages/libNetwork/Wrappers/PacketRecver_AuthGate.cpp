//##############################################################################################
// PacketRecver.cpp
//##############################################################################################
#include "PacketRecver.h"

#include "PacketSender.h"
#include "BuildDefine.h"

#include "OnlineHandler.h"
/*
 
#include "Client.h"
#include "MessageHandler.h"
#include "PatchInfo.h"
#include "GlobalInclude.h"*/


//##############################################################################################
// Authentication
//##############################################################################################

RPHI( PTCL_ANS_GATE_AUTHENTICATION			)
{
	ANS_GATE_AUTHENTICATION* RecvPacket = (ANS_GATE_AUTHENTICATION*)(InPacket->Data);

	FnLog("NET: RECV, ANS_GATE_AUTHENTICATION");

	GOnlineInfo->SetEventURL( RecvPacket->EventDomain );
	GOnlineInfo->SetPatchURL( RecvPacket->PatchDomain );
	GOnlineInfo->SetSocialURL( RecvPacket->WemadeSocialDomain );

	GOnlineInfo->AllowGuest( RecvPacket->AllowGuestType ); 

	//GOnlineInfo->SetPatchURL( "http://118.36.245.5:9001/Patch/" );

	if( RecvPacket->AnsCode == ANS_GATE_AUTHENTICATION::eSuccess )
	{
		// Success 
		FnLog("NET: RECV, ANS_GATE_AUTHENTICATION success");
/*
		if( GPatchInfo->IsNeedFirstPatch() )
		{
			// Disconnect Gate 
			OLDisconnectGateServer();

			HideProgressMessage();

			GPatchInfo->SkipPatch( OLPatchComplete );
		}
		else
 */
		{
			// Request Game Server List
			OLRequestServerChannels();
		}
	}
	else
	{
		// Failed 
		FnLog("NET: RECV, ANS_GATE_AUTHENTICATION failed: %d", RecvPacket->AnsCode);

		// Disconnect Gate 
		///OLDisconnectGateServer();

		///HideProgressMessage();

		switch( RecvPacket->AnsCode )
		{
		case ANS_GATE_AUTHENTICATION::eFail_Major:
		case ANS_GATE_AUTHENTICATION::eFail_Protocol:
            {
                FnLog("NET: RECV, ANS_GATE_AUTHENTICATION Need Update Server Ver = %d.%d", RecvPacket->MajorVersion, RecvPacket->MinorVersion );
                ///ShowAlertMessage( EMBA_OK, EMCT_Update, IUGetLocalText( L"string_message_need_appupdate" ) );
            }
			break;
		case ANS_GATE_AUTHENTICATION::eFail_Minor:				// Minor Version Different
			{
				FnLog("NET: RECV, ANS_GATE_AUTHENTICATION Need Patch");
				/*if( GPatchInfo->IsNeedFirstPatch() )
				{
					GPatchInfo->StartPatch( GOnlineInfo->GetPatchURL(), OLPatchComplete );
                    
					//GPatchInfo->SkipPatch( OLPatchComplete ); // Debug
				}
				else
				{
					ShowAlertMessage( EMBA_OK, EMCT_Patch, IUGetLocalText( L"info_new_patch" ) ); // Force Restart
				}*/
			}
			break;
		case ANS_GATE_AUTHENTICATION::eFail:
FnLog("NET: RECV, ANS_GATE_AUTHENTICATION eFail");
                
//			ShowAlertMessage( EMBA_OK, EMCT_Exit, L"%s||%s||%s", IUGetLocalText(L"info_server_check"), IUGetLocalText(L"info_error_restart"), IUGetLocalText(L"info_endgame") ); // Force Exit
			break;
		case ANS_GATE_AUTHENTICATION::eFail_Maintenance:
			/* {
				if( strlen( GOnlineInfo->EventURL ) > 0 && strncmp( GOnlineInfo->EventURL, "http", 4 ) != 0 )
				{
					WCHAR TempWideText[256];
					GetWideString( GOnlineInfo->EventURL, TempWideText, 256 );

					ShowAlertMessage( EMBA_OK, EMCT_Maintenance, TempWideText );
				}
				else
				{
					ShowAlertMessage( EMBA_OK, EMCT_Maintenance, L"%s||%s", IUGetLocalText( L"info_server_check" ), IUGetLocalText( L"info_endgame" ) ); // Force Exit
				}
			} */
			break;
		default:
                /*
			ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s||%s", IUGetLocalText(L"info_error_service"), IUGetLocalText(L"info_error_restart") ); // Force Exit  */
			break;
		}
	}
}

RPHI( PTCL_ANS_GAME_INFO					)
{
	ANS_GAME_INFO* RecvPacket = (ANS_GAME_INFO*)(InPacket->Data);

	FnLog("NET: RECV, ANS_GAME_INFO");

	// Find channel 
	int ChannelCount = MIN( RecvPacket->ChannelCount, UD_MAX_CHANNEL_COUNT );
	
	GOnlineInfo->ResetChannels( ChannelCount );

	for( int i = 0; i < ChannelCount; i++ )
	{
		NServerChannel* NewChannel = GOnlineInfo->GetChannel( i );
		if( NewChannel )
		{
			memcpy( NewChannel->IP, RecvPacket->Channel[i].IP, UD_IP_LEN );
			NewChannel->Port		= RecvPacket->Channel[i].Port;        
			NewChannel->ID			= RecvPacket->Channel[i].ID;
			NewChannel->Users		= RecvPacket->Channel[i].OnlineClientCount;
			NewChannel->MaxUsers	= MIN( RecvPacket->Channel[i].FullClientCount, UD_MAX_CHANNEL_USERS );
		}
	}

	NServerChannel* SelectedChannel = GOnlineInfo->SelectChannel();

	// Disconnect Gate
	OLDisconnectGateServer();

	if( SelectedChannel )
	{
		FnLog("NET: RECV, ANS_GAME_INFO found, Login Game Server");

		if( GOnlineInfo->IsChannelMigrating() )
			{
			if( !GOnlineInfo->IsMigratingChannelID( SelectedChannel->ID ) )
			{
				OLEndedMatchMigrating();
			}

			GOnlineInfo->ResetChannelMigration();
	}

		// Progress timeout
		// ShowProgressMessage( 15000, EMCT_Reconnect );

		// Check disconnect
		OLDisconnectGateServer();

		// Login to GameServer, Will call OLCallbackGateConnected()
		OLConnectGameServer();
	}
	else
	{
		FnLog("NET: RECV, ANS_GAME_INFO full channel");

/*
		if( ChannelCount <= 0 )
		{
			ShowAlertMessage( EMBA_OK, EMCT_Exit, L"%s||%s||%s..", IUGetLocalText(L"info_server_check"), IUGetLocalText(L"info_error_restart"), IUGetLocalText(L"info_endgame") ); // Force Exit
		}
		else
		{
			ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s|| %s", IUGetLocalText(L"info_login_fail_full"), IUGetLocalText(L"info_error_restart") );
		}
 */
	}

}
