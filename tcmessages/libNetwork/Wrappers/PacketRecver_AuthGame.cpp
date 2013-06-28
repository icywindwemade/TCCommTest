//##############################################################################################
// PacketRecver.cpp
//##############################################################################################
#include "PacketRecver.h"

#include "PacketSender.h"
#include "BuildDefine.h"
#include "OnlineHandler.h"
//#include "Client.h"
//#include "MessageHandler.h"
//#include "PlatformHandler.h"
//
//#include "GlobalInclude.h"
//#include "TitleScene.h"
//#include "MainScene.h"
//#include "NickNameScene.h"

//#include "GameInfo.h"




//##############################################################################################
// Authentication
//##############################################################################################
RPHI( PTCL_ANS_GAME_LOGIN					)
{
	ANS_GAME_LOGIN* RecvPacket = (ANS_GAME_LOGIN*)(InPacket->Data);

	FnLog("NET: RECV, ANS_GAME_LOGIN");

	GOnlineInfo->AuthID = RecvPacket->gUid;
	GOnlineInfo->AuthToken = RecvPacket->authToken;

/*
 
	// Close Progress
	HideProgressMessage();
 */

	if( RecvPacket->AnsCode == ANS_GAME_LOGIN::eSuccess )
	{
		// Success 
		FnLog("NET: RECV, ANS_GAME_LOGIN success %d", (int)RecvPacket->UKey );

		// Login complete ( Init user info, platform info, etc...)
	///	GOnlineInfo->DidLogin( RecvPacket->ConUID, RecvPacket->UKey, RecvPacket->gUid, RecvPacket->authToken );

		FnLog("NET: RECV, Login ID %d", RecvPacket->ConUID );
		FnLog("NET: RECV, Login Key %lld", RecvPacket->UKey );
		FnLog("NET: RECV, Login gUid %lld", RecvPacket->gUid );
		FnLog("NET: RECV, Login authToken %d", RecvPacket->authToken );

		// Request all info
		GOnlineInfo->DidLoaded( false );

/*
 
		// Start Ping Check
		GOnlineInfo->StartPingTime( GNetworkManager->GetNetworkTime() );

		// Write member to file
        OLWriteSaveMember();

		OLLoadDataAll(); */
	}
	else
	{
		// Failed 
		FnLog("NET: RECV, ANS_GAME_LOGIN failed %d", RecvPacket->AnsCode );

		///OLDisconnectGameServer();

		///HideProgressMessage();
/*
		switch( RecvPacket->AnsCode )
		{
		case ANS_GAME_LOGIN::eFail:
			ShowAlertMessage( EMBA_OK, EMCT_Login, IUGetLocalText( "info_login_fail" ) );
			break;
		case ANS_GAME_LOGIN::eFail_Protocol:
		case ANS_GAME_LOGIN::eFail_Major:	// Major Version Different
		case ANS_GAME_LOGIN::eFail_AndroidCRC :
			ShowAlertMessage( EMBA_OK, EMCT_Update, IUGetLocalText( L"string_message_need_appupdate" ) );
			break;
		case ANS_GAME_LOGIN::eFail_Minor:	// Minor Version Different
			ShowAlertMessage( EMBA_OK, EMCT_Patch, IUGetLocalText( L"info_new_patch" ) ); // Force Restart
			break;
		case ANS_GAME_LOGIN::eFail_NoAccount:
            ShowAlertMessage( EMBA_OK, EMCT_Join, IUGetLocalText( L"info_login_confirm" ) );
			break;
		case ANS_GAME_LOGIN::eFail_Duplication:
			ShowAlertMessage( EMBA_OK, EMCT_Join, IUGetLocalText( L"info_login_fail_id_same" ) );
			break;
		case ANS_GAME_LOGIN::eFail_Full_Channel:
			ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s||%s", IUGetLocalText(L"info_login_fail_full"), IUGetLocalText(L"info_error_restart") );
			break;
		case ANS_GAME_LOGIN::eFail_Auth:
			ShowAlertMessage( EMBA_OK, EMCT_Login, IUGetLocalText( L"info_login_fail" ) );
			break;
		case ANS_GAME_LOGIN::eFail_Logoff:
			ShowAlertMessage( EMBA_OK, EMCT_Login, IUGetLocalText( L"info_logout_login" ) );
			break;
		case ANS_GAME_LOGIN::eFail_KakaoTalk_MoreInformation:
			{
                ClientOpenWebView( 2, eUDWSRT_AddEmail );
			}
			break;
		case ANS_GAME_LOGIN::eFail_Token:
			ShowAlertMessage( EMBA_OK, EMCT_Join, IUGetLocalText( L"info_login_fail" ) );
			break;
		case ANS_GAME_LOGIN::eFail_Name:
			{
                Telegram* NewMsg = new Telegram( 0, EOMC_InputNick, 0, CustomMessage::Online );
                IUGameManager().Message( NewMsg );
			}
			break;
		case ANS_GAME_LOGIN::eFail_MemberDeletion:
			{
				int RemainDay = RecvPacket->MemberDeletionRemainingHours / 24;
				int RemainHour = RecvPacket->MemberDeletionRemainingHours % 24;
				WCHAR TempWideText[256];
				IUPrintf( TempWideText, IUGetLocalText(L"info_unregister_check"), RemainDay, RemainHour );
				WCHAR TempWideID[256];
				GetWideString( GOnlineInfo->PlatformUserID, TempWideID, 256 );
				
				PFResetRegistration();

				ShowAlertMessage( EMBA_OK, EMCT_Exit, L"%s|| ||%s %s||%s", 
					TempWideText, IUGetLocalText(L"info_unregister_app_id"), TempWideID, IUGetLocalText(L"info_unregister_support") ); // Force Exit
			}
			break;
        case ANS_GAME_LOGIN::eFail_Line_Auth:
            {
                ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s||%s", IUGetLocalText(L"info_login_fail"), IUGetLocalText(L"info_error_restart") ); // Force Exit                
            }
            break;
		default:
			ShowAlertMessage( EMBA_OK, EMCT_Restart, L"%s||%s", IUGetLocalText(L"info_error_service"), IUGetLocalText(L"info_error_restart") ); // Force Exit
			break;
		} */
	}
 
}

RPHI( PTCL_ANS_GAME_LOGOUT					)
{
	ANS_GAME_LOGOUT* RecvPacket = (ANS_GAME_LOGOUT*)(InPacket->Data);

	FnLog("NET: RECV, ANS_GAME_LOGOUT");

	///HideProgressMessage();

	if( RecvPacket->AnsCode == ANS_GAME_LOGOUT::eSuccess )
	{
		FnLog("NET: RECV, ANS_GAME_LOGOUT Success");
/*
		strcpy( GOnlineInfo->PlatformUserName, "" );
        GOnlineInfo->DidLogout();

        OLDisconnectCurrentServer( true );
        OLClearSaveMember();

        if( PFIsValidPlatformCode( GOnlineInfo->PlatformCode ) )
        {
            PFLogout();
        }
        else
        {
            ChangeScene( eID_Scene_Title );
        } */
	}
	else
	{
		// Skip
		FnLog("NET: RECV, ANS_GAME_LOGOUT Failed");

		/* ShowAlertMessage( EMBA_OK, EMCT_Join, IUGetLocalText( L"info_connect_error_sync2" ) ); */
	}
}

RPHI( PTCL_ANS_DELETE_MEMBER				)
{
	ANS_DELETE_MEMBER* RecvPacket = (ANS_DELETE_MEMBER*)(InPacket->Data);

	FnLog("NET: RECV, ANS_DELETE_MEMBER");

	/// HideProgressMessage();

	if( RecvPacket->AnsCode == ANS_DELETE_MEMBER::eSuccess )
	{
		FnLog("NET: RECV, ANS_DELETE_MEMBER Success");

/*
		strcpy( GOnlineInfo->PlatformUserName, "" );

        OLDisconnectCurrentServer( true );
        OLClearSaveMember();

		GSaveData->SetValue("Stipulation","");
		GSaveData->SaveData();

        if( PFIsValidPlatformCode( GOnlineInfo->PlatformCode ) )
        {
            PFUnregister();
        }
        else
        {
            OLResetMember();
        } */
	}
	else
	{
        printf("NET: RECV, ANS_DELETE_MEMBER FAILED\n");
		switch( RecvPacket->AnsCode )
		{
		case ANS_DELETE_MEMBER::eFail_NonMember:
			///ShowAlertMessage( EMBA_OK, EMCT_None, IUGetLocalText( L"info_unregister_notmember" ) );
                printf("NET: RECV, ANS_DELETE_MEMBER eFail_NonMemeber\n");
                break;
		case ANS_DELETE_MEMBER::eFail_Social:
			///ShowAlertMessage( EMBA_OK, EMCT_None, IUGetLocalText( L"info_unregister_error" ) );
                printf("NET: RECV, ANS_DELETE_MEMBER eFail_Social\n");
                break;
		case ANS_DELETE_MEMBER::eFail:
		default:
			///ShowAlertMessage( EMBA_OK, EMCT_None, IUGetLocalText( L"info_unregister_fail" ) );
                printf("NET: RECV, ANS_DELETE_MEMBER eFail\n");
		}
	}
}

RPHI( PTCL_ANS_CURRENT_TIME					)
{
	ANS_CURRENT_TIME* RecvPacket = (ANS_CURRENT_TIME*)(InPacket->Data);

	FnLog("\nNET: RECV, ANS_CURRENT_TIME");
    FnLog("Time: %lld", RecvPacket->Time);

	/// HideProgressMessage();

	GOnlineInfo->SetOnlineTime( RecvPacket->Time );

	// For One login logic
	if( GOnlineInfo->IsLoaded() ) return;

	FnLog("NET: LOGIC, Load All Ended...");

	GOnlineInfo->DidLoaded( true );

	///GGameInfo->LoadReplays( GOnlineInfo->UserKey );

	// Process Next
    /*
	if( GOnlineInfo->IsMatchMigrating() )
	{
		FnLog("NET: LOGIC, Progressing Custom Match");

		OLBeganMatching( GOnlineInfo->BattleMode, GOnlineInfo->MatchMode );
	}
	else
	{
		OLReportChangeGameStatus( GOnlineInfo->GetSceneBattleMode(), GOnlineInfo->GetScenePosition() );

        // Set User Key
		GOfflineReward->SetUserKey( GOnlineInfo->UserKey );

		// Notify to scene
        Telegram* NewMsg = new Telegram( 0, EOMC_LoggedIn, 0, CustomMessage::Online );
        IUGameManager().Message( NewMsg );
	} */
}
