//##############################################################################################
// PacketRecver.cpp
//##############################################################################################
#include "PacketRecver.h"

#include "PacketSender.h"
#include "BuildDefine.h"
#include "OnlineHandler.h"
//#include "Client.h"
//#include "MessageHandler.h"
//#include "GlobalInclude.h"
//#include "FriendsInfo.h"
//#include "FriendsScene.h"
//#include "FriendTab_FindFriends.h"
//#include "FriendTab_GameFriends.h"


//##############################################################################################
// Friend
//##############################################################################################

// FriendSub_BattleInfo
RPHI( PTCL_ANS_DECK )
{
	ANS_DECK* RecvPacket = (ANS_DECK*)(InPacket->Data);

	FnLog("NET: RECV, PTCL_ANS_DECK");
    FnLog("-----------------------------");

    for( int i = 0; i < UD_UNIT_SLOT_COUNT; i++ )
    {
        /// GFriendsInfo->GameFriendUnitIDs[i] = RecvPacket->UnitIDs[i];
        FnLog("Unit[%d] = %d", i, RecvPacket->UnitIDs[i]);
    }
    
    FnLog("-----------------------------");
    
    /*
     GFriendsInfo->GameFriendCastleID = RecvPacket->CastleID;
    
	// Send Message to FriendSub_BattleInfo
    Telegram* NewTelegram = new Telegram( 0, EOMC_UserDeck, 0, CustomMessage::Online );
    IUGameManager().Message( NewTelegram ); */
}

// FriendTab_FindFriends
RPHI( PTCL_ANS_FRIEND_RECOMMENDATION_LIST					)
{
	///GFriendsInfo->ResetSearchedFriends();

	ANS_FRIEND_RECOMMENDATION_LIST* RecvPacket = (ANS_FRIEND_RECOMMENDATION_LIST*)(InPacket->Data);

	FnLog("NET: RECV, ANS_FRIEND_RECOMMENDATION_LIST");
    FnLog("Total Recommendations = %d", RecvPacket->FriendCount);

	for( int i = 0; i < RecvPacket->FriendCount; i++ )
	{
        FnLog("Name: %s", RecvPacket->Friend[i].Name );
        FnLog("Ukey: %lld", RecvPacket->Friend[i].UKey);
		FnLog("PortraitNo: %d", RecvPacket->Friend[i].Portrait);
		FnLog("Status: %d", RecvPacket->Friend[i].Status);
		FnLog("Type: %d", RecvPacket->Friend[i].Type);
        /*
		FUserProfile TargetUser;
		TargetUser.UserKey			= RecvPacket->Friend[i].UKey;
		TargetUser.PortraitNo		= RecvPacket->Friend[i].Portrait;
		TargetUser.Status			= RecvPacket->Friend[i].Status;
		TargetUser.FriendType		= RecvPacket->Friend[i].Type; // eUDFriendType

		strcpy( TargetUser.UserNick, RecvPacket->Friend[i].Name );

		if( TargetUser.FriendType != eUDFT_InGame )
		{
			TargetUser.PlatformKey = appAtoi64( RecvPacket->Friend[i].ID );
			strcpy( TargetUser.PlatformID, RecvPacket->Friend[i].ID );
		}

		// add Recommendation Friend 
		GFriendsInfo->AddFriend( &GFriendsInfo->SearchedFriendList, &TargetUser );
         */
	}

	// Send Message to FriendTab_FindFriends
	///Telegram* NewTelegram = new Telegram( 0, EOMC_RecommendFriend, 0, CustomMessage::Online );
	///IUGameManager().Message( NewTelegram ); */
}

// FriendTab_GameFriends
RPHI( PTCL_ANS_FRIEND_LIST					)
{
	ANS_FRIEND_LIST* RecvPacket = (ANS_FRIEND_LIST*)(InPacket->Data);

	FnLog("NET: RECV, ANS_FRIEND_LIST");

    FnLog("Total Friends = %d", RecvPacket->FriendCount);
    
	for( int i = 0; i < RecvPacket->FriendCount; i++ )
	{
        FnLog("----------------------------------------");
        FnLog("Name: %s", RecvPacket->Friend[i].Name );
        FnLog("Ukey: %lld", RecvPacket->Friend[i].UKey);
		FnLog("PortraitNo: %d", RecvPacket->Friend[i].Portrait);
		FnLog("Status: %d", RecvPacket->Friend[i].Status);
		FnLog("Type: %d", RecvPacket->Friend[i].Type);
    }
/*    bool bMyFriends = true;
	if( GOnlineInfo->UserKey != RecvPacket->UKey )
		bMyFriends = false;
    
    if( bMyFriends )
    {
        // My Friends
		for( int i = 0; i < RecvPacket->FriendCount; i++ )
		{
            FUserProfile* TargetUser = GFriendsInfo->GetGameFriend( RecvPacket->Friend[i].UKey );
            if( TargetUser )
            {
                TargetUser->UserKey = RecvPacket->Friend[i].UKey;
                TargetUser->Status = RecvPacket->Friend[i].Status; // eUDUserInvitationStatus                
                TargetUser->PortraitNo = RecvPacket->Friend[i].Portrait;
                TargetUser->FriendType = RecvPacket->Friend[i].Type; // eUDFriendType
				TargetUser->IsFriend	 = true;
                strcpy( TargetUser->UserNick, RecvPacket->Friend[i].Name );
            }
            else
            {
                FUserProfile NewUser;
                NewUser.UserKey = RecvPacket->Friend[i].UKey;
                NewUser.Status = RecvPacket->Friend[i].Status;
                NewUser.PortraitNo = RecvPacket->Friend[i].Portrait;
                NewUser.FriendType = RecvPacket->Friend[i].Type;
				NewUser.IsFriend	= true;
                strcpy( NewUser.UserNick, RecvPacket->Friend[i].Name );
                
                if( NewUser.FriendType != eUDFT_InGame )
                {
                    NewUser.PlatformKey = appAtoi64( RecvPacket->Friend[i].ID );
                    strcpy( NewUser.PlatformID, RecvPacket->Friend[i].ID );
					NewUser.PlatformFriendType = PFT_BothFriend;
                }
                
                NewUser.PlatformIndex = -2;
                GFriendsInfo->GameFriends.AddItem( NewUser );
            }
		}
        
        GFriendsInfo->GameFriendCurrentPage = RecvPacket->Page;
        GFriendsInfo->GameFriendTotalPage = RecvPacket->PageCount;

		Telegram* NewTelegram = new Telegram( 0, EOMC_OwnerGameFriends, 0, CustomMessage::Online );
		IUGameManager().Message( NewTelegram );
    }
    else
    {
        // Other Friends
        if( RecvPacket->Page == 1 )
            GFriendsInfo->ResetOtherFriends();
        
        // My Friends
		for( int i = 0; i < RecvPacket->FriendCount; i++ )
		{
            FUserProfile NewUser;
            NewUser.UserKey = RecvPacket->Friend[i].UKey;
            NewUser.Status = RecvPacket->Friend[i].Status;
            NewUser.PortraitNo = RecvPacket->Friend[i].Portrait;
            NewUser.FriendType = RecvPacket->Friend[i].Type;
            strcpy( NewUser.UserNick, RecvPacket->Friend[i].Name );
            
            if( NewUser.FriendType != eUDFT_InGame )
            {
                NewUser.PlatformKey = appAtoi64( RecvPacket->Friend[i].ID );
                strcpy( NewUser.PlatformID, RecvPacket->Friend[i].ID );
            }
            
            NewUser.PlatformIndex = -2;
            GFriendsInfo->OtherFriends.AddItem( NewUser );
		}

		Telegram* NewTelegram = new Telegram( 0, EOMC_OtherGameFriends, 0, CustomMessage::Online );
		IUGameManager().Message( NewTelegram );
    }*/
}

// FriendTab_FindFriends 
RPHI( PTCL_ANS_SEARCH_FRIEND				)
{
	ANS_SEARCH_FRIEND* RecvPacket = (ANS_SEARCH_FRIEND*)(InPacket->Data);

	FnLog("NET: RECV, ANS_SEARCH_FRIEND");
    

    FnLog("Name: %s", RecvPacket->Name );
    FnLog("Ukey: %lld", RecvPacket->UKey);
	FnLog("PortraitNo: %d", RecvPacket->Portrait);
	FnLog("isFriend: %d", RecvPacket->IsFriend);
    FnLog("Status: %d", RecvPacket->Status);


	/* // send message to child scene
	++GFriendsInfo->SearchedPacketCount;

	if ( RecvPacket->UKey != 0 && GOnlineInfo->UserKey != RecvPacket->UKey )
	{
		FUserProfile TargetUser;
		TargetUser.UserKey			= RecvPacket->UKey;
		TargetUser.PortraitNo		= RecvPacket->Portrait;
		TargetUser.IsFriend			= RecvPacket->IsFriend;
		TargetUser.Status			= RecvPacket->Status;

		strcpy( TargetUser.UserNick, RecvPacket->Name );
					
		// add Searched Friend 
		GFriendsInfo->AddFriend( &GFriendsInfo->SearchedFriendList, &TargetUser );
	}

	if ( GFriendsInfo->SearchedPacketCount >= RecvPacket->AnsQueryCount )
	{
		// Send Message to FriendTab_FindFriends
		Telegram* NewTelegram = new Telegram( 0, EOMC_FindFriend, 0, CustomMessage::Online );
		IUGameManager().Message( NewTelegram );
	}	*/
}

// FriendTab_FindFriends - plan to Fix
RPHI( PTCL_ANS_ADD_FRIEND					)
{
	// more fix
	ANS_ADD_FRIEND* RecvPacket = (ANS_ADD_FRIEND*)(InPacket->Data);
    static const char *ErrStr [] = {
        "eSuccess",
        "eFail",
        "eFail_Duplication",
        "eFail_NoMember",
        "eFail_None_AddType"
    } ;

	FnLog("NET: RECV, ANS_ADD_FRIEND");
    FnLog(" AnsCode = %d (%s) \n", RecvPacket->AnsCode,  ErrStr[RecvPacket->AnsCode]);
    /*
    eSuccess = 0,
    eFail,
    eFail_Duplication,
    eFail_NoMember,
    eFail_None_AddType,
     */

/*
	HideProgressMessage();

#if ( BK_NOT_USE == 0 )
	// not use
	// OnlineHandler - OLCheckAutoAddFriend()
	if( GFriendsManager->ArgFriendKakaoID != 0 )
	{
		GFriendsManager->AddedFriendKakaoID = GFriendsManager->ArgFriendKakaoID;

		if( RecvPacket->AnsCode == ANS_ADD_FRIEND::eSuccess )
		{
			if( GFriendsManager->IsBlockMessage( GFriendsManager->AddedFriendKakaoID ) == false)
			{
				ShowAlertMessage( EMBA_OK|EMBA_Cancel, EMCT_AddedFriend, IUGetLocalText( L"info_friendadd_kakao_push_01" ) );
			}
			else
			{
				GFriendsManager->AddedFriendKakaoID = 0;
				ShowAlertMessage( EMBA_OK, EMCT_None, IUGetLocalText( L"info_friendadd_success" ) );
			}

			if( IUSceneManager().GetCurrentScene()->GetChildScene() != NULL )
			{
				if( IUSceneManager().GetCurrentScene()->GetChildScene( eID_Scene_Child_FriendPage ) )
				{
					//FriendKakao* pFriendSearch;
					FriendPage* pScene = (FriendPage *)IUSceneManager().GetCurrentScene()->GetChildScene();

					if( pScene )//&& pScene->GetChildScene() && pScene->GetChildScene()->GetID() == FriendTab_FriendKakao )
					{
						pScene->Close();
						pScene->SetTab(FriendTab_FriendList);
						pScene->OpenTab();

					}
				}
			}

			GFriendsManager->ArgFriendKakaoID = 0;

			return;
		}

		GFriendsManager->ArgFriendKakaoID = 0;

		return;
	}
#endif	

	switch( RecvPacket->AnsCode )
	{
	case ANS_ADD_FRIEND::eSuccess:
		{
			// 친구 추가를 성공했으면
			// 메세지 받기 off 가아닐경우 메세지 전송
			// on 일경우 성공popup만
//			if( GFriendsManager->IsBlockMessage( GFriendsManager->AddingFriendKakaoID ) == false)
//			{
//				ShowAlertMessage( EMBA_OK|EMBA_Cancel, EMCT_AddingFriend, IUGetLocalText( L"info_friendadd_kakao_push_01" ) );
//			}
//			else
//			{
//				GFriendsManager->AddingFriendKakaoID = 0;
//				ShowAlertMessage( EMBA_OK, EMCT_None, IUGetLocalText( L"info_friendadd_success" ) );
//			}
			
			Telegram* NewTelegram = new Telegram( 0, EOMC_AddFriendSuccess, 0, CustomMessage::Online );
			IUGameManager().Message( NewTelegram );
			
		}
		break;
	case ANS_ADD_FRIEND::eFail_Duplication:
		ShowAlertMessage( EMBA_OK, EMCT_None, IUGetLocalText( L"info_friendadd_duplication" ) );
		break;
	case ANS_ADD_FRIEND::eFail_NoMember:
		{
			ShowAlertMessage( EMBA_OK, EMCT_None, IUGetLocalText( L"info_friendadd_nomember" ) );

			if (RecvPacket->Friend.Type == eUDFT_KakaoTalk)
			{
				long long lUserKey = appAtoi64( RecvPacket->Friend.Name );

				//LogPrintf(" ### lUserKey %lli",lUserKey);
				if (lUserKey)
				{
//				GFriendsManager->ChangeKakaoFriendType( lUserKey, KFT_NotAppFriend );
				}

				//if( IUSceneManager().GetCurrentScene()->GetChildScene() != NULL )
				//{
				//	if( IUSceneManager().GetCurrentScene()->GetChildScene()->GetID() == eID_Scene_Child_FriendPage )
				//	{
				//		FriendPage* pScene = (FriendPage *)IUSceneManager().GetCurrentScene()->GetChildScene();

				//		if( pScene && pScene->GetID() == FriendTab_FriendKakao )
				//		{
				//			FriendKakao* KakaoTab = (FriendKakao*)( pScene->GetCurrentTab() );
				//			KakaoTab->SetUIFriendPage();
				//		}
				//	}
				//}
			}
		}
		break;
	case ANS_ADD_FRIEND::eFail:
	default:
		ShowAlertMessage( EMBA_OK, EMCT_None, IUGetLocalText( L"info_friendadd_fail" ) );
	} */
}

// FriendTab_GameFriends
RPHI( PTCL_ANS_DELETE_FRIEND					)
{
	ANS_DELETE_FRIEND* RecvPacket = (ANS_DELETE_FRIEND*)(InPacket->Data);

    static const char *ErrStr [] = {
        "eSuccess",
        "eFail",
        "eFail_NoMember",
        "eFail_Unknown_Req"
    } ;
	FnLog( "NET: RECV, ANS_DELETE_FRIEND: %d (%s)", RecvPacket->AnsCode, ErrStr[RecvPacket->AnsCode] );
    /*
	HideProgressMessage();

	switch( RecvPacket->AnsCode )
	{
	case ANS_DELETE_FRIEND::eSuccess:
		GFriendsInfo->RemoveGameFriends( RecvPacket->FriendUKey );
		ShowAlertMessage( EMBA_OK, EMCT_None, IUGetLocalText( L"help_friends_del" ) );
		break;

	case ANS_DELETE_FRIEND::eFail_NoMember:
		ShowAlertMessage( EMBA_OK, EMCT_None, IUGetLocalText( L"info_friendadd_nomember" ) );
		break;
	case ANS_DELETE_FRIEND::eFail:
	case ANS_DELETE_FRIEND::eFail_Unknown_Req:
	default:
		ShowAlertMessage( EMBA_OK, EMCT_None, IUGetLocalText( L"info_friendelete_fail" ) );
	}

	// Send Message to FriendTab_GameFriends
	Telegram* NewTelegram = new Telegram( 0, EOMC_DeleteGameFriend, 0, CustomMessage::Online );
	IUGameManager().Message( NewTelegram ); */
}

// FriendTab_PlatForm
RPHI( PTCL_ANS_FRIEND_MATCH_IN_GAME )
{
	///ANS_FRIEND_MATCH_IN_GAME* RecvPacket = (ANS_FRIEND_MATCH_IN_GAME*)(InPacket->Data);

	FnLog("NET: RECV, ANS_FRIEND_MATCH_IN_GAME");
    /*
    int TargetIndex = -1;
    FUserProfile* TargetUser = 0;
    for ( int i = 0 ; i < 10 ; i++)
    {
        if ( !strcmp(RecvPacket->FriendID[i],"") )
            continue;

        long long PlatformKey = appAtoi64( RecvPacket->FriendID[i] );

        TargetUser = GFriendsInfo->ComparedPlatformFriendType( PlatformKey, RecvPacket->IsFriend[i] ? PFT_BothFriend : PFT_Max );
        
        if( TargetUser && TargetIndex < 0 )
        {
            TargetIndex = TargetUser->PlatformIndex;
        }
    }
    
	// Send Message to FriendTab_PlatForm
	Telegram* NewTelegram = new Telegram( 0, EOMC_CompareFriends, TargetIndex, CustomMessage::Online );
	IUGameManager().Message( NewTelegram ); */
}
