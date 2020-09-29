#pragma once

#include "NitroConfig_Physics.h"

namespace Nitro
{
	using namespace Math;

	namespace Physics
	{
		class BoundingBox
		{
		public:
			BoundingBox();
			BoundingBox(Vector3 minBound, Vector3 maxBound);
			INLINE Vector3 GetMinBound() const { return m_MinBound; }
			INLINE Vector3 GetMaxBound() const { return m_MaxBound; }
			INLINE void SetMinBound(Vector3 minBound) { this->m_MinBound = minBound; }
			INLINE void SetMaxBound(Vector3 maxBound) { this->m_MinBound = maxBound; }

		private:
			Vector3 m_MinBound;
			Vector3 m_MaxBound;
		};

		INLINE BoundingBox::BoundingBox() {}

		INLINE BoundingBox::BoundingBox(Vector3 minBound, Vector3 maxBound)
			: m_MinBound(minBound), m_MaxBound(maxBound)
		{}
	}
}