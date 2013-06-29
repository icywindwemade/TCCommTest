//
//  Scene2.cpp
//  CCBTest2X
//
//  Created by Rickie Cheng on 6/12/13.
//
//

#include "Scene2.h"
#include "MainMenuScene.h"
#include "OnlineDefine.h"
#include "OnlineHandler.h"
#include "PacketSender.h"

USING_NS_CC;
USING_NS_CC_EXT;

//==============================================================================
//  Scene class
//============================================================================== 
CCScene* Scene2::scene()
{
    CCScene* scene = CCScene::create();
    scene->addChild(Scene2Loader::load());
    
    return scene;
}

SEL_MenuHandler Scene2::onResolveCCBCCMenuItemSelector(cocos2d::CCObject *pTarget, const char *pSelectorName)
{
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "pressedButton", Scene2::pressedButton);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "pressedBack", Scene2::pressedBack);
    return NULL;
}

SEL_CCControlHandler Scene2::onResolveCCBCCControlSelector(cocos2d::CCObject *pTarget, const char *pSelectorName)
{
    return NULL;
}

bool Scene2::onAssignCCBMemberVariable(cocos2d::CCObject *pTarget, const char *pMemberVariableName, cocos2d::CCNode *pNode)
{
    char btnName[40];
    for(int ii=0;ii< BUTTONS_TOTAL; ii++) {
        sprintf( btnName, "btn%d", ii);
        CCB_MEMBERVARIABLEASSIGNER_GLUE(this, btnName, CCMenuItemImage*, this->btn[ii]);
    }
    return false;
}

void Scene2::pressedBack(cocos2d::CCObject *pSender)
{
    printf("Back pressed\n");
    CCDirector::sharedDirector()->replaceScene( CCTransitionMoveInL::create( 0.333, MainMenuScene::scene()));
}

long long GFriendID = 10000000005;
const char * GFriendName = "\267\347\300\314\301\366";
void Scene2::pressedButton(cocos2d::CCObject *pSender)
{
    CCMenuItemImage * button = (CCMenuItemImage*)pSender;
    int tag = button->getTag();
    printf("Button %d pressed\n", tag);
    switch(tag){
        case 1:
            OLRequestFriendList(10000000850, 1);
            break;
        case 2:
            OLRequestRecFriendList();
            break;
        case 3:
            //void OLRequestAddFriend( long long InUserKey, const char* InUserNick , unsigned char inAddType )
            // OLRequestAddFriend(10000000005, "TEST", '\0' );
            OLRequestAddFriend(GFriendID, GFriendName, '\0' );
            break;
        case 4:
            OLRequestDeleteFriend(GFriendID);
            break;
        case 5:
            OLRequestSearchFriend(GFriendName);
            break;
        case 6:
            OLRequestFriendDeck(GFriendID);
            break;
        case 7:
            OLRequestPurchase(75900001);
            break;
        default:
            break;
    }
}

//==============================================================================
//  Loader class
//============================================================================== 
Scene2* Scene2Loader::load()
{
    cocos2d::extension::CCNodeLoaderLibrary * ccNodeLoaderLibrary = cocos2d::extension::CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();
    
    ccNodeLoaderLibrary->registerCCNodeLoader("Scene2", Scene2Loader::loader());
    
    /* Create an autorelease CCBReader. */
    cocos2d::extension::CCBReader * ccbReader = new cocos2d::extension::CCBReader(ccNodeLoaderLibrary);
    ccbReader->autorelease();
    
    /* Read a ccbi file. */
    cocos2d::CCNode * node = ccbReader->readNodeGraphFromFile("Scene2.ccbi");
    
    Scene2* layer = (Scene2*)node;
    
    return layer;
}
