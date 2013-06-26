/*
 *  PreDefinedNames.h
 *  Core
 *
 *  Created by Youngmin Kim on 10. 08. 15.
 *  Copyright 2010 SwallowStudio. All rights reserved.
 *
 */

/*-----------------------------------------------------------------------------
	Macros.
 -----------------------------------------------------------------------------*/

/** index of highest hardcoded name to be replicated by index by the networking code
 * @warning: changing this number or making any change to the list of hardcoded names with index
 * less than this value breaks network compatibility (update GEngineMinNetVersion)
 * @note: names with a greater value than this can still be replicated, but they are sent as
 * strings instead of an efficient index
 */
#define MAX_NETWORKED_HARDCODED_NAME 1250

// Define a message as an enumeration.
#ifndef REGISTER_NAME
#define REGISTER_NAME(num,name) NAME_##name = num,
#define REGISTERING_ENUM
enum EName {
#endif
	
	/*-----------------------------------------------------------------------------
	 Hardcoded names which are not messages.
	 -----------------------------------------------------------------------------*/
	
	// Special zero value, meaning no name.
	REGISTER_NAME(   0, None            )
	REGISTER_NAME(	 1, Critical		)
	REGISTER_NAME(	 2, Log				)
	REGISTER_NAME(	 3, FriendlyError	)
	REGISTER_NAME(	 4, DevConfig		)
	REGISTER_NAME(	 5, Warning			)
	REGISTER_NAME(	 6, Init			)
    REGISTER_NAME(   7, Exit            )

	// Register skill names
	REGISTER_NAME(  8, skill_name_healingcircle			)	//30000001
	REGISTER_NAME(  9, skill_name_paincircle			)	//30000002
	REGISTER_NAME( 10, skill_name_haste					)	//30000003
	REGISTER_NAME( 11, skill_name_slowly				)	//30000004
	REGISTER_NAME( 12, skill_name_bombing				)	//30000005
	REGISTER_NAME( 13, skill_name_nuked					)	//30000006
	REGISTER_NAME( 14, skill_name_stonehand				)	//30000007
	REGISTER_NAME( 15, skill_name_earthquake			)	//30000008
	REGISTER_NAME( 16, skill_name_insight				)	//30000009
	REGISTER_NAME( 17, skill_name_towerfortify1			)	//30000010
	REGISTER_NAME( 18, skill_name_towerfortifyfull		)	//30000011
	REGISTER_NAME( 19, skill_name_productfortify1		)	//30000012
	REGISTER_NAME( 20, skill_name_productfortifyfull	)	//30000013
	REGISTER_NAME( 21, skill_name_castlefortify1		)	//30000014
	REGISTER_NAME( 22, skill_name_unitreinforce			)	//30000015
	
#ifdef REGISTERING_ENUM
};
#undef REGISTER_NAME
#undef REGISTERING_ENUM
#endif

















