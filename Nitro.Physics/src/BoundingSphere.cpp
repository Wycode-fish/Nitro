#include "..\include\BoundingSphere.h"

namespace Nitro
{
	namespace Physics
	{
		BoundingSphere::BoundingSphere()
			: m_Repr()
		{
		}
		BoundingSphere::BoundingSphere(Vector3 center, Scalar radius)
			: m_Repr(center, radius)
		{
		}
		BoundingSphere::BoundingSphere(Vector4 sphere)
			: m_Repr(sphere)
		{
		}
	}
}