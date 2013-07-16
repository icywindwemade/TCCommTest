#include "IUDefine.h"

#include "Path.h"

inline Vector2 CPath::GetProjectedPos( const Vector2 &pos )
{
	Vector2 ProjectedPos;
	GetProjectedPos( pos, ProjectedPos );
	return ProjectedPos;
}

float CPath::GetProjectedPos( const Vector2 &pos, Vector2 &vo, Vector2 &vao, Vector2 &vbo  )
{
	float dist = 100000.0f;
	for( int i = 0; i < m_Count-1; i++ )
	{
		Vector2 p;
		float d = GetNearPos( p, pos, m_Pos[i], m_Pos[i+1] );
		if( d < dist )
		{
			dist = d;
			vo = p;
			vao = m_Pos[i];
			vbo = m_Pos[i+1];
		}
	}
	return dist;
}

bool CPath::GetNextPos( const Vector2 &pos, bool bDir, Vector2 &vo )
{
	for( int i = 0; i < m_Count; i++ )
	{
		if( m_Pos[i] == pos )
		{
			if(bDir)
			{
				if( i < m_Count -1 )
					vo = m_Pos[i+1];
				else
					vo = m_Pos[i];
			}
			else
			{
				if( i > 0 )
					vo = m_Pos[i-1];
				else
					vo = m_Pos[i];
			}
			return true;
		}
	}
	return false;
}