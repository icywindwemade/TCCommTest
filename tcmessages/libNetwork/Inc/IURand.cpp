#include "IUDefine.h"

#include "IURand.h"
//#if ANDROID
#include <stdlib.h>
//#endif	// ANDROID

CIURandom::CIURandom()
: m_iSeed( 0 )
{
}

CIURandom::~CIURandom()
{
}

void CIURandom::SetSeed( UINT seed )
{
	m_iSeed = seed; 
}

UINT CIURandom::Rand()
{
	srand( m_iSeed );
	m_iSeed = rand();
	return m_iSeed; 
}
