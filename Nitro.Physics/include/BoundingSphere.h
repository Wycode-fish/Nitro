#pragma once

#include "NitroConfig_Physics.h"

namespace Nitro
{
	using namespace Math;

	namespace Physics
	{
		class BoundingSphere
		{
		public:
			BoundingSphere();
			BoundingSphere(Vector3 center, Scalar radius);
			explicit BoundingSphere(Vector4 sphere);

			INLINE Vector3 GetCenter() const { return Vector3(this->m_Repr); }
			INLINE Scalar GetRadius() const { return this->m_Repr.GetW(); }
		private:
			Vector4 m_Repr;
		};
	}
}