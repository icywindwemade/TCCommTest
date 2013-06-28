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
USING_NS_CC;
USING_NS_CC_EXT;


#define BUTTONS_TOTAL 7

class MainMenuScene :
                    public CCLayer
                    ,public CCBSelectorResolver
                    ,public CCBMemberVariableAssigner
{
//    
//	virtual bool init();
//    
//	~MainMenuScene();
//    
//public:
//	CREATE_FUNC(MainMenuScene);
void StartNetworking();
    CCMenuItemImage * btn[ BUTTONS_TOTAL ];
    
public:
    static cocos2d::CCScene* scene();
    
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(MainMenuScene, create);
    
    virtual SEL_MenuHandler onResolveCCBCCMenuItemSelector(CCObject * pTarget, const char* pSelectorName);
    virtual SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    virtual bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    //virtual bool onAssignCCBCustomProperty(CCObject* pTarget, const char* pMemberVariableName, CCBValue* pCCBValue);
    
    
    void pressedButton  (CCObject * pSender);
    void onClose  (CCObject * pSender);
    
};

#endif
