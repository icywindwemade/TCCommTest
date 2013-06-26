#ifndef __BUILDDEFINE_H__
#define __BUILDDEFINE_H__

#ifndef FOR_JAVA
#include "Core.h"
#endif

//##############################################################################################
//	Setting
//##############################################################################################

// Select Platform
#define BUILD_PLATFORM_NONE             0 // Login for CND
#define BUILD_PLATFORM_KAKAO            1
#define BUILD_PLATFORM_HSP              2 // Login for Line
#define BUILD_PLATFORM_FACEBOOK         3

////////////////////////////////////////////////////////////////////////////////////////////////
#if IOS || ANDROID
	#define BUILD_WITH_PLATFORM_TARGET      BUILD_PLATFORM_KAKAO
	//#define BUILD_WITH_PLATFORM_TARGET      BUILD_PLATFORM_HSP
	//#define BUILD_WITH_PLATFORM_TARGET      BUILD_PLATFORM_FACEBOOK
	//#define BUILD_WITH_PLATFORM_TARGET      BUILD_PLATFORM_NONE
#else
	#define BUILD_WITH_PLATFORM_TARGET      BUILD_PLATFORM_NONE
#endif

#define BUILD_WITH_GAME_CODE			202
#define	BUILD_DETECT_MEMORY_LEAK		0
























#endif	// __BUILDDEFINE_H__
