#include "IUDefine.h"

#include "Vec3.h"



float GetNearPos(Vector3& po_Out, const Vector3& p, const Vector3& s0, const Vector3& s1)
{
	Vector3 v1 = p - s0;
	Vector3 v2 = s1 - s0;

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
			const Vector3 prj = (dot1/dot2) * v2;
			const Vector3 H = v1 - prj;

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


const Vector3 Vector3::ZERO(0, 0, 0);
const Vector3 Vector3::AXISX(1, 0, 0);
const Vector3 Vector3::AXISY(0, 1, 0);
const Vector3 Vector3::AXISZ(0, 0, 1);