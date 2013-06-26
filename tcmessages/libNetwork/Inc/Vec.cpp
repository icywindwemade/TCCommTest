#include "IUDefine.h"

#include "Vec.h"



float GetNearPos(Vector2& po_Out, const Vector2& p, const Vector2& s0, const Vector2& s1)
{
	Vector2 v1 = p - s0;
	Vector2 v2 = s1 - s0;

	float dot1 = DotProduct( v1, v2 );

	if(dot1 < 0.0001f)
	{
		po_Out = s0;
		return v1.Length();
	}
	else
	{
		float dot2 = DotProduct( v2, v2);
		if(dot1 < dot2)
		{
			const Vector2 prj = (dot1/dot2) * v2;
			const Vector2 H = v1 - prj;

			po_Out = s0 + prj;
			return H.Length();
		}
		else
		{
			po_Out = s1;
			return (s1-p).Length();
		}
	}
}