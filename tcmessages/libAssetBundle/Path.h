#pragma once

#include "Vec.h"

class CPath
{
public :
	CPath() : m_Pos(NULL), m_Count( 0 ) {}
	CPath( int num, Vector2 *pos ) : m_Pos(pos), m_Count( num ) {}
	~CPath() { SAFE_DELETE_ARRAY( m_Pos ); }

	Vector2 GetProjectedPos( const Vector2 &pos );
	float	GetProjectedPos( const Vector2 &pos, Vector2 &vo ) {Vector2 va, vb; return GetProjectedPos( pos, vo, va, vb );};
	float	GetProjectedPos( const Vector2 &pos, Vector2 &vo, Vector2 &va, Vector2 &vb );
	bool	GetNextPos( const Vector2 &pos, bool bDir, Vector2 &vo );

	void GetStartPos( Vector2 &pos ) { pos = m_Pos[0]; };
	void GetEndPos( Vector2 &pos )	{ pos = m_Pos[m_Count-1]; }

	int m_Count;
	Vector2 *m_Pos;
};
