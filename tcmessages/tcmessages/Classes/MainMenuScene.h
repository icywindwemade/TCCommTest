//
//  MainMenuScene.h
//  TestCCB2X
//
//  Created by Rickie Cheng on 6/11/13.
//
//

#ifndef TestCCB2X_MainMenuScene_h
#define TestCCB2X_MainMenuScene_h

#include "cocos2d.h"
#include "cocos-ext.h"

class MainMenuScene :
                    public cocos2d::CCLayer
                    ,public cocos2d::extension::CCBSelectorResolver
{
//    
//	virtual bool init();
//    
//	~MainMenuScene();
//    
//public:
//	CREATE_FUNC(MainMenuScene);
void StartNetworking();
    
    
public:
    static cocos2d::CCScene* scene();
    
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(MainMenuScene, create);
    
    virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(CCObject * pTarget, const char* pSelectorName);
    virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    void pressedButton  (cocos2d::CCObject * pSender);
    void onClose  (cocos2d::CCObject * pSender);
    
};

#endif
