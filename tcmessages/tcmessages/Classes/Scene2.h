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

USING_NS_CC;
USING_NS_CC_EXT;


#define BUTTONS_TOTAL 8

//==============================================================================
//  Scene class
//==============================================================================
class Scene2 :
     public CCLayer
    ,public CCBSelectorResolver
    ,public CCBMemberVariableAssigner
{
    
    CCMenuItemImage * btn[ BUTTONS_TOTAL ];
    
public:
    static cocos2d::CCScene* scene();
    
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(Scene2, create);
    
    virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(CCObject * pTarget, const char* pSelectorName);
    virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    virtual bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    
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
