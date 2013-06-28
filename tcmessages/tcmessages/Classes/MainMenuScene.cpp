//
//  MainMenuScene.cpp
//  TestCCB2X
//
//  Created by Rickie Cheng on 6/11/13.
//
//

#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

#include "MainMenuScene.h"
#include "MainMenuSceneLoader.h"
#include "Scene2.h"

//------- libNetwork headers ----------
#include "PacketSender.h"
#include "NetworkManager.h"
#include "NetworkMonitor.h"
#include "OnlineDefine.h"
#include "OnlineHandler.h"

NNetworkManager* GNetworkManager=NULL;
NNetworkMonitor* GNetworkMonitor=NULL;



CCScene* MainMenuScene::scene()
{
    CCScene* scene = CCScene::create();
    scene->addChild(MainMenuSceneLoader::load());
    
    return scene;
}

SEL_MenuHandler MainMenuScene::onResolveCCBCCMenuItemSelector(cocos2d::CCObject *pTarget, const char *pSelectorName)
{
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "pressedButton", MainMenuScene::pressedButton);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onClose", MainMenuScene::onClose);
    return NULL;
}

SEL_CCControlHandler MainMenuScene::onResolveCCBCCControlSelector(cocos2d::CCObject *pTarget, const char *pSelectorName)
{
    return NULL;
}

bool MainMenuScene::onAssignCCBMemberVariable(cocos2d::CCObject *pTarget, const char *pMemberVariableName, cocos2d::CCNode *pNode)
{
    char btnName[40];
    for(int ii=0;ii< BUTTONS_TOTAL; ii++) {
        sprintf( btnName, "btn%d", ii);
        CCB_MEMBERVARIABLEASSIGNER_GLUE(this, btnName, CCMenuItemImage*, this->btn[ii]);
    }
    //CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "livesLabel", CCLabelTTF*, this->livesLabel);
    //CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "dodgesLabel", CCLabelTTF*, this->dodgesLabel);
    return false;
}
//bool MainMenuScene::onAssignCCBCustomProperty(cocos2d::CCObject *pTarget, const char *pMemberVariableName, cocos2d::extension::CCBValue *pCCBValue)
//{
//    return false;
//}


void MainMenuScene::onClose(cocos2d::CCObject *pSender)
{
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void MainMenuScene::pressedButton(cocos2d::CCObject *pSender)
{
    CCMenuItemImage * button = (CCMenuItemImage*)pSender;
    int tag = button->getTag();
    printf("Button %d pressed\n", tag);
    switch(tag){
        case 1:
            StartNetworking();
            OLAuthenticateLogin();
            for( int ii = 1; ii<BUTTONS_TOTAL; ii++) {
                btn[ii]->setEnabled(true);
            }
            break;
        case 2:
            OLRequestInventoryCastles();
            OLRequestInventoryUnits();
            break;
        case 3:
            OLRequestLoadDeck();
            break;
        case 4:
            OLRequestMissionHistory();
            break;
        case 5:
            OLRequestProfileBaseInfo(10000000850); // wemadeID
            OLRequestProfilePrivateInfo();
            break;
        case 6:
            OLRequestEventRecord( 1486 );
            OLRequestEventRecord( 1501 );
            OLRequestEventRecord( 1502 );
            break;
        case 7:
            OLLoadServerTime();
            break;
        case 8:
            CCDirector::sharedDirector()->replaceScene( CCTransitionMoveInR::create( 0.3, Scene2::scene()));
            break;
        default:
            //printf("Button %d pressed\n", tag);
            break;
    }
}

void MainMenuScene:: StartNetworking() {
    if( !GNetworkMonitor ) GNetworkMonitor = NNetworkMonitor::Instance();
    if( !GNetworkManager ) GNetworkManager = NNetworkManager::Instance();

    if( !GOnlineInfo ) GOnlineInfo = NOnlineInfo::Instance();
    
    OLInit();
    GNetworkMonitor->Instance()->Start();
}

