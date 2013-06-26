//##############################################################################################
// PacketSender.h
//##############################################################################################
#ifndef _PACKET_SENDER_H_
#define _PACKET_SENDER_H_

//##############################################################################################
//
//##############################################################################################
extern void OLReportCheckPing( int InTime );

// Changed
extern void OLReportChangeGameStatus( int InBattleMode, int InScenePosition, int InStatus=0, int InTime=0 );
extern void OLReportChangeGameOption( bool InMatchInvitation, bool InAllowMessage );

//##############################################################################################
// Authentication
//##############################################################################################
extern void OLAuthenticateLogin();

// GateServer
extern void OLRequestAuthGateServer();
extern void OLRequestServerChannels();

// GameServer
extern void OLRequestGameLogin();
extern void OLRequestGameLogout();
extern void OLRequestDeleteMember();

// Load Logic
extern void OLLoadDataAll();
extern void OLLoadServerTime(); // Final login logic end

//##############################################################################################
// Inventory
//##############################################################################################
// Deck
extern void OLRequestLoadDeck();
extern void OLRequestSaveDeck();

// Inventory Item
extern void OLRequestInventoryUnits();
extern void OLRequestInventoryCastles();
extern void OLRequestDeleteItem( int ItemSeqNo );
extern void OLRequestMixItem( int ItemSeqNo[] );
extern void OLRequestGrade( int ItemSeqNo[] );
extern void OLRequestOpenCapsule( int ItemSeqNo );
extern void OLRequestTrialProduct();
extern void OLRequestPurchase( int nID );
extern void OLRequestPurchasePermission( int productID );

#if ANDROID
extern void OLRequestInAppPurchase( const char *receipt, char* pSignedDataPtr, char* pSignaturePtr );
#else	// WIN32 || IOS
extern void OLRequestInAppPurchase( const char *receipt );
#endif	// ANDROID

extern void OLRequestTStoreInfo();

//##############################################################################################
// Profile
//##############################################################################################
extern void OLRequestProfilePrivateInfo();
extern void OLRequestProfileBaseInfo( long long wemadeID );
extern void OLRequestProfileBattleRecommendation( long long wemadeID );
extern void OLRequestProfileBattleHistory( long long wemadeID );
extern void OLRequestProfileGroupRanking( long long wemadeID, int page );
extern void OLRequestProfileAchievementHistory( long long wemadeID, int page );

extern void OLRequestMissionHistory();
// Event ( KakaoPlus )
extern void OLRequestEventRecord( int InEventID, bool InSave=false );

//##############################################################################################
// Friend
//##############################################################################################
extern void OLRequestFriendList(long long UKey, int page);
extern void OLRequestRecFriendList();
extern void OLRequestAddFriend( long long InUserKey, const char* InUserNick ,unsigned char inAddType );
extern void OLRequestFriendDeck(long long UKey);
extern void OLRequestSearchFriend( const char* InUserNick );
extern void OLRequestDeleteFriend( long long UKey );
extern void OLRequestFriendCompare( const char (*friendName)[52], int count );

//##############################################################################################
// Game ( Match, Replication )
//##############################################################################################
extern void OLReportCustomMatch( int InMsg, int InMatchMode, int InBattleMode, int InMatchTarget, long long InTargetKey, const char* InTargetNick, int InUserStatus );

extern void OLRequestStartMatch( int InMatchMode, int InBattleMode, int InMatchTarget, long long InTargetKey=0, int InAIMode=0, int MissionNO=0,int Ability=0 );
extern void OLRequestStopMatch();

extern void OLRequestStartGame( int InMapID );
extern void OLRequestStopGame( int InError );

extern void OLReportGameState( int InResult, int InType=0, int InOwnerID=0 );
extern void OLRequestGameRecord( int InMissionID=0, int InRank=0, int nScore = 0);

// Replication Begin //////////////////////////////////////////////////////////////////////////////////
extern void OLReplicateReady( int InType=1 );
extern void OLReplicateAction( int InAction, int InOwner, int InBase, int InCode, int InIndex, int InCost, int InX, int InY, int InGold, int InDerection=0 );
extern void OLReplicatePing( int InPing );
// Replication End   //////////////////////////////////////////////////////////////////////////////////

// Ranking Begin //////////////////////////////////////////////////////////////////////////////////
extern void OLRequestWeeklyRanking(int nSendCount = 0);
extern void OLRequestFriendStatus();
extern void OLRequestRankingInfo();
extern void OLRequestStarCount();
extern void OLRequestStarBox(int nPage = 1);
extern void OLRequestRecvStar(int nPost);
extern void OLRequestRecvStarAll();
extern void OLRequestSendPost(long long Target, long long UKey, bool bProud);


extern void OLRequestFrenInvite( const char* UserId, unsigned char szType );
extern void OLRequestFrenInviteList();
extern void OLRequestSendPlayerInfo( long long UKey, const char *pID );
extern void OLReportTop20WeeklyRanking( int nMissionNo );
extern void OLRequestMissionOpen( int nMissionNo );
extern void OLRequestGameComment( const char* pStr , int nMissionNo);
extern void OLRequestFriendState( long long *pUKey, int	iFriendCount );
// Ranking End //////////////////////////////////////////////////////////////////////////////////

//extern void OLReportGameComment( const char* pStr , int nMissionNo);
#ifndef FnLog
#define FnLog(...)  printf(__VA_ARGS__);printf("\n")
#endif


#endif // _PACKET_SENDER_H_
