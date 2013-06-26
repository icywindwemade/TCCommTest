#pragma once

class CIURandom
{
public :
	CIURandom();
	~CIURandom();

	void SetSeed( UINT seed );
	UINT Rand();

private :
	UINT m_iSeed;
};