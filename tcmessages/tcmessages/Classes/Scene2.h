//==============================================================================
//
//  Scene2.h
//  CCBTest2X
//
//  Created by Rickie Cheng on 6/12/13.
//
//
//==============================================================================

#ifndef CCBTest2X_Scene2_h
#define CCBTest2X_Scene2_h
#include "cocos2d.h"
#include "cocos-ext.h"


#define DIFFICULTY_EASY_BUTTON_TAG 1
#define DIFFICULTY_MEDIUM_BUTTON_TAG 2
#define DIFFICULTY_HARD_BUTTON_TAG 3

//==============================================================================
//  Scene class
//============================================================================== 
class Scene2 : public cocos2d::CCLayer
,public cocos2d::extension::CCBSelectorResolver
{
    
public:
    static cocos2d::CCScene* scene();
    
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(Scene2, create);
    
    virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(CCObject * pTarget, const char* pSelectorName);
    virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    void pressedBack(cocos2d::CCObject * pSender);
    void pressedButton(cocos2d::CCObject * pSender);
    
};


//==============================================================================
//  Loader class
//============================================================================== 
class Scene2Loader : public cocos2d::extension::CCLayerLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(Scene2Loader, loader);
    static Scene2* load();
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(Scene2);
    
};



#endif
