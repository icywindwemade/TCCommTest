//
//  MainMenuScene.cpp
//  TestCCB2X
//
//  Created by Rickie Cheng on 6/11/13.
//
//

#include "MainMenuScene.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

#include "MainMenuSceneLoader.h"

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
            break;
        case 2:
            break;
        case 3:
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

