//
//  Scene2.cpp
//  CCBTest2X
//
//  Created by Rickie Cheng on 6/12/13.
//
//

#include "Scene2.h"
#include "MainMenuScene.h"

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

void Scene2::pressedBack(cocos2d::CCObject *pSender)
{
    printf("Back pressed\n");
    CCDirector::sharedDirector()->replaceScene( CCTransitionMoveInL::create( 0.333, MainMenuScene::scene()));
}

void Scene2::pressedButton(cocos2d::CCObject *pSender)
{
    CCMenuItemImage * button = (CCMenuItemImage*)pSender;
    int tag = button->getTag();
    switch(tag){
        case 1:
        case 2:
        case 3:
       default:
            printf("Button %d pressed\n", tag);
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
